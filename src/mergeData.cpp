//Merge Nodes
//This script will take a node and compare it's dimensions against a vector of nodes.
//Matching nodes will return the vector they matched and nonmatching nodes will return a -1.
//This was designed for Moses and created on July 14, 2016.

#include "mergeData.h"


int mergeNodes(std::vector< double > node, std::vector< std::vector< double > > nodeArrayList, double epsilon) {
  bool usingEpsilon = (epsilon != 0);
  
  for (int i = 0; i < nodeArrayList.size(); i++) {
    int tests = nodeArrayList[i].size();
    int passes = 0;
    for (int j = 0; j < tests; j++){
      if (usingEpsilon) {
        if (nodeArrayList[i][j] - node[j] < epsilon || nodeArrayList[i][j] - node[j] > -epsilon){
          passes++;
        } else {
          //If one doesn't match don't check others
          break;
        }
        
      } else {
        if (nodeArrayList[i][j] == node[j]){
          //Requires same memory value
          passes++;
        } else {
          break;
        }
      }
    }
    
    if (passes == tests) {
      return i;
    }
  }
  
  
  
  return -1;
}

//This does nothing effectively. Yet...
int mergeElements(std::vector< int > element, std::vector< std::vector< int > > elementArrayList) {
  
  for (int i = 0; i < elementArrayList.size(); i++){
    //Element at 1 is the number of tags and it offsets the important part by 2 + tags.
    int startingLocation = 2 + element[1];
    int tests = element.size() - startingLocation;
    int passes = 0;
    
    for (int j = startingLocation; j < element.size(); j++) {
      if (elementArrayList[i][j] == element[j]) {
        passes++;
      } else {
        break;
      }
    }
    
    
    if (passes == tests) {
      return i;
    }
    
  }
  
  return -1;
}






