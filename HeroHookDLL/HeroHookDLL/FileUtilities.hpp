#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip> // For std::fixed and std::setprecision
#include <filesystem> // For path validation
#include <stdexcept> // For exception handling
#include "../imGUI/imgui.h"
#include "GameManip.h"

ImVector<PositionCode> readPositionCodesFromFile();
bool writePositionCodesToFile(ImVector<PositionCode>* positionCodes);