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
  bool abomb_planted;
  bool bbomb_planted;
  double abomb_dline = 181.0;
  double bbomb_dline = 181.0;
  double timer = 180.0;
  
  // Methods
  Map();
  Map(const std::string);
  ~Map();
  void read_file(std::string, bool v);
  int find_C_spawn();
  int find_T_spawn();
};
