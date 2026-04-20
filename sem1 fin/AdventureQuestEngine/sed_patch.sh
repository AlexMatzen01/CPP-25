#!/bin/bash
sed -i '/#include "..\/include\/FileIO.h"/r patch_fileio.cpp' src/FileIO.cpp
