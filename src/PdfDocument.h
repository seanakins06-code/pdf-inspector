#pragma once
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

struct PdfInfo {
    std::string version;
    int objectCount = 0;
    std::optional<int> pageCount;          // empty = could not tell
    bool encrypted = false;
    std::optional<std::string> title, author;
    std::vector<std::string> fonts;
};

class PdfDocument {
public:
    static PdfDocument fromFile(const std::string& path);  // throws on error
    static PdfDocument fromBytes(std::string bytes);        // used by tests
    PdfInfo inspect() const;
private:
    explicit PdfDocument(std::string bytes) : data_(std::move(bytes)) {}
    std::unordered_map<int, std::string_view> buildObjectIndex() const;
    std::string data_;
};