//ExoConverter contains a bunch of functions in the following format:
//
//[type]toex[type]
//

//NOTE, while normal human readable code is zero indexed, exodus is 1 indexed because
//the people behind fortran, and I say this loving fortran (as much as I hate it) since
//it is a million times better than assembly, didn't know that zero indexing would ever exist.
//
//Unrelatedly if they ever release moon lander/mars rover code from a newer spacecraft with fortran 77 on it I would love NASA forever.
//And then I would turn my raspberry pi into a rocket ship powered by openMPI with a heat shield made from toast.
//Bonus points if it's even more modern and written in C or CXX. The toast that is.



#include "exoConverter.h"

//algorithm includes swap, a function which lets me swap vector values I think maybe hopefully)
#include <algorithm>

using namespace std;

exoIIElement elementResolver(vector<vector<double>> *nodeList, vector<int> mshElement) {
  exoIIElement exoElement;
  
  exoElement.elementType = mshElement[0];
  //AFAIK only the first tag matters for what I am trying to do.
  exoElement.elementTag  = mshElement[2];
  
  int nodes = mshElement.size() - 2 - mshElement[1]; //Size of mshElement minus the tags
  
  for (int i = 0; i < nodes; i++){
    exoElement.nodeIDs.push_back(mshElement[2 + mshElement[1] + i]);
    exoElement.nodalCoordinates.push_back(nodeList->at( exoElement.nodeIDs[i] - 1 ));
  }
  
  return exoElement;
}


vector<exoIIElementBlock> blockResolver(vector<exoIIElement> elements, int dimensions, int elementBlockSize) {
  vector<exoIIElementBlock> blocks;
  int max = getMaximumElement(&elements);
  int min = getMinimumElement(&elements);
  
  
  
  int neededBlocks = ((((max - min) / elementBlockSize) + 1) * 19);
  blocks.reserve(neededBlocks);
  
  cout << neededBlocks << "blocks needed" << endl;
  
  

  
  vector<exoIIElementBlock> temporaryBlocks;
  temporaryBlocks.reserve(neededBlocks);
  for (int i = 0; i < neededBlocks; i++){
    temporaryBlocks.push_back(exoIIElementBlock{blockID( (i % 19), dimensions), nodesPerElement(i % 19)});
  }
  for (int i = 0; i < elements.size(); i++) {
    temporaryBlocks[ elements[i].elementType - 1 + (((elements[i].elementTag - min) / elementBlockSize) * 19) ].elements.push_back(elements[i]);
  }
  
  
  for (int i = 0; i < temporaryBlocks.size(); i++) {
    if (temporaryBlocks[i].elements.size() != 0 ) {
      blocks.push_back(temporaryBlocks[i]);
    }
  }
    
  return blocks;
}

int getMinimumElement(vector<exoIIElement> *elements){
  int minimum = 1073741824;
  for (int i = 0; i < elements->size(); i++){
    if (minimum > elements->at(i).elementTag){
      minimum = elements->at(i).elementTag;
    }
  }
  return minimum;
  
}

int getMaximumElement(vector<exoIIElement> *elements){
  
  int maximum = 0;
  for (int i = 0; i < elements->size(); i++){
    if (maximum < elements->at(i).elementTag){
      maximum = elements->at(i).elementTag;
    }
  }
  return maximum;
  
}


