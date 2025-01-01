#include "FileUtilities.hpp"
#include <format>
#define FILE_PATH "POSCODES.CSV"

ImVector<PositionCode> readPositionCodesFromFile()
{
    ImVector<PositionCode> retval;
    std::ifstream positionCodeFile(FILE_PATH);

    if (!positionCodeFile.is_open()) {
        std::cerr << "Error: Unable to open file at " << FILE_PATH << "\n";
        return retval;
    }

    std::string line;

    //SKIP THE HEADER
    if (std::getline(positionCodeFile, line));
    

    // Read each line
    while (std::getline(positionCodeFile, line)) {
        std::istringstream lineStream(line);
        PositionCode code;

        std::string temp;
        if (std::getline(lineStream, temp, ';')) code.x = std::stof(temp);
        if (std::getline(lineStream, temp, ';')) code.y = std::stof(temp);
        if (std::getline(lineStream, temp, ';')) code.z = std::stof(temp);
        if (std::getline(lineStream, temp, ';')) code.levelId = std::stoi(temp);
        if (std::getline(lineStream, temp, ';')) code.title = temp;

        retval.push_back(code);
    }

    return retval;
}

bool writePositionCodesToFile(ImVector<PositionCode>* positionCodes)
{
	if (!positionCodes) // Check if the pointer is valid
	{
		return false;
	}

    try
    {
        if (!std::filesystem::exists(FILE_PATH))
        {
            std::ofstream createFile(FILE_PATH);
            if (!createFile.is_open())
            {
                std::cerr << "Error: Could not create file at " << FILE_PATH << "\n";
                return false;
            }
            createFile.close();
        }

        std::ofstream positionCodeFile(FILE_PATH);
        if (!positionCodeFile.is_open())
        {
            return false;
        }

        positionCodeFile << "X;Y;Z;LEVELID;TITLE\n";

        for (const auto& code : *positionCodes)
        {
            positionCodeFile << std::fixed << std::setprecision(2)
                << code.x << ";" << code.y << ";" << code.z << ";"
                << static_cast<int>(code.levelId) << ";" << code.title << "\n";
        }

        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error writing position codes: " << e.what() << "\n";
        return false;
    }
}
