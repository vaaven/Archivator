#pragma once
#include <cstdint>
#include <istream>
#include <vector>

class Reader {
public:
    static const size_t BYTE_SIZE = 8;
    explicit Reader(std::istream& stream);
    std::vector<bool> ReadBits(const size_t& count);
    bool ReadOneBit();
    void ResetStream();
    bool Eof() const;

private:
    uint8_t buffer_;
    uint8_t bits_;
    std::istream& stream_;
    bool eof_;
};
