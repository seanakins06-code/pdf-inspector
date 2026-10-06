#include "Formatter.h"
#include <sstream>

std::string TextFormatter::format(const PdfInfo& i) const {
    std::ostringstream out;
    out << "PDF version : " << i.version << "\n"
        << "Objects     : " << i.objectCount << "\n"
        << "Pages       : " << (i.pageCount ? std::to_string(*i.pageCount) : "unknown (compressed)") << "\n"
        << "Encrypted   : " << (i.encrypted ? "yes" : "no") << "\n"
        << "Title       : " << i.title.value_or("-") << "\n"
        << "Author      : " << i.author.value_or("-") << "\n"
        << "Fonts       : " << i.fonts.size() << "\n";
    for (const auto& f : i.fonts) out << "  - " << f << "\n";
    return out.str();
}

std::unique_ptr<Formatter> makeFormatter() {
    return std::make_unique<TextFormatter>();
}