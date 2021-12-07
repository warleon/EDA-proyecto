#include <BBox.hpp>
#include <DiskNode.hpp>
#include <DiskRtree.hpp>
#include <Point.hpp>
#include <SVGRenderer.hpp>
#include <cassert>
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
using bbox_t = BBox<point_t, 32>;
using node_t = DiskNode<1000, bbox_t, 32>;
using pool_t = typename node_t::pool_t;
using rtree_t = DiskRTree<node_t>;

std::string readAll(std::ifstream& in) {
  std::ostringstream sstr;
  sstr << in.rdbuf();
  return sstr.str();
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
  std::vector<std::string> data;
  SVGRenderer<rtree_t> render(tree);

  for (size_t i = 0; i < config["files"].size(); i++) {
    // std::cout << config["files"][i] << std::endl;
    CSVReader reader(config["files"][i], ',');
    while (reader.ok() && count++ < max) {
      reader.next();

      for (size_t j = 0; j < dim; j++) {
        coords[j] = stold(reader(config["coordNames"][j]));
      }
      data = reader.currentLine;
      point_t temp(coords, data);
      if (count % (max / 5) == 0) {
        std::cout << temp << "\n";
      }
      tree.insert(temp);
    }
  }
  std::cout << tree << "\n";
  render("./RTree.html", 1000, 1000);

  return 0;
}