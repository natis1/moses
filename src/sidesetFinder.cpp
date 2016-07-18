
#include "sidesetFinder.h"

using namespace std;


std::vector<exoIISideSet> automaticSidesetFinder ( std::vector<exoIIElement> allElements, int minimumSSTag, int maximumSSTag) {
  
  vector<exoIISideSetComponent> sidesets;
  sidesets.reserve(maximumSSTag - minimumSSTag + 1);
  
  
  
  for (int i = 0; i < allElements.size(); i++) {
    if (allElements[i].elementTag >= minimumSSTag && allElements[i].elementTag <= maximumSSTag && allElements[i].elementType == 1) {
      exoIIElement el = allElements[i];
      for (int j = 0; j < allElements.size(); j++){
        exoIISideSetComponent ss;
        ss = quad4SSScanner ( el, allElements[j], j, (el.elementTag - minimumSSTag) );
        if (ss.elementSide != -1){
          sidesets.push_back(ss);
        }
      }
    }
  }
  
  
  for (int i = 0; i < sidesets.size(); i++) {
    cout << sidesets[i].elementID << ", " << sidesets[i].elementSide << endl;
  }
  
  vector<exoIISideSet> allSideSets;
  
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
      allSideSets.push_back(tagSet);
    }
  }
  
  
  
  
}

//...ssssssscanner
//Returns a sideset with element ID -1 and element side -1 if the scan doesn't turn anything up.
exoIISideSetComponent quad4SSScanner ( exoIIElement sideSet, exoIIElement quad4, int elementID, int sideSetID) {
  
  exoIISideSetComponent ss;
  ss.elementID = -1;
  ss.elementSide = -1;
  ss.sideSetID = sideSetID;
  for (int i = 0; i < 4; i++) {
    if (quad4.nodeIDs[i] == sideSet.nodeIDs[0]) {
      if (quad4.nodeIDs[(i + 1) % 4] == sideSet.nodeIDs[1]) {
        ss.elementID = elementID;
        //It should be i but exodus counts from 1.
        ss.elementSide = i + 1;
        return ss;
      } else if (quad4.nodeIDs[(i - 1) % 4] == sideSet.nodeIDs[1]) {
        ss.elementID = elementID;
        //It should be i - 1 but exodus counts from 1.
        ss.elementSide = i;
        return ss;
      }
    }
  }
  return ss;
}