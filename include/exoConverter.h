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



exoIIElement elementResolver(std::vector< std::vector< double > >* nodeList, std::vector< int > mshElement);
exoIIElement elementConverter(exoIIElement inputElement);

int getMinimumElement(std::vector<exoIIElement> *elements);
int getMaximumElement(std::vector<exoIIElement> *elements);


std::vector<exoIIElementBlock> blockResolver(std::vector< exoIIElement > elements, int dimensions, int elementBlockSize);
std::string blockID(int block, int dimensions);
int nodesPerElement(int block);

std::vector<std::vector<double>> flipNodes(std::vector<std::vector<double>> nodes);



exoIIElement point1ToExPoint(exoIIElement point1Element);
exoIIElement line2ToExTruss(exoIIElement line2Element);
exoIIElement line3ToExTruss(exoIIElement line3Element);
exoIIElement line4ToExTruss(exoIIElement line4Element);
exoIIElement triangle3ToExTri(exoIIElement triangle3Element);
exoIIElement triangle6ToExTri(exoIIElement triangle6Element);
exoIIElement triangle9ToExTri(exoIIElement triangle9Element);
exoIIElement quad4ToExQuad(exoIIElement quad4Element);
exoIIElement quad8ToExQuad(exoIIElement quad8Element);
exoIIElement quad9ToExQuad(exoIIElement quad9Element);
exoIIElement tetra4ToExTetra(exoIIElement tetra4Element);
exoIIElement tetra10ToExTetra(exoIIElement tetra10Element);
exoIIElement hexa8ToExHexa(exoIIElement hexa8Element);
exoIIElement hexa20ToExHexa(exoIIElement hexa20Element);
exoIIElement hexa27ToExHexa(exoIIElement hexa27Element);
exoIIElement prism6ToExWedge(exoIIElement prism6Element);
exoIIElement prism15ToExWedge(exoIIElement prism15Element);
exoIIElement prism18ToExWedge(exoIIElement prism18Element);
exoIIElement pyramid5ToExPyramid(exoIIElement pyramid5Element);
exoIIElement pyramid13ToExPyramid(exoIIElement pyramid13Element);
exoIIElement pyramid14ToExPyramid(exoIIElement pyramid14Element);



std::vector<double> getVectorMidpoint (std::vector<std::vector<double>> points);