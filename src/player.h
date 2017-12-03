/*
  player class header

 */

#pragma once

#include "std_includes.h"

class Player {
 public:
  // Variables
  int HP;
  int Money;
  std::string Name;
  bool isCT;
  
  // Methods
  Player();
  ~Player();

  
};
