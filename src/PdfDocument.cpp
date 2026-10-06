#include "PdfDocument.h"
#include <fstream>
#include <iterator>
#include <regex>
#include <set>
#include <stdexcept>

// ---------- Load layer ----------
PdfDocument PdfDocument::fromFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);  // RAII: file closes automatically
    if (!in) throw std::runtime_error("Cannot open file: " + path);
    std::string bytes((std::istreambuf_iterator<char>(in)),
                      std::istreambuf_iterator<char>());
    return PdfDocument(std::move(bytes));
}

PdfDocument PdfDocument::fromBytes(std::string bytes) {
    return PdfDocument(std::move(bytes));
}

// ---------- Helpers ----------
namespace {
int countMatches(const std::string& s, const std::regex& re) {
    return static_cast<int>(std::distance(
        std::sregex_iterator(s.begin(), s.end(), re), std::sregex_iterator()));
}

std::optional<std::string> firstCapture(const std::string& s, const std::regex& re) {
    std::smatch m;
    if (std::regex_search(s, m, re)) return m[1].str();
    return std::nullopt;
}
}  // namespace

// ---------- Index layer ----------
std::unordered_map<int, std::string_view> PdfDocument::buildObjectIndex() const {
    std::unordered_map<int, std::string_view> index;
    const std::regex header(R"((\d+)\s+\d+\s+obj\b)");
    for (auto it = std::sregex_iterator(data_.begin(), data_.end(), header);
         it != std::sregex_iterator(); ++it) {
        size_t start = it->position() + it->length();
        size_t end = data_.find("endobj", start);
        if (end == std::string::npos) break;
        // Later objects with the same number replace earlier ones (incremental updates)
        index[std::stoi((*it)[1].str())] = std::string_view(data_).substr(start, end - start);
    }
    return index;
}

// ---------- Analyse layer ----------
PdfInfo PdfDocument::inspect() const {
    if (data_.rfind("%PDF-", 0) != 0) throw std::invalid_argument("Not a PDF file");

    PdfInfo info;
    info.version = data_.substr(5, 3);

    auto index = buildObjectIndex();
    info.objectCount = static_cast<int>(index.size());

    // \b stops "/Page" from also matching "/Pages" (the page-tree node)
    int pages = countMatches(data_, std::regex(R"(/Type\s*/Page\b)"));
    if (pages > 0) info.pageCount = pages;

    info.encrypted = data_.find("/Encrypt") != std::string::npos;

    // Follow the trailer's "/Info N 0 R" reference straight to the right object
    if (auto infoNum = firstCapture(data_, std::regex(R"(/Info\s+(\d+)\s+\d+\s+R)"))) {
        auto it = index.find(std::stoi(*infoNum));  // O(1) average lookup
        if (it != index.end()) {
            std::string infoDict(it->second);
            info.title  = firstCapture(infoDict, std::regex(R"(/Title\s*\(([^)]*)\))"));
            info.author = firstCapture(infoDict, std::regex(R"(/Author\s*\(([^)]*)\))"));
        }
    }
    // fonts: added later, test first
    std::set<std::string> fontSet;
    const std::regex fontRe(R"(/BaseFont\s*/([^\s/<>\[\]()]+))");
    for (auto it = std::sregex_iterator(data_.begin(), data_.end(), fontRe);
         it != std::sregex_iterator(); ++it) {
        fontSet.insert((*it)[1].str());
    }
    info.fonts.assign(fontSet.begin(), fontSet.end());
    return info;
}