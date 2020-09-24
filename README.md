# cpp-coordinate-parser
Geographical coordinate parser in C++

A C++ port of the coordinate parser written by [otto-dev](https://github.com/otto-dev/coordinate-parser)

### Usage
'''cpp
#include "coordinate_parser.h"

...

std::string coordinate_str = "N40°7’22.8, W74°7’22.8";
double lat = 0.0;
double lon = 0.0;
try {
    coordinate_parser coords(coordinate_str);
    lat = coords.getLatitude();
    lon = coords.getLongitude();
} catch (const std::invalid_argument& e) {
    std::cout << "Error Parsing Coordinates: " << e.what() << "\n";
}

std::cout << "Latitude: " << lat << ", Longitude: " << lon << "\n";
'''
