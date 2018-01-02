/*
  tocs: Text Only CounterStrike
  
  See xkcd#91 (https://xkcd.com/91)
 */

#include "game.h"

namespace po = boost::program_options;

//************************************************************************//
//************************************************************************//
int main(int argc, char *argv[]){
  Game game;
  srand(std::time(0));
  std::string sitrep_file, inv_file, arsenal_file, names_file, maps_dir;

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help",                                                                                "Show this help screen.")
    ("verbose,v",                                                                           "show lots of info")
    ("sitrep-file,s",  po::value<std::string>(&sitrep_file)->default_value("dat/sitreps"),  "location of the sitrep data file")
    ("inv-file,i",     po::value<std::string>(&inv_file)->default_value("dat/inv"),         "location of the inv data file")
    ("arsenal-file,a", po::value<std::string>(&arsenal_file)->default_value("dat/arsenal"), "location of the arsenal data file")
    ("names-file,n",   po::value<std::string>(&names_file)->default_value("dat/names"),     "location of the names data file")
    ("maps-file,m",    po::value<std::string>(&maps_dir)->default_value("dat/maps"),        "location of the maps directory")
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  if (vm.count("help")){
    std::cerr << desc << std::endl;
    return 0;
  }
  
  if (vm.count("verbose")){
    game.verbose = true;
  }
  int repl = 0;
  if (game.verbose){
    std::cout << "" << std::endl;
    std::cout << "  reading data" << std::endl;
  }
  game.read_dictionary("dat/dict", game.dictionary);
  game.read_sitreps(sitrep_file, game.a_sitreps, game.s_sitreps, game.m_sitreps);
  game.read_inventory(inv_file);
  game.read_arsenal(arsenal_file);
  game.read_names(names_file);
  game.get_maps(maps_dir, game.maps);
  
  std::cout << "" << std::endl;
  std::cout << "  welcome to:" << std::endl;
  std::cout << "    Text Only CounterStrike" << std::endl;
  std::cout << "" << std::endl;
  std::cout << "  would you like to setup a new game, or play a quick game?" << std::endl;
  repl = game.get_clean_input(game.pr_str, 0);
  
  switch (repl){
  case 0:
    // new game
    if (game.verbose){std::cout << "  setting up new game" << std::endl;}
    // TODO
    break;
  case 1:
    // quick game
    if (game.verbose){std::cout << "  setting up quick game" << std::endl;}
    game.quick_setup();
    break;
  default:
    // How on earth...?
    break;
  }

  std::cout << "\n  starting game as a ";
  if (game.players[0]->isCT){
    std::cout << "counter-";
  }
  std::cout << "terrorist, codename: \"" << game.players[0]->Name
	    << "\" on map: \"" << game.game_map.Name
	    << "\" with " << game.players.size() << " players\n" <<  std::endl;
  
  // start game loop!
  game.game_loop();
  return 0;
}
