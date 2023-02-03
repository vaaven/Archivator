#pragma once
#include <vector>
#include <cstdint>
#include <iostream>

class Writer {
public:
    static const size_t BYTE_SIZE = 8;
    explicit Writer(std::ostream& stream);
    void WriteBits(std::vector<bool> data);
    void WriteNumber(size_t data, size_t bits);
    void Close();

private:
    uint8_t buffer_;
    uint8_t bits_;
    std::ostream& stream_;
};
