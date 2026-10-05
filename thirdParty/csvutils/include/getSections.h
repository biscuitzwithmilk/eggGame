#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <variant>
#include <map>
#include <unordered_map>
class Effect{
private:
std::variant<int, float, double, std::string> currentValue;
void addDefault(std::string &input);
public:
std::string effectName;
std::variant<int, float, double, std::string> defaultValue;
size_t startPos;
size_t endPos;
void addEffect(std::string &input);
Effect(const std::string name, std::string &input, std::variant<int, float, double, std::string> defaultValueInput);
};
class CSVParser{
private:
std::vector<std::multimap<size_t, std::variant<int, float, double, std::string>>> effectMap;
std::unordered_map<std::string,std::variant<int, float, double, std::string>> effectNamesAndDefaults;
size_t fullSize;
std::string inputCopy;
struct characterDefault{
    const std::string characterName;
    const std::unordered_map<std::string, std::variant<int, float, double, std::string>> defaultEffects;
    
};
void resizeMaps(size_t fullSizeParam);
void setCharacterDefaults(std::vector<characterDefault> &charDefaults, const std::string &currentCharName);
void makeEffectMaps(std::multimap<size_t, std::variant<int, float, double, std::string>>& effectMapParam,
const std::pair<std::string,std::variant<int, float, double, std::string>>& effectNamesAndDefaultsParam,
const std::string& nameToFind);
void findEffects();
void sortEffects();
void createEffects(std::string &input);
void parseCSV(std::string &input, const std::string &currentCharName);
public:
std::vector<Effect> effects;
CSVParser(  const std::unordered_map<std::string,std::variant<int, float, double, std::string>> effectNamesAndDefaultsParam, 
            std::string &input, const std::string &charNameParam);
};
class CSV {
private:
std::ifstream iFile;
struct line {
    std::string key;
    std::string charName;
    std::string context;
    std::string english;
    std::string translation;
    std::vector<Effect> effects;
};
struct section{
    std::string name;
    std::vector<line> lines;
};
void insertOutput(line &currentLine, std::string output);
public:
std::vector<section> sections;
CSV(std::string path);
void getNextSection();
};