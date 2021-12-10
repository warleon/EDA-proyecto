#include <BBox.hpp>
#include <DiskNode.hpp>
#include <DiskRtree.hpp>
#include <Point.hpp>
#include <SVGRenderer.hpp>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

#include "CSVReader.hpp"
#include "EntryManager.hpp"

using json = nlohmann::json;

using ld = long double;
using point_t = Point<std::vector<std::string>, ld, 2>;
using bbox_t = BBox<point_t, 3>;
using node_t = DiskNode<1000, bbox_t, 32>;
using pool_t = typename node_t::pool_t;
using rtree_t = DiskRTree<node_t>;

#define minPrecision (14 + 3)

template <typename T>
bool between(T x, T a, T b) {
  return (x >= a) && (x <= b);
}

int main(int argc, char** argv) {
  assert(argc >= 2);
  std::string configFilePath = argv[1];
  std::ifstream configFile(configFilePath);

  json config;
  configFile >> config;
  size_t max = config["max"];

  const size_t dim = config["coordNames"].size();

  ld coords[dim];
  rtree_t tree;
  size_t count = 0;
  size_t countIgnore = 0;
  std::vector<std::string> data;
  SVGRenderer<rtree_t> render(tree);

  for (size_t i = 0; i < config["files"].size(); i++) {
    // std::cout << config["files"][i] << std::endl;
    CSVReader reader(config["files"][i], ',');
    while (reader.ok() && count < max) {
      try {
        if (!reader.next()) continue;  // check for invalid line
        for (size_t j = 0; j < dim; j++) {
          // ignore low precision coords
          auto value = reader(config["coordNames"][j]);
          if (value.size() < minPrecision)
            throw "coordenada con poca precision";
          coords[j] = stold(value);
        }
        if (!between(coords[0], (ld)-74.0, (ld)-73.75))
          throw "coordenada X fuera de rango";
        if (!between(coords[1], (ld)40.0, (ld)41.0))
          throw "coordenada Y fuera de rango";
        // if (!coords[0] || !coords[1]) throw "coordenadas invalidas";//si
        // coord es 0 deberia ser atrapada x poca precision
      } catch (...) {  // ignore empty or bad formated coords
        countIgnore++;
        continue;
      }
      count++;
      if (count % 100000 == 0) {
        std::cout << count << " points inserted\n";
        std::cout << countIgnore << " points ignored\n";
      }
      data = reader.currentLine;
      point_t temp(coords, data);
      tree.insert(temp);
    }
  }
  std::cout << tree << "\n";
  std::cout << count << " points inserted in total\n";
  std::cout << countIgnore << " points ignored in total\n";
  if (config["render"]) {
    render("./RTree.svg", 5000, 5000);
  }

  return 0;
}