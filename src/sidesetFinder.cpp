
#include "sidesetFinder.h"
#include <dlib/threads.h>

using namespace std;


vector<exoIISideSet> automaticSidesetFinder ( vector<exoIIElement> *allElements, int minimumSSTag, int maximumSSTag,
                                              vector<exoIIElement> *sidesetElements, int threads, int elementSize, int dimensions) {
  
  
  
  vector<exoIISideSetComponent> sidesets;

  
  int sidesetCount = 0;
  
  
  
  dlib::parallel_for (threads, 0, sidesetElements->size(), [&](long i) {
    //exoIIElement el = sidesetElements->at(i);
    
    for (int j = 0; j < elementSize; j++){
      if ( fastSSScan( &(sidesetElements->at(i)), &(allElements->at(j))) ) {
        
        exoIISideSetComponent ss = SSScanner ( sidesetElements->at(i), allElements->at(j), j, dimensions);
        if (ss.elementSide != -1){
          ss.sideSetID = sidesetElements->at(i).elementTag - minimumSSTag;
          sidesets.push_back(ss);
          
          sidesetCount++;
        }
        
      }
      
    }
    
  });
  
  cout << sidesetCount << " sidesets found from " << sidesetElements->size() << " sideset elements" << endl;
  
  
  vector<exoIISideSet> allSidesets;
  
  for (int i = 0; i < maximumSSTag - minimumSSTag + 1; i++) {
    exoIISideSet tagSet;
    int tagSize = 0;
    for (int j = 0; j < sidesets.size(); j++) {
      if (sidesets[j].sideSetID == i) {
        tagSet.components.push_back(sidesets[j]);
        tagSize++;
      }
    }
    if (tagSize != 0) {
      allSidesets.push_back(tagSet);
    }
  }
  
  
  return allSidesets;
}

//A simple system which tells you if you should bother
bool fastSSScan ( exoIIElement *sideset, exoIIElement *testedElement ) {
  int maxSSNode = 0;
  int minSSNode = 1073741824;
  //I feel like it's faster to just set it to a power of two
  int maxENode  = 0;
  int minENode  = 1073741824;
  for (int i = 0; i < sideset->nodeIDs.size(); i++) {
    if (sideset->nodeIDs[i] < minSSNode){
      minSSNode = sideset->nodeIDs[i];
    }
    if (sideset->nodeIDs[i] > maxSSNode) {
      maxSSNode = sideset->nodeIDs[i];
    }
  }
  
  for (int i = 0; i < testedElement->nodeIDs.size(); i++) {
    if (testedElement->nodeIDs[i] < minENode){
      minENode = testedElement->nodeIDs[i];
    }
    if (testedElement->nodeIDs[i] > maxENode) {
      maxENode = testedElement->nodeIDs[i];
    }
  }
  
  return ( maxSSNode >= minENode && maxENode >= minSSNode );
}


//...ssssssscanner
//Returns a sideset with element ID -1 and element side -1 if the scan doesn't turn anything up.
exoIISideSetComponent SSScanner ( exoIIElement sideset, exoIIElement testedElement, int elementID, int dimensions) {
  exoIISideSetComponent ss;
  
  switch (testedElement.elementType) {
    case  1: ss = TwoDimensionalSSScan(sideset, testedElement, elementID, dimensions); break;
    case  2: ss = TwoDimensionalSSScan(sideset, testedElement, elementID, dimensions); break;
    case  3: ss = TwoDimensionalSSScan(sideset, testedElement, elementID, dimensions); break;
    case  4: ss = tetra4Scan(sideset, testedElement, elementID); break;
    case  10:ss = quad89SSScan(sideset, testedElement, elementID); break;
    case  16:ss = quad89SSScan(sideset, testedElement, elementID); break;
    
  }
  //Unknown type but I can try anyway
  ss = TwoDimensionalSSScan(sideset, testedElement, elementID, dimensions);
  
  return ss;
}

