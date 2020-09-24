// Port from javascript parser at:
// https://github.com/otto-dev/coordinate-parser

#include "coordinate_parser.h"
#include <cmath>
#include <regex>

coordinate_parser::coordinate_parser(std::string coordinateString)
{
    this->latitude = 0.0;
    this->longitude = 0.0;
    this->coordinates = coordinateString;
    this->validate();
    this->parse();
}

void coordinate_parser::validate() {
    checkContainsNoLetters(this->coordinates);
    checkValidOrientation(this->coordinates);
    checkNumbers(this->coordinates);
}

void coordinate_parser::parse() {
    this->groupCoordinateNumbers();
    this->latitude = this->extractLatitude();
    this->longitude = this->extractLongitude();
}

std::vector<std::string> slice_smatch(std::vector<std::string> input, size_t start, size_t end) {
    std::vector<std::string> output;
    for (size_t i = start; i < end; i++) {
        output.push_back(input.at(i));
    }
    return output;
}

void coordinate_parser::groupCoordinateNumbers() {
    std::vector<std::string> coordinateNumbers = this->extractCoordinateNumbers(this->coordinates);
    unsigned long numberCountEachCoordinate = coordinateNumbers.size() / 2;
    this->latitudeNumbers = slice_smatch(coordinateNumbers, 0, numberCountEachCoordinate);
    this->longitudeNumbers = slice_smatch(coordinateNumbers, coordinateNumbers.size() - numberCountEachCoordinate, coordinateNumbers.size());
}

std::vector<std::string> coordinate_parser::extractCoordinateNumbers(std::string coordinates) {
    std::regex reg("-?\\d+(\\.\\d+)?");
    std::vector<std::string> matches;

    std::sregex_iterator iter(coordinates.begin(), coordinates.end(), reg);
    std::sregex_iterator end;
    while(iter != end) {
        matches.push_back((*iter)[0]);
        iter++;
    }
    return matches;
}

double coordinate_parser::extractLatitude() {
    double latitude = this->coordinateNumbersToDecimal(this->latitudeNumbers);
    if (this->latitudeIsNegative()) {
      latitude = latitude * -1.0;
    }
    return latitude;
}

double coordinate_parser::extractLongitude() {
    double longitude = this->coordinateNumbersToDecimal(this->longitudeNumbers);
    if (this->longitudeIsNegative()) {
      longitude = longitude * -1.0;
    }
    return longitude;
}

double coordinate_parser::coordinateNumbersToDecimal(std::vector<std::string> coordinateNumbers) {
    coordinate_number coordinate(coordinateNumbers);
    coordinate.detectSpecialFormats();
    return coordinate.toDecimal();
}

bool coordinate_parser::latitudeIsNegative() {
    std::regex reg("s", std::regex_constants::icase);
    return std::regex_search(this->coordinates, reg);
}

bool coordinate_parser::longitudeIsNegative() {
    std::regex reg("w", std::regex_constants::icase);
    return std::regex_search(this->coordinates, reg);
}

double coordinate_parser::getLatitude() {
    return this->latitude;
}

double coordinate_parser::getLongitude() {
    return this->longitude;
}

// validation functions - from validator.js
// Used to make sure the input is actually coordinates (or at least looks like coordinates)
void coordinate_parser::checkContainsNoLetters(std::string coordinates) {
    std::regex reg("(?![neswd])[a-z]", std::regex_constants::icase);
    if (std::regex_search(coordinates, reg)) {
        throw std::invalid_argument("Coordinate contains invalid alphanumeric characters");
    }
}

void coordinate_parser::checkValidOrientation(std::string coordinates) {
    std::regex reg("^[^nsew]*[ns]?[^nsew]*[ew]?[^nsew]*$", std::regex_constants::icase);
    if (!std::regex_search(coordinates, reg)) {
        throw std::invalid_argument("Invalid cardinal direction");
    }
}

void coordinate_parser::checkNumbers(std::string coordinates) {
    std::vector<std::string> coordinateNumbers = this->extractCoordinateNumbers(coordinates);
    this->checkAnyCoordinateNumbers(coordinateNumbers);
    this->checkEvenCoordinateNumbers(coordinateNumbers);
    this->checkMaximumCoordinateNumbers(coordinateNumbers);
}

