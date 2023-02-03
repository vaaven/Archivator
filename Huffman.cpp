#include "Huffman.h"
#include "MyPriorityQueue.h"

HuffmanTree::HuffmanTree(const std::vector<bool>& data, size_t num) : data_(data), num_(num) {
}

HuffmanTree* Merge(const HuffmanTree* a, const HuffmanTree* b) {
    HuffmanTree* nw = new HuffmanTree(std::min(a->data_, b->data_), a->num_ + b->num_);
    nw->go_[0] = a;
    nw->go_[1] = b;
    return nw;
}

bool HuffmanTree::operator<(const HuffmanTree& other) const {
    if (num_ == other.num_) {
        return data_ < other.data_;
    }
    return num_ < other.num_;
}

struct MyComparator {
    bool operator()(const HuffmanTree* lhs, const HuffmanTree* rhs) const {
        return *lhs < *rhs;
    };
};

HuffmanTree* HuffmanTree::Build(const std::unordered_map<std::vector<bool>, size_t>& count) {
    PriorityQueue<HuffmanTree*, MyComparator> q;
    for (const auto& [bits, num] : count) {
        q.Push(new HuffmanTree(bits, num));
    }
    while (q.Size() > 1) {
        auto a = q.Pop();
        auto b = q.Pop();
        q.Push(Merge(a, b));
    }
    return q.Top();
}

const std::vector<bool>& HuffmanTree::GetData() const {
    return data_;
}

HuffmanTree::~HuffmanTree() {
    delete go_[0];
    delete go_[1];
}
