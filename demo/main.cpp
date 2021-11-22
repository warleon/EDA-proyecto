#include <fstream>
#include <iostream>
#include <string>

#include "EntryManager.hpp"

int main(int argc, char **argv) {
  EntryManager manager;
  std::string path =
      "/mnt/c/Users/Anthony/Desktop/repos/EDA-proyecto/data/"
      "green_tripdata_2015-01.csv";
  std::ifstream file(path);

  std::cout << "demo working\n";
  std::string line;
  int pos;
  for (int i = 0; i < 10; i++) {
    pos = file.tellg();
    std::getline(file, line);
    manager.add(path, pos, line.size());
  }
  std::cout << manager << "\n";

  return 0;
}