/*
  tocs: Text Only CounterStrike
  
  See xkcd#91 (https://xkcd.com/91)
 */

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

//************************************************************************//
//* globals, sweet globals ***********************************************//
//************************************************************************//
typedef std::map<std::tuple<int, std::string >, int > utr;
typedef std::vector<std::string> strvec;
typedef std::vector<Player *> pplavec;
typedef std::vector<Item *> pitmvec;
typedef std::vector<Weapon *> pwpnvec;
typedef std::string str;

utr dictionary;
Map game_map;
pplavec players;
pitmvec global_inventory;
pwpnvec global_arsenal;
strvec a_sitreps;
strvec s_sitreps;
strvec m_sitreps;
strvec maps;
strvec names;
str pr_str = "  >";
bool verbose = false;
bool friendly_fire = false;
std::vector<std::tuple<double, std::string > > messages; 

//************************************************************************//
//* what time is it? *****************************************************//
//************************************************************************//
double time_to_move = 0.8;
double time_to_kill = 1.2;
double time_to_kill_penalty = 0.5;
double time_to_equip = 0.6;
double time_to_hide = 2.5;
double time_to_camp = 3.2;
double time_to_buy = 0.8;
double time_to_fup = 0.1;
double time_to_plant = 12.8;

//************************************************************************//
//** main "header" *******************************************************//
//************************************************************************//
void read_dictionary(str, utr &);
void read_sitreps(str, strvec &, strvec &, strvec &);
int get_clean_input(str, int);
str get_raw_lower_input(str, bool);
int randint(int, int);
str get_rand_sitrep(int);
void get_maps(str, strvec &);
void quick_setup();
void game_loop();
void read_inventory(str);
void read_arsenal(str);
void read_names(str);
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

namespace po = boost::program_options;

//************************************************************************//
//************************************************************************//
int main(int argc, char *argv[]){
  srand(std::time(0));
  str sitrep_file, inv_file, arsenal_file, names_file, maps_dir;

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "Show this help screen.")
    ("verbose,v", "show lots of info")
    ("sitrep-file,s", po::value<str>(&sitrep_file)->default_value("dat/sitreps"), "location of the sitrep data file")
    ("inv-file,i", po::value<str>(&inv_file)->default_value("dat/inv"), "location of the inv data file")
    ("arsenal-file,a", po::value<str>(&arsenal_file)->default_value("dat/arsenal"), "location of the arsenal data file")
    ("names-file,n", po::value<str>(&names_file)->default_value("dat/names"), "location of the names data file")
    ("maps-file,m", po::value<str>(&maps_dir)->default_value("dat/maps"), "location of the maps directory")
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")){
    std::cerr << desc << std::endl;
    return 0;
  }
  
  if (vm.count("verbose")){
    verbose = true;
  }
  int repl = 0;
  if (verbose){
    std::cout << "" << std::endl;
    std::cout << "  reading data" << std::endl;
  }

  read_dictionary("dat/dict", dictionary);
  read_sitreps(sitrep_file, a_sitreps, s_sitreps, m_sitreps);
  read_inventory(inv_file);
  read_arsenal(arsenal_file);
  read_names(names_file);
  get_maps(maps_dir, maps);
  
  std::cout << "" << std::endl;
  std::cout << "  welcome to:" << std::endl;
  std::cout << "    Text Only CounterStrike" << std::endl;
  std::cout << "" << std::endl;
  std::cout << "  would you like to setup a new game, or play a quick game?" << std::endl;
  repl = get_clean_input(pr_str, 0);
  
  switch (repl){
  case 0:
    // new game
    if (verbose){std::cout << "  setting up new game" << std::endl;}
    // TODO
    break;
  case 1:
    // quick game
    if (verbose){std::cout << "  setting up quick game" << std::endl;}
    quick_setup();
    break;
  default:
    // How on earth...?
    break;
  }

  std::cout << "\n  starting game as a ";
  if (players[0]->isCT){
    std::cout << "counter-";
  }
  std::cout << "terrorist, codename: \"" << players[0]->Name
	    << "\" on map: \"" << game_map.Name
	    << "\" with " << players.size() << " players\n" <<  std::endl;
  
  // start game loop!
  game_loop();
  return 0;
}

