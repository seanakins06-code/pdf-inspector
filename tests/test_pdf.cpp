#include <gtest/gtest.h>
#include "PdfDocument.h"

const std::string kMinimal =
    "%PDF-1.7\n"
    "1 0 obj << /Type /Catalog /Pages 2 0 R >> endobj\n"
    "2 0 obj << /Type /Pages /Kids [3 0 R] /Count 1 >> endobj\n"
    "3 0 obj << /Type /Page /Parent 2 0 R /Resources << /Font << /F1 5 0 R >> >> >> endobj\n"
    "4 0 obj << /Title (Hello) /Author (Sean) >> endobj\n"
    "5 0 obj << /Type /Font /BaseFont /Helvetica >> endobj\n"
    "trailer << /Root 1 0 R /Info 4 0 R >>\n%%EOF";

TEST(PdfDocument, ReadsVersion) {
    EXPECT_EQ(PdfDocument::fromBytes(kMinimal).inspect().version, "1.7");
}

TEST(PdfDocument, CountsPagesButNotPageTree) {
    EXPECT_EQ(PdfDocument::fromBytes(kMinimal).inspect().pageCount, 1);
}

TEST(PdfDocument, ReadsMetadata) {
    auto info = PdfDocument::fromBytes(kMinimal).inspect();
    EXPECT_EQ(info.title, "Hello");
    EXPECT_EQ(info.author, "Sean");
}

TEST(PdfDocument, FollowsInfoReferenceNotFirstTitle) {
    // A decoy /Title appears BEFORE the real metadata object
    std::string pdf = "%PDF-1.7\n"
                      "9 0 obj << /Title (Decoy) >> endobj\n"
                      "4 0 obj << /Title (Real) >> endobj\n"
                      "trailer << /Info 4 0 R >>\n%%EOF";
    EXPECT_EQ(PdfDocument::fromBytes(pdf).inspect().title, "Real");
}

TEST(PdfDocument, RejectsNonPdf) {
    EXPECT_THROW(PdfDocument::fromBytes("hello").inspect(), std::invalid_argument);
}

// TDD: this test is written BEFORE the font code exists, so it fails at first
TEST(PdfDocument, ListsFontsWithoutDuplicates) {
    auto info = PdfDocument::fromBytes(kMinimal + "\n6 0 obj << /BaseFont /Helvetica >> endobj").inspect();
    ASSERT_EQ(info.fonts.size(), 1u);
    EXPECT_EQ(info.fonts[0], "Helvetica");
}