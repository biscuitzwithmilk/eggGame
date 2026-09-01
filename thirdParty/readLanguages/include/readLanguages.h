#pragma once

#include <fstream>
#include <string>
#include "getSections.h"
#include <vector>

namespace getLang{
    struct language{
        std::string path;
    };
    struct languages{
        std::vector<language> languageList;
        int nbrLanguages = 0;
    };
    languages getLanguages();
    bool changeLanguage(Section::section &currentSection, int language);
}
