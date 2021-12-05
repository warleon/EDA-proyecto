#include <BBox.hpp>
#include <DiskNode.hpp>
#include <DiskRTree.hpp>
#include <Point.hpp>
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
using point_t = Point<std::string, ld, 2>;
using bbox_t = BBox<point_t, 3>;
using node_t = DiskNode<1000, bbox_t, 3>;
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
  // EntryManager manager;

  json config;
  configFile >> config;
  size_t max = config["max"];

  // std::cout << config.dump() << std::endl;
  const size_t dim = config["coordNames"].size();

  ld coords[dim];
  rtree_t tree;
  size_t count = 0;
  std::string data;

  for (size_t i = 0; i < config["files"].size(); i++) {
    // std::cout << config["files"][i] << std::endl;
    CSVReader reader(config["files"][i], ',');
    while (reader.ok() && count++ < max) {
      reader.next();
      // entryId =
      // manager.add(config["files"][i], reader.fileOffset(), reader.size());

      std::cout << "coords: ";
      for (size_t j = 0; j < dim; j++) {
        coords[j] = stold(reader(config["coordNames"][j]));
        std::cout << coords[j] << " ";
      }
      std::cout << std::endl;
      data = reader.getline();
      std::cout << "data: " << data << std::endl;
      point_t temp(coords, data);
      std::cout << temp;
      tree.insert(temp);
    }
  }
  // std::cout << tree;
  return 0;
}