//For testing simple elements
exoIISideSetComponent TwoDimensionalSSScan (exoIIElement sideset, exoIIElement testedElement, int elementID, int dimensions) {
  
  exoIISideSetComponent ss;
  int elementSize = testedElement.nodeIDs.size();
  
  if (elementSize == 1){
    return { -1, -1, -1};
  }
  
  int lastNodeMatched = -3;
  bool firstNodeMatched = false;
  
  
  for (int i = 0; i < testedElement.nodeIDs.size(); i++) {
    for (int j = 0; j < sideset.nodeIDs.size(); j++) {
      if (coordinateCompare ( &(sideset.nodalCoordinates[j]), &(testedElement.nodalCoordinates[i]), dimensions)) {
        
        if (lastNodeMatched == i - 1 || (firstNodeMatched && i == (elementSize - 1) ) ) {
          return { elementID, (i), 0 };
        }
        
        if (i == 0) firstNodeMatched = true;
        lastNodeMatched = i;
        break;
      }
    }
  }
  return {-1, -1, -1};
  
}

bool coordinateCompare (vector<double> *element1, vector<double> *element2, int dimensions) {
  int dimPassed = 0;
  for (int i = 0; i < dimensions; i++) {
    if (element1->at(i) == element2->at(i)) {
      dimPassed++;
    }
  }
  
  return (dimPassed == dimensions);
  
}


exoIISideSetComponent tetra4Scan (exoIIElement sideset, exoIIElement testedElement, int elementID) {
  //If any three points are in sideset then a sideset is formed.
  //012 = side 0, 123 = side 1, 230 = side 2, 301 = side 3
  
  bool overlappingElements[4] = { false };
  int overlappingCount = 0;
  
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < sideset.nodeIDs.size(); j++) {
      if ( testedElement.nodeIDs[i] == sideset.nodeIDs[j] ) {
        overlappingElements[i] = true;
        overlappingCount++;
        break;
      }
    }
  }
  
  if (overlappingCount == 3){
    if (overlappingElements[0] && overlappingElements[1]){
      if (overlappingElements[2]) {
        return { elementID, 1, 0};
      } else {
        return { elementID, 4, 0};
      }
    } else {
      if (overlappingElements[1]) { 
        return { elementID, 2, 0};
      } else {
        return { elementID, 3, 0};
      }
      
    }
  }
  
  return { -1, -1, -1};
}



//For testing second order quads and shells. Only look at 1-4
exoIISideSetComponent quad89SSScan (exoIIElement sideset, exoIIElement testedElement, int elementID) {
  
  
  int lastNodeMatched = -2;
  bool firstNodeMatched = false;
  
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < sideset.nodeIDs.size(); j++) {
      if (sideset.nodeIDs[j] == testedElement.nodeIDs[j]) {
        
        if (lastNodeMatched == i - 1 || (firstNodeMatched && i == 3) ) {
          return { elementID, (i + 1), 0 };
        }
        
        if (i == 0) firstNodeMatched = true;
        lastNodeMatched = i;
        break;
      }
    }
  }
  
  return { -1, -1, -1};
  
}


vector<vector<int>> automaticNodesetFinder ( vector< vector< double > >* allNodes, int minimumSSTag, int maximumSSTag,
                                             vector<exoIIElement> *nodesetElements) {
  
  vector<exoIINodesetComponent> nodesets;
  
  int nodesetCount = 0;
  
  exoIINodesetComponent buffer[6];
  
  for (int i = 0; i < 6; i++){
    buffer[i] = { -1, -1 };
  }
  

 
  for (int i = 0; i < nodesetElements->size(); i++) {
    
    for (int j = 0; j < nodesetElements->at(i).nodeIDs.size(); j++){
      
      //This prevents duplicate nodes from being added unintentionally to the same nodeset.
      bool bufferConflict = false;
      
      for (int k = 0; k < 6; k++) {
        //Reused node
        if (buffer[k].nodeID == nodesetElements->at(i).nodeIDs[j]){
          //Check if reused node is in same nodeset
          if ( (nodesetElements->at(i).elementTag - minimumSSTag) == buffer[k].nodesetID ) {
            bufferConflict = true;
            break;
          }
        }
      }
      
      if (!bufferConflict) {
        nodesetCount++;
        nodesets.push_back( {nodesetElements->at(i).nodeIDs[j], nodesetElements->at(i).elementTag - minimumSSTag} );
        //Rather than changing the buffer order, it's a lot easier to just cycle through it.
        buffer[nodesetCount % 6] = {nodesetElements->at(i).nodeIDs[j], nodesetElements->at(i).elementTag - minimumSSTag};
        
      }
      
    }
  }
  
  
  vector<vector<int>> allNodesets;
  allNodesets.reserve(maximumSSTag - minimumSSTag + 1);
  
  for (int i = 0; i < maximumSSTag - minimumSSTag + 1; i++) {
    vector<int> nodes;
    int tagSize = 0;
    for (int j = 0; j < nodesets.size(); j++) {
      if (nodesets[j].nodesetID == i) {
        nodes.push_back(nodesets[j].nodeID);
        tagSize++;
      }
    }
    if (tagSize != 0) {
      allNodesets.push_back(nodes);
    }
    
  }
  
  cout << allNodesets.size() << " nodesets, " << nodesetCount << " nodes, " << nodesetElements->size() << " nodeset elements" << endl;
  
  return allNodesets;
  
}

