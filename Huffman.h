#pragma once
#include <vector>
#include <unordered_map>
#include <queue>

class HuffmanTree {
private:
    const std::vector<bool>& data_;
    size_t num_;

public:
    HuffmanTree(const std::vector<bool>& data, size_t num);

    friend HuffmanTree* Merge(const HuffmanTree* a, const HuffmanTree* b);
    static HuffmanTree* Build(const std::unordered_map<std::vector<bool>, size_t>& count);
    const std::vector<bool>& GetData() const;
    bool operator<(const HuffmanTree& other) const;
    const HuffmanTree* go_[2] = {nullptr, nullptr};

    ~HuffmanTree();
};
