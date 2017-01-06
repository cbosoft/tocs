#
# tocs - Text Only Counter Strike
# XKCD # 91 (https://xkcd.com/91/)
#

# imports
from random import randint
from glob import glob


def game_loop(players, weapons, items, ma_p):
    game_over = False
    mesg = ""
    counter = 0
    while (not game_over):
        # draw "graphics"
        # current cell description
        graph_str = ma_p.cells[players[0].loc].name + "\n\n" + ma_p.cells[players[0].loc].desc + "\n\n"
        pop_list = get_cell_pop(players[0].loc, players)
        if (len(pop_list) == 0):
            graph_str += "\n" + get_rand_str(2)
        elif (len(pop_list) == 1):
            graph_str += pop_list[0] + " " + get_rand_str(1)
        else:
            graph_str += pop_list[0]
            for i in range(1, len(pop_list) - 1):
                graph_str += ", " + pop_list[i]
            graph_str += ", and " + pop_list[len(pop_list) - 1] + " " + get_rand_str(0)
        graph_str += "\n"
        if (mesg != ""):
            graph_str += "\n" + mesg
            mesg = ""
        draw_wrapped([graph_str])
        # get input
        inp = get_clean_input("What do?", 5)
        # do input
        mesg = do_game_inp(inp, players, ma_p, counter)
        # do ai players stuff

        # is gameover?
        # if player has no health
        if (players[0].health == 0):
            game_over = True
            mesg = "You died."
        # if bomb went off

        # if all terrorists dead
        run_cnt = 0
        for p in players:
            if (p.team == "t"):
                run_cnt += 1

        if (run_cnt == 0):
            game_over = True
            mesg = "Counter Terrorists Win."

        # if all counter-terrorists dead
        run_cnt = 0
        for p in players:
            if (p.team == "ct"):
                run_cnt += 1

        if (run_cnt == 0):
            game_over = True
            mesg = "Terrorists Win."

        # if mesg==quitquitquit
        if (mesg == "quitquitquit"):
            game_over = True
            mesg = "You quit."

        counter += 1
    draw_wrapped(["Game Over!\n", mesg, "\n Thanks for playing tocs!"])


def do_game_inp(inp, players, ma_p, counter):
    ret_str = ""
    if (inp == "h"):
        # show help
        ret_str = ("\nYou can move in any of the cardinal directions: north, south, east, and west and inbetween them (north east etc).\n\n" +
        "Also you can [kill] an enemy, [plant] a bomb (if you're a terrorist), just [camp], or [buy] equipment (if its the start of the round).")
    elif (inp == "k"):
        # try to kill something
        pass  # TODO
    elif (inp == "p"):
        # plant a bomb
        if (players[0].team == "t"):
            if ("bomb" in ma_p.cells[players[0].loc].spec):
                pass  # TODO
            else:
                ret_str = "This is not the objective! You cannot plant a bomb here."
        else:
            ret_str = "You are not a terrorist. You cannot plant a bomb. Traitor."
    elif (inp == "b"):
        # buy stuff
        if (counter < 3):
            pass  # TODO
    elif (inp == "c"):
        # do nothing
        ret_str = "You're camping."
    elif (inp == "q"):
        # quit
        ret_str = "quitquitquit"
    else:
        if (inp == "n"):
            dr = 0
        elif (inp == "ne"):
            dr = 1
        elif (inp == "e"):
            dr = 2
        elif (inp == "se"):
            dr = 3
        elif (inp == "s"):
            dr = 4
        elif (inp == "sw"):
            dr = 5
        elif (inp == "w"):
            dr = 6
        else:
            dr = 7

        if (ma_p.cells[players[0].loc].can_go_(dr)):
            # player can go
            players[0].loc = int(ma_p.cells[players[0].loc].dirs[dr])
        else:
            # player cannot go
            ret_str = "You cannot go that way!"

    return ret_str


