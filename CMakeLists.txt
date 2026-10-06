cmake_minimum_required(VERSION 3.20)
project(pdf_inspector CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(pdfcore src/PdfDocument.cpp src/Formatter.cpp)
target_include_directories(pdfcore PUBLIC src)

add_executable(pdf-inspector src/main.cpp)
target_link_libraries(pdf-inspector PRIVATE pdfcore)

include(FetchContent)
FetchContent_Declare(googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(tests tests/test_pdf.cpp)
target_link_libraries(tests PRIVATE pdfcore GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(tests)