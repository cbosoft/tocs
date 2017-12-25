############################################################
############################################################

CXX = g++
CXXFLAGS = -O2 -pedantic -Wall -std=c++14
LIBLINK = -lboost_program_options -lboost_system -lboost_filesystem

############################################################
############################################################

obj/%.o: src/%.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

############################################################
############################################################

GAME=\
obj/game.o\
obj/game_io.o\
obj/game_display.o\
obj/game_actions.o\
obj/game_ai.o

TOCS =\
$(GAME)\
obj/tocs.o\
obj/map.o\
obj/player.o\
obj/item.o\
obj/weapon.o\
obj/cell.o

############################################################
############################################################

clean:
	rm -rf obj/* *~ tocs

tocs: $(TOCS)
	$(CXX) -o $@ $(CXXFLAGS) $(TOCS) $(LIBLINK)
