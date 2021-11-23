#include <BBox.hpp>
#include <Point.hpp>
#include <RTree.hpp>
#include <RTreeNode.hpp>
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
using point_t = Point<size_t, ld, 2>;
using bbox_t = BBox<point_t, 3>;
using node_t = RTreeNode<bbox_t, 3>;
using rtree_t = RTree<node_t>;

std::string readAll(std::ifstream& in) {
  std::ostringstream sstr;
  sstr << in.rdbuf();
  return sstr.str();
}

int main(int argc, char** argv) {
  assert(argc >= 2);
  std::string configFilePath = argv[1];
  std::ifstream configFile(configFilePath);
  EntryManager manager;
  size_t max;
  if (argc > 2) {
    max = std::stoul(argv[2]);
  } else {
    max = 20;
  }

  json config;
  configFile >> config;

  std::cout << config.dump() << std::endl;
  size_t entryId;
  const size_t dim = config["coordNames"].size();

  ld coords[dim];
  rtree_t tree;

  for (size_t i = 0; i < config["files"].size(); i++) {
    std::cout << config["files"][i] << std::endl;
    CSVReader reader(config["files"][i], ',');
    while (reader.ok() && entryId < max) {
      reader.next();
      entryId =
          manager.add(config["files"][i], reader.fileOffset(), reader.size());

      for (size_t j = 0; j < dim; j++) {
        coords[j] = stold(reader(config["coordNames"][j]));
      }
      point_t temp(coords);
      temp.data = entryId;

      tree.insert(temp);
    }
  }
  std::cout << tree;
  return 0;
}