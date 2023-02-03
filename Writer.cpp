#include "Writer.h"

Writer::Writer(std::ostream& stream) : buffer_(0), bits_(0), stream_(stream) {
}

void Writer::WriteBits(std::vector<bool> data) {
    for (bool bit : data) {
        buffer_ |= (bit << (BYTE_SIZE - 1 - bits_));
        ++bits_;
        if (bits_ == BYTE_SIZE) {
            stream_.put(buffer_);
            bits_ = 0;
            buffer_ = 0;
        }
    }
}

void Writer::WriteNumber(size_t data, size_t bits) {
    std::vector<bool> num(bits, false);
    for (size_t i = 0; i < bits; i++) {
        num[i] = data & (1 << (bits - i - 1));
    }
    WriteBits(num);
}

void Writer::Close() {
    if (bits_) {
        stream_ << buffer_;
    }
    stream_.flush();
}
