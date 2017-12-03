#include "std_includes.h"
#include "cell.h"

Cell::Cell(){
  // Blank Constructor
}

Cell::~Cell(){
  // Destructor
}

Cell::Cell(int id, std::string name, std::string desc, bool tspawn, bool cspawn, bool bomba, bool bombb, std::vector<int> dirs){
  ID = id;
  Name = name;
  Description = desc;
  isTspawn = tspawn;
  isCspawn = cspawn;
  isBombA = bomba;
  isBombB = bombb;
  Directions = dirs;
}
