#pragma once

#include <filesystem>
#include <vector>
#include <unordered_map>
#include "Writer.h"

static const std::vector<bool> FILENAME_END = {true, false, false, false, false, false, false, false, false};
static const std::vector<bool> ONE_MORE_FILE = {true, false, false, false, false, false, false, false, true};
static const std::vector<bool> ARCHIVE_END = {true, false, false, false, false, false, false, true, false};
static const uint8_t BYTE_SIZE = 9;
static const uint8_t INIT_BYTE_SIZE = 8;

void Compress(const std::filesystem::path& file, Writer& w, bool is_last);
void Decompress(const std::filesystem::path& file);
std::unordered_map<std::vector<bool>, std::vector<bool>> BuildCanon(
    const std::vector<std::pair<size_t, std::vector<bool>>>& lengths);

class BuildCanonException : std::exception {};

class OpenStreamException : std::exception {};
