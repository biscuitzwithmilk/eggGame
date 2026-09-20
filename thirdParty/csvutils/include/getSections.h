#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <variant>
#include "basicutils.h"
namespace rlib{
    #include <raylib.h>
};
namespace Effect{
    struct effect{
        std::string effectName;
        std::vector<basicutils::Param> value;
        basicutils::Param defaultValue;
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
    void addEffect(Section::line &currentLine, const std::string &name, const std::string &endName, std::string &output);
};