void coordinate_parser::checkAnyCoordinateNumbers(std::vector<std::string> coordinateNumbers) {
    if (coordinateNumbers.size() == 0) {
        throw std::invalid_argument("Could not find any coordinate number");
    }
}

void coordinate_parser::checkEvenCoordinateNumbers(std::vector<std::string> coordinateNumbers) {
    if (coordinateNumbers.size() % 2) {
        throw std::invalid_argument("Uneven count of latitude/longitude numbers");
    }
}

void coordinate_parser::checkMaximumCoordinateNumbers(std::vector<std::string> coordinateNumbers) {
    if (coordinateNumbers.size() > 6) {
        throw std::invalid_argument("Too many coordinate numbers");
    }
}



// Coordinate number functions - from coordinate-number.js
// Used to convert a bunch of numbers (as strings) into a single decimal degrees number
coordinate_number::coordinate_number(std::vector<std::string> coordinateNumbers) {
    std::vector<double> coordinateNumbersDoubles = this->normalizeCoordinateNumbers(coordinateNumbers);
    this->degrees = coordinateNumbersDoubles.size() >= 1 ? coordinateNumbersDoubles.at(0) : 0.0;
    this->minutes = coordinateNumbersDoubles.size() >= 2 ? coordinateNumbersDoubles.at(1) : 0.0;
    this->seconds = coordinateNumbersDoubles.size() >= 3 ? coordinateNumbersDoubles.at(2) : 0.0;
    this->milliseconds = coordinateNumbersDoubles.size() >= 4 ? coordinateNumbersDoubles.at(3) : 0.0;
    this->degreesCanBeSpecial = (coordinateNumbersDoubles.size() < 2);
    this->sign = this->normalizedSignOf(this->degrees);
    this->degrees = std::abs(this->degrees);
}

std::vector<double> coordinate_number::normalizeCoordinateNumbers(std::vector<std::string> coordinateNumbers) {
    std::vector<double> normalizedNumbers;
    for (unsigned int i = 0; i < coordinateNumbers.size(); i++) {
        double num = 0.0;
        try {
            num = std::stod(coordinateNumbers.at(i));
        } catch (...) {}
        normalizedNumbers.push_back(num);
    }
    return normalizedNumbers;
}

double coordinate_number::normalizedSignOf(double number) {
    if (number >= 0) {
      return 1.0;
    } else {
      return -1.0;
    }
}

void coordinate_number::detectSpecialFormats() {
    if (this->degreesCanBeSpecial) {
        if (this->degreesCanBeMilliseconds()) {
            this->degreesAsMilliseconds();
        } else if (this->degreesCanBeDegreesMinutesAndSeconds()) {
            this->degreesAsDegreesMinutesAndSeconds();
        } else if (this->degreesCanBeDegreesAndMinutes()) {
            this->degreesAsDegreesAndMinutes();
        }
    }
}

bool coordinate_number::degreesCanBeMilliseconds() {
    if (this->degrees > 909090) {
        return true;
    } else {
        return false;
    }
}

void coordinate_number::degreesAsMilliseconds() {
    this->milliseconds = this->degrees;
    this->degrees = 0;
}

bool coordinate_number::degreesCanBeDegreesMinutesAndSeconds() {
  if (this->degrees > 9090) {
      return true;
  } else {
      return false;
  }
}

void coordinate_number::degreesAsDegreesMinutesAndSeconds() {
    double newDegrees = std::floor(this->degrees / 10000);
    this->minutes = std::floor((this->degrees - newDegrees * 10000) / 100);
    this->seconds = std::floor(this->degrees - newDegrees * 10000 - this->minutes * 100);
    this->degrees = newDegrees;
}

bool coordinate_number::degreesCanBeDegreesAndMinutes() {
    if (this->degrees > 360) {
        return true;
    } else {
        return false;
    }
}

void coordinate_number::degreesAsDegreesAndMinutes() {
    double newDegrees = std::floor(this->degrees / 100);
    this->minutes = this->degrees - newDegrees * 100;
    this->degrees = newDegrees;
}

double coordinate_number::toDecimal() {
    return this->sign * (this->degrees + this->minutes / 60 + this->seconds / 3600 + this->milliseconds / 3600000);
}
