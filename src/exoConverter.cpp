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


exoIIElement elementConverter(exoIIElement inputElement) {
  switch (inputElement.elementType) {
    case 1:
      cout << "2 node lines are not currently supported" << endl;
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
    case 16:
      return quad8ToExQuad(inputElement);
      break;
  }
  
  cout << "3D meshes are not currently supported" << endl;
  exit(1);
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