#include <functional>
#include <vector>

template <typename T, typename K = std::less<T>>
class PriorityQueue {
    class EmptyQueueException : std::exception {};

public:
    T Top() const {
        return heap_[0];
    }

    void Heapify(size_t vertex) {
        if (heap_.empty()) {
            return;
        }
        size_t left = vertex * 2 + 1;
        size_t right = vertex * 2 + 2;
        size_t largest = vertex;
        if (left < heap_.size() && K()(heap_[left], heap_[largest])) {
            largest = left;
        }
        if (right < heap_.size() && K()(heap_[right], heap_[largest])) {
            largest = right;
        }
        if (largest != vertex) {
            std::swap(heap_[largest], heap_[vertex]);
            Heapify(largest);
        }
    }

    T Pop() {
        if (heap_.empty()) {
            throw EmptyQueueException();
        }
        T result = heap_[0];
        std::swap(heap_[0], heap_[heap_.size() - 1]);
        heap_.pop_back();
        Heapify(0);
        return result;
    }

    void Push(T new_val) {
        heap_.push_back(new_val);
        size_t current_vertex = heap_.size() - 1;
        while (current_vertex > 0 && K()(heap_[current_vertex], heap_[(current_vertex - 1) / 2])) {
            std::swap(heap_[current_vertex], heap_[(current_vertex - 1) / 2]);
            current_vertex = (current_vertex - 1) / 2;
        }
    }

    size_t Size() const {
        return heap_.size();
    }

private:
    std::vector<T> heap_;
};