string blockID(int block, int dimensions) {
  
  
  //Geometric shape is defined in the exodus manual
  //http://gsjaardema.github.io/seacas/exodusII-new.pdf
  //The order, however, follows gmsh.
  //Keep in mind the number here is zero indexed while the number gmsh uses is 1 indexed
  switch (block) {
    //first order
    case 0:
      if (dimensions == 1) return "TRUSS2";//               2/Trusses
      else if (dimensions == 2) return "SHELL2";
      else if (dimensions == 3) return "INVLD_ELEMNT_3DTRUSS";// This cannot exist
      
    case 1:
      if (dimensions == 1) return "INVLD_ELEMNT_1DTRI";// 1D triangles cannot exist, silly. At least not in this universe 🌠
      else if (dimensions == 2) return "TRIANGLE3";
      else if (dimensions == 3) return "INVLD_ELEMNT_3DTRI";// return "TRIANGLE3";//             3/Triangles
    case 2:
      if (dimensions == 1) return "INVLD_ELEMNT_1DQUAD";
      else if (dimensions == 2) return "QUAD4";// Quad meshes
      else if (dimensions == 3) return "SHELL4";// 3D shells
    case 3:
      if (dimensions == 1) return "INVLD_ELEMNT_1DTETRA";
      else if (dimensions == 2) return "INVLD_ELEMNT_2DTETRA";// Tetrahedrons are exclusively 3D
      else if (dimensions == 3) return "TETRA4";
    case 4:
      if (dimensions == 1) return "INVLD_ELEMNT_1DHEXA";
      else if (dimensions == 2) return "INVLD_ELEMNT_2DHEXA";
      else if (dimensions == 3) return "HEX8";//I think 8 is the minimum element size of a hexahedron
    case 5:
      if (dimensions == 1) return "INVLD_ELEMNT_1DWEDGE";
      else if (dimensions == 2) return "INVLD_ELEMNT_2DWEDGE";
      else if (dimensions == 3) return "WEDGE6";
      return "WEDGE";//                6/Prisms
    case 6:
      if (dimensions == 1) return "INVLD_ELEMNT_1DPYRA";
      else if (dimensions == 2) return "INVLD_ELEMNT_2DPYRA";
      else if (dimensions == 3) return "PYRAMID5";
      
    //second order
    case 7:
      if (dimensions == 1) return "TRUSS3";
      else if (dimensions == 2) return "SHELL3";
      else if (dimensions == 3) return "INVLD_ELEMNT_3DTRUSS";
    case 8:
      if (dimensions == 1) return "INVLD_ELEMNT_1DTRI";
      else if (dimensions == 2) return "TRIANGLE6";
      else if (dimensions == 3) return "INVLD_ELEMNT_3DTRI";
    case 9:
      if (dimensions == 1) return "INVLD_ELEMNT_1DQUAD";
      else if (dimensions == 2) return "QUAD9";
      else if (dimensions == 3) return "SHELL9";
    case 10:
      if (dimensions == 1) return "INVLD_ELEMNT_1DTETRA";
      else if (dimensions == 2) return "INVLD_ELEMNT_2DTETRA";
      else if (dimensions == 3) return "TETRA10";
    case 11:
      if (dimensions == 1) return "INVLD_ELEMNT_1DHEXA";
      else if (dimensions == 2) return "INVLD_ELEMNT_2DHEXA";
      else if (dimensions == 3) return "HEX27";
    case 12:
      if (dimensions == 1) return "INVLD_ELEMNT_1DWEDGE";
      else if (dimensions == 2) return "INVLD_ELEMNT_2DWEDGE";
      else if (dimensions == 3) return "WEDGE18";
    case 13:
      if (dimensions == 1) return "INVLD_ELEMNT_1DPYRA";
      else if (dimensions == 2) return "INVLD_ELEMNT_2DPYRA";
      else if (dimensions == 3) return "PYRAMID14";//Pyramid isn't supported in like any finite element program but it is in Gmsh.
    case 14:
      //Not a bug, there is no real way for someone to specify circle radius in Gmsh. It's just impossible.
      //If they ever add floating points to elements (such as a floating point tag) I can add this.
      return "CIRCLE_NOT_SUPPORTED";
    case 15:
      if (dimensions == 1) return "INVLD_ELEMNT_1DQUAD";
      else if (dimensions == 2) return "QUAD8";//Second order without centerpoint
      else if (dimensions == 3) return "SHELL8";
    case 16:
      if (dimensions == 1) return "INVLD_ELEMNT_1DHEXA";
      else if (dimensions == 2) return "INVLD_ELEMNT_2DHEXA";
      else if (dimensions == 3) return "HEX20";//Second order without faces
    case 17:
      if (dimensions == 1) return "INVLD_ELEMNT_1DWEDGE";
      else if (dimensions == 2) return "INVLD_ELEMNT_2DWEDGE";
      else if (dimensions == 3) return "WEDGE15";
    case 18:
      if (dimensions == 1) return "INVLD_ELEMNT_1DPYRA";
      else if (dimensions == 2) return "INVLD_ELEMNT_2DPYRA";
      else if (dimensions == 3) return "PYRAMID13";
  }
  if (dimensions < 1 || dimensions > 3) {
    cout << "You are seeing this because you think you are so clever.\n"
    "You think that you can mesh in " << dimensions << " dimensions.\n"
    "Well, you can't, exodus is 1-3D. Moses is exiting to prevent segfault." << endl;
    exit(1);
  }
  
  return "UNKNOWN_ELEM";
  
  
}

