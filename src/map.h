/*
  map class header file
 */

#pragma once

#include "std_includes.h"
#include "cell.h"

class Map {
 public:
  // Variables
  std::string Name;
  std::vector<Cell> Cells;
  
  // Methods
  Map();
  Map(const std::string);
  ~Map();
  void read_file(std::string, bool v);
};
