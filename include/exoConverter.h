//ExoConverter is a method which takes mesh element types and converts them to higher order (or possibly later on lower order) exoII elements.
//
//ExoConverter automatically detects element types.
//
//
//Supported Elements: Line -> Truss, Quad 4 -> Quad 9, Quad 8 -> Quad 9, Triangle 3 -> Triangle 7, Triangle 6 -> Triangle 7
//Prism 15 -> Wedge
//
//This file was created by Eli Stone on July 15th, 2016.
//



#pragma once


#include "meshStructures.h"

#include <iostream>
#include <vector>

exoIIElement elementResolver(std::vector<std::vector<double>> nodeList, std::vector<int> mshElement);