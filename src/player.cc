#include "std_includes.h"
#include "player.h"

Player::Player(){
  HP = 100;
  plant_timer = 0;
}
Player::~Player(){}

bool Player::Dead(){
  if (HP <= 0){
    return true;
  }
  else{
    return false;
  }
}
