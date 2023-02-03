#include "Archivator.h"
#include "Reader.h"
#include "Writer.h"
#include "Huffman.h"
#include <fstream>
#include <vector>

std::unordered_map<std::vector<bool>, std::vector<bool>> BuildCanon(
    const std::vector<std::pair<size_t, std::vector<bool>>>& lengths) {
    std::vector<bool> current_code(lengths[0].first, false);
    std::unordered_map<std::vector<bool>, std::vector<bool>> result;
    for (size_t i = 0; i < lengths.size(); ++i) {
        result[lengths[i].second] = current_code;
        if (i + 1 == lengths.size()) {
            break;
        }
        size_t j = current_code.size();
        while (j && current_code[j - 1]) {
            current_code[j - 1] = false;
            --j;
        }
        if (j) {
            current_code[j - 1] = true;
        } else {
            std::cout << "Failed to build Canonical Huffman code" << std::endl;
            throw BuildCanonException();
        }
        for (size_t _ = 0; _ < lengths[i + 1].first - lengths[i].first; ++_) {
            current_code.push_back(false);
        }
    }
    return result;
}

void Compress(const std::filesystem::path& file, Writer& writer, bool is_last) {
    std::ifstream stream(file);
    size_t slash_index = file.string().rfind('/');
    std::string filename = file.string().substr((slash_index == std::string::npos ? 0 : slash_index + 1));
    if (!stream.is_open() || stream.bad()) {
        std::cout << "Failed to open output file" << std::endl;
        throw OpenStreamException();
    }
    Reader reader(stream);
    std::unordered_map<std::vector<bool>, size_t> count;
    std::vector<std::vector<bool>> filename_bits;

    for (auto i : filename) {
        std::vector<bool> symbol(BYTE_SIZE, false);
        for (size_t j = 0; j < BYTE_SIZE; j++) {
            symbol[BYTE_SIZE - 1 - j] = i & 1;
            i >>= 1;
        }
        ++count[symbol];
        filename_bits.push_back(symbol);
    }

    while (!reader.Eof()) {
        std::vector<bool> symbol = reader.ReadBits(INIT_BYTE_SIZE);
        if (reader.Eof()) {
            break;
        }
        symbol.insert(symbol.begin(), false);
        ++count[symbol];
    }

    ++count[FILENAME_END];
    ++count[ONE_MORE_FILE];
    ++count[ARCHIVE_END];

    HuffmanTree* tree(HuffmanTree::Build(count));

    std::cout << "Huffman code ready!!!" << std::endl;

    std::vector<std::pair<size_t, std::vector<bool>>> symbols;

    std::queue<std::pair<const HuffmanTree*, size_t>> q;
    q.push(std::make_pair(tree, 0));

    while (!q.empty()) {
        auto v = q.front();
        q.pop();
        if (v.first->go_[0] != nullptr) {
            q.push(std::make_pair(v.first->go_[0], v.second + 1));
            q.push(std::make_pair(v.first->go_[1], v.second + 1));
        } else {
            symbols.push_back(std::make_pair(v.second, v.first->GetData()));
        }
    }

    delete tree;

    std::sort(symbols.begin(), symbols.end());

    auto codes = BuildCanon(symbols);

    std::cout << "Canonical Huffman code ready!" << std::endl;

    writer.WriteNumber(symbols.size(), BYTE_SIZE);
    std::vector<size_t> size_count(symbols.size(), 0);
    size_t max_code_size = 0;
    for (const auto& [len, symbol] : symbols) {
        writer.WriteBits(symbol);
        ++size_count[len];
        max_code_size = std::max(max_code_size, len);
    }

    for (size_t len = 1; len <= max_code_size; ++len) {
        writer.WriteNumber(size_count[len], BYTE_SIZE);
    }
    for (const auto& symbol_bits : filename_bits) {
        writer.WriteBits(codes[symbol_bits]);
    }
    writer.WriteBits(codes[FILENAME_END]);

    reader.ResetStream();

    while (true) {
        std::vector<bool> symbol = reader.ReadBits(INIT_BYTE_SIZE);
        if (reader.Eof()) {
            break;
        }
        symbol.insert(symbol.begin(), false);
        writer.WriteBits(codes[symbol]);
    }

    writer.WriteBits(codes[(is_last ? ARCHIVE_END : ONE_MORE_FILE)]);
}

size_t VectorBoolToInt(const std::vector<bool>& a) {
    size_t answer = 0;
    for (size_t i = 0; i < BYTE_SIZE; ++i) {
        answer <<= 1;
        answer += a[i];
    }
    return answer;
}

void Decompress(const std::filesystem::path& file) {
    std::ifstream stream(file);
    if (!stream.is_open() || stream.bad()) {
        throw OpenStreamException();
    }
    Reader reader(stream);
    size_t number_of_file = 0;
    while (true) {
        std::cout << "Start to decompress file #" << number_of_file << std::endl;
        size_t symbol_count = VectorBoolToInt(reader.ReadBits(BYTE_SIZE));
        std::vector<std::vector<bool>> symbols;
        for (size_t i = 0; i < symbol_count; ++i) {
            symbols.push_back(reader.ReadBits(BYTE_SIZE));
        }

        std::vector<std::pair<size_t, std::vector<bool>>> lengths;
        size_t length = 0;
        size_t current_count = 0;

        for (size_t i = 0; i < symbol_count; ++i) {
            while (!current_count) {
                current_count = VectorBoolToInt(reader.ReadBits(BYTE_SIZE));
                ++length;
            }
            lengths.push_back(std::make_pair(length, symbols[i]));
            --current_count;
        }

        auto code_by_symbol = BuildCanon(lengths);

        std::cout << "Canonical Huffman code ready!" << std::endl;

        std::unordered_map<std::vector<bool>, std::vector<bool>> codes;
        for (const auto& [symbol, code] : code_by_symbol) {
            codes[code] = symbol;
        }
        std::vector<bool> current;
        std::string output_filename;
        while (true) {
            while (!codes.count(current)) {
                current.push_back(reader.ReadOneBit());
            }
            if (codes[current] != FILENAME_END) {
                char symbol = 0;
                std::vector<bool> code = codes[current];
                for (size_t i = 0; i < BYTE_SIZE; ++i) {
                    symbol <<= 1;
                    if (code[i]) {
                        symbol |= 1;
                    }
                }
                output_filename.push_back(symbol);
                current.clear();
            } else {
                break;
            }
        }

        std::cout << output_filename << std::endl;

        std::ofstream file_output(output_filename);
        if (!file_output.is_open() || file_output.bad()) {
            std::cout << "Failed to open output file" << std::endl;
            throw OpenStreamException();
        }
        Writer writer(file_output);
        current.clear();
        while (true) {
            while (!codes.count(current)) {
                current.push_back(reader.ReadOneBit());
            }
            std::vector<bool> current_code = codes[current];
            if (current_code != ARCHIVE_END && current_code != ONE_MORE_FILE) {
                current_code.erase(current_code.begin());
                writer.WriteBits(current_code);
                current.clear();
            } else {
                break;
            }
        }
        writer.Close();
        std::cout << "file #" << ++number_of_file << " decompressed successfully" << std::endl;
        if (codes[current] == ARCHIVE_END) {
            break;
        }
    }
}
