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

exoIIElement elementResolver(vector<vector<double>> nodeList, vector<int> mshElement) {
  exoIIElement exoElement;
  
  exoElement.elementType = mshElement[0];
  //AFAIK only the first tag matters for what I am trying to do.
  exoElement.elementTag  = mshElement[2];
  
  int nodes = mshElement.size() - 2 - mshElement[1]; //Size of mshElement minus the tags
  
  for (int i = 0; i < nodes; i++){
    exoElement.nodeIDs.push_back(mshElement[2 + mshElement[1] + i]);
    exoElement.nodalCoordinates.push_back(nodeList[ exoElement.nodeIDs[i] - 1 ]);
  }
  
  return exoElement;
}


vector<exoIIElementBlock> blockResolver(vector<exoIIElement> elements) {
  vector<exoIIElementBlock> blocks;
  blocks.reserve(31);
  
  

  
  vector<exoIIElementBlock> temporaryBlocks;
  temporaryBlocks.reserve(31);
  for (int i = 0; i < 31; i++){
    temporaryBlocks.push_back(exoIIElementBlock{blockID(i), nodesPerElement(i)});
  }
  for (int i = 0; i < elements.size(); i++) {
    temporaryBlocks[ elements[i].elementType - 1 ].elements.push_back(elements[i]);
  }
  
  
  for (int i = 0; i < temporaryBlocks.size(); i++) {
    if (temporaryBlocks[i].elements.size() != 0 ) {
      blocks.push_back(temporaryBlocks[i]);
    }
  }
    
  return blocks;
}


string blockID(int block) {
  
  
  //Geometric shape is defined in the exodus manual
  //http://gsjaardema.github.io/seacas/exodusII-new.pdf
  //The order, however, follows gmsh.
  //Keep in mind the number here is zero indexed while the number gmsh uses is 1 indexed
  switch (block) {
    //first order
    case 0: return "TRUSS";//               2/Trusses
    case 1: return "TRIANGLE";//             3/Triangles
    case 2: return "QUADRANGLE";//           4/Quadrangles
    case 3: return "TETRA";//          4/Tetrahedrons
    case 4: return "HEX";//           8/Hexahedrons
    case 5: return "WEDGE";//                6/Prisms
    case 6: return "PYRAMID";//              5/Pyramids
    //second order
    case 7: return "TRUSS";
    case 8: return "TRIANGLE";
    case 9: return "QUADRANGLE";
    case 10: return "TETRA";
    case 11: return "HEX";
    case 12: return "WEDGE";
    case 13: return "PYRAMID";
    case 14: return "CIRCLE";  //TODO add circle support
    case 15: return "QUADRANGLE";
    case 16: return "HEX";
    case 17: return "WEDGE";
    case 18: return "PYRAMID";
    //third+ order
    
    //9, 12, 15 triangles are incomplete and may be buggy
    //TODO figure out what to do with incomplete triangles
    case 19: return "TRIANGLE";//incomplete
    case 20: return "TRIANGLE";
    case 21: return "TRIANGLE";//incomplete
    case 22: return "TRIANGLE";//incomplete
    case 23: return "TRIANGLE";
    case 24: return "TRIANGLE";
    //TODO: add edges to things which determine sidesets
    case 25: return "BEAM";//Edges, or 4th-6th order TRUSS
    case 26: return "BEAM";
    case 27: return "BEAM";
    case 28: return "TETRA";//20
    case 29: return "TETRA";//35
    case 30: return "TETRA";//56
    
  }
  
  
  
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
    case 1:
      //cout << "2 node lines are not currently supported" << endl;
      return line2ToExTruss(inputElement);
      break;
    case 2:
      cout << "3 node triangles are not currently supported" << endl;
      exit(1);
      break;
    case 3:
      return quad4ToExQuad(inputElement);
      break;
    case 10:
      return quad9ToExQuad(inputElement);
      break;
    case 15:
      return point1ToExPoint(inputElement);
      break;
    case 16:
      return quad8ToExQuad(inputElement);
      break;
  }
  
  cout << "3D meshes are not currently supported" << endl;
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








