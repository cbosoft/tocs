/*
  player class header

 */

#pragma once

#include "std_includes.h"
#include "cell.h"


class Player {
 public:
  // Variables
  int HP;
  int Money;
  std::string Name;
  bool isCT;
  Cell* Position;
  bool Dead();
  double AccMod_in = 1.0,
    AccMod_out = 1.0;
  
  // Methods
  Player();
  ~Player();

  
};