int nodesPerElement(int block) {
  
  switch (block) {
    //first order
    case 0: return   2;//         Trusses
    case 1: return   3;//         Triangles
    case 2: return   4;//         Quadrangles
    case 3: return   4;//         Tetrahedrons
    case 4: return   8;//         Hexahedrons
    case 5: return   6;//         Prisms
    case 6: return   5;//         Pyramids
    //second order
    case 7: return   3;
    case 8: return   6;
    case 9: return   9;
    case 10: return 10;
    case 11: return 27;
    case 12: return 18;
    case 13: return 14;
    case 14: return  1;//         TODO add circle support
    case 15: return  8;
    case 16: return 20;
    case 17: return 15;
    case 18: return 13;
    //third+ order
    case 19: return  9;//         incomplete
    case 20: return 10;
    case 21: return 12;//         incomplete
    case 22: return 15;//         incomplete
    case 23: return 15;
    case 24: return 21;
    case 25: return  4;
    case 26: return  5;
    case 27: return  6;
    case 28: return 20;
    case 29: return 35;
    case 30: return 56;
    
  }
  
}


exoIIElement elementConverter(exoIIElement inputElement) {
  switch (inputElement.elementType) {
    case  1: return line2ToExTruss(inputElement);
    case  2: return triangle3ToExTri(inputElement);
    case  3: return quad4ToExQuad(inputElement);
    case  4: return tetra4ToExTetra(inputElement);
    case  5: return hexa8ToExHexa(inputElement);
    case  6: return prism6ToExWedge(inputElement);
    case  7: return pyramid5ToExPyramid(inputElement);
    case  8: return line3ToExTruss(inputElement);
    case  9: return triangle6ToExTri(inputElement);
    case 10: return quad9ToExQuad(inputElement);
    case 11: return tetra10ToExTetra(inputElement);
    case 12: return hexa27ToExHexa(inputElement);
    case 13: return prism18ToExWedge(inputElement);
    case 14: return pyramid14ToExPyramid(inputElement);
    case 15: return point1ToExPoint(inputElement);
    case 16: return quad8ToExQuad(inputElement);
    case 17: return hexa20ToExHexa(inputElement);
    case 18: return prism15ToExWedge(inputElement);
    case 19: return pyramid13ToExPyramid(inputElement);
    //Not supported: incomplete triangles (though there is some code for allocating them if you are crazy enough to want them.
  }
  
  cout << "Your current mesh element is not supported" << endl;
  cout << "The element I attempted to process was element: " << inputElement.elementType << endl;
  exit(1);
}


vector<vector<double>> flipNodes(vector<vector<double>> nodes) {
  vector<vector<double>> flippedNodes;
  flippedNodes.reserve(nodes[0].size());
  
  for (int i = 0; i < nodes[0].size(); i++) {
    vector<double> v;
    v.reserve(nodes.size());
    for (int j = 0; j < nodes.size(); j++) {
      v.push_back(nodes[j][i]);
    }
    flippedNodes.push_back(v);
    
  }
  
  return flippedNodes;
}

//Simplest 1D element
exoIIElement point1ToExPoint(exoIIElement point1Element) {
  return point1Element;
}

exoIIElement line2ToExTruss(exoIIElement line2Element) {
  return line2Element;
}

exoIIElement line3ToExTruss(exoIIElement line3Element) {
  return line3Element;
}

//Again, it is impossible to mess up a higher order 2D element.
exoIIElement line4ToExTruss(exoIIElement line4Element) {
  return line4Element;
}


exoIIElement triangle3ToExTri(exoIIElement triangle3Element) {
  //It is literally impossible to mess up the order of a triangle 3
  //Because all triangle 3s are the same, just rotated no matter which node you label first.
  return triangle3Element;
}

exoIIElement triangle6ToExTri(exoIIElement triangle6Element) {
  
  
  return triangle6Element;
}

exoIIElement triangle9ToExTri(exoIIElement triangle9Element) {
  
  //Also works for complete triangle 10
  return triangle9Element;
}


exoIIElement quad4ToExQuad(exoIIElement quad4Element) {
  return quad4Element;
}

//Quad 8/9 happen to be the same
exoIIElement quad8ToExQuad(exoIIElement quad8Element) {
  return quad8Element;
}

