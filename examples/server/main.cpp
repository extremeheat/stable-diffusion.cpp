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

// Stable Diffusion
// #include "preprocessing.hpp"
#include "stable-diffusion.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"

// #define DEBUG_LOG(...) std::cout << __VA_ARGS__ << std::endl
#define DEBUG_LOG(...)

#define REPLACE(STR, FROM, TO) STR = std::regex_replace(STR, std::regex(FROM), TO);

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
// / Stable Diffusion

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
    std::string image;  // for img2img
    float cfg_scale;
    float denoising_strength;
    float style_ratio;
    float control_strength;
    int height;
    int width;
    sample_method_t sampling_method;
    int steps;
    int64_t seed;
    int batch_count;
};

struct Result {
    bool ok;
    std::string message;
    ServerConfig config;
    bool wasRateLimited;
};

void server_dump_config(ServerConfig& config) {
    std::cout << "[SERVER] **" << config.mode << " request**" << std::endl;
    std::cout << "model: " << config.model << std::endl;
    std::cout << "prompt: " << config.prompt << std::endl;
    std::cout << "negative-prompt: " << config.negative_prompt << std::endl;
    std::cout << "cfg-scale: " << config.cfg_scale << std::endl;
    std::cout << "denoising-strength: " << config.denoising_strength << std::endl;
    std::cout << "style-ratio: " << config.style_ratio << std::endl;
    std::cout << "control-strength: " << config.control_strength << std::endl;
    std::cout << "height: " << config.height << std::endl;
    std::cout << "width: " << config.width << std::endl;
    std::cout << "sampling-method: " << config.sampling_method << std::endl;
    std::cout << "steps: " << config.steps << std::endl;
    std::cout << "seed: " << config.seed << std::endl;
}

#define CHECK_REQ_ARG(KeyStr, ConfigKey, PP)                        \
    if (key == KeyStr) {                                            \
        if (std::find(seen.begin(), seen.end(), key) != seen.end()) \
            return {false, "Bad Input"};                            \
        seen.push_back(key);                                        \
        config.ConfigKey = PP(value);                               \
    }

Result parse_payload(std::string& payload, ServerConfig& config) {
    std::istringstream iss(payload);
    std::string token;

    std::vector<std::string> seen;

    while (std::getline(iss, token, '\n')) {
        std::istringstream line(token);
        std::string key;
        std::string value;
        std::getline(line, key, ' ');
        std::getline(line, value);
        CHECK_REQ_ARG("image", image, std::string)
        CHECK_REQ_ARG("model", model, std::string)
        CHECK_REQ_ARG("prompt", prompt, std::string)
        CHECK_REQ_ARG("negative-prompt", negative_prompt, std::string)
        CHECK_REQ_ARG("cfg-scale", cfg_scale, std::stof)
        CHECK_REQ_ARG("style-ratio", style_ratio, std::stof)
        CHECK_REQ_ARG("denoising-strength", denoising_strength, std::stof)  // i2i
        CHECK_REQ_ARG("control-strength", control_strength, std::stof)
        CHECK_REQ_ARG("height", height, std::stoi)
        CHECK_REQ_ARG("width", width, std::stoi)
        CHECK_REQ_ARG("steps", steps, std::stoi)
        CHECK_REQ_ARG("seed", seed, std::stoull)
        CHECK_REQ_ARG("batch-count", batch_count, std::stoi)

        if (key == "sampling-method") {
            config.sampling_method = EULER_A;
            // clang-format off
            if (value == "EULER_A") config.sampling_method = EULER_A;
            if (value == "EULER") config.sampling_method = EULER;
            if (value == "HEUN") config.sampling_method = HEUN;
            if (value == "DPM2") config.sampling_method = DPM2;
            if (value == "DPMPP2S_A") config.sampling_method = DPMPP2S_A;
            if (value == "DPMPP2M") config.sampling_method = DPMPP2M;
            if (value == "DPMPP2Mv2") config.sampling_method = DPMPP2Mv2;
            if (value == "LCM") config.sampling_method = LCM;
            // clang-format on
        }
    }

    // replace "\n" with real newline in prompt and negative prompt
    REPLACE(config.prompt, "\\\\n", "\n");
    REPLACE(config.negative_prompt, "\\\\n", "\n");
    return {true, "OK", config};
}

