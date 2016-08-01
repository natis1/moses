
#include "sidesetFinder.h"
#include <dlib/threads.h>

using namespace std;


vector<exoIISideSet> automaticSidesetFinder ( vector<exoIIElement> *allElements, int minimumSSTag, int maximumSSTag, vector<exoIIElement> sidesetElements, int threads) {
  
  
  
  vector<exoIISideSetComponent> sidesets;

  
  int sidesetCount = 0;
    
  
  
  dlib::parallel_for (threads, 0, sidesetElements.size(), [&](long i) {
    //exoIIElement el = sidesetElements[i];
    
    for (int j = 0; j < allElements->size(); j++){
      exoIISideSetComponent ss;
      ss = SSScanner ( sidesetElements[i], allElements->at(j), j, (sidesetElements[i].elementTag - minimumSSTag) );
      if (ss.elementSide != -1){
        sidesets.push_back(ss);
        
        sidesetCount++;
      }
    }
    
  });
  
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


exoIISideSetComponent TwoDimensionalSSScan (exoIIElement sideset, exoIIElement testedElement) {
  
  
}

//For testing second order quads and shells
exoIISideSetComponent quadShell89SSScan (exoIIElement sideset, exoIIElement testedElement) {
  
  
  
}


vector<vector<int>> automaticNodesetFinder ( vector< vector< double > >* allNodes, int minimumSSTag, int maximumSSTag, vector<exoIIElement> nodesetElements) {
  
  vector<exoIINodesetComponent> nodesets;
  
  int nodesetCount = 0;
  
  exoIINodesetComponent buffer[6];
  
  for (int i = 0; i < 6; i++){
    buffer[i] = { -1, -1 };
  }
  

 
  for (int i = 0; i < nodesetElements.size(); i++) {
    
    for (int j = 0; j < nodesetElements[i].nodeIDs.size(); j++){
      
      //This prevents duplicate nodes from being added unintentionally to the same nodeset.
      bool bufferConflict = false;
      
      for (int k = 0; k < 6; k++) {
        //Reused node
        if (buffer[k].nodeID == nodesetElements[i].nodeIDs[j]){
          //Check if reused node is in same nodeset
          if ( (nodesetElements[i].elementTag - minimumSSTag) == buffer[k].nodesetID ) {
            bufferConflict = true;
            break;
          }
        }
      }
      
      if (!bufferConflict) {
        nodesetCount++;
        nodesets.push_back( {nodesetElements[i].nodeIDs[j], nodesetElements[i].elementTag - minimumSSTag} );
        //Rather than changing the buffer order, it's a lot easier to just cycle through it.
        buffer[nodesetCount % 6] = {nodesetElements[i].nodeIDs[j], nodesetElements[i].elementTag - minimumSSTag};
        
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
  
  cout << allNodesets.size() << " nodesets, " << nodesetCount << " nodes, " << nodesetElements.size() << " nodeset elements" << endl;
  
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