exoIIElement quad9ToExQuad(exoIIElement quad9Element) {
  return quad9Element;
}

//Hip Hip horray for the world's simplest 3D shape.
//In it's simplicity, it is again impossible to "mess up" the higher order stuff.
exoIIElement tetra4ToExTetra(exoIIElement tetra4Element) {
  return tetra4Element;
}


//All the easy conversions are done. Onto the hard stuff :'(
exoIIElement tetra10ToExTetra(exoIIElement tetra10Element) {
  // Order: 0, 1, 2, 3, 4, 5, 6, 7, 9, 8
  swap (tetra10Element.nodalCoordinates[8], tetra10Element.nodalCoordinates[9]);
  swap (tetra10Element.nodeIDs[8], tetra10Element.nodeIDs[9]);
  
  return tetra10Element;
}

//What is wrong with Gmsh?
exoIIElement hexa8ToExHexa(exoIIElement hexa8Element) {
  return hexa8Element;
}


exoIIElement hexa20ToExHexa(exoIIElement hexa20Element) {
  // Order: 0, 1, 2, 3, 4, 5, 6, 7, 8, 11, 13, 9, 10, 12, 14, 15, 16, 18, 19, 17
  exoIIElement exHexa;
  exHexa.nodalCoordinates.reserve(20);
  exHexa.nodeIDs.reserve(20);
  exHexa.elementTag = hexa20Element.elementTag;
  exHexa.elementType = hexa20Element.elementType;
  
  for (int i = 0; i < 8; i++){
    exHexa.nodalCoordinates.push_back(hexa20Element.nodalCoordinates[i]);
    exHexa.nodeIDs.push_back(hexa20Element.nodeIDs[i]);
  }
  //There's probably a more elegant solution from a "number of lines" persepctive
  //But cycles wise this is not only the fastest way but also the fastest method to code.
  //Plus it makes this file much more compressable via lzma and others.
  exHexa.nodalCoordinates.push_back(hexa20Element.nodalCoordinates[11]);
  exHexa.nodeIDs.push_back(hexa20Element.nodeIDs[11]);
  exHexa.nodalCoordinates.push_back(hexa20Element.nodalCoordinates[13]);
  exHexa.nodeIDs.push_back(hexa20Element.nodeIDs[13]);
  exHexa.nodalCoordinates.push_back(hexa20Element.nodalCoordinates[9]);
  exHexa.nodeIDs.push_back(hexa20Element.nodeIDs[9]);
  exHexa.nodalCoordinates.push_back(hexa20Element.nodalCoordinates[10]);
  exHexa.nodeIDs.push_back(hexa20Element.nodeIDs[10]);
  exHexa.nodalCoordinates.push_back(hexa20Element.nodalCoordinates[12]);
  exHexa.nodeIDs.push_back(hexa20Element.nodeIDs[12]);
  exHexa.nodalCoordinates.push_back(hexa20Element.nodalCoordinates[14]);
  exHexa.nodeIDs.push_back(hexa20Element.nodeIDs[14]);
  exHexa.nodalCoordinates.push_back(hexa20Element.nodalCoordinates[15]);
  exHexa.nodeIDs.push_back(hexa20Element.nodeIDs[15]);
  exHexa.nodalCoordinates.push_back(hexa20Element.nodalCoordinates[16]);
  exHexa.nodeIDs.push_back(hexa20Element.nodeIDs[16]);
  exHexa.nodalCoordinates.push_back(hexa20Element.nodalCoordinates[18]);
  exHexa.nodeIDs.push_back(hexa20Element.nodeIDs[18]);
  exHexa.nodalCoordinates.push_back(hexa20Element.nodalCoordinates[19]);
  exHexa.nodeIDs.push_back(hexa20Element.nodeIDs[19]);
  exHexa.nodalCoordinates.push_back(hexa20Element.nodalCoordinates[17]);
  exHexa.nodeIDs.push_back(hexa20Element.nodeIDs[17]);
  
  
  
  return exHexa;
}

//Hexa 27 labels the faces.
//
//For reference as to what they should look like, I used
//http://www.colorado.edu/engineering/CAS/courses.d/AFEM.d/AFEM.Ch11.d/AFEM.Ch11.pdf
//because it follows the exodusII format with the 20 node one.
//
//
//The first 6 nodes are on the faces and one is at the center
//
//The face order appears to be:
//1234, 1256, 2367, 3478, 1458, 5678, midpoint
//
//


