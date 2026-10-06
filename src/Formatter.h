#pragma once
#include <memory>
#include <string>
#include "PdfDocument.h"

class Formatter {
    public:
    virtual ~Formatter() = default;
    virtual std::string format(const PdfInfo& info) const = 0;
};

class TextFormatter : public Formatter {
    public:
        std::string format(const PdfInfo& info) const override;
};
std::unique_ptr<Formatter> makeFormatter();