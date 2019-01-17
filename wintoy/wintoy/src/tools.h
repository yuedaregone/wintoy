#pragma once
#include <iostream>
#include <vector>
#include <string>

std::string GetHouzhui(std::string p);
std::string GetOnlyNameFromTotalName(std::string name);
std::string GetTotalName(std::string p);
std::string GetPath(std::string p);
std::string GetCurrentPath();
void LoadAllFileNames(const char* path, std::vector<std::string>& files, bool subdir, const char* subfix = NULL);

