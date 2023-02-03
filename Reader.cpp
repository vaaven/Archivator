#include "Reader.h"
#include <iostream>

Reader::Reader(std::istream& stream) : buffer_(0), bits_(0), stream_(stream), eof_(false) {
}

bool Reader::Eof() const {
    return eof_;
}

void Reader::ResetStream() {
    buffer_ = 0;
    bits_ = 0;
    eof_ = false;
    stream_.clear();
    stream_.seekg(0);
}

std::vector<bool> Reader::ReadBits(const size_t& count) {
    std::vector<bool> answer(count, false);
    for (size_t i = 0; i < count; ++i) {
        if (!bits_) {
            buffer_ = 0;
            bits_ = BYTE_SIZE;
            stream_.get(reinterpret_cast<char&>(buffer_));
            if (stream_.eof()) {
                eof_ = true;
                buffer_ = 0;
            }
        }
        answer[i] = (buffer_ >> (bits_ - 1)) & 1;
        --bits_;
    }
    return answer;
}

bool Reader::ReadOneBit() {
    if (!bits_) {
        buffer_ = 0;
        bits_ = BYTE_SIZE;
        stream_.get(reinterpret_cast<char&>(buffer_));
        if (stream_.eof()) {
            eof_ = true;
            buffer_ = 0;
        }
    }
    --bits_;
    return ((buffer_ >> (bits_)) & 1);
}
