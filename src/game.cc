#include "game.h"

//************************************************************************//
//************************************************************************//
void Game::game_loop(){
  int status = 0;
  while(status == 0){
    status = game_status();
    // display current cell
    display();
    // get user input
    sort_input(get_clean_input(pr_str, 2));
  }

  std::cout << pr_str << "game over\n\n";
  if (status < 0){
    std::cout << pr_str << "YOU LOST\n\n";
  }
  else {
    std::cout << pr_str << "YOU WON!\n\n";
  }
  status = std::abs(status);
  
  switch (status) {
  case 1:
    // All terrorists dead: Counter Terrorists win.
    std::cout << pr_str << "Terrorists neutralised: Counter-Terrorists win";
    break;
  case 2:
    // Bomb went off: Terrorists win.
    std::cout << pr_str << "the bomb went off: Terrorists win";
    break;
  case 3:
    // Time ran out: Counter Terrorists win.
    std::cout << pr_str << "ran out of time: Counter-Terrorists win";
    break;
  case 4:
    // Player quit
    std::cout << pr_str << "you quit";
    break;
  }
}

//************************************************************************//
//************************************************************************//
int Game::game_status(){
  if (players[0]->Dead()){
    return -1; // you died
  }
  else if (players[0]->isCT){
    // Counter terrorist team

    // All terrorists dead?
    bool all_t_dead = true;
    for (unsigned int i = 1; i < players.size(); i++){
      if (!players[i]->isCT && !players[i]->Dead()){
	all_t_dead = false;
      }
    }
    if (all_t_dead)
      return 1;

    // Bomb gone off?
    if ((game_map.abomb_planted && game_map.abomb_dline >= game_map.timer) ||
	(game_map.bbomb_planted && game_map.bbomb_dline >= game_map.timer)) {
      return -2;
    }

    if (game_map.timer == 0){
      return 3;
    }
  }
  else{
    // Terrorist team

    // All CT dead?
    bool all_t_dead = true;
    for (unsigned int i = 1; i < players.size(); i++){
      if (!players[i]->isCT && !players[i]->Dead()){
	all_t_dead = false;
      }
    }
    if (all_t_dead)
      return 1;

    // Bomb gone off?
    if ((game_map.abomb_planted && game_map.abomb_dline >= game_map.timer) ||
	(game_map.bbomb_planted && game_map.bbomb_dline >= game_map.timer)) {
      return 2;
    }

    if (game_map.timer == 0){
      return -3;
    }
  }
  return 0;
}

//************************************************************************//
//************************************************************************//
void Game::quick_setup(){
  game_map.read_file(maps[randint(maps.size(), 0)], verbose);
  int cts = game_map.find_C_spawn(), ts = game_map.find_T_spawn();
  int nop = randint(5, 3);
  bool oe_team = (randint(2,1) == 2);
  Player *p;
  if (verbose){std::cout << "  setting up players" << std::endl;}
  for (int i = 0; i < nop; i++){
    p = new Player;
    p->Ammobelt.assign(20, 0);
    p->isCT = oe_team;
    p->Name = names[randint(int(names.size()), 0)];
    p->Arsenal.push_back(&(*global_arsenal[0]));
    p->Ammobelt[0] = global_arsenal[0]->AmmoMax;
    p->Equipped = p->Arsenal[0];
    if (oe_team){
      p->Position = &game_map.Cells[cts];
    }
    else{
      p->Position = &game_map.Cells[ts];
    }
    if (verbose){ std::cout << "    > spawned player \"" << p->Name << "\" in " << p->Position->Name << std::endl;}
    players.push_back(p);
  }
  for (int i = 0; i < nop; i++){
    p = new Player;
    p->Ammobelt.assign(20, 0);
    p->isCT = !oe_team;
    p->Name = names[randint(names.size(), 0)];
    p->Arsenal.push_back(&(*global_arsenal[0]));
    p->Ammobelt[0] = global_arsenal[0]->AmmoMax;
    p->Equipped = p->Arsenal[0];
    if (!oe_team){
      p->Position = &game_map.Cells[cts];
    }
    else{
      p->Position = &game_map.Cells[ts];
    }
    if (verbose){ std::cout << "    > spawned player \"" << p->Name << "\" in " << p->Position->Name << std::endl;}
    players.push_back(p);
  }
}

