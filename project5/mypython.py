#!/usr/bin/env python

# Author: Tyler McGinnis
# CS 344
# Spring 2016
# Program 5

import string
import random

def create_file(file_name, file_contents):
    with open(file_name, "w+") as f:
        f.write(file_contents + "\n")

def print_file(file_name):
    with open(file_name, "r") as f:
        print "%s contents:\n%s" % (file_name, f.read())

def create_random_string(string_length):
    # http://codereview.stackexchange.com/questions/47529/creating-a-string-of-random-characters
    return ''.join(random.choice(string.ascii_lowercase) for c in range(string_length))

def main():
    file_names = ["result_file1", "result_file2", "result_file3"]
    for name in file_names:
        create_file(name, create_random_string(10))
        print_file(name)

    random_numbers = (random.randint(1,42), random.randint(1,42))

    # http://stackoverflow.com/questions/522563/accessing-the-index-in-python-for-loops
    for i, number in enumerate(random_numbers):
        print "Random number %i: %i" %(i+1, number)

    product = random_numbers[0] * random_numbers[1]
    print "Product of random numbers: %i" % product

if __name__=="__main__":
    main()
