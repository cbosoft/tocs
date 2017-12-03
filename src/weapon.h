/*
  weapon class header
*/

#pragma once

#include "std_includes.h"

class Weapon {
 public:
  // Variables
  std::string Name;
  std::string Type;
  int Price;
  int Accuracy;
  int Damage;
  
  // Methods
  Weapon();
  ~Weapon();
};