Result parse_txt2img(std::string payload) {
    ServerConfig config{"txt2img"};
    auto ret = parse_payload(payload, config);
    if (!config.prompt.length())
        return {false, "Missing prompt"};
    return ret;
}

Result parse_img2img(std::string payload) {
    ServerConfig config{"txt2img"};
    auto ret = parse_payload(payload, config);
    if (!config.image.length())
        return {false, "Missing image"};
    if (!config.prompt.length())
        return {false, "Missing prompt"};
    return ret;
}

#undef CHECK_REQ_ARG

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
Result server_prepare_model(std::string modelFileName, bool ignoreBusy = false) {
    // return {true, "Model is already set"};
    if ((modelFileName == server_active_model) && (sd_ctx != NULL)) {
        DEBUG_LOG("[Server] Model is already set\n");
        return {true, "Model is already set"};
    }

    RATE_LIMIT_CHECK(last_prepare_model_time, (Result{false, "Rate limited", {}, true}));
    if (isBusy && !ignoreBusy) {
        return {false, "Server is busy", {}, true};
    }

    // Check that the model exists
    const ModelDescriptor* modelDesc = nullptr;
    for (auto& model : g_server_found_models) {
        // DEBUG_LOG("Comparing model '%s' with '%s'\n", model.fileName.c_str(), modelFileName.c_str());
        if (model.fileName == modelFileName) {
            modelDesc = &model;
            break;
        }
    }

    if (modelDesc == nullptr) {
        printf("[Server] Model not found: %s\n", modelFileName.c_str());
        return {false, "Model not found"};
    }

    printf("[Server] Preparing model: %s\n", modelDesc->fullPath.c_str());

    SDParams params;
    params.model_path = modelDesc->fullPath;

    bool vae_decode_only = true;

    if (sd_ctx) {  // free the old context
        free_sd_ctx(sd_ctx);
    }

    sd_ctx =
        new_sd_ctx(params.model_path.c_str(), params.vae_path.c_str(),
                   params.taesd_path.c_str(), params.controlnet_path.c_str(),
                   params.lora_model_dir.c_str(), params.embeddings_path.c_str(),
                   params.stacked_id_embeddings_path.c_str(), vae_decode_only,
                   params.vae_tiling, false, params.n_threads, params.wtype,
                   params.rng_type, params.schedule, params.clip_on_cpu,
                   params.control_net_cpu, params.vae_on_cpu);

    if (sd_ctx == NULL) {
        printf("[Server] new_sd_ctx_t failed\n");
        return {false, "Failed to create new Stable Diffusion context"};
    }

    printf("[Server] Model prepared: %s\n", modelDesc->fullPath.c_str());

    server_active_model = modelDesc->fileName;

    return {true};
}

std::string raw_image_to_png_b64(int width, int height, unsigned char* data, int channels) {
    //      int stbi_write_png_to_func(stbi_write_func *func, void *context, int
    //      w, int h, int comp, const void  *data, int stride_in_bytes);
    std::vector<unsigned char> png_data;
    stbi_write_func* write_func = [](void* context, void* data, int size) {
        auto* png_data = (std::vector<unsigned char>*)context;
        png_data->insert(png_data->end(), (unsigned char*)data,
                         (unsigned char*)data + size);
    };
    stbi_write_png_to_func(write_func, &png_data, width, height, channels, data,
                           width * channels);
    return base64_encode(std::string(png_data.begin(), png_data.end()));
}

