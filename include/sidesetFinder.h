#include "meshStructures.h"
#include <vector>

#pragma once



std::vector<exoIISideSet> automaticSidesetFinder ( std::vector< exoIIElement > *allElements, int minimumSSTag, int maximumSSTag, std::vector< exoIIElement > sidesetElements);
exoIISideSetComponent SSScanner ( exoIIElement sideSet, exoIIElement testedElement, int elementID, int sideSetID);
std::vector<std::vector<int>> automaticNodesetFinder ( std::vector<std::vector<double>> *allNodes, int minimumSSTag, int maximumSSTag, std::vector<exoIIElement> nodesetElements);
exoIINodesetComponent NSScanner ( exoIIElement nodeset, std::vector<double> node, int ID);

std::vector<exoIIElement> sideSetExtractor ( std::vector<exoIIElement> originalElementArray, int minimumSSTag, int maximumSSTag );
std::vector<exoIIElement> removeSets ( std::vector<exoIIElement> originalElementArray, int minimumSSTag, int maximumSSTag );
std::vector<exoIIElement> nodeSetExtractor ( std::vector<exoIIElement> originalElementArray, int minimumSSTag, int maximumSSTag );