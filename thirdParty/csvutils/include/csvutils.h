#pragma once

#include <fstream>
#include <string>
#include <vector>

namespace readSection{
    struct effect{
        std::string effectName;
        std::string effectedText;
        int startPos;
        int endPos;
    };
    struct color{
        std::string colorValue;
        std::string effectedText;
        int startPos;
        int endPos;
    };
    struct line{
        std::string key;
        std::string charName;
        std::string context;
        std::string english;
        std::string translation;
        std::vector<effect> effects;
    };
    
    struct section{
        std::string name;
        std::vector<line> lines;
    };
    
    void ClearAllLineEffects(readSection::line &currentLine);
    void getEffect(readSection::line &currentLine, std::string tag, std::string endTag, std::string &output);
    bool getNextSection(std::ifstream &iFile, readSection::section &currentSection);
    bool getToSection(std::ifstream &iFile, readSection::section &currentSection, const std::string &sectionName);
}
