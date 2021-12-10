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
using bbox_t = BBox<point_t, 32>;
using node_t = DiskNode<1000, bbox_t, 10>;
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
  size_t countIgnore = 0;
  std::vector<std::string> data;
  SVGRenderer<rtree_t> render(tree);

  for (size_t i = 0; i < config["files"].size(); i++) {
    // std::cout << config["files"][i] << std::endl;
    CSVReader reader(config["files"][i], ',');
    while (reader.ok() && count < max) {
      try {
        if (!reader.next()) continue;
        for (size_t j = 0; j < dim; j++) {
          coords[j] = stold(reader(config["coordNames"][j]));
        }
      } catch (...) {  // ignore empty or bad formated coords
        countIgnore++;
        continue;
      }
      /*
      if (!coords[0] || !coords[1]) {
        countIgnore++;
        continue;
      }
      */
      if (fmod(coords[0], 0e-14) == 0 ||
          fmod(coords[1], 0e-14) == 0) {  // ignore 0 and low precision coords
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
    render("./RTree.svg", 1000, 1000);
  }

  return 0;
}