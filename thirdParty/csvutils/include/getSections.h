#pragma once

#include <any>
#include <fstream>
#include <string>
#include <vector>
#include <variant>
#include <functional>

namespace Effect{
    struct effect{
        std::string effectName;
        std::string effectedText;
        std::variant<int, float, std::string> value;
        int startPos;
        int endPos;
    };
    
};
namespace Section{
    struct line{
        std::string key;
        std::string charName;
        std::string context;
        std::string english;
        std::string translation;
        std::vector<Effect::effect> effects;
    };
    struct section{
        std::string name;
        std::vector<line> lines;
    };
    bool getNextSection(std::ifstream &iFile, Section::section &currentSection);
    bool getToSection(std::ifstream &iFile, Section::section &currentSection, const std::string &sectionName);
};
namespace Effect{
    void ClearAllLineEffects(Section::line &currentLine);
    void addEffect(Section::line &currentLine, std::string name, std::string endName, std::string &output);
    void onEffect(Section::line &currentLine, const std::string &effectName, const std::function<void(std::string)> func, const std::string params);
    void onFontSize(std::string test);
};