int run_sdci_txt2img(uint64_t jobId, ServerConfig params) {
    std::cout << "[Server] Running txt2img, under jobId: " << jobId << std::endl;
    isBusy = true;
    auto prepareResult = server_prepare_model(params.model, true);
    if (!prepareResult.ok) {
        printf("[Server] Failed to prepare model '%s': %s\n", params.model.c_str(),
               prepareResult.message.c_str());
        jobs[jobId].status = prepareResult.wasRateLimited ? "BUSY" : "ERROR";
        jobs[jobId].result = "Failed to prepare model '" + params.model + "': " + prepareResult.message;
        isBusy             = false;
        return 1;
    }

    sd_image_t* control_image = NULL;

    int batch_count = params.batch_count;

    printf("[Server] Running txt2img with %d batch count\n", batch_count);

    auto results =
        txt2img(sd_ctx, params.prompt.c_str(), params.negative_prompt.c_str(),
                0,  // params.clip_skip
                params.cfg_scale, params.width,
                params.height, params.sampling_method, params.steps,
                params.seed, batch_count, control_image,
                params.control_strength, params.style_ratio,
                0,  // params.normalize_input
                ""  // params.input_id_images_path.c_str()
        );

    if (results == NULL) {
        jobs[jobId].status = "ERROR";
        jobs[jobId].result = "Generation failed";
        isBusy             = false;
        return 1;
    }

    std::vector<std::string> outResults;

    for (int i = 0; i < batch_count; i++) {
        if (results[i].data == NULL) {
            continue;
        }
        sd_image_t current_image = results[i];
        std::string b64 =
            raw_image_to_png_b64(current_image.width, current_image.height,
                                 current_image.data, current_image.channel);
        outResults.push_back(b64);
        free(current_image.data);
    }

    jobs[jobId].status = "SUCCESS";
    jobs[jobId].result = outResults.size() + "\n" + join(outResults, "\n");
    free(results);
    isBusy = false;
    return 0;
}

int run_sdci_img2img(uint64_t jobId, ServerConfig config) {
    std::cout << "[Server] Running img2img under jobId: " << jobId << std::endl;
    isBusy               = true;
    auto prepareResult   = server_prepare_model(config.model, true);
    if (!prepareResult.ok) {
        jobs[jobId].status = prepareResult.wasRateLimited ? "BUSY" : "ERROR";
        jobs[jobId].result = "Failed to prepare model '" + config.model + "': " + prepareResult.message;
        isBusy             = false;
        return 1;
    }

    std::string image = base64_decode(config.image);
    int width, height, channels;
    unsigned char* image_data =
        stbi_load_from_memory((unsigned char*)image.c_str(), image.length(),
                              &width, &height, &channels, 0);

    if (image_data == NULL) {
        jobs[jobId].status = "ERROR";
        jobs[jobId].result =
            "Failed to decode image: " + std::string(stbi_failure_reason());
        isBusy = false;
        return 1;
    }
    if ((width <= 0) || (height <= 0) || (channels < 0)) {
        jobs[jobId].status = "ERROR";
        jobs[jobId].result = "Invalid image dimensions";
        stbi_image_free(image_data);
        isBusy = false;
        return 1;
    }

    // Resize the image to fit the desired dimensions
    uint8_t* resized_image_buffer =
        (uint8_t*)malloc(config.height * config.width * 3);
    if (resized_image_buffer == NULL) {
        fprintf(stderr, "error: allocate memory for resize input image\n");
        stbi_image_free(image_data);
        return 1;
    }

    stbir_resize(image_data, width, height, 0, resized_image_buffer, config.width,
                 config.height, 0, STBIR_TYPE_UINT8, 3 /*RGB channel*/,
                 STBIR_ALPHA_CHANNEL_NONE, 0, STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP,
                 STBIR_FILTER_BOX, STBIR_FILTER_BOX, STBIR_COLORSPACE_SRGB,
                 nullptr);

    stbi_image_free(image_data);

    sd_image_t input_image;
    input_image.width   = width;
    input_image.height  = height;
    input_image.channel = channels;
    input_image.data    = image_data;

    auto results = img2img(
        sd_ctx, input_image, config.prompt.c_str(),
        config.negative_prompt.c_str(), 0,  // clip_skip
        config.cfg_scale, config.width, config.height, config.sampling_method,
        config.steps, config.denoising_strength, config.seed, config.batch_count,
        nullptr, config.control_strength, config.style_ratio, 0,
        "");  // normalize input, input_id_images_path

    if (results == NULL) {
        jobs[jobId].status = "ERROR";
        jobs[jobId].result = "Generation failed";
        isBusy             = false;
        return 1;
    }

    std::vector<std::string> outResults;

    for (int i = 0; i < config.batch_count; i++) {
        if (results[i].data == NULL) {
            continue;
        }
        sd_image_t current_image = results[i];
        std::string b64 =
            raw_image_to_png_b64(current_image.width, current_image.height,
                                 current_image.data, current_image.channel);
        outResults.push_back(b64);
        free(current_image.data);
    }

    jobs[jobId].status = "SUCCESS";
    jobs[jobId].result = outResults.size() + "\n" + join(outResults, "\n");
    free(results);
    isBusy = false;
    return 0;
}

