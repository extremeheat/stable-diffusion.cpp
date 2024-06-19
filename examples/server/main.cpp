#include <chrono>
#include <cstring>
#include <filesystem>
#include <future>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

typedef SOCKET socket_t;
#define close_socket(s) closesocket(s)
#define socket_error() WSAGetLastError()
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
typedef int socket_t;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define close_socket(s) close(s)
#define socket_error() errno
#define WSACleanup()
#endif

#if __has_include("stable-diffusion.h")
#define STABLE_DIFFUSION
// #include "preprocessing.hpp"
#include "stable-diffusion.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"

std::string join(const std::vector<std::string>& v, const std::string& delim) {
    std::string s;
    for (size_t i = 0; i < v.size(); ++i) {
        if (i != 0)
            s += delim;
        s += v[i];
    }
    return s;
}

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_STATIC
#include "stb_image_write.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_STATIC
#include "stb_image_resize.h"
#endif

#include "./bundle.h"

const int DEFAULT_PORT = 8024;
const int BUFFER_SIZE  = 4096;
// Seconds to rate limit switching models and file system searching
const int RATE_LIMIT = 5;

inline std::string base64_encode(const std::string& data) {
    static constexpr char kEncodingTable[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string encoded;
    encoded.reserve(((data.size() / 3) + (data.size() % 3 > 0)) * 4);

    for (size_t i = 0; i < data.size(); i += 3) {
        uint32_t octet_a = i < data.size() ? static_cast<uint8_t>(data[i]) : 0;
        uint32_t octet_b =
            (i + 1) < data.size() ? static_cast<uint8_t>(data[i + 1]) : 0;
        uint32_t octet_c =
            (i + 2) < data.size() ? static_cast<uint8_t>(data[i + 2]) : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded.push_back(kEncodingTable[(triple >> 3 * 6) & 0x3F]);
        encoded.push_back(kEncodingTable[(triple >> 2 * 6) & 0x3F]);
        encoded.push_back(
            i + 1 < data.size() ? kEncodingTable[(triple >> 1 * 6) & 0x3F] : '=');
        encoded.push_back(
            i + 2 < data.size() ? kEncodingTable[(triple >> 0 * 6) & 0x3F] : '=');
    }

    return encoded;
}

inline std::string base64_decode(const std::string& in) {
    if (in.length() % 4 != 0)
        return "";  // Non-base64 character

    std::string out;
    std::string chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++)
        T[chars[i]] = i;

    int val = 0, valb = -8;
    for (unsigned char c : in) {
        if (c == '=')
            break;
        if (T[c] == -1)
            return "";  // Non-base64 character
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

struct ModelDescriptor {
    std::string fileName;
    unsigned int byteSize;
    std::string fullPath;
    std::string displayName;
};

std::vector<std::string> acceptedModelExtensions = {".ckpt", ".gguf", ".ggml",
                                                    ".safetensors"};

std::vector<ModelDescriptor> g_server_found_models;

std::vector<ModelDescriptor> server_find_models(std::string starting_dir) {
    std::cout << "[Server] Finding models in " << starting_dir << std::endl;
    // recursively look for all model files
    std::vector<ModelDescriptor> models;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(
             starting_dir,
             std::filesystem::directory_options::skip_permission_denied)) {
        if (entry.is_regular_file()) {
            std::string path = entry.path().string();
            std::string ext  = entry.path().extension().string();
            // std::cout << "Found file: " << path << std::endl;
            if (std::find(acceptedModelExtensions.begin(),
                          acceptedModelExtensions.end(),
                          ext) != acceptedModelExtensions.end()) {
                ModelDescriptor model;
                model.fileName = entry.path().filename().string();
                model.byteSize = std::filesystem::file_size(entry.path());
                // absolute path
                model.fullPath = std::filesystem::absolute(entry.path()).string();
                model.displayName =
                    model.fileName + " (" + std::to_string(model.byteSize) + " bytes)";
                models.push_back(model);
                std::cout << "[Server] Found model: " << model.fullPath << std::endl;
            }
        }
    }
    return models;
}

#define RATE_LIMIT_CREATE(Name) std::time_t g_server_##Name = 0;
#define RATE_LIMIT_CHECK(Name, Return)                                          \
    {                                                                           \
        auto refreshTime = std::chrono::system_clock::now();                    \
        auto duration    = refreshTime.time_since_epoch();                      \
        std::time_t currentTime =                                               \
            std::chrono::duration_cast<std::chrono::seconds>(duration).count(); \
        if ((currentTime - g_server_##Name) < RATE_LIMIT) {                     \
            return Return;                                                      \
        }                                                                       \
        g_server_##Name = currentTime;                                          \
    }

RATE_LIMIT_CREATE(last_models_refresh_time);
bool server_reload_models(std::vector<std::string> files_dirs) {
    RATE_LIMIT_CHECK(last_models_refresh_time, false);
    g_server_found_models.clear();
    for (auto& dir : files_dirs) {
        auto models = server_find_models(dir);
        g_server_found_models.insert(g_server_found_models.end(), models.begin(),
                                     models.end());
    }
    std::cout << "[Server] Found " << g_server_found_models.size() << " models"
              << std::endl;
    return true;
}

/*
Payload looks like this:
prompt [prompt]
negative-prompt [negative_prompt]
cfg-scale [cfg_scale]
strength [denoising strength]
...
*/

struct ServerConfig {
    std::string mode;
    std::string model;
    std::string prompt;
    std::string negative_prompt;
    std::string cfg_scale;
    std::string denoising_strength;
    std::string style_ratio;
    std::string control_strength;
    std::string height;
    std::string width;
    std::string sampling_method;
    std::string steps;
    std::string seed;
    std::string batch_count;
};

struct Result {
    bool ok;
    ServerConfig config;
};

void server_dump_config(ServerConfig& config) {
    std::cout << "[SERVER] **" << config.mode << " request**" << std::endl;
    std::cout << "Model:\t" << config.model << std::endl;
    std::cout << "Prompt:\t" << config.prompt << std::endl;
    std::cout << "Negative Prompt:\t" << config.negative_prompt << std::endl;
    std::cout << "CFG Scale:\t" << config.cfg_scale << std::endl;
    std::cout << "Denoising Strength:\t" << config.denoising_strength
              << std::endl;
    std::cout << "Style Ratio:\t" << config.style_ratio << std::endl;
    std::cout << "Control Strength:\t" << config.control_strength << std::endl;
    std::cout << "Height:\t" << config.height << std::endl;
    std::cout << "Width:\t" << config.width << std::endl;
    std::cout << "Sampling Method:\t" << config.sampling_method << std::endl;
    std::cout << "Steps:\t" << config.steps << std::endl;
    std::cout << "Seed:\t" << config.seed << std::endl;
    std::cout << "Batch Count:\t" << config.batch_count << std::endl;
}

#define SERVER_CHECK_KEY_VALUE(KeyStr, ConfigKey) \
    if (key == KeyStr) {                          \
        if (config.ConfigKey.length())            \
            return {false};                       \
        config.ConfigKey = value;                 \
    }

Result parse_txt2img(std::string payload) {
    std::istringstream iss(payload);
    std::string token;

    ServerConfig config{"txt2img"};

    while (std::getline(iss, token, '\n')) {
        std::istringstream line(token);
        std::string key;
        std::string value;
        std::getline(line, key, ' ');
        std::getline(line, value);
        SERVER_CHECK_KEY_VALUE("model", model)
        SERVER_CHECK_KEY_VALUE("prompt", prompt)
        SERVER_CHECK_KEY_VALUE("negative-prompt", negative_prompt)
        SERVER_CHECK_KEY_VALUE("cfg-scale", cfg_scale)
        SERVER_CHECK_KEY_VALUE("style-ratio", style_ratio)
        SERVER_CHECK_KEY_VALUE("control-strength", control_strength)
        SERVER_CHECK_KEY_VALUE("height", height)
        SERVER_CHECK_KEY_VALUE("width", width)
        SERVER_CHECK_KEY_VALUE("sampling-method", sampling_method)
        SERVER_CHECK_KEY_VALUE("steps", steps)
        SERVER_CHECK_KEY_VALUE("seed", seed)
        SERVER_CHECK_KEY_VALUE("batch-count", batch_count)
    }

    // replace "\n" with real newline in prompt and negative prompt
    config.prompt = std::regex_replace(config.prompt, std::regex("\\\\n"), "\n");
    config.negative_prompt =
        std::regex_replace(config.negative_prompt, std::regex("\\\\n"), "\n");
    return {true, config};
}
Result parse_img2img(std::string payload) {
    std::istringstream iss(payload);
    std::string token;

    ServerConfig config{"txt2img"};

    while (std::getline(iss, token, '\n')) {
        std::istringstream line(token);
        std::string key;
        std::string value;
        std::getline(line, key, ' ');
        std::getline(line, value);
        SERVER_CHECK_KEY_VALUE("image", negative_prompt)
        SERVER_CHECK_KEY_VALUE("model", model)
        SERVER_CHECK_KEY_VALUE("prompt", prompt)
        SERVER_CHECK_KEY_VALUE("negative-prompt", negative_prompt)
        SERVER_CHECK_KEY_VALUE("cfg-scale", cfg_scale)
        SERVER_CHECK_KEY_VALUE("denoising-strength", denoising_strength)
        SERVER_CHECK_KEY_VALUE("style-ratio", style_ratio)
        SERVER_CHECK_KEY_VALUE("control-strength", control_strength)
        SERVER_CHECK_KEY_VALUE("height", height)
        SERVER_CHECK_KEY_VALUE("width", width)
        SERVER_CHECK_KEY_VALUE("sampling-method", sampling_method)
        SERVER_CHECK_KEY_VALUE("steps", steps)
        SERVER_CHECK_KEY_VALUE("seed", seed)
        SERVER_CHECK_KEY_VALUE("batch-count", batch_count)
    }

    // replace "\n" with real newline in prompt and negative prompt
    config.prompt = std::regex_replace(config.prompt, std::regex("\\\\n"), "\n");
    return {true, config};
}
#undef SERVER_CHECK_KEY_VALUE

std::string server_build_response_ok(std::string payload,
                                     std::string mimeType = "text/plain") {
    return "HTTP/1.1 200 OK\r\nContent-Type: " + mimeType +
           "\r\nConnection: close\r\n\r\n" + payload;
}

std::string server_build_response_error(std::string payload,
                                        std::string mimeType = "text/plain") {
    return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: " + mimeType +
           "\r\nConnection: close\r\n\r\n" + payload;
}

struct ServerJob {
    uint64_t id;  // timestamp
    ServerConfig config;
    std::string status;
    std::string result;
};
std::map<uint64_t, ServerJob> jobs;

volatile bool isBusy = false;

sd_ctx_t* sd_ctx = nullptr;
std::string server_active_model;

enum PrepareModelResult {
    PrepareRateLimit      = -2,
    PrepareNotFound       = -1,
    PrepareGeneralFailure = 0,
    PrepareOK             = 1
};

struct SDParams {
    int n_threads = -1;

    std::string model_path;
    std::string vae_path;
    std::string taesd_path;
    std::string esrgan_path;
    std::string controlnet_path;
    std::string embeddings_path;
    std::string stacked_id_embeddings_path;
    std::string input_id_images_path;
    sd_type_t wtype = SD_TYPE_COUNT;
    std::string lora_model_dir;
    std::string output_path = "output.png";
    std::string input_path;
    std::string control_image_path;

    std::string prompt;
    std::string negative_prompt;
    float min_cfg     = 1.0f;
    float cfg_scale   = 7.0f;
    float style_ratio = 20.f;
    int clip_skip     = -1;  // <= 0 represents unspecified
    int width         = 512;
    int height        = 512;
    int batch_count   = 1;

    int video_frames         = 6;
    int motion_bucket_id     = 127;
    int fps                  = 6;
    float augmentation_level = 0.f;

    // sample_method_t sample_method = EULER_A;
    schedule_t schedule    = DEFAULT;
    int sample_steps       = 20;
    float strength         = 0.75f;
    float control_strength = 0.9f;
    rng_type_t rng_type    = CUDA_RNG;
    int64_t seed           = 42;
    bool verbose           = false;
    bool vae_tiling        = false;
    bool control_net_cpu   = false;
    bool normalize_input   = false;
    bool clip_on_cpu       = false;
    bool vae_on_cpu        = false;
    bool canny_preprocess  = false;
    bool color             = false;
    int upscale_repeats    = 1;
};

RATE_LIMIT_CREATE(last_prepare_model_time);
int server_prepare_model(std::string modelFileName) {
    RATE_LIMIT_CHECK(last_prepare_model_time, -2);

    // Check that the model exists
    const ModelDescriptor* modelDesc = nullptr;
    for (auto& model : g_server_found_models) {
        if (model.fileName == modelFileName) {
            modelDesc = &model;
            break;
        }
    }

    if (modelDesc == nullptr) {
        return PrepareNotFound;
    }

    if ((modelDesc->fileName == server_active_model) && sd_ctx) {
        return PrepareOK;  // Already active
    }

    SDParams params;
    params.model_path = modelDesc->fullPath;

    bool vae_decode_only = true;

    if (sd_ctx) {
        free_sd_ctx(sd_ctx);
    }

    sd_ctx =
        new_sd_ctx(params.model_path.c_str(), params.vae_path.c_str(),
                   params.taesd_path.c_str(), params.controlnet_path.c_str(),
                   params.lora_model_dir.c_str(), params.embeddings_path.c_str(),
                   params.stacked_id_embeddings_path.c_str(), vae_decode_only,
                   params.vae_tiling, true, params.n_threads, params.wtype,
                   params.rng_type, params.schedule, params.clip_on_cpu,
                   params.control_net_cpu, params.vae_on_cpu);

    if (sd_ctx == NULL) {
        printf("[Server] new_sd_ctx_t failed\n");
        return PrepareGeneralFailure;
    }

    server_active_model = modelDesc->fileName;

    return true;
}

std::string raw_image_to_png_b64(int width, int height, unsigned char* data, int channels) {
    //      int stbi_write_png_to_func(stbi_write_func *func, void *context, int w, int h, int comp, const void  *data, int stride_in_bytes);
    std::vector<unsigned char> png_data;
    stbi_write_func* write_func = [](void* context, void* data, int size) {
        auto* png_data = (std::vector<unsigned char>*)context;
        png_data->insert(png_data->end(), (unsigned char*)data, (unsigned char*)data + size);
    };
    stbi_write_png_to_func(write_func, &png_data, width, height, channels, data, width * channels);
    return base64_encode(std::string(png_data.begin(), png_data.end()));
}

int run_sdci_txt2img(uint64_t jobId, ServerConfig* configPtr) {
    std::cout << "Running sdci_txt2img... jobId: " << jobId << std::endl;
    isBusy               = true;
    ServerConfig& params = *configPtr;
    if (!server_prepare_model(params.model)) {
        jobs[jobId].status = "ERROR";
        jobs[jobId].result = "Failed to prepare model";
        isBusy             = false;
        return 1;
    }
    // std::string cmd = "sd.exe";
    // cmd += " --mode txt2img";
    // cmd += " --model " + config.model;
    // cmd += " --prompt \"" + config.prompt + "\"";
    // cmd += " --negative-prompt \"" + config.negative_prompt + "\"";
    // cmd += " --cfg-scale " + config.cfg_scale;
    // cmd += " --strength " + config.denoising_strength;
    // cmd += " --style-ratio " + config.style_ratio;
    // cmd += " --control-strength " + config.control_strength;
    // cmd += " --height " + config.height;
    // cmd += " --width " + config.width;
    // cmd += " --sampling-method " + config.sampling_method;
    // cmd += " --steps " + config.steps;
    // cmd += " --seed " + config.seed;
    // cmd += " --batch-count " + config.batch_count;
    // std::cout << "Command: " << cmd << std::endl;
    // std::string result = exec(cmd.c_str());
    // std::cout << "Result: " << result << std::endl;

    sd_image_t* control_image = NULL;

    sample_method_t samplingMethod = EULER_A;
    if (params.sampling_method == "EULER_A")
        samplingMethod = EULER_A;
    if (params.sampling_method == "EULER")
        samplingMethod = EULER;
    if (params.sampling_method == "HEUN")
        samplingMethod = HEUN;
    if (params.sampling_method == "DPM2")
        samplingMethod = DPM2;
    if (params.sampling_method == "DPMPP2S_A")
        samplingMethod = DPMPP2S_A;
    if (params.sampling_method == "DPMPP2M")
        samplingMethod = DPMPP2M;
    if (params.sampling_method == "DPMPP2Mv2")
        samplingMethod = DPMPP2Mv2;
    if (params.sampling_method == "LCM")
        samplingMethod = LCM;

    int batch_count = std::stoi(params.batch_count);

    auto results = txt2img(sd_ctx,
                           params.prompt.c_str(),
                           params.negative_prompt.c_str(),
                           0,  // params.clip_skip
                           std::stof(params.cfg_scale),
                           std::stoi(params.width),
                           std::stoi(params.height),
                           samplingMethod,
                           std::stoi(params.steps),
                           std::stoull(params.seed),
                           batch_count,
                           control_image,
                           std::stof(params.control_strength),
                           std::stof(params.style_ratio),
                           0,  // params.normalize_input
                           ""  // params.input_id_images_path.c_str()
    );
    if (results == NULL) {
        jobs[jobId].status = "ERROR";
        jobs[jobId].result = "Generation failed";
    }

    std::vector<std::string> outResults;

    for (int i = 0; i < batch_count; i++) {
        if (results[i].data == NULL) {
            continue;
        }
        sd_image_t current_image = results[i];
        std::string b64          = raw_image_to_png_b64(current_image.width, current_image.height, current_image.data, current_image.channel);
        outResults.push_back(b64);
    }

    jobs[jobId].status = "SUCCESS";
    jobs[jobId].result = outResults.size() + "\n" + join(outResults, "\n");
    isBusy             = false;
    return 0;
}

int run_sdci_img2img(uint64_t jobId, ServerConfig* configPtr) {
    std::cout << "Running sdci_img2img... jobId: " << jobId << std::endl;
    isBusy               = true;
    ServerConfig& config = *configPtr;
    if (!server_prepare_model(config.model)) {
        jobs[jobId].status = "ERROR";
        jobs[jobId].result = "Failed to prepare model";
        isBusy             = false;
        return 1;
    }
    std::string cmd = "sd.exe";
    cmd += " --mode img2img";
    cmd += " --model " + config.model;
    cmd += " --image " + config.prompt;
    cmd += " --prompt \"" + config.prompt + "\"";
    cmd += " --negative-prompt \"" + config.negative_prompt + "\"";
    cmd += " --cfg-scale " + config.cfg_scale;
    cmd += " --denoising-strength " + config.denoising_strength;
    cmd += " --style-ratio " + config.style_ratio;
    cmd += " --control-strength " + config.control_strength;
    cmd += " --height " + config.height;
    cmd += " --width " + config.width;
    cmd += " --sampling-method " + config.sampling_method;
    cmd += " --steps " + config.steps;
    cmd += " --seed " + config.seed;
    cmd += " --batch-count " + config.batch_count;
    std::cout << "Command: " << cmd << std::endl;
    // std::string result = exec(cmd.c_str());
    // std::cout << "Result: " << result << std::endl;
    jobs[jobId].status = "SUCCESS";
    jobs[jobId].result = "(Mock result)";
    isBusy             = false;
    return 0;
}

uint64_t server_queue_txt2img(ServerConfig config) {
    auto currentTime = std::chrono::system_clock::now();
    auto duration    = currentTime.time_since_epoch();
    uint64_t id =
        std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    jobs[id] = {id, config, "PENDING"};
    id++;
    std::future<int> promise =
        std::async(std::launch::async, run_sdci_txt2img, id, &config);
    return id;
}

std::string handle_text2img(std::string payload) {
    Result result = parse_txt2img(payload);
    if (result.ok) {
        if (isBusy) {
            return server_build_response_ok("BUSY\nPlease try again later.");
        }
        uint64_t jobId = server_queue_txt2img(result.config);
        std::cout << "Returning OK... jobId: " << jobId << "\n";
        return server_build_response_ok("OK\n" + std::to_string(jobId));
    } else {
        return server_build_response_error("Error");
    }
}

std::string handle_img2img(std::string payload) {
    Result result = parse_img2img(payload);
    if (result.ok) {
        if (isBusy) {
            return server_build_response_ok("BUSY\nPlease try again later.");
        }
        uint64_t jobId = server_queue_txt2img(result.config);
        std::cout << "Returning OK... jobId: " << jobId << "\n";
        return server_build_response_ok("OK\n" + std::to_string(jobId));
    } else {
        return server_build_response_error("Error");
    }
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    std::memset(buffer, 0, BUFFER_SIZE);

    // Read the request
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received == SOCKET_ERROR) {
        std::cerr << "Failed to read from socket\n";
        close_socket(client_socket);
        return;
    }

    std::string request(buffer);
    // Debug
    std::cout << "Received request:\n----\n"
              << request << "\n----" << std::endl;

    // Extract the HTTP method and path
    std::istringstream request_stream(request);
    std::string method, path;
    request_stream >> method >> path;

    if (method.empty() || path.empty()) {
        std::cerr << "Invalid request\n";
        close_socket(client_socket);
        return;
    }

    std::string response;

    if (method == "GET") {
        if ((path == "/") || path.starts_with("/?")) {
            response = server_build_response_ok(html_bundle, "text/html");
        } else if (path == "/api/v0/models") {
            std::string models;
            for (auto& model : g_server_found_models) {
                models += std::to_string(model.byteSize) + " " + model.fileName + "\n";
            }
            response = server_build_response_ok(models);
        } else if (path == "/api/v0/models/refresh") {
            if (server_reload_models({"."})) {
                response = server_build_response_ok("OK");
            } else {
                response = server_build_response_error(
                    "BUSY\nRate limite exceeded. Please try again later.");
            }
        } else if (path.starts_with("/api/v0/prepare/")) {
            std::string modelPath = path.substr(14);
            // replace %20 with space
            modelPath          = std::regex_replace(modelPath, std::regex("%20"), " ");
            auto prepareResult = server_prepare_model(modelPath);
            if (prepareResult == 1) {
                response = server_build_response_ok("OK");
            } else if (prepareResult == -2) {
                response = server_build_response_error(
                    "BUSY\nRate limite exceeded. Please try again later.");
            } else {
                response = server_build_response_error("500 Internal Server Error");
            }
        } else if (path.starts_with("/api/v0/check/")) {
            std::string id_str = path.substr(14);
            std::cout << "Checking job id: " << id_str << std::endl;
            uint64_t id = std::stoull(id_str);
            if (jobs.find(id) != jobs.end()) {
                auto job = jobs[id];
                response = server_build_response_ok(job.status + "\n" + job.result);
            } else {
                response = server_build_response_error("404 Not Found");
            }
        } else {
            response = server_build_response_error("404 Not Found");
        }
    } else if (method == "POST") {
        std::string body = request.substr(request.find("\r\n\r\n") + 4);
        if (body.empty()) {
            response = server_build_response_error("400 Bad Request");
        } else {
            // std::cout << "Received POST data: [" << body << "]" << std::endl;
            if (path == "/api/v0/txt2img") {
                response = handle_text2img(body);
            } else if (path == "/api/v0/img2img") {
                response = handle_img2img(body);
            } else {
                response = server_build_response_error("404 Not Found");
            }
        }
    } else {
        response = server_build_response_error("405 Method Not Allowed");
    }

    std::cout << "Response:\n"
              << response << std::endl;
    send(client_socket, response.c_str(), response.length(), 0);
    close_socket(client_socket);
}

int server_start(int port, std::vector<std::string> files_dirs) {
    server_reload_models(files_dirs);
#ifdef _WIN32
    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << "\n";
        return 1;
    }
#endif

    // Create a socket
    socket_t server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket: " << socket_error() << "\n";
        WSACleanup();
        return 1;
    }

    // Bind the socket to an IP / port
    sockaddr_in server_address;
    server_address.sin_family      = AF_INET;
    server_address.sin_port        = htons(port == 0 ? DEFAULT_PORT : port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_address,
             sizeof(server_address)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind to port: " << socket_error() << "\n";
        close_socket(server_socket);
        WSACleanup();
        return 1;
    }

    // Listen for connections
    if (listen(server_socket, 10) == SOCKET_ERROR) {
        std::cerr << "Failed to listen on socket: " << socket_error() << "\n";
        close_socket(server_socket);
        WSACleanup();
        return 1;
    }
    std::cout << "[Server] Server is listening on port " << port << std::endl;

    // Accept connections
    while (true) {
        sockaddr_in client_address;
        socklen_t client_size = sizeof(client_address);
        socket_t client_socket =
            accept(server_socket, (struct sockaddr*)&client_address, &client_size);
        if (client_socket == INVALID_SOCKET) {
            std::cerr << "Failed to accept connection: " << socket_error() << "\n";
            continue;
        }

        // Handle the client in a separate function
        handle_client(client_socket);
    }

    // Close the server socket
    close_socket(server_socket);
    WSACleanup();
    return 0;
}

