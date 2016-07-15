//ExoConverter contains a bunch of functions in the following format:
//
//[type]toex[type]
//

#include "exoConverter.h"



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


