#pragma once

#include <iostream>
#include <string>

#include "stampbc-lib/project.hpp"

void printUsage();
int processOptions(int argc, char** argv, Project& proj);
int main(int argc, char* argv[]);