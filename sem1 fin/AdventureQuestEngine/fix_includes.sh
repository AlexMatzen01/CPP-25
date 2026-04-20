sed -i 's/#include "include\/Types.h"/#include "..\/include\/Types.h"/g' src/FileIO.cpp
sed -i 's/#include "include\/json.hpp"/#include "..\/include\/json.hpp"/g' src/FileIO.cpp
sed -i 's/#include "include\/FileIO.h"//g' src/FileIO.cpp
