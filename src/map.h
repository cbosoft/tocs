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
  int find_C_spawn();
  int find_T_spawn();
  bool abomb_planted;
  bool bbomb_planted;
  int abomb_timer = 5;
  int bbomb_timer = 5;
  int timer = 180;
};
