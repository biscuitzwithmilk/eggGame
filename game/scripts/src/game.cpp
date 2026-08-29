#include <fstream>
#include <iostream>
#include "windowManager.h"
#include "csvutils.h"
#include "readLanguages.h"
#include "jsonutils.h"
#include "inputMap.h"
using namespace std;
namespace rlib{
    #include <raylib.h>
};

struct PictureAsset{
    rlib::Image image;
    rlib::Texture2D texture;
};
struct BackgroundImage{
    PictureAsset combined;
    PictureAsset depth;
};
PictureAsset LoadImageGeneral(const std::string& name){
    PictureAsset asset;

    std::string image = name+".png";
    asset.image = rlib::LoadImage(image.c_str());
    asset.texture = LoadTextureFromImage(asset.image);
    
    return asset;
};
BackgroundImage LoadBackground(const std::string& name){
    BackgroundImage background;

    std::string depth = name+"_depth";
    background.combined = LoadImageGeneral(name);
    background.depth = LoadImageGeneral(depth);

    return background;
};

void onWindowMove(window &window1, PictureAsset asset){

    window1.transform.scale.Width = rlib::GetScreenWidth();
    window1.transform.scale.Height = rlib::GetScreenHeight();

    window1.toImageRatio.X=asset.image.width/window1.transform.scale.Width; //add current image
    window1.toImageRatio.Y=asset.image.height/window1.transform.scale.Height;
};

int main(){  
    ifstream iFile;
    iFile.open(jsonUtils::getJson("game/json/textSettings.json", "/CurrentLanguage/path"));
    readSection::section currentSection;
    readSection::getToSection(iFile, currentSection, jsonUtils::getJson("game/json/textSettings.json", "/CurrentSection"));
    int totLang;
    int currentLine=jsonUtils::getJson("game/json/textSettings.json", "/CurrentLine");
    int currentlanguage = jsonUtils::getJson("game/json/textSettings.json", "/CurrentLanguage/key");
    window window1;
    rlib::SetConfigFlags(rlib::FLAG_BORDERLESS_WINDOWED_MODE | rlib::FLAG_VSYNC_HINT);
    rlib::SetTargetFPS(window1.targetFPS);
    rlib::InitWindow(window1.transform.scale.Width,window1.transform.scale.Height, "test game");

    while(!input::isQuitting()){
        rlib::BeginDrawing();
        rlib::ClearBackground(rlib::BLACK);
        if(input::isRClickPressed()){
            getLang::getLanguages();
            totLang = jsonUtils::getJson("game/json/textSettings.json", "/TotalLanguages").get<int>();
            currentlanguage+=1;
            if(currentlanguage>totLang-1){
                currentlanguage=0;
            }
            getLang::changeLanguage(currentSection, currentlanguage);
            
        }
        if(input::isLClickPressed()){
            if(currentLine>=currentSection.lines.size()-1){
                currentLine=0;
            }
            currentLine+=1;
            jsonUtils::editJson("game/json/textSettings.json", "/CurrentLine", currentLine);
            cout << currentSection.lines[currentLine].charName+":\n"+currentSection.lines[currentLine].translation << endl;
            
            for (int i=0; i<currentSection.lines[currentLine].effects.size(); i++){
                cout << currentSection.lines[currentLine].effects[i].effectName + ": " + currentSection.lines[currentLine].effects[i].effectedText << endl;
                cout << currentSection.lines[currentLine].effects[i].startPos << endl;
            }
        }
        rlib::Vector2 currentSpecialPos = {180, 220};
        for(int n=0; n<currentSection.lines[currentLine].translation.size(); n++){
            const char *currentSpecialText = currentSection.lines[currentLine].translation.c_str();
            
            int codepointSize = 0;
            int codepoint = rlib::GetCodepoint(&currentSpecialText[n], &codepointSize);
            float fontSize = 20;
            for(int i=0; i<currentSection.lines[currentLine].effects.size(); i++){
                if (currentSection.lines[currentLine].effects[i].effectName=="strong"){

                    if(n >= currentSection.lines[currentLine].effects[i].startPos){
                        fontSize = currentSection.lines[currentLine].effects[i].value;
                    }
                    if(n >= currentSection.lines[currentLine].effects[i].endPos){
                        fontSize = 20;
                    }
                }
            }

            
            rlib::DrawTextCodepoint(rlib::GetFontDefault(), codepoint, currentSpecialPos, fontSize, rlib::RED);
            if(rlib::GetFontDefault().glyphs[rlib::GetGlyphIndex(rlib::GetFontDefault(), codepoint)].advanceX == 0){
                currentSpecialPos.x+= (float)rlib::GetFontDefault().recs[rlib::GetGlyphIndex(rlib::GetFontDefault(), codepoint)].width * (fontSize / rlib::GetFontDefault().baseSize)+2.0f;
            }else{
                currentSpecialPos.x+= (float)rlib::GetFontDefault().glyphs[rlib::GetGlyphIndex(rlib::GetFontDefault(), codepoint)].advanceX * (fontSize / rlib::GetFontDefault().baseSize)+2.0f;
            }
            n += (codepointSize - 1);
        }
        // rlib::DrawText(rlib::TextFormat(currentSection.lines[currentLine].charName.c_str()), 0, 0, 20, rlib::RED);
        // rlib::DrawText(rlib::TextFormat(currentSection.lines[currentLine].translation.c_str()), 0, 20, 20, rlib::RED);
        rlib::EndDrawing();
    }
    rlib::CloseWindow();
    // rlib::SetWindowState(window1.State);

    // BackgroundImage eggImage1;
    // eggImage1 = LoadBackground("egg");


    // Shader shader = LoadShader(NULL, "depthfocus.fs");

    // int subColorLoc = GetShaderLocation(shader, "subtractColor");
    // int spread = GetShaderLocation(shader, "spread");
    // float valueToMultiply = 1.0f;
    // float colorToSubtract[4] = {0.5f, 0.5f, 0.5f, 0.5f};
    // while (isQuitting() == false)
    // {
    //     if(window1.isMoving()){
    //         onWindowMove(window1, eggImage1.depth);
    //     }
        
    //     Color currentColor = GetImageColor(eggImage1.depth.image, GetMouseX()*window1.toImageRatio.X, GetMouseY()*window1.toImageRatio.Y);
    //     colorToSubtract[0] = currentColor.r/255.0f;
    //     colorToSubtract[1] = currentColor.g/255.0f;
    //     colorToSubtract[2] = currentColor.b/255.0f;
        
    //     SetShaderValue(shader, subColorLoc, colorToSubtract, SHADER_UNIFORM_VEC4);
    //     SetShaderValue(shader, spread, &valueToMultiply, SHADER_UNIFORM_FLOAT);
    //     Rectangle sourceRec = {0.0f, 0.0f, float(eggImage1.combined.texture.width), float(eggImage1.combined.texture.height)};
    //     Rectangle destRec = {0.0f,0.0f,float(window1.transform.scale.Width),float(window1.transform.scale.Height)};

    //     BeginDrawing();
    //     ClearBackground(BLACK);
    //     BeginShaderMode(shader);
    //     DrawTexturePro(eggImage1.depth.texture, sourceRec, destRec, {0.0f,0.0f}, 0.0f, WHITE);
    //     EndShaderMode();


    //     EndDrawing();
    // }
    // CloseWindow();
    
    return 0;
}
