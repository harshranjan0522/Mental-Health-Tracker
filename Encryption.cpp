#include "Encryption.h"
#include <sstream>
#include <iomanip>

static std::string xorData(const std::string &data, const std::string &key) {
    if (key.empty()) return data;
    std::string out = data;
    for (size_t i = 0; i < data.size(); ++i) {
        out[i] = data[i] ^ key[i % key.size()];
    }
    return out;
}

static std::string toHex(const std::string &data) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (unsigned char c : data) {
        oss << std::setw(2) << static_cast<int>(c);
    }
    return oss.str();
}

static std::string fromHex(const std::string &hex) {
    std::string out;
    out.reserve(hex.size() / 2);
    for (size_t i = 0; i + 1 < hex.size(); i += 2) {
        std::string byteStr = hex.substr(i, 2);
        char byte = static_cast<char>(std::strtol(byteStr.c_str(), nullptr, 16));
        out.push_back(byte);
    }
    return out;
}

std::string Encryption::encrypt(const std::string &plain, const std::string &key) {
    return toHex(xorData(plain, key));
}

std::string Encryption::decrypt(const std::string &cipherHex, const std::string &key) {
    std::string bin = fromHex(cipherHex);
    return xorData(bin, key);
}
