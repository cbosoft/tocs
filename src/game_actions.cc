#include "game.h"

//************************************************************************//
//************************************************************************//
int Game::move(Player* p, int direction){
  p->AccMod_in = 1.0;
  p->AccMod_out = 1.0;
  int dest_id = p->Position->Directions[direction];
  if (dest_id >= 0){
    std::cout << dest_id << std::endl;
    p->Position = find_cell(dest_id);
    return 1;
  }
  else {
    return -1;
  }
}

//************************************************************************//
//************************************************************************//
int Game::kill_with(Player* p, int weapon, bool iseq){
  p->AccMod_in = 1.0;
  p->AccMod_out = 1.0;
  std::vector<Player*> pop;
  Player* tohit;
  int popc = 0;
  for (auto op: players){
    if (op->Position == p->Position){
      if (friendly_fire || (p->isCT != op->isCT)){
	pop.push_back(op);
	popc += 1;
      }
    }
  }
  tohit = pop[randint(popc, 0)];

  bool playerhas = false;
  for (auto w: p->Arsenal){
    if (w == global_arsenal[weapon]){
      playerhas = true;
    }
  }

  if (!playerhas){
    return -1;
  }

  int damage = global_arsenal[weapon]->Damage + randint(20, 0) - 10;
  double hits = 0.0, rof, hitp;
  rof = (double)global_arsenal[weapon]->ROF;

  for (int i = 0; i < rof; i++){
    if (randint(100, 0) < (int)((double)(global_arsenal[weapon]->Accuracy) * p->AccMod_out * tohit->AccMod_in)){
      tohit->HP -= damage;
      hits += 1.0;
    }
  }

  hitp = hits / rof;
  
  if (hitp == 0.0){
    return -2;
  }
  else if ((hitp > 0.0) && (hitp < 1.0)){
    if (!iseq){game_map.timer -= 1;}
    return 1;
  }
  else{
    if (!iseq){game_map.timer -= 1;}
    return 2;
  }
}

//************************************************************************//
//************************************************************************//
int Game::equip(Player* p, int weapon){
  bool playerhas = false;
  for (auto w: p->Arsenal){
    if (w == global_arsenal[weapon]){
      playerhas = true;
    }
  }

  if (!playerhas){
    return -1;
  }

  p->Equipped = global_arsenal[weapon];
  return 1;
}

//************************************************************************//
//************************************************************************//
void Game::hide(Player* p){
  p->AccMod_in = 0.8;
  p->AccMod_out = 0.6;
}


//************************************************************************//
//************************************************************************//
void Game::camp(Player* p){
  p->AccMod_in = 1.1;
  p->AccMod_out = 1.02;
}

//************************************************************************//
//************************************************************************//
int Game::buy(){
  bool done = false;
  
  while (!done){
    int c = 1;
    // show buy screen
    std::cout
      << std::setw(4) << "ID" << std::endl
      << std::setw(32) << "Name"
      << std::setw(10) << "Type"
      << std::setw(10) << "Price"
      << std::setw(14) << "Ammo Price"
      << std::endl;
    for (auto it: global_arsenal){
      std::cout
	<< std::setw(4) << c
	<< std::setw(32) << it->Name
	<< std::setw(10) << it->Type
	<< std::setw(10) << it->Price
	<< std::setw(14) << it->AmmoPrice
	<< std::endl;
      c += 1;
    }
    int retv;
    retv = get_clean_input(pr_str, 3);
    if (retv > 0 && retv <= 20){
      // Can player afford it?
      if (players[0]->Money > global_arsenal[retv - 1]->Price){
	// Does player already have it?
	for (auto w: players[0]->Arsenal){
	  if (w == global_arsenal[retv - 1]){
	    return -2;
	  }
	}
	// Cool, buy it!
	players[0]->Arsenal.push_back(global_arsenal[retv - 1]);
	players[0]->Money -= global_arsenal[retv - 1]->Price;
	players[0]->Ammobelt[(retv - 1)] = global_arsenal[retv - 1]->AmmoMax;
	done = true;
      }
      else{
	return -1;
      }
    }
    else if (retv > 20 && retv <= 40){
      // Can player afford it?
      if (players[0]->Money > global_arsenal[retv - 21]->AmmoPrice){
	// Does player already have it?
	if (players[0]->Ammobelt[retv - 21] == global_arsenal[retv - 21]->AmmoMax){
	  return -3;
	}
	
	// Cool, buy it!
	players[0]->Money -= global_arsenal[retv - 21]->AmmoPrice;
	players[0]->Ammobelt[(retv - 21)] = global_arsenal[retv - 21]->AmmoMax;
	done = true;
      }
      else{
	return -1;
      }
    }
    else if (retv == 41){
      return -4;
    }
  }
  return 1;
}

//************************************************************************//
//************************************************************************//
int Game::plant(){

  // plant bomb

  // is terrorist
  // is at bomb site?
  if (players[0]->plant_timer > 0) {
    players[0]->plant_timer -= 1;

    if (players[0]->plant_timer <= 0){
      // planted
      if (players[0]->Position->isBombA) {
	game_map.abomb_planted = true;
	game_map.abomb_dline = game_map.timer - time_to_blow;
      }
      else if (players[0]->Position->isBombB) {
	game_map.bbomb_planted = true;
	game_map.bbomb_dline = game_map.timer - time_to_blow;
      }
      else {
	// HOW?!
      }

      add_message_important("The bomb has been planted!");
    }
    
  }
  else if (!players[0]->isCT){
    if (players[0]->Position->isBombA || players[0]->Position->isBombB){
      // do planting
      players[0]->plant_timer = 12;
      add_message_quick("you are planting a bomb");
      game_map.timer -= time_to_plant;
    }
    else{
      game_map.timer -= time_to_fup;
      return -1;
    }
  }
  else{
    game_map.timer -= time_to_fup;
    return -3;
  }

  return 2;  
}
