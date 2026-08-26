#pragma once

#include <fstream>
#include <string>
#include "csvutils.h"

namespace getLang{
    struct language{
        std::string path;
    };
    struct languages{
        language* languageList;
        int nbrLanguages = 0;
    };
    languages getLanguages();
    bool changeLanguage(readSection::section &currentSection, int language);
}
