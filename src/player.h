/*
  player class header

 */

#pragma once

#include "std_includes.h"
#include "cell.h"
#include "item.h"
#include "weapon.h"

typedef std::vector<Item *> pitmvec;
typedef std::vector<Weapon *> pwpnvec;

class Player {
 public:
  // Variables
  int HP;
  int Money;
  std::vector<int> Ammobelt;
  std::string Name;
  bool isCT;
  bool plant_timer;
  Cell* Position;
  bool Dead();
  double AccMod_in = 1.0,
    AccMod_out = 1.0;
  Weapon* Equipped;
  
  pwpnvec Arsenal;
  pitmvec Armour;
  
  // Methods
  Player();
  ~Player();
};