uint64_t server_queue_txt2img(ServerConfig config) {
    auto currentTime = std::chrono::system_clock::now();
    auto duration    = currentTime.time_since_epoch();
    uint64_t id =
        std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    jobs[id] = {id, config, "PENDING"};
    // spawn a thread to run the job
    std::thread(run_sdci_txt2img, id, config).detach();
    return id;
}

uint64_t server_queue_img2img(ServerConfig config) {
    auto currentTime = std::chrono::system_clock::now();
    auto duration    = currentTime.time_since_epoch();
    uint64_t id =
        std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    jobs[id] = {id, config, "PENDING"};
    // spawn a thread to run the job
    std::thread(run_sdci_img2img, id, config).detach();
    return id;
}

std::string handle_text2img(std::string payload) {
    Result result = parse_txt2img(payload);
    if (result.ok) {
        if (isBusy) {
            return server_build_response_ok("BUSY\nPlease try again later.");
        }
        uint64_t jobId = server_queue_txt2img(result.config);
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
        uint64_t jobId = server_queue_img2img(result.config);
        return server_build_response_ok("OK\n" + std::to_string(jobId));
    } else {
        return server_build_response_error("Error");
    }
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    std::memset(buffer, 0, BUFFER_SIZE);

    // set timeout
    struct timeval tv;
    tv.tv_sec  = 4;
    tv.tv_usec = 0;
    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv,
               sizeof tv);

    // Read the request
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received == SOCKET_ERROR) {
        std::cerr << "Failed to read from socket\n";
        close_socket(client_socket);
        return;
    }

    std::string request(buffer);
    // Debug
    DEBUG_LOG("Received request (" << bytes_received << " bytes):\n----\n"
                                   << request << "\n----");

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
            std::string models = "OK\n";
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
            std::string modelPath = path.substr(16);
            // replace %20 with space
            modelPath          = std::regex_replace(modelPath, std::regex("%20"), " ");
            auto prepareResult = server_prepare_model(modelPath);
            if (prepareResult.ok) {
                response = server_build_response_ok("OK\n" + prepareResult.message);
            } else {
                response = server_build_response_error("500 Internal Server Error\n" +
                                                       prepareResult.message);
            }
        } else if (path.starts_with("/api/v0/check/")) {
            std::string id_str = path.substr(14);
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
            printf("[Server] No POST data received for request '%s'\n", path.c_str());
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

    DEBUG_LOG("Sending response (" << response.length() << " bytes):\n----\n"
                                   << response << "\n----");
    int sendResult = send(client_socket, response.c_str(), response.length(), 0);
    if (sendResult == SOCKET_ERROR) {
        std::cerr << "Failed to send response to client\n";
    } else {
        DEBUG_LOG("Response sent successfully: " << sendResult);
    }
    close_socket(client_socket);
}

int server_start(int port, std::vector<std::string> files_dirs) {
    server_reload_models(files_dirs);
    // Replace placeholders in the HTML bundle
    REPLACE(html_bundle, "window.API_HOST", "''");
    REPLACE(html_bundle, "window.API_PATH", "'/api/v0'");
    // resolve the files_dirs
    std::vector<std::string> resolved_dirs;
    for (auto& dir : files_dirs) {
        resolved_dirs.push_back(std::filesystem::absolute(dir).string());
    }
    std::string files_dirs_str = join(resolved_dirs, "*");
    REPLACE(files_dirs_str, "\\\\", "/");
    REPLACE(html_bundle, "window.FILES_DIR", "'" + files_dirs_str + "'");

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

    int finalPort = port == 0 ? DEFAULT_PORT : port;

    // Bind the socket to an IP / port
    sockaddr_in server_address;
    server_address.sin_family      = AF_INET;
    server_address.sin_port        = htons(finalPort);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_address,
             sizeof(server_address)) == SOCKET_ERROR) {
        fprintf(stderr, "Failed to bind to port %d. Error %d\n", finalPort, socket_error());
        close_socket(server_socket);
        WSACleanup();
        return 1;
    }

    // Listen for connections
    if (listen(server_socket, 10) == SOCKET_ERROR) {
        std::cerr << "Failed to listen on socket. Error " << socket_error() << "\n";
        close_socket(server_socket);
        WSACleanup();
        return 1;
    }
    std::cout << "[Server] Server is listening on port " << finalPort << std::endl;

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