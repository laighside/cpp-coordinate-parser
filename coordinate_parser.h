// Port from javascript parser at:
// https://github.com/otto-dev/coordinate-parser

#ifndef COORDINATE_PARSER_H
#define COORDINATE_PARSER_H

#include <string>
#include <vector>

class coordinate_parser
{
public:
    coordinate_parser(std::string coordinateString);
    double getLatitude();
    double getLongitude();

private:
    double latitude;
    double longitude;
    std::string coordinates;
    std::vector<std::string> latitudeNumbers;
    std::vector<std::string> longitudeNumbers;

    void validate();
    void parse();
    void groupCoordinateNumbers();
    std::vector<std::string> extractCoordinateNumbers(std::string coordinates);
    double extractLatitude();
    double extractLongitude();
    double coordinateNumbersToDecimal(std::vector<std::string> coordinateNumbers);
    bool latitudeIsNegative();
    bool longitudeIsNegative();

    // validation functions
    void checkContainsNoLetters(std::string coordinates);
    void checkValidOrientation(std::string coordinates);
    void checkNumbers(std::string coordinates);
    void checkAnyCoordinateNumbers(std::vector<std::string> coordinateNumbers);
    void checkEvenCoordinateNumbers(std::vector<std::string> coordinateNumbers);
    void checkMaximumCoordinateNumbers(std::vector<std::string> coordinateNumbers);

};

class coordinate_number
{
public:
    coordinate_number(std::vector<std::string> coordinateNumbers);
    void detectSpecialFormats();
    double toDecimal();

private:
    double sign;
    double degrees;
    double minutes;
    double seconds;
    double milliseconds;
    bool degreesCanBeSpecial;

    std::vector<double> normalizeCoordinateNumbers(std::vector<std::string> coordinateNumbers);
    double normalizedSignOf(double number);
    bool degreesCanBeMilliseconds();
    void degreesAsMilliseconds();
    bool degreesCanBeDegreesMinutesAndSeconds();
    void degreesAsDegreesMinutesAndSeconds();
    bool degreesCanBeDegreesAndMinutes();
    void degreesAsDegreesAndMinutes();
};

#endif // COORDINATE_PARSER_H
