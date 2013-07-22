GNU Radio LTE Receiver
================

The *gr-lte* project is an Open Source Software Package which aims to provide a GNU Radio LTE Receiver to receive, synchronize and decode LTE signals. Its development was focused on modularity. This makes the project easy to extend and reuse its blocks.

This project was initiated as a Bachelor thesis at the *Communication Engineering Lab (CEL)* at the *Karlsruhe Institute of Technology (KIT)*, Germany, <http://www.cel.kit.edu>.


Requirements
------------

- GR 3.6.5.1 for message passing functionality
- MATLAB (R2011b tested) for .m-files (for old versions)
If you want, have a look at the m-files


Build/Install instructions for Linux
------------------------------------

1. Get, build and install GNU Radio from <http://www.gnuradio.org>

2. Get *gr-lte* from github

3. Configure it using CMake
Assuming you are already in the project folder
cd code/
mkdir build
cp cmake_install.sh build/
cd build
./cmake_install.sh

This will just run a little script to aid with project setup for Code::Blocks

4. run *sudo make install* 
make -jX where X is a number of choice (Often the number of cores available)
sudo make install

5. Have fun
If everything runs without errors the -gr-lte blocks should be available in GRC