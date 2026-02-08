# ANALYSIS OF ALGORITHMS LABS

## How to build the labs

The project is in made so that there is a root makefile that builds all the labs in C.
The labs are built with C23 standard. In the current state the project is built with debug information.
Right now in the project there are following dependencies:

- libsrt (For data structures)
- gmp (GNU Multiple Precision Arithmetic Library)

The libraries are not installed by default with cmake. You can install them from source or with a package manager.
The project is meant to be built and run under linux. There is a hard dependency on linux kernel for time measurement.
It may run under other unix-like systems as well. But this is not tested.

## Lab 1 - Study and Empirical Analysis of Algorithms for Determining Fibonacci N-th Term

To get the graphs used in the report you can run the generate.sh under utils/lab_1/generate.sh
For this script to run you will need to have the requirements for python installed which can be found under utils/requirements.txt

## Link to the libraries
- [libsrt](https://faragon.github.io/libsrt.html)
- [libgmp](https://gmplib.org/)
## Link to the template used for the report
[Report Template](https://github.com/The1Dani/AA-REPORT-TEMPLATE/tree/main?tab=readme-ov-file)