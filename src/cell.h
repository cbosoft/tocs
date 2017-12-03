/*
  cell class header file
*/

#pragma once

#include "std_includes.h"

class Cell {
 public:
  // Variables
  int ID;
  std::string Name;
  std::string Description;
  std::vector<int> Directions;
  bool isTspawn;
  bool isCspawn;
  bool isBombA;
  bool isBombB;

  // Methods
  Cell();
  ~Cell();
  Cell(int, std::string, std::string, bool, bool, bool, bool, std::vector<int>);
};
