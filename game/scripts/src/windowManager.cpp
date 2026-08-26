#include <iostream>
#include <raylib.h>
#include "windowManager.h"
using namespace std;

bool window::isMoving(){
    return GetWindowPosition().x!=0.0f || GetWindowPosition().y!=0.0f;
}
