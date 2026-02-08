#!/bin/bash
# You need to have cmake installed on the machine with the standard cxx build tools

# This line creates and inits cmake in the build folder
cmake -Bbuild

#This builds the project
make -C build

