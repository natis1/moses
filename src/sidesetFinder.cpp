
#include "sidesetFinder.h"

using namespace std;


vector<exoIISideSet> automaticSidesetFinder ( vector<exoIIElement> allElements, int minimumSSTag, int maximumSSTag, vector<exoIIElement> sidesetElements) {
  
  
  
  vector<exoIISideSetComponent> sidesets;

  
  int sidesetCount = 0;
    
  
  for (int i = 0; i < sidesetElements.size(); i++) {
    exoIIElement el = sidesetElements[i];
    
    for (int j = 0; j < allElements.size(); j++){
      exoIISideSetComponent ss;
      ss = SSScanner ( el, allElements[j], j, (el.elementTag - minimumSSTag) );
      if (ss.elementSide != -1){
        sidesets.push_back(ss);
        sidesetCount++;
      }
    }
    
  }
  
  cout << sidesetCount << " sidesets found from " << sidesetElements.size() << " sideset elements" << endl;
  
  
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


//...ssssssscanner
//Returns a sideset with element ID -1 and element side -1 if the scan doesn't turn anything up.
exoIISideSetComponent SSScanner ( exoIIElement sideset, exoIIElement testedElement, int elementID, int sideSetID) {
  
  exoIISideSetComponent ss;
  ss.elementID = -1;
  ss.elementSide = -1;
  ss.sideSetID = sideSetID;
  int elementSize = testedElement.nodeIDs.size();
  
  if (elementSize == 1){
    return ss;
  }
  
  
  for (int i = 0; i < elementSize; i++) {
    if (testedElement.nodeIDs[i] == sideset.nodeIDs[0]) {
      if (testedElement.nodeIDs[(i + 1) % elementSize] == sideset.nodeIDs[1]) {
        ss.elementID = elementID;
        //It should be i but exodus counts from 1.
        ss.elementSide = i + 1;
        return ss;
      } else if (testedElement.nodeIDs[ ((i - 1) % elementSize + elementSize) % elementSize ] == sideset.nodeIDs[1]) {
        ss.elementID = elementID;
        //It should be i - 1 but exodus counts from 1.
        ss.elementSide = i;
        return ss;
      }
    }
  }
  return ss;
}


vector<vector<int>> automaticNodesetFinder ( vector<vector<double>> allNodes, int minimumSSTag, int maximumSSTag, vector<exoIIElement> nodesetElements) {
  
  vector<exoIINodesetComponent> nodesets;
  
  int nodesetCount = 0;
  
  for (int i = 0; i < nodesetElements.size(); i++) {
    exoIIElement el = nodesetElements[i];
    
    for (int j = 0; j < allNodes.size(); j++) {
      exoIINodesetComponent nodeset = NSScanner ( el, allNodes[j], (el.elementTag - minimumSSTag) );
      if (nodeset.nodeID != -1){
        nodesets.push_back(nodeset);
        nodesetCount++;
      }
    }
  }
  cout << nodesetCount << " nodesets found from " << nodesetElements.size() << " sideset elements" << endl;
  
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



vector<exoIIElement> sideSetExtractor ( vector<exoIIElement> originalElementArray, int minimumSSTag, int maximumSSTag ) {
  
  vector<exoIIElement> sideSetArray;
  
  for (int i = 0; i < originalElementArray.size(); i++){
    if (originalElementArray[i].elementTag >= minimumSSTag && originalElementArray[i].elementTag <= maximumSSTag && originalElementArray[i].elementType == 1){
      sideSetArray.push_back(originalElementArray[i]);
    }
  }
  
  return sideSetArray;
}


vector<exoIIElement> removeSets ( vector<exoIIElement> originalElementArray, int minimumSSTag, int maximumSSTag ) {
  
  vector<exoIIElement> newElementArray;
  
  //Huge performance boost (~10x) and not much memory loss unless you have tons of sidesets.
  newElementArray.reserve(originalElementArray.size());
  
  for (int i = 0; i < originalElementArray.size(); i++){
    if ( !(originalElementArray[i].elementTag >= minimumSSTag && originalElementArray[i].elementTag <= maximumSSTag &&
      (originalElementArray[i].elementType == 1 || originalElementArray[i].elementType == 15) ) ){
      newElementArray.push_back(originalElementArray[i]);
    }
  }
  return newElementArray;
  
}

vector<exoIIElement> nodeSetExtractor ( vector<exoIIElement> originalElementArray, int minimumSSTag, int maximumSSTag ) {
  
  vector<exoIIElement> nodeSetArray;
  
  
  for (int i = 0; i < originalElementArray.size(); i++){
    if (originalElementArray[i].elementTag >= minimumSSTag && originalElementArray[i].elementTag <= maximumSSTag && originalElementArray[i].elementType == 15){
      nodeSetArray.push_back(originalElementArray[i]);
    }
  }
  
  return nodeSetArray;
  
  
  
  
}