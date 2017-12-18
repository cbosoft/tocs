#include "game.h"

//************************************************************************//
//************************************************************************//
void Game::read_names(std::string npath){
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
void Game::read_inventory(std::string ipath){
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
void Game::read_arsenal(std::string apath){
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
std::string Game::get_raw_lower_input(std::string prompt){
  std::cout << prompt << std::flush;
  std::string outp;
  std::cin >> outp;
  std::transform(outp.begin(), outp.end(), outp.begin(), ::tolower);
  return outp;
}

//************************************************************************//
//************************************************************************//
void Game::read_dictionary(std::string dictfile, std::map<std::tuple<int, std::string >, int > &dict){
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
void Game::get_maps(std::string map_dir, std::vector<std::string> &maps){
  boost::filesystem::path p(map_dir);
  maps.clear();
  for (auto i = boost::filesystem::directory_iterator(p); i != boost::filesystem::directory_iterator(); i++){
    maps.push_back(i->path().string());
    if (verbose){std::cout << "    > found map \"" << i->path().string() << "\"" << std::endl;}
  }
}

//************************************************************************//
//************************************************************************//
void Game::read_sitreps(std::string sitrepfile, std::vector<std::string> &alone_sitrep, std::vector<std::string> &single_sitrep, std::vector<std::string> &mult_sitrep){
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