void server_show_help() {
    std::cout << "Usage: server.exe [--port PORT] [--filesDir DIR1] [--filesDir "
                 "DIR2] ...\n";
}

// accept two cli args: --port and many --filesDir's
int main(int argc, char* argv[]) {
    std::vector<std::string> searchable_dirs;
    int port = 0;
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--port") {
            if (i + 1 < argc) {
                port = std::stoi(argv[i + 1]);
                i++;
            }
        } else if (std::string(argv[i]) == "--filesDir") {
            if (i + 1 < argc) {
                searchable_dirs.push_back(argv[i + 1]);
                i++;
            }
        } else {
            server_show_help();
            return 1;
        }
    }
    if (searchable_dirs.empty()) {
        searchable_dirs.push_back(".");
    }
    return server_start(port, searchable_dirs);
}

/*
API doc
GET /
  - Return the index.html, a web interface for the server.
GET /api/v0/models
  - Return a list of models available on the server. Each line is a model
descriptor with the format "{byteSize} {fileName}"
GET /api/v0/models/refresh
  - Refresh the list of models available on the server. Return `OK\n` on
success. Otherwise a 500 with error message as body.
GET /api/v0/check/{id}
  - Returns the status of the job with id.
  - Return `PENDING\n` or `SUCCESS\n{result}` on success. Result is base64
encoded image data. Otherwise a 500 with error message as body. POST
/api/v0/txt2img
  - payload: txt2img payload. A sequence of lines with "[key] [value]" similar
to stable-diffusion.cpp CLI arguments
  - return: `OK\n{jobId}` or `BUSY\n...error message...` on success ; otherwise
a 500 with error message as body
POST /api/v0/img2img
  - payload: img2img payload. A sequence of lines with "[key] [value]" similar
to stable-diffusion.cpp CLI arguments
  - return: `OK\n{jobId}` or `BUSY\n...error message...` on success ; otherwise
*/