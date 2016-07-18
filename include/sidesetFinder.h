#include "meshStructures.h"
#include <vector>

#pragma once



std::vector<exoIISideSet> automaticSidesetFinder ( std::vector<exoIIElement> allElements, int minimumSSTag, int maximumSSTag);
exoIISideSetComponent quad4SSScanner ( exoIIElement sideSet, exoIIElement quad4, int elementID, int sideSetID);