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
utr dictionary;
Map game_map;
std::vector<Player> players;
std::vector<Item> global_inventory;
std::vector<Weapon> global_arsenal;
strvec a_sitreps;
strvec s_sitreps;
strvec m_sitreps;
strvec maps;
strvec names;
std::string pr_str = "  >";
bool verbose = false;

//************************************************************************//
//** main "header" *******************************************************//
//************************************************************************//
void read_dictionary(std::string, utr &);
void read_sitreps(std::string, strvec &, strvec &, strvec &);
int get_clean_input(std::string, int);
std::string get_raw_lower_input(std::string, bool);
int randint(int, int);
std::string get_randj_statement(int);
void get_maps(std::string, strvec &);
void quick_setup();
void game_loop();
void read_inventory(std::string);
void read_arsenal(std::string);
void read_names(std::string);
bool is_game_over();

namespace po = boost::program_options;

//************************************************************************//
//************************************************************************//
int main(int argc, char *argv[]){
  std::string sitrep_file, inv_file, arsenal_file, names_file, maps_dir;

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "Show this help screen.")
    ("verbose,v", "show lots of info")
    ("sitrep-file,s", po::value<std::string>(&sitrep_file)->default_value("dat/sitreps"), "location of the sitrep data file")
    ("inv-file,i", po::value<std::string>(&inv_file)->default_value("dat/inv"), "location of the inv data file")
    ("arsenal-file,a", po::value<std::string>(&arsenal_file)->default_value("dat/arsenal"), "location of the arsenal data file")
    ("names-file,n", po::value<std::string>(&names_file)->default_value("dat/names"), "location of the names data file")
    ("maps-file,m", po::value<std::string>(&maps_dir)->default_value("dat/maps"), "location of the maps directory")
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
    // TODO
    break;
  case 1:
    // quick game
    quick_setup();
    break;
  default:
    // How on earth...?
    break;
  }

  std::cout << "\n  starting game as a ";
  if (players[0].isCT){
    std::cout << "counter-";
  }
  std::cout << "terrorist, codename: \"" << players[0].Name << "\" on map: \"" << game_map.Name << "\"\n" << std::endl;
  
  // start game loop!
  //game_loop();
  return 0;
}

//************************************************************************//
//************************************************************************//
void game_loop(){
  while(!is_game_over()){
    // display current cell
    // how many people in this cell?
    // describe it appropriately
    // get user input
  }
}

//************************************************************************//
//************************************************************************//
bool is_game_over(){
  return false;
}

//************************************************************************//
//************************************************************************//
void quick_setup(){
  game_map.read_file(maps[randint(maps.size(), 0)], verbose);

  int nop = randint(5, 3);
  bool oe_team = (randint(2,1) == 2);
  for (int i = 0; i < nop; i++){
    players.push_back(Player());
    players[i].isCT = oe_team;
    players[i].Name = names[randint(names.size(), 0)];
  }
  for (int i = 0; i < nop; i++){
    players.push_back(Player());
    players[i + nop].isCT = oe_team;
    players[i + nop].Name = names[randint(names.size(), 0)];
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
  Item t_item;
  
  while(invf.good()){
    std::getline(invf, line, ',');
    if (verbose){std::cout << "    > added item \"" << line << "\"" << std::endl;}
    t_item.Name = line;
    std::getline(invf, line, '\n');
    t_item.Mod = std::stoi(line);
    global_inventory.push_back(t_item);
  }
}

//************************************************************************//
//************************************************************************//
void read_arsenal(std::string apath){
  std::string line;
  std::ifstream arf(apath);
  Weapon t_wep;
  
  while(arf.good()){
    std::getline(arf, line, ',');
    if (verbose){std::cout << "    > added weapon \"" << line << "\"" << std::endl;}
    t_wep.Name = line;
    std::getline(arf, line, ',');
    t_wep.Type = line;
    std::getline(arf, line, ',');
    t_wep.Price = std::stoi(line);
    std::getline(arf, line, ',');
    t_wep.Accuracy = std::stoi(line);
    std::getline(arf, line, '\n');
    t_wep.Damage = std::stoi(line);
    global_arsenal.push_back(t_wep);
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
int get_clean_input(std::string prompt, int context){
  std::string inp_raw;
  bool isokay = false, isfirst = true;
  int retv = -1;
  
  while (! isokay){
    if (isfirst){
      inp_raw = get_raw_lower_input(prompt);
    }
    else {
      
      inp_raw = get_raw_lower_input("( ? ) " + prompt);
    }

    retv = dictionary[std::make_tuple(context, inp_raw)];

    if (retv > 0){
      isokay = true;
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
  srand(time(NULL));
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