//************************************************************************//
//************************************************************************//
void game_loop(){
  int status = 0;
  while(status == 0){
    status = game_status();
    // display current cell
    display();
    // get user input
    sort_input(get_clean_input(pr_str, 2));
  }

  switch (status) {
  case -4:
    // Player quit
    break;
  case -3:
    // Time ran out: Counter Terrorists (!Player) win.
  case -2:
    // Bomb went off: Terrorists (!Player) win.
    break;
  case -1:
    // Player died.
    break;
  case 1:
    // All terrorists dead: Counter Terrorists (+Player) win.
    break;
  case 2:
    // Bomb went off: Terrorists (+Player) win.
    break;
  case 3:
    // Time ran out: Counter Terrorists (+Player) win.
    break;
  }
}

//************************************************************************//
//************************************************************************//
void add_message(std::string m_text, double endsat){
  auto m = std::make_tuple(endsat, m_text);
  messages.push_back(m);
}

//************************************************************************//
//************************************************************************//
void add_message_quick(std::string m_text){
  add_message(m_text, game_map.timer - 0.1);
}

//************************************************************************//
//************************************************************************//
void add_message_important(std::string m_text){
  add_message(m_text, game_map.timer - 10.0);
}

//************************************************************************//
//************************************************************************//
void display(){
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
  str inb = "";
  strvec also_here;
  
  for (pplavec::iterator it = players.begin(); it != players.end(); it++, i++){
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
  for (strvec::iterator it = also_here.begin(); it != also_here.end(); it++, i++){
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
int equipped_ammo_left(Player* p){
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

//************************************************************************//
//************************************************************************//
int game_status(){
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
void quick_setup(){
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
void read_names(std::string npath){
  std::string line;
  std::ifstream namf(npath);

  while(namf.good()){
    std::getline(namf, line, '\n');
    //std::cout << "    > read name \"" << line << "\"" << std::endl;
    names.push_back(line);
  }
}

//************************************************************************//
//************************************************************************//
void read_inventory(std::string ipath){
  std::string line;
  std::ifstream invf(ipath);
  Item* t_item;
  
  while(invf.good()){
    t_item = new Item;
    std::getline(invf, line, ',');
    if (verbose){std::cout << "    > added item \"" << line << "\"" << std::endl;}
    t_item->Name = line;
    std::getline(invf, line, '\n');
    t_item->Mod = std::stoi(line);
    global_inventory.push_back(t_item);
  }
}

//************************************************************************//
//************************************************************************//
void read_arsenal(std::string apath){
  std::string line;
  std::ifstream arf(apath);
  Weapon* t_wep;
  
  while(arf.good()){
    t_wep = new Weapon;
    //NAME , TYPE , COST , AMMO COST , DAM , ACC , CAP , ROF
    std::getline(arf, line, ',');
    t_wep->Name = line;
    std::getline(arf, line, ',');
    t_wep->Type = line;
    std::getline(arf, line, ',');
    t_wep->Price = std::stoi(line);
    std::getline(arf, line, ',');
    t_wep->AmmoPrice = std::stoi(line);
    std::getline(arf, line, ',');
    t_wep->Damage = std::stoi(line);
    std::getline(arf, line, ',');
    t_wep->Accuracy = std::stoi(line);
    std::getline(arf, line, ',');
    t_wep->AmmoMax = std::stoi(line);
    t_wep->AmmoLeft = std::stoi(line);
    std::getline(arf, line, '\n');
    t_wep->ROF = std::stoi(line);
    global_arsenal.push_back(t_wep);
    
    if (verbose){std::cout << "    > added weapon \"" << t_wep->Name << "\"" << std::endl;}
  }
}

//************************************************************************//
//************************************************************************//
std::string get_raw_lower_input(std::string prompt){
  std::cout << prompt << std::flush;
  std::string outp;
  std::cin >> outp;
  std::transform(outp.begin(), outp.end(), outp.begin(), ::tolower);
  return outp;
}

//************************************************************************//
//************************************************************************//
void read_dictionary(std::string dictfile, utr &dict){
  int ci, pri;
  std::string us, line;
  
  std::ifstream dictf(dictfile);
  std::stringstream icv;
  
  while (dictf.good()){
    if (verbose){std::cout << "    > added dictionary entry \"" << std::flush;}
    std::getline(dictf, line, ':');
    //std::cout << "| ci " << line << std::endl;
    ci = std::stoi(line);
    std::getline(dictf, line, ':');
    //std::cout << "| us " <<  line << std::endl;
    if (verbose){std::cout << line << "\"" << std::endl;}
    us = line;
    std::getline(dictf, line, '\n');
    //std::cout << "  pi " <<  line << std::endl;
    pri = std::stoi(line);
    dict[std::make_tuple(ci, us)] = pri;
  }
}

//************************************************************************//
//************************************************************************//
void get_maps(std::string map_dir, strvec &maps){
  boost::filesystem::path p(map_dir);
  maps.clear();
  for (auto i = boost::filesystem::directory_iterator(p); i != boost::filesystem::directory_iterator(); i++){
    maps.push_back(i->path().string());
    if (verbose){std::cout << "    > found map \"" << i->path().string() << "\"" << std::endl;}
  }
}

//************************************************************************//
//************************************************************************//
void sort_input(int retv){
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
    case -4:
      add_message_quick(" you stoppped setting the bomb ");
      break;
    case -3:
      add_message_quick(" you are not a terrorist ");
      break;
    case -2:
      add_message_quick(" you do not have a bomb ");
      break;
    case -1:
      add_message_quick(" you are not at a bomb-site ");
      break;
    case 1:
      add_message_important(" !! the bomb has been planted !! ");
      break;
    case 2:
      add_message_quick(" you are planting the bomb ");
      break;
    }
  }
}

//************************************************************************//
//************************************************************************//
Cell* find_cell(int id){
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
int move(Player* p, int direction){
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
int kill_with(Player* p, int weapon, bool iseq){
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
int equip(Player* p, int weapon){
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
void hide(Player* p){
  p->AccMod_in = 0.8;
  p->AccMod_out = 0.6;
}


//************************************************************************//
//************************************************************************//
void camp(Player* p){
  p->AccMod_in = 1.1;
  p->AccMod_out = 1.02;
}

//************************************************************************//
//************************************************************************//
int buy(){
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
void ai_buy(Player* p, int index, int type){
}

//************************************************************************//
//************************************************************************//
int plant(){

  // plant bomb

  // is terrorist?
  // is at bomb site?

  // plant

  // if not planting:
  //   set to planting
  //   increment plant timer by 1.2s

  // if planting and plant timer not at 12s:
  //   increment plant timer by 1.2s

  // if planting and plant timer >= 12s: (i.e. is finished)
  //   start bomb timer for the plant site
  //   set to not planting

  return 2;
  
}

//************************************************************************//
//************************************************************************//
int get_clean_input(std::string prompt, int context){
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
int randint(int max, int min){
  return rand() % max + min;
}

//************************************************************************//
//************************************************************************//
void read_sitreps(std::string sitrepfile, strvec &alone_sitrep, strvec &single_sitrep, strvec &mult_sitrep){
  std::string line;
  
  std::ifstream sitrepf(sitrepfile);
  std::stringstream icv;
  
  while (sitrepf.good()){
    if (verbose){std::cout << "    > added sitrep \"" << std::flush;}
    std::getline(sitrepf, line, ':');

    if (line == "a"){
      std::getline(sitrepf, line, '\n');
      if (verbose){std::cout << line << "\"" << std::endl;}
      alone_sitrep.push_back(line);
    }
    else if (line == "s"){
      std::getline(sitrepf, line, '\n');
      if (verbose){std::cout << line << "\"" << std::endl;}
      single_sitrep.push_back(line);
    }
    else if (line == "m"){
      std::getline(sitrepf, line, '\n');
      if (verbose){std::cout << line << "\"" << std::endl;}
      mult_sitrep.push_back(line);
    }
  }
}

//************************************************************************//
//************************************************************************//
std::string get_rand_sitrep(int type){
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