//Never try running this code on a mac.
//Seriously though I hope this doesn't cause any problems what with them and their nextstep libraries.

//NSScanner will return the node id if the element and node match and -1 otherwise.
exoIINodesetComponent NSScanner ( exoIIElement nodeset, vector<double> node, int ID) {
  int tests = node.size();
  int passes = 0;
  
  exoIINodesetComponent NSC = {-1, ID};
  
  
  for (int i = 0; i < node.size(); i++){
    //All the nodes must be either 3d or 2d but not a mix or you get an error
    if (node[i] == nodeset.nodalCoordinates[0][i]){
      passes++;
    } else {
      break;
    }
  }
  if (passes == tests){
    NSC.nodeID = nodeset.nodeIDs[0];
  }
  return NSC;
  
}



vector<exoIIElement> sideSetExtractor ( vector<exoIIElement> *originalElementArray, int minimumSSTag, int maximumSSTag ) {
  
  vector<exoIIElement> sideSetArray;
  
  for (int i = 0; i < originalElementArray->size(); i++){
    if (originalElementArray->at(i).elementTag >= minimumSSTag && originalElementArray->at(i).elementTag <= maximumSSTag && 
      (originalElementArray->at(i).elementType == 15 || originalElementArray->at(i).elementType == 1)
      || originalElementArray->at(i).elementType == 8){
      sideSetArray.push_back(originalElementArray->at(i));
    }
  }
  
  return sideSetArray;
}


vector<exoIIElement> removeSets ( vector<exoIIElement> *originalElementArray, int minimumSSTag, int maximumSSTag ) {
  
  vector<exoIIElement> newElementArray;
  
  //Huge performance boost (~10x) and not much memory loss unless you have tons of sidesets.
  newElementArray.reserve(originalElementArray->size());
  
  for (int i = 0; i < originalElementArray->size(); i++){
    if ( !(originalElementArray->at(i).elementTag >= minimumSSTag && originalElementArray->at(i).elementTag <= maximumSSTag &&
      (originalElementArray->at(i).elementType == 1 || originalElementArray->at(i).elementType == 15
      || originalElementArray->at(i).elementType == 8) ) ){
      newElementArray.push_back(originalElementArray->at(i));
    }
  }
  return newElementArray;
  
}

vector<exoIIElement> nodeSetExtractor ( vector<exoIIElement> *originalElementArray, int minimumSSTag, int maximumSSTag ) {
  
  vector<exoIIElement> nodeSetArray;
  
  
  for (int i = 0; i < originalElementArray->size(); i++){
    if (originalElementArray->at(i).elementTag >= minimumSSTag && originalElementArray->at(i).elementTag <= maximumSSTag &&
      (originalElementArray->at(i).elementType == 15 || originalElementArray->at(i).elementType == 1
      || originalElementArray->at(i).elementType == 8)){
      nodeSetArray.push_back(originalElementArray->at(i));
    }
  }
  
  return nodeSetArray;
  
  
  
  
}