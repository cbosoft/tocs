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
  int Damage;
  int Accuracy;
  int Capacity;
  int AmmoLeft;
  int AmmoMax;
  int AmmoPrice;
  int ROF;
  
  // Methods
  Weapon();
  ~Weapon();
};