def quick_setup():
    # size medium, ai casual, random map
    set_up_ai(8, 1)
    map_list = get_map_list()
    mp = map()
    mp = mp.from_file(map_list[randint(0, len(map_list) - 1)])


def set_up_ai(number, iq):
    for i in range(0, number):
        if ((i + 1) < (number / 2)):
            a = player(get_random_name(), pcs[0].team, True, iq)
        else:
            if (pcs[0].team == "ct"):
                a = player(get_random_name(), "t", True, iq)
            else:
                a = player(get_random_name(), "ct", True, iq)
        pcs.append(a)


def get_map_list():
    return glob("maps/*")


def get_rand_str(type):
    choices = ["oops"]
    if type == 0:
        # are here too
        choices = ["are here too.", "are uncomfortably close.", "are nearby.", "are skulking about.",
        "are looking at you like you're a fool.", "are doing a little dance routine. They're quite good.", "are close.",
        "are laying about.", "are in formation nearby."]
    elif type == 1:
        # is here too
        choices = ["is here.", "is standing next to you.", "is right behind you! :O",
        "is prone two feet in front of you.", "is at your six!", "is a hot single in your area! ;)", "is skulking about",
        "is nearby", "is here too", "is having a little lie down"]
    elif type == 2:
        # are alone
        choices = ["No-one is here", "You are alone. So alone.", "There is no-one here",
        "Nobody is around, that you can see.", "Area clear.", "You are alone."]

    return choices[randint(0, len(choices) - 1)]


def get_cell_pop(cel_ind, playrs):
    out_nams = [".."]
    for i in range(0, len(playrs)):
        if (playrs[i].loc == cel_ind):
            if (out_nams[0] == ".."):
                out_nams[0] = playrs[i].name + "(" + playrs[i].team.upper() + ")"
            else:
                out_nams.append(playrs[i].name + "(" + playrs[i].team.upper() + ")")
    return out_nams


def get_map_list_nice():
    map_list = glob("maps/*")
    for i in range(0, len(map_list)):
        map_list[i] = map_list[i][5:-4]
    return map_list


def get_map_list_str():
    map_list = glob("maps/*")
    outstr = ""
    for m in map_list:
        outstr += m[5:-4] + "\n"
    return outstr


def get_random_name():
    names_file = open("names.txt")
    names_list = names_file.readlines()
    names_file.close()
    return names_list[randint(0, len(names_list) - 1)][:-1]


def wrap_lines(str_list):
    return str_list


def draw_wrapped(str_list):
    for s in wrap_lines(str_list):
        print s


def get_input(prompt):
    print(("\n" + prompt))
    print((">>>")),
    inp_raw = raw_input()
    return inp_raw