exoIIElement point1ToExPoint(exoIIElement point1Element) {
  return point1Element;
}

exoIIElement line2ToExTruss(exoIIElement line2Element) {
  return line2Element;
}

exoIIElement line3ToExTruss(exoIIElement line3Element) {
  return line3Element;
}

exoIIElement line4ToExTruss(exoIIElement line4Element) {
  return line4Element;
}




/*
 * 
 *   //The center of the lines on the edges.
 f or (int i = 0; i < 4; i++){                                                              *
 quad4Element.nodeIDs.push_back(nextNode + i);
 vector<vector<double>> quadEdges;
 quadEdges.push_back(quad4Element.nodalCoordinates[(i % 4)]);
 quadEdges.push_back(quad4Element.nodalCoordinates[( (i + 1) % 4)]);
 quad4Element.nodalCoordinates.push_back(getVectorMidpoint(quadEdges));
 }
 
 //The center center point
 quad4Element.nodeIDs.push_back(nextNode + 4);
 quad4Element.nodalCoordinates.push_back(getVectorMidpoint(quad4Element.nodalCoordinates));
 * 
 * */



exoIIElement quad4ToExQuad(exoIIElement quad4Element) {
  return quad4Element;
}

exoIIElement quad8ToExQuad(exoIIElement quad8Element) {
  return quad8Element;
}

exoIIElement quad9ToExQuad(exoIIElement quad9Element) {
  return quad9Element;
}


/*

//Hip Hip horray for the world's simplest 3D shape.
exoIIElement tetra4ToExTetra(exoIIElement tetra4Element) {
  
  //The center of the lines on the edges.
  for (int i = 0; i < 3; i++) {
    tetra4Element.nodeIDs.push_back(nextNode + i);
    vector<vector<double>> tetraEdges;
    tetraEdges.push_back(tetra4Element.nodalCoordinates[(i % 3)]);
    tetraEdges.push_back(tetra4Element.nodalCoordinates[( (i + 1) % 3)]);
    tetra4Element.nodalCoordinates.push_back(getVectorMidpoint(tetraEdges));
  }
  
  for (int i = 3; i < 6; i++) {
    
    //The midpoints between the exoII 1 and 4, 2 and 4, etc.
    tetra4Element.nodeIDs.push_back(nextNode + i);
    vector<vector<double>> tetraEdges;
    tetraEdges.push_back(tetra4Element.nodalCoordinates[(i - 3)]);
    tetraEdges.push_back(tetra4Element.nodalCoordinates[3]);
    tetra4Element.nodalCoordinates.push_back(getVectorMidpoint(tetraEdges));
  }
  
  //The mid mid point
  tetra4Element.nodeIDs.push_back(nextNode + 6);
  tetra4Element.nodalCoordinates.push_back(getVectorMidpoint(tetra4Element.nodalCoordinates));
  
  return tetra4Element;
}


//All the easy conversions are done. Onto the hard stuff :'(


exoIIElement tetra10ToExTetra(exoIIElement tetra10Element) {
  // Order: 0, 1, 2, 3, 4, 5, 6, 7, 9, 8, MP
  // I kid you not.
  // I blame Gmsh because the exodus format actually makes sense
  
  swap (tetra10Element.nodalCoordinates[8], tetra10Element.nodalCoordinates[9]);
  swap (tetra10Element.nodeIDs[8], tetra10Element.nodeIDs[9]);
  
  tetra10Element.nodeIDs.push_back(nextNode);
  tetra10Element.nodalCoordinates.push_back(getVectorMidpoint(tetra10Element.nodalCoordinates));
  
  return tetra10Element;
}*/


//What is wrong with Gmsh?
exoIIElement hexa8ToExHexa(exoIIElement hexa8Element) {
  
  
}


exoIIElement hexa20ToExHexa(exoIIElement hexa20Element) {
  // Order: 0, 1, 2, 3, 4, 5, 6, 7, 8, 11, 13, 9, 10, 12, 14, 15, 16, 18, 19, 17, MP
  
  
  
  
  
}


exoIIElement hexa27ToExHexa(exoIIElement hexa27Element) {
  
  
  
  
  
  
  
}






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