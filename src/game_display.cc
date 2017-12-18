#include "game.h"

//************************************************************************//
//************************************************************************//
void Game::add_message(std::string m_text, double endsat){
  auto m = std::make_tuple(endsat, m_text);
  messages.push_back(m);
}

//************************************************************************//
//************************************************************************//
void Game::add_message_quick(std::string m_text){
  add_message(m_text, game_map.timer - 0.1);
}

//************************************************************************//
//************************************************************************//
void Game::add_message_important(std::string m_text){
  add_message(m_text, game_map.timer - 10.0);
}

//************************************************************************//
//************************************************************************//
void Game::display(){
  std::cout << "  -------------------------------------------------------\n\n";
  std::cout << "  [" << ((int)game_map.timer) << "s to go]"
	    << "  [HP:" << players[0]->HP << "]"
	    << "  [" << players[0]->Equipped->Name
	    << " : " << equipped_ammo_left(players[0])
	    << " / " << players[0]->Equipped->AmmoMax << "]\n\n";

  int mc = 0;
  double m_time;
  std::string m;
  for (auto message: messages){
    m_time = std::get<0>(message);

    if (m_time >= game_map.timer){
      messages.erase(messages.begin() + mc);
    }
    mc += 1;
  }
  
  for (auto message: messages){
    m = std::get<1>(message);
    std::cout << "  " << pr_str << m << std::endl << std::endl;
  }
  
  std::cout << "  " << players[0]->Position->Description << "\n\n";
  int plc = 0, i = 0, pltc = 0;
  std::string inb = "";
  std::vector<std::string> also_here;
  
  for (std::vector<Player*>::iterator it = players.begin(); it != players.end(); it++, i++){
    if (players[i]->Position->ID == players[0]->Position->ID && i > 0){
      plc += 1;
      pltc += 1;
      also_here.push_back(players[i]->Name);
    }
  }
  
  if (plc > 1){
    plc = 2;
  }
  else if (plc == 1){
    plc = 1;
  }
  else{
    plc = 0;
  }
  
  i = 0;
  for (std::vector<std::string>::iterator it = also_here.begin(); it != also_here.end(); it++, i++){
    if (i == 0){
      std::cout << "  " << also_here[i];
    }
    else if (i > 0){
      if (i == (pltc - 1)){
	std::cout << ", and " << also_here[i];
      }
      else{
	std::cout << ", " << also_here[i];
      }
    }
  }
  if (plc == 0){std::cout << " ";}
  std::cout << " " << get_rand_sitrep(plc) << "\n\n  What would you like to do?\n" << std::endl;
}

//************************************************************************//
//************************************************************************//
std::string Game::get_rand_sitrep(int type){
  std::vector<std::string> choices;
  
  switch (type){
  case 0:
    // Player is alone
    return a_sitreps[randint(a_sitreps.size(), 0)];
  case 1:
    // Player is with a single other
    return s_sitreps[randint(s_sitreps.size(), 0)];
  case 2:
    // Player is with multiple others
    return m_sitreps[randint(m_sitreps.size(), 0)];
  default:
    return "Oops";
  }
}
