# username generator
# appends one noun to another

# imports
from random import randint

noun_file = open("nouns", "r")
nouns = noun_file.readlines()
noun_file.close()

for i in range(0, len(nouns)):
    nouns[i] = nouns[i][:-1]

count = 100
names = open("names", "w")

for i in range(0, count):
    names.write((nouns[randint(0, len(nouns) - 1)] + nouns[randint(0, len(nouns) - 1)] + "\n"))
names.close()
