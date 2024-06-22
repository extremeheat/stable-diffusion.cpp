#pragma once

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
