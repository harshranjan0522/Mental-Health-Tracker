#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <string>

class Encryption {
public:
    // Simple XOR + hex "encryption" (demo only, not real AES security)
    static std::string encrypt(const std::string &plain, const std::string &key);
    static std::string decrypt(const std::string &cipherHex, const std::string &key);
};

#endif