def get_clean_input(prompt, context):
    inp_raw = ""
    isokay = False
    isfirst = True
    while (not isokay):
        inp_raw = get_input(prompt).lower()
        # context is an integer:
            # 0 is quickgame/new game
            #
        isokay = True
        if (context == 0):
            if (inp_raw == "quick game"):
                inp_raw = "qg"
            elif (inp_raw == "quick"):
                inp_raw = "qg"
            elif (inp_raw == "qga"):
                inp_raw = "qg"
            elif (inp_raw == "q"):
                inp_raw = "qg"
            elif (inp_raw == "qui"):
                inp_raw = "qg"
            elif (inp_raw == "just let me frigging play"):
                inp_raw = "qg"
            elif (inp_raw == "play"):
                inp_raw = "qg"
            elif (inp_raw == "play now"):
                inp_raw = "qg"
            elif (inp_raw == "pn"):
                inp_raw = "qg"
            elif (inp_raw == "new game"):
                inp_raw = "ng"
            elif (inp_raw == "newg"):
                inp_raw = "ng"
            elif (inp_raw == "nga"):
                inp_raw = "ng"
            elif (inp_raw == "new"):
                inp_raw = "ng"
            elif (inp_raw == "ne"):
                inp_raw = "ng"
            elif (inp_raw == "new g"):
                inp_raw = "ng"

            if (inp_raw != "qg" and inp_raw != "ng"):
                isokay = False
        if (context == 1):
            if (inp_raw == "small"):
                inp_raw = 4
            elif (inp_raw == "s"):
                inp_raw = 4
            elif (inp_raw == "medium"):
                inp_raw = 8
            elif (inp_raw == "med"):
                inp_raw = 8
            elif (inp_raw == "m"):
                inp_raw = 8
            elif (inp_raw == "large"):
                inp_raw = 16
            elif (inp_raw == "l"):
                inp_raw = 16
            elif (inp_raw == "huge"):
                inp_raw = 96
            elif (inp_raw == "brobdingnagian"):
                inp_raw = 96
            elif (inp_raw == "battlefield"):
                inp_raw = 96
            elif (inp_raw == "bf"):
                inp_raw = 96
            elif (inp_raw == "insane"):
                inp_raw = 96
            elif (inp_raw == "busy"):
                inp_raw = 96

            try:
                if (int(inp_raw) == 4 or int(inp_raw) == 8 or int(inp_raw) == 16 or int(inp_raw) == 96):
                    pass
                else:
                    isokay = False
            except TypeError:
                isokay = False
        if (context == 2):
            if (inp_raw == "dumb"):
                inp_raw = 0
            elif (inp_raw == "d"):
                inp_raw = 0
            elif (inp_raw == "casual"):
                inp_raw = 1
            elif (inp_raw == "casul"):
                inp_raw = 1
            elif (inp_raw == "cas"):
                inp_raw = 1
            elif (inp_raw == "competitive"):
                inp_raw = 2
            elif (inp_raw == "comp"):
                inp_raw = 2
            elif (inp_raw == "cmp"):
                inp_raw = 2
            elif (inp_raw == "dark souls"):
                inp_raw = 3
            elif (inp_raw == "ds"):
                inp_raw = 3
            elif (inp_raw == "hard"):
                inp_raw = 3

            try:
                if (int(inp_raw) == 0 or int(inp_raw) == 1 or int(inp_raw) == 2 or int(inp_raw) == 3):
                    pass
                else:
                    isokay = False
            except TypeError:
                isokay = False

        if (context == 3):
            maps = get_map_list_nice()
            for i in range(0, len(maps)):
                if (inp_raw == maps[i].lower()):
                    inp_raw = i

            try:
                a = int(inp_raw)
                if (a > (len(maps) - 1)):
                    isokay = False
            except TypeError:
                isokay = False

        if (context == 4):
            pass

        if (context == 5):
            # in game
            if (inp_raw == "help"):
                inp_raw = "h"
            elif (inp_raw == "hepl"):
                inp_raw = "h"
            elif (inp_raw == "man tocs"):
                inp_raw = "h"
            elif (inp_raw == "/?"):
                inp_raw = "h"
            elif (inp_raw == "hint"):
                inp_raw = "h"
            elif (inp_raw == "go north"):
                inp_raw = "n"
            elif (inp_raw == "north"):
                inp_raw = "n"
            elif (inp_raw == "nroth"):
                inp_raw = "n"
            elif (inp_raw == "gn"):
                inp_raw = "n"
            elif (inp_raw == "go north east"):
                inp_raw = "ne"
            elif (inp_raw == "northeast"):
                inp_raw = "ne"
            elif (inp_raw == "nrothesat"):
                inp_raw = "ne"
            elif (inp_raw == "north east"):
                inp_raw = "ne"
            elif (inp_raw == "nroth esat"):
                inp_raw = "ne"
            elif (inp_raw == "gne"):
                inp_raw = "ne"
            elif (inp_raw == "go east"):
                inp_raw = "e"
            elif (inp_raw == "east"):
                inp_raw = "e"
            elif (inp_raw == "esat"):
                inp_raw = "e"
            elif (inp_raw == "ge"):
                inp_raw = "e"
            elif (inp_raw == "go south east"):
                inp_raw = "se"
            elif (inp_raw == "southeast"):
                inp_raw = "se"
            elif (inp_raw == "sotuhesat"):
                inp_raw = "se"
            elif (inp_raw == "south east"):
                inp_raw = "se"
            elif (inp_raw == "sotuh esat"):
                inp_raw = "se"
            elif (inp_raw == "gse"):
                inp_raw = "se"
            elif (inp_raw == "go south"):
                inp_raw = "s"
            elif (inp_raw == "south"):
                inp_raw = "s"
            elif (inp_raw == "sotuh"):
                inp_raw = "s"
            elif (inp_raw == "gs"):
                inp_raw = "s"
            elif (inp_raw == "go south west"):
                inp_raw = "sw"
            elif (inp_raw == "southwest"):
                inp_raw = "sw"
            elif (inp_raw == "sotuheswt"):
                inp_raw = "sw"
            elif (inp_raw == "south west"):
                inp_raw = "sw"
            elif (inp_raw == "sotuh eswt"):
                inp_raw = "sw"
            elif (inp_raw == "go west"):
                inp_raw = "w"
            elif (inp_raw == "west"):
                inp_raw = "w"
            elif (inp_raw == "eswt"):
                inp_raw = "w"
            elif (inp_raw == "gw"):
                inp_raw = "w"
            elif (inp_raw == "go north west"):
                inp_raw = "nw"
            elif (inp_raw == "northwest"):
                inp_raw = "nw"
            elif (inp_raw == "nrotheswt"):
                inp_raw = "nw"
            elif (inp_raw == "north west"):
                inp_raw = "nw"
            elif (inp_raw == "nroth eswt"):
                inp_raw = "nw"
            elif (inp_raw == "gnw"):
                inp_raw = "nw"
            elif (inp_raw == "kill"):
                inp_raw = "k"
            elif (inp_raw == "kll"):
                inp_raw = "k"
            elif (inp_raw == "plant bomb"):
                inp_raw = "p"
            elif (inp_raw == "plant"):
                inp_raw = "p"
            elif (inp_raw == "pl"):
                inp_raw = "p"
            elif (inp_raw == "buy"):
                inp_raw = "b"
            elif (inp_raw == "camp"):
                inp_raw = "c"
            elif (inp_raw == "cmp"):
                inp_raw = "c"

            if (inp_raw != "n" and inp_raw != "ne" and inp_raw != "e" and inp_raw != "se" and inp_raw != "s" and inp_raw != "sw"
            and inp_raw != "w" and inp_raw != "nw" and inp_raw != "h" and inp_raw != "k" and inp_raw != "p" and inp_raw != "b"
            and inp_raw != "c" and inp_raw != "q"):
                isokay = False
        if isfirst:
            prompt = "Type properly\n" + prompt
            isfirst = False
    return inp_raw