//Hexahedrons are actually the most complex of all 3d elements, but gmsh has them as the second 3d element in their list
exoIIElement hexa27ToExHexa(exoIIElement hexa27Element) {
  exoIIElement exHexa = hexa20ToExHexa(hexa27Element);
  exHexa.nodalCoordinates.reserve(27);
  exHexa.nodeIDs.reserve(27);
  
  //gmsh ordering is (equivalent to): 1234, 1256, 1458, 2367, 3478, 5678, midpoint
  //Or in other words to convert, use node numbers: 20, 21, 23, 24, 22, 25, 26
  exHexa.nodalCoordinates.push_back(hexa27Element.nodalCoordinates[20]);
  exHexa.nodeIDs.push_back(hexa27Element.nodeIDs[20]);
  exHexa.nodalCoordinates.push_back(hexa27Element.nodalCoordinates[21]);
  exHexa.nodeIDs.push_back(hexa27Element.nodeIDs[21]);
  exHexa.nodalCoordinates.push_back(hexa27Element.nodalCoordinates[23]);
  exHexa.nodeIDs.push_back(hexa27Element.nodeIDs[23]);
  exHexa.nodalCoordinates.push_back(hexa27Element.nodalCoordinates[24]);
  exHexa.nodeIDs.push_back(hexa27Element.nodeIDs[24]);
  exHexa.nodalCoordinates.push_back(hexa27Element.nodalCoordinates[22]);
  exHexa.nodeIDs.push_back(hexa27Element.nodeIDs[22]);
  exHexa.nodalCoordinates.push_back(hexa27Element.nodalCoordinates[25]);
  exHexa.nodeIDs.push_back(hexa27Element.nodeIDs[25]);
  exHexa.nodalCoordinates.push_back(hexa27Element.nodalCoordinates[26]);
  exHexa.nodeIDs.push_back(hexa27Element.nodeIDs[26]);
  
  
  return exHexa;
}

//Prisms are to hexahedrons as triangles are to squares. In exodus, these are labelled identically to hexahedrons
//albeit with one missing side. First order wedges have just 6 nodes, second order 15, and third 18
//
//
//
//
exoIIElement prism6ToExWedge(exoIIElement prism6Element) {
  return prism6Element;
}


//
//To convert use the nodenubmers
//0, 1, 2, 3, 4, 5, 6, 9, 7, 8, 10, 11, 12, 14, 13

exoIIElement prism15ToExWedge(exoIIElement prism15Element) {
  exoIIElement exWedge;
  exWedge.nodalCoordinates.reserve(15);
  exWedge.nodeIDs.reserve(15);
  exWedge.elementTag = prism15Element.elementTag;
  exWedge.elementType = prism15Element.elementType;
  
  for (int i = 0; i < 7; i++) {
    exWedge.nodalCoordinates.push_back(prism15Element.nodalCoordinates[i]);
    exWedge.nodeIDs.push_back(prism15Element.nodeIDs[i]);
  }
  exWedge.nodalCoordinates.push_back(prism15Element.nodalCoordinates[9]);
  exWedge.nodeIDs.push_back(prism15Element.nodeIDs[9]);
  exWedge.nodalCoordinates.push_back(prism15Element.nodalCoordinates[7]);
  exWedge.nodeIDs.push_back(prism15Element.nodeIDs[7]);
  exWedge.nodalCoordinates.push_back(prism15Element.nodalCoordinates[8]);
  exWedge.nodeIDs.push_back(prism15Element.nodeIDs[8]);
  exWedge.nodalCoordinates.push_back(prism15Element.nodalCoordinates[10]);
  exWedge.nodeIDs.push_back(prism15Element.nodeIDs[10]);
  exWedge.nodalCoordinates.push_back(prism15Element.nodalCoordinates[11]);
  exWedge.nodeIDs.push_back(prism15Element.nodeIDs[11]);
  exWedge.nodalCoordinates.push_back(prism15Element.nodalCoordinates[12]);
  exWedge.nodeIDs.push_back(prism15Element.nodeIDs[12]);
  exWedge.nodalCoordinates.push_back(prism15Element.nodalCoordinates[14]);
  exWedge.nodeIDs.push_back(prism15Element.nodeIDs[14]);
  exWedge.nodalCoordinates.push_back(prism15Element.nodalCoordinates[13]);
  exWedge.nodeIDs.push_back(prism15Element.nodeIDs[13]);
  
  
  return exWedge;
}

