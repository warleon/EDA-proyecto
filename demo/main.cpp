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

  std::cout << config.dump() << std::endl;

  for (size_t i = 0; i < config["files"].size(); i++) {
    std::cout << config["files"][i] << std::endl;
    CSVReader reader(config["files"][i], ',');
    for (auto& it : reader) {
      std::cout << it << " ";
    }
    std::cout << std::endl;
  }
  return 0;
}