def load_weapons():
    wep_file = open("arsenal.txt", "r").readlines()
    for wep_str in wep_file:
        w = weapon(wep_str)
        if (weps[0].name == ".."):
            weps[0] = w
        else:
            weps.append(w)


def load_items():
    itm_file = open("inv.txt", "r").readlines()
    for itm_str in itm_file:
        i = equipment(itm_str)
        if (inv[0].name == ".."):
            inv[0] = i
        else:
            inv.append(i)


def spawn():
    cspawn = 0
    tspawn = 0
    for i in range(0, len(mp.cells)):
        if (mp.cells[i].spec == "cspawn"):
            cspawn = i
        elif (mp.cells[i].spec == "tspawn"):
            tspawn = i

    for i in range(0, len(pcs)):
        if (pcs[i].team == "t"):
            pcs[i].loc = tspawn
        else:
            pcs[i].loc = cspawn
        pcs[i].wep = weps[0]


class weapon(object):

    name = ".."
    clas = ".."
    cost = 0
    acc = 70
    dmg = 1

    def __init__(self, strin):
        props = strin.split(",", strin.count(","))
        self.name = props[0]
        self.clas = props[1]
        self.cost = int(props[2])
        self.acc = int(props[3])
        self.dmg = int(props[4])


class player(object):

    # general stuff
    name = ".."
    team = ".."
    health = 100
    max_hp = 100
    money = 500
    loc = 0
    wep = weapon("a,a,1,1,1")

    # ai stuff
    isai = False
    iq = 0

    def __init__(self, name_, team_, isai_=False, iq_=0):
        self.name = name_
        self.team = team_
        self.isai = isai_
        self.iq = iq_