//The reference used for Prism18 was provided by:
//https://libmesh.github.io/doxygen/classlibMesh_1_1Prism18.html
//
//because it uses an identical prism15 format to exodus and therefore it's 18 format is probably correct.
//
exoIIElement prism18ToExWedge(exoIIElement prism18Element) {
  exoIIElement exWedge = prism15ToExWedge(prism18Element);
  exWedge.nodalCoordinates.reserve(18);
  exWedge.nodeIDs.reserve(18);
  
  //Order is 15, 17, 16.
  exWedge.nodalCoordinates.push_back(prism18Element.nodalCoordinates[15]);
  exWedge.nodeIDs.push_back(prism18Element.nodeIDs[15]);
  exWedge.nodalCoordinates.push_back(prism18Element.nodalCoordinates[17]);
  exWedge.nodeIDs.push_back(prism18Element.nodeIDs[17]);
  exWedge.nodalCoordinates.push_back(prism18Element.nodalCoordinates[16]);
  exWedge.nodeIDs.push_back(prism18Element.nodeIDs[16]);
  
  return exWedge;
}


exoIIElement pyramid5ToExPyramid(exoIIElement pyramid5Element) {
  //happy to report these are the same
  return pyramid5Element;
}

//
//
exoIIElement pyramid13ToExPyramid(exoIIElement pyramid13Element) {
  //5, 8, 10, 6, 7, 9, 11, 12.
  //
  exoIIElement exPyramid;
  exPyramid.nodalCoordinates.reserve(13);
  exPyramid.nodeIDs.reserve(13);
  exPyramid.elementTag = pyramid13Element.elementTag;
  exPyramid.elementType = pyramid13Element.elementType;
  
  for (int i = 0; i < 6; i++) {
    exPyramid.nodalCoordinates.push_back(pyramid13Element.nodalCoordinates[i]);
    exPyramid.nodeIDs.push_back(pyramid13Element.nodeIDs[i]);
  }
  exPyramid.nodalCoordinates.push_back(pyramid13Element.nodalCoordinates[8]);
  exPyramid.nodeIDs.push_back(pyramid13Element.nodeIDs[8]);
  exPyramid.nodalCoordinates.push_back(pyramid13Element.nodalCoordinates[10]);
  exPyramid.nodeIDs.push_back(pyramid13Element.nodeIDs[10]);
  exPyramid.nodalCoordinates.push_back(pyramid13Element.nodalCoordinates[6]);
  exPyramid.nodeIDs.push_back(pyramid13Element.nodeIDs[6]);
  exPyramid.nodalCoordinates.push_back(pyramid13Element.nodalCoordinates[7]);
  exPyramid.nodeIDs.push_back(pyramid13Element.nodeIDs[7]);
  exPyramid.nodalCoordinates.push_back(pyramid13Element.nodalCoordinates[9]);
  exPyramid.nodeIDs.push_back(pyramid13Element.nodeIDs[9]);
  exPyramid.nodalCoordinates.push_back(pyramid13Element.nodalCoordinates[11]);
  exPyramid.nodeIDs.push_back(pyramid13Element.nodeIDs[11]);
  exPyramid.nodalCoordinates.push_back(pyramid13Element.nodalCoordinates[12]);
  exPyramid.nodeIDs.push_back(pyramid13Element.nodeIDs[12]);
  
  return exPyramid;
}


exoIIElement pyramid14ToExPyramid(exoIIElement pyramid14Element) {
  exoIIElement exPyramid = pyramid13ToExPyramid(pyramid14Element);
  exPyramid.nodalCoordinates.reserve(14);
  exPyramid.nodalCoordinates.reserve(14);
  exPyramid.nodalCoordinates.push_back(pyramid14Element.nodalCoordinates[13]);
  exPyramid.nodeIDs.push_back(pyramid14Element.nodeIDs[13]);
  return exPyramid;
}







//This function is not used in this program but provided in case the user wants to find the last node in a third order element.
vector<double> getVectorMidpoint (vector<vector<double>> points){
  vector<double> midpoint;
  int pointsSize = points.size();
  
  for (int i = 0; i < points[0].size(); i++){
    double pointsSum = 0;
    for (int j = 0; j < pointsSize; j++){
      pointsSum += points[j][i];
      
    }
    midpoint.push_back(pointsSum);
    
  }
  
  return midpoint;
}