//************************************************************************//
//************************************************************************//
void Game::sort_input(int retv){
  if (retv > 0 && retv <= 8){
    if (move(players[0], retv - 1) > 0){
      game_map.timer -= time_to_move;
      //add_message_quick(" moved ");
    }
    else{
      game_map.timer -= time_to_fup;
      add_message_quick(" cannot move there! ");
    }
  }
  else if (retv > 8 && retv <= 29){
    int res = kill_with(players[0], retv - 9, (retv == 9));

    if (res == 2){
      game_map.timer -= time_to_kill;
      add_message_quick(" you shot accurately ");
    }
    else if (res == 1){
      game_map.timer -= time_to_kill;
      //add_message_quick(" you shot ");
    }
    else if (res == -1){
      game_map.timer -= time_to_fup;
      add_message_quick(" you don't have that ");
    }
    else{
      game_map.timer -= time_to_kill;
      add_message_quick(" you missed ");
    }
  }
  else if (retv > 29 && retv < 50){
    int res;
    res = equip(players[0], retv - 30);

    switch (res){
    case 1:
      game_map.timer -= time_to_equip;
      //add_message_quick(" successfully equipped ");
      break;
    case -1:
      game_map.timer -= time_to_fup;
      add_message_quick(" you don't have that ");
      break;
    }
  }
  else if (retv == 50){
    game_map.timer -= time_to_hide;
    hide(players[0]);
    add_message_quick(" hiding ");
  }
  else if (retv == 51){
    game_map.timer -= time_to_camp;
    camp(players[0]);
    add_message_quick(" camping ");
  }
  else if (retv == 52){
    int res = buy();

    if (game_map.timer > 176){
      game_map.timer -= time_to_buy;
      switch (res){
      case -4:
	add_message_quick(" buying cancelled ");
	break;
      case -3:
	add_message_quick(" you are maxed out! ");
	break;
      case -2:
	add_message_quick(" you already have that ");
	break;
      case -1:
	add_message_quick(" you cannot afford that ");
	break;
      case 1:
	add_message_quick(" successful purchase! ");
	break;
      }
    }
    else {
      game_map.timer -= time_to_fup;
      add_message_quick(" its too late to buy! ");
    }
  }
  else if (retv == 53){
    int res;
    res = plant(); // time decrement is handled here
    switch (res){
    case -3:
      add_message_quick(" you are not a terrorist ");
      break;
    case -2:
      add_message_quick(" you do not have a bomb ");
      break;
    case -1:
      add_message_quick(" you are not at a bomb-site ");
      break;
    }
  }
}

//************************************************************************//
//************************************************************************//
Cell* Game::find_cell(int id){
  int i = 0;
  for (std::vector<Cell>::iterator it = game_map.Cells.begin(); it != game_map.Cells.end(); it++){
    if (game_map.Cells[i].ID == id){
      return &game_map.Cells[i];
    }
    i += 1;
  }
  // TODO: ERROR OUT HERE
  assert(0);
  return &game_map.Cells[0];
}

//************************************************************************//
//************************************************************************//
int Game::get_clean_input(std::string prompt, int context){
  std::string inp_raw;
  bool isokay = false, isfirst = true;
  int retv = -1;
  
  while (! isokay){
    if (isfirst){
      inp_raw = get_raw_lower_input("  " + prompt);
    }
    else {
      
      inp_raw = get_raw_lower_input("?)" + prompt);
    }

    retv = dictionary[std::make_tuple(context, inp_raw)];
    
    if (retv > 0 && retv != 99){
      isokay = true;
    }
    else if (retv == 99){
      // show help
      switch (context){
      case 0:
	std::cout
	  << "\n  It ain't rocket science. 'new game', or \n"
	  << "  'quick game'\n" << std::endl;
	break;
      case 1:
	std::cout
	  << "\n  ... really? 't' for terrorists, 'c' for \n"
	  << "  counter-terrorists\n" << std::endl;
	break;
      case 2:
	std::cout
	  << "\n  Cardinal directions (north, east, south, west) to move, \n"
	  << "  combinations work too. \n\n"
	  << "  'kill' to use your currently equipped weapon, or \n"
	  << "  equip <name> to change equipped weapon. You can also\n"
	  << "  'camp' if you want to kill time, or 'hide' if you\n"
	  << "  wanna be all sneaky like.\n" << std::endl;
	break;
      case 3:
	std::cout
	  << "  'buy <name>' to buy, 'buy ammo <name>' to buy ammo."
	  << std::endl;
	break;
      }
    }
    else{
      isfirst = false;
    }
  }

  return retv;
}

//************************************************************************//
//************************************************************************//
int Game::randint(int max, int min){
  return rand() % max + min;
}

//************************************************************************//
//************************************************************************//
int Game::equipped_ammo_left(Player* p){
  int i = 0;
  for (auto w: global_arsenal){
    if (w == p->Equipped){
      return p->Ammobelt[i];
    }
    i += 1;
  }
  // TODO error out here
  assert(0);
  return 0;
}