class equipment(object):

    name = ".."
    effect = 50

    def __init__(self, strin):
        props = strin.split(",", 1)
        self.name = props[0]
        self.effect = int(props[1])


class cell(object):

    name = ".."
    desc = ".."
    dirs = [""]
    spec = ""

    def __init__(self, name_="..", desc_="..", dirs_="", spec_=""):
        self.name = name_
        self.desc = desc_
        self.dirs = dirs_.split(",", 7)
        self.spec = spec_

    def can_go_(self, dr):
        try:
            int(self.dirs[dr])
            return True
        except TypeError:
            return False


class map(object):

    name = ".."
    cells = [cell("..", "..", "")]

    def __init__(self):
        pass

    def from_file(self, path):
        map_dat = open(path, "r").readlines()
        map_dat = open("maps/MY_FLAT.txt", "r").readlines()
        reading_cell = False
        ce_ = cell()
        for i in range(0, len(map_dat)):
            cur_str = map_dat[i][:-1]
            if (reading_cell):
                if (cur_str[:5] == "name="):
                    ce_.name = cur_str[5:]
                elif (cur_str[:5] == "desc="):
                    ce_.desc = cur_str[5:]
                elif (cur_str[:5] == "dirs="):
                    ce_.dirs = cur_str[5:].split(",", 7)
                elif (cur_str[:5] == "spec="):
                    ce_.spec = cur_str[5:]
                elif (cur_str[:] == "endcell"):
                    self.add_cell(ce_)
                    ce_ = cell()
                    reading_cell = False
            else:
                if (cur_str[:5] == "name="):
                    self.name = cur_str[5:]
                elif (cur_str[:] == "cell"):
                    reading_cell = True

        return self

    def add_cell(self, cell):
        if (self.cells[0].name == ".."):
            self.cells[0] = cell
        else:
            self.cells.append(cell)

# debug bit
# print(get_map_list_str())

# globals
pcs = [player(get_random_name(), "t")]
mp = map()
weps = [weapon("a,a,1,1,1")]
inv = [equipment("a,1")]

# start
# quick game or set up?
# if quick game:
    # do auto set up
# else:
    # get game size (small=4, medium=8, large=16, insane=96)
    # get ai skill (stupid=0, casual=1, decent=2, assholes=3)
    # get map (DE_DUST etc)

print("Welcome to Text-Only Counterstrike")
inp = get_clean_input("Play quick game, or set up new game?", 0)
if (inp == "qg"):
    quick_setup()
else:
    inp = get_clean_input("What size of game?\nSmall, medium, large, or battlefield?", 1)
    gs = int(inp)
    inp = get_clean_input("How smart should the AI be?\nDumb, casual, competitive, or dark souls?", 2)
    aiq = int(inp)
    inp = get_clean_input("Which map do you want to play on?\n" + get_map_list_str(), 3)
    mp.from_file(get_map_list()[int(inp)])

inp = get_clean_input("Press enter to begin", 4)

# load weapons and items
load_weapons()
load_items()

# spawn players
spawn()

for c in mp.cells:
    print c.dirs

# play game
game_loop(pcs, weps, inv, mp)