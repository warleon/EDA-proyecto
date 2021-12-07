#pragma once

#include <cassert>
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;

template <typename T>
class SVGRenderer {
  T& object;

 public:
  SVGRenderer(T& o) : object(o) {}
  ~SVGRenderer() {}
  void operator()(fs::path path, size_t width, size_t height) {
    std::string svg = toSVG(object, 0, 0, width, height);
    std::ofstream os(path);
    assert(os.is_open());
    os << "<!DOCTYPE html> <html><body> <svg width =\"" << width
       << "\" height=\"" << height << "\">\n";
    os << svg;
    os << "</svg></body></html>";

    os.close();
  }
};