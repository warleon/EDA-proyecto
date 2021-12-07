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
    os << "<svg width =\"" << width << "\" height=\"" << height
       << "\" xmlns=\"http://www.w3.org/2000/svg\" version = \"1.1\" >\n ";
    os << svg;
    os << "</svg>";

    os.close();
  }
};