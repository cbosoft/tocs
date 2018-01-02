#pragma once

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <algorithm>
#include <map>
#include <tuple>
#include <stdlib.h>
#include "std_includes.h"
#include "map.h"
#include "player.h"
#include "item.h"
#include "weapon.h" 

class Game{
 public:
  // Attributes
  std::map<std::tuple<int, std::string >, int > dictionary;
  Map game_map;
  std::vector<Player *> players;
  std::vector<Item *> global_inventory;
  std::vector<Weapon *> global_arsenal;
  std::vector<std::string> a_sitreps;
  std::vector<std::string> s_sitreps;
  std::vector<std::string> m_sitreps;
  std::vector<std::string> maps;
  std::vector<std::string> names;
  std::string pr_str = "  >";
  bool verbose = false;
  bool friendly_fire = false;
  std::vector<std::tuple<double, std::string > > messages;

  // Times
  double time_to_move = 0.8;
  double time_to_kill = 1.2;
  double time_to_kill_penalty = 0.5;
  double time_to_equip = 0.6;
  double time_to_hide = 2.5;
  double time_to_camp = 3.2;
  double time_to_buy = 0.8;
  double time_to_fup = 0.1;
  double time_to_plant = 1.1;
  double time_to_blow = 23.5;
  
  // Methods
  void read_dictionary(std::string, std::map<std::tuple<int, std::string >, int > &);
  void read_sitreps(std::string, std::vector<std::string> &, std::vector<std::string> &, std::vector<std::string> &);
  int get_clean_input(std::string, int);
  std::string get_raw_lower_input(std::string);
  std::string get_raw_lower_input(std::string, bool);
  int randint(int, int);
  std::string get_rand_sitrep(int);
  void get_maps(std::string, std::vector<std::string> &);
  void quick_setup();
  void game_loop();
  void read_inventory(std::string);
  void read_arsenal(std::string);
  void read_names(std::string);
  Cell* find_cell(int);
  int game_status();
  void display();
  void sort_input(int);
  int move(Player*, int);
  int kill_with(Player*, int, bool);
  int equip(Player*, int);
  void camp(Player*);
  void hide(Player*);
  int buy();
  void ai_buy(Player*, int, int);
  int plant();
  int equipped_ammo_left(Player*);
  void add_message(std::string, double);
  void add_message_quick(std::string);
  void add_message_important(std::string);
};
