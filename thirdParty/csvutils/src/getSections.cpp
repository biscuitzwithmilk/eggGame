#include <exception>
#include <iostream>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <variant>
#include <vector>
#include "getSections.h"
#include "jsonutils.h"

void Effect::ClearAllLineEffects(Section::line &currentLine){
    currentLine.effects.clear();
}
void Effect::addEffect(Section::line &currentLine, std::string name, std::string endName, std::string &output){
    if (name.empty() || endName.empty() || output.empty()) {
        return;
    }

    Effect::effect currentEffect;
    std::string tagName = name.substr(1, name.length() - 2);
    std::string namePrefix = name.substr(0, name.length() - 1) + "=";
    std::string defaultPrefix = name.substr(0, name.length() - 1) + "_default=";
    std::variant<int, float, std::string> currentValue;

    while (output.find(tagName) != std::string::npos) {
        if (output.find(endName) == std::string::npos) {
            return;
        }

        size_t openStart = std::string::npos;
        size_t openEnd = std::string::npos;
        size_t openDefStart = std::string::npos;
        size_t openDefEnd = std::string::npos;
        size_t openDefLen = 0;
        currentEffect.effectName = tagName;
        currentEffect.value.clear();

        size_t defaultPos = output.find(defaultPrefix);
        if (defaultPos != std::string::npos) {
            openDefStart = defaultPos;
            openDefEnd = output.find(">", openDefStart);
            if (openDefEnd == std::string::npos) {
                return;
            }
            openDefLen = openDefEnd - openDefStart + 1;
            std::string defaultValueText = output.substr(openDefStart + defaultPrefix.length(), openDefEnd - (openDefStart + defaultPrefix.length()));
            try {
                currentEffect.defaultValue = std::stof(defaultValueText);
            } catch (std::exception) {
                currentEffect.defaultValue = defaultValueText;
            }
            output.erase(openDefStart, openDefLen);
        }

        size_t valuePos = output.find(namePrefix);
        if (valuePos != std::string::npos) {
            openStart = valuePos;
            openEnd = output.find(">", openStart);
            if (openEnd == std::string::npos) {
                return;
            }
            std::string valueText = output.substr(openStart + namePrefix.length(), openEnd - (openStart + namePrefix.length()));
            try {
                currentValue = std::stof(valueText);
            } catch (std::exception) {
                currentValue = valueText;
            }
        } else {
            openStart = output.find(name);
            if (openStart == std::string::npos) {
                return;
            }
            openEnd = openStart + name.length() - 1;
            currentValue = currentEffect.defaultValue;
        }

        size_t openTagLen = openEnd - openStart + 1;
        currentEffect.startPos = static_cast<int>(openStart);

        output.erase(openStart, openTagLen);

        size_t closeStart = output.find(endName);
        if (closeStart == std::string::npos) {
            return;
        }

        currentEffect.endPos = static_cast<int>(closeStart);
        size_t closeTagLen = endName.length();
        output.erase(closeStart, closeTagLen);

        for (size_t i = 0; i < currentLine.effects.size(); ++i) {
            currentLine.effects[i].value.clear();
            if (currentLine.effects[i].startPos > static_cast<int>(openStart)) {
                currentLine.effects[i].startPos -= (currentLine.effects[i].startPos > static_cast<int>(closeStart)) ? static_cast<int>(openTagLen + closeTagLen) : static_cast<int>(openTagLen);
                currentLine.effects[i].value.reserve(openTagLen + closeTagLen + openDefLen);
            }
            if (currentLine.effects[i].endPos > static_cast<int>(openStart)) {
                currentLine.effects[i].endPos -= (currentLine.effects[i].endPos > static_cast<int>(closeStart)) ? static_cast<int>(openTagLen + closeTagLen) : static_cast<int>(openTagLen);
                currentLine.effects[i].value.reserve(openTagLen + closeTagLen + openDefLen);
            }

        }

        for (size_t n = 0; n < output.size(); ++n) {
            if (n < static_cast<size_t>(currentEffect.startPos) || n >= static_cast<size_t>(currentEffect.endPos)) {
                currentEffect.value.push_back(currentEffect.defaultValue);
            } else {
                currentEffect.value.push_back(currentValue);
            }
        }


        currentLine.effects.push_back(currentEffect);
        continue;
    }
}
bool Section::getNextSection(std::ifstream &iFile, Section::section &currentSection){
    std::string output;
    std::string column;
    Section::line currentLine;
    Effect::effect currentEffect;

    if (!iFile.is_open()){
        std::cerr << "getNextSection : Failed to open file\n";
        return false;
    }

    if (!std::getline(iFile, output)){
        std::cerr << "getNextSection : Failed to get line / cannot read new Section\n";
        return false;
    }

    currentSection.name = output;
    jsonUtils::editJson("game/json/textSettings.json", "/CurrentSection", output);

    while (std::getline(iFile, output)){
        if (output.find("//endsection") != std::string::npos){
            break;
        };
        if (output.empty()){
            continue;
        };
        //separate and insert output
        std::stringstream rowStream(output);
        std::getline(rowStream, column, ',');
        currentLine.key = column;
        std::getline(rowStream, column, ',');
        currentLine.charName = column;
        std::getline(rowStream, column, ',');
        currentLine.context = column;
        std::getline(rowStream, column, ',');
        currentLine.english = column;
        std::getline(rowStream, column, ',');
        Effect::ClearAllLineEffects(currentLine);
        Effect::addEffect(currentLine, "<shakey>", "</shakey>", column);
        Effect::addEffect(currentLine, "<fontSize>", "</fontSize>", column);
        Effect::addEffect(currentLine, "<slow>", "</slow>", column);
        Effect::addEffect(currentLine, "<color>", "</color>", column);
        Effect::addEffect(currentLine, "<bold>", "</bold>", column);
        
        currentLine.translation = column;
        if(jsonUtils::getJson("game/json/debug.json", "/nextSectionDebug") == true){
            std::cout << currentLine.key << ", ";
            std::cout << currentLine.charName << ", ";
            std::cout << currentLine.context << ", ";
            std::cout << currentLine.english << ", ";
            std::cout << currentLine.translation << "\n";
            
        }
        currentSection.lines.push_back(currentLine);
    };
    for(int currentLine=0; currentLine<currentSection.lines.size(); currentLine++){
        for(int effect=0; effect<currentSection.lines[currentLine].effects.size();effect++){
            std::cout << "effect : \n" << currentSection.lines[currentLine].translation << ", ";
            std::cout << currentSection.lines[currentLine].effects[effect].effectName << ", ";
            std::cout << currentSection.lines[currentLine].effects[effect].startPos << ", ";
            std::cout << currentSection.lines[currentLine].effects[effect].endPos << "\n";
        }
    }

    if (static_cast<int>(currentSection.lines.size()) == 0){
        std::cerr << "getNextSection : Failed to get to section\n";
        return false;
    }
    return true;
}
bool Section::getToSection(std::ifstream &iFile, Section::section &currentSection, const std::string &sectionName){
    if (!iFile.is_open()){
        return false;
    }
    iFile.clear();
    while (currentSection.name != sectionName && iFile.good()){
        if(!Section::getNextSection(iFile, currentSection)){
            return false;
        }
    }
    return true;


    

}

