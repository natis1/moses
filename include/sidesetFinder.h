#include "meshStructures.h"
#include <vector>

#pragma once



std::vector<exoIISideSet> automaticSidesetFinder ( std::vector< exoIIElement >* allElements, int minimumSSTag, int maximumSSTag,
                                                   std::vector< exoIIElement >* sidesetElements, int threads, int elementSize, int dimensions);
exoIISideSetComponent SSScanner ( exoIIElement sideset, exoIIElement testedElement, int elementID, int dimensions);
bool coordinateCompare (std::vector<double> *element1, std::vector<double> *element2, int dimensions);

std::vector<std::vector<int>> automaticNodesetFinder ( std::vector< std::vector< double > >* allNodes, int minimumSSTag, int maximumSSTag, std::vector< exoIIElement > *nodesetElements);
exoIINodesetComponent NSScanner ( exoIIElement nodeset, std::vector<double> node, int ID);

std::vector< exoIIElement > sideSetExtractor ( std::vector< exoIIElement >* originalElementArray, int minimumSSTag, int maximumSSTag );
std::vector<exoIIElement> removeSets ( std::vector< exoIIElement >* originalElementArray, int minimumSSTag, int maximumSSTag );
std::vector<exoIIElement> nodeSetExtractor ( std::vector< exoIIElement >* originalElementArray, int minimumSSTag, int maximumSSTag );


bool fastSSScan ( exoIIElement *sideset, exoIIElement *testedElement );
exoIISideSetComponent TwoDimensionalSSScan (exoIIElement sideset, exoIIElement testedElement, int elementID, int dimensions);
exoIISideSetComponent tetra4Scan (exoIIElement sideset, exoIIElement testedElement, int elementID);
exoIISideSetComponent quad89SSScan (exoIIElement sideset, exoIIElement testedElement, int elementID);