#include <algorithm>
#include "std_includes.h"
#include "map.h"
#include "cell.h"

typedef std::vector<Cell> celvec;

Map::Map(){
  // Blank Constructor
}

Map::~Map(){
  // Destructor
}

Map::Map(std::string mpath){
  // Construct from file:
  read_file(mpath, false);
}

void Map::read_file(std::string mpath, bool verbose){
  std::ifstream mapf(mpath);
  std::string line;
  Cell t_cell;
  
  if (verbose){std::cout << "  reading map \"" << std::flush;}
  
  // Map name
  std::getline(mapf, line, '\n');
  if (verbose){std::cout << line << "\"" << std::endl;}
  Name = line;
    
  while (mapf.good()){  
    // Start reading cell
    std::getline(mapf, line, '\n');
    if (line == "endmap"){break;}
    if (verbose){std::cout << "    > cell \"" << line << "\"" << std::flush;}
    t_cell.Name = line;
    std::getline(mapf, line, '\n');
    std::replace(line.begin(), line.end(), '#', '\n');
    t_cell.Description = line;
    
    t_cell.isCspawn = false;
    t_cell.isTspawn = false;
    t_cell.isBombA = false;
    t_cell.isBombB = false;

    for (int i = 0; i < 7; i++){
      std::getline(mapf, line, ',');
      //std::cout << line << std::endl;
      t_cell.Directions.push_back(std::stoi(line));
    }
    
    std::getline(mapf, line, '\n');
    //std::cout << line << std::endl;
    t_cell.Directions.push_back(std::stoi(line));
    
    while (line != "endcell"){
      std::getline(mapf, line, '\n');
      if (line == "tspawn"){
	t_cell.isTspawn = true;
	if (verbose){std::cout << " [" << line << "]" << std::flush;}
      }
      else if (line == "cspawn"){
	t_cell.isCspawn = true;
	if (verbose){std::cout << " [" << line << "]" << std::flush;}
      }
      else if (line == "bomba"){
	t_cell.isBombA = true;
	if (verbose){std::cout << " [" << line << "]" << std::flush;}
      }
      else if (line == "bombb"){
	t_cell.isBombB = true;
	if (verbose){std::cout << " [" << line << "]" << std::flush;}
      }
    }
    
    if (verbose){std::cout << std::endl;}
    t_cell.ID = Cells.size();
    Cells.push_back(t_cell);
  }
}

int Map::find_T_spawn(){
  int i = 0;
  for (celvec::iterator it = Cells.begin(); it != Cells.end(); it++, i++){
    if (Cells[i].isTspawn){
      return i;
    }
  }
  return -1;
}


int Map::find_C_spawn(){
  int i = 0;
  for (celvec::iterator it = Cells.begin(); it != Cells.end(); it++, i++){
    if (Cells[i].isCspawn){
      return i;
    }
  }
  return -1;
}
