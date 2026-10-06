# PDF Inspector

![CI](https://github.com/seanakins06-code/pdf-inspector/actions/workflows/ci.yml/badge.svg)

**What it does:** you give it a PDF file, and it prints facts about that file: the PDF version, how many pages it has, who wrote it, which fonts it uses, and so on.

**What's special about it:** it doesn't use a PDF library. It opens the file as raw bytes and reads the PDF format by hand, written in C++17.

## Example

```
> pdf-inspector samples/task2.pdf
PDF version : 1.5
Objects     : 23
Pages       : 1
Encrypted   : no
Title       : -
Author      : Mecheng
Fonts       : 2
  - ABCDEE+Calibri
  - ABCDEE+Calibri,Bold
```

A `-` means the PDF doesn't contain that piece of information. The `ABCDEE+` at the start of a font name is a tag Word adds when it embeds only the characters used, not the whole font.

## How to build and run

You need CMake and a C++ compiler. From the project folder:

```
cmake -B build                  # set up the build
cmake --build build             # compile
ctest --test-dir build -C Debug --output-on-failure   # run the tests
```

Then run it on a PDF (on Windows the program is in `build\Debug`):

```
pdf-inspector <file.pdf>
```

If something goes wrong it exits with a code: `0` = worked, `1` = you forgot the file name, `2` = the file couldn't be opened or isn't a PDF.

## How it works

A PDF is a text-like file made of numbered chunks called **objects**, for example `4 0 obj << /Title (Hello) >> endobj`. A line at the end called the **trailer** says things like "the document information is in object 4". The program works in four steps, each in its own piece of code:

1. **Load** (`PdfDocument::fromFile`). Read every byte of the file into memory. The file closes itself when the function ends (this is called RAII).
2. **Index** (`buildObjectIndex`). Scan the file once and build a lookup table (a hash map) from object number to that object's text. After this, finding object 4 is instant.
3. **Analyse** (`inspect`). Read the version, count pages and fonts, and follow the trailer's "document info is object 4" note to find the title and author. The results go into a simple `PdfInfo` struct.
4. **Present** (`Formatter` and `TextFormatter`). Turn the `PdfInfo` into text for the screen. `Formatter` is a general "something that formats results" blueprint (an abstract base class), and `TextFormatter` is one version of it. To add JSON output later, you'd add one more subclass and change nothing else.

The loading, indexing and analysing code is a separate library (`pdfcore`) from `main.cpp`, so the tests can use it without the command line.

## Tests

Tests use GoogleTest. They feed the program tiny fake PDFs written as strings, so no files are needed. They check that it:

- reads the version, page count, title and author correctly
- counts pages but not the page-tree entry (`/Page` vs `/Pages`)
- picks the real title when a fake one appears earlier in the file
- rejects files that aren't PDFs
- lists each font only once (this one was written before the code, which is test-driven development)

GitHub Actions builds and tests every change on both Windows and Linux. Changes are made on a branch and merged through a pull request once the checks pass.

## Known limitations

- It can't read parts of a PDF that are compressed (common from PDF 1.5 onward), so some fonts, titles or page counts may be missed.
- It can tell a PDF is encrypted but can't read it.
- It uses `std::regex`, which is simple but slow on very large files.
- Non-English or special characters in titles aren't decoded.

## Ideas for next steps

- Decompress the compressed parts using zlib.
- Add JSON output (`--json`) with a new `Formatter` subclass.
- Replace the pattern matching with a proper parser.

## Glossary

| Term | Plain meaning |
| --- | --- |
| Object | One numbered chunk of a PDF file |
| Trailer | The note at the end of a PDF pointing to the main objects |
| Hash map (`unordered_map`) | A lookup table that finds an item by its key almost instantly |
| RAII | Resources like files clean themselves up automatically |
| Abstract base class | A blueprint that says what a class must do, not how |
| Polymorphism | Using different subclasses through one common interface |
| `unique_ptr` | A smart pointer with one owner that frees memory automatically |
| CI | A robot that builds and tests your code on every change |
| Test-driven development | Write a failing test first, then write the code to pass it |