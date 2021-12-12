#include <BBox.hpp>
#include <DiskNode.hpp>
#include <DiskRtree.hpp>
#include <Point.hpp>
#include <SVGRenderer.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>
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

const size_t dimensions = 2;
const size_t leafCap = 50;
const size_t internCap = leafCap;
const size_t poolCap = 1000;

using ld = long double;
using point_t = Point<std::vector<std::string>, ld, dimensions>;
using bbox_t = BBox<point_t, leafCap>;
using node_t = DiskNode<poolCap, bbox_t, internCap>;
using pool_t = typename node_t::pool_t;
using rtree_t = DiskRTree<node_t>;
using coord_t = typename point_t::coord_t;

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;
typedef bg::model::point<ld, dimensions, bg::cs::cartesian> boostPoint_t;
typedef bg::model::box<boostPoint_t> boostBox_t;

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

  const size_t dim = config["coordNames"].size();

  ld coords[dim];
  rtree_t tree;
  bgi::rtree<boostPoint_t, bgi::linear<leafCap, 1>> rt;
  size_t count = 0;
  size_t countIgnore = 0;
  std::vector<std::string> data;
  SVGRenderer<rtree_t> render(tree);

  for (size_t i = 0; i < config["files"].size(); i++) {
    std::cout << config["files"][i] << std::endl;
    CSVReader reader(config["files"][i]["name"], ',');
    size_t max = config["files"][i]["max"];
    size_t skip = config["files"][i]["from"];
    for (size_t c = 0; reader.ok() && c < skip; c++) {
      reader.next();
    }
    while (reader.ok() && count < max) {
      try {
        if (!reader.next()) throw "linea invalida";
        for (size_t j = 0; j < dim; j++) {
          // ignore low precision coords
          auto value = reader(config["coordNames"][j]);
          if (value.size() < minPrecision)
            throw "coordenada con poca precision";
          coords[j] = stold(value);
        }
        if (!between(coords[0], (ld)config["filter"][0][0],
                     (ld)config["filter"][0][1]))
          throw "coordenada X fuera de rango";
        if (!between(coords[1], (ld)config["filter"][1][0],
                     (ld)config["filter"][1][1]))
          throw "coordenada Y fuera de rango";
      } catch (...) {  // ignore empty or bad formated coords
        countIgnore++;
        continue;
      }
      data = reader.currentLine;
      point_t temp(coords, data);
      tree.insert(temp);
      if (config["boostTest"]) rt.insert(boostPoint_t(coords[0], coords[1]));
      count++;
      if (count % (size_t)config["logInterval"] == 0) {
        std::cout << count << " points inserted\n";
        std::cout << countIgnore << " points ignored\n";
      }
    }
    std::cout << count << " points inserted in total\n";
    std::cout << countIgnore << " points ignored in total\n";
    // update config
    config["files"][i]["from"] =
        (size_t)config["files"][i]["from"] + count + countIgnore;
    count = 0;
    countIgnore = 0;
  }
  std::vector<point_t> myResult;
  std::vector<boostPoint_t> boostResult;
  ld qCoords[][2] = {
      {config["queryRegion"][0][0], config["queryRegion"][0][1]},
      {config["queryRegion"][1][0], config["queryRegion"][1][1]}};
  boostBox_t box_region(boostPoint_t(qCoords[0][0], qCoords[0][1]),
                        boostPoint_t(qCoords[1][0], qCoords[1][1]));

  point_t a(qCoords[0]), b(qCoords[1]);

  tree.getRegion(myResult, a, b);
  if (config["boostTest"])
    rt.query(bgi::intersects(box_region), std::back_inserter(boostResult));

  std::cout << tree << "\n";

  std::cout << "my rtree answer size:" << myResult.size() << "\n";
  if (config["boostTest"])
    std::cout << "boost rtree answer size:" << boostResult.size() << "\n";
  if (config["render"]) {
    render("./RTree.svg", 5000, 5000);
  }
  // update config
  std::ofstream os(configFilePath);
  os << config;

  return 0;
}