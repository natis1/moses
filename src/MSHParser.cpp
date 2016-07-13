//Mesh input parser
//Created by Eli on July 8, 2016
//This file is licensed under the GPLv3

#include <fstream>
#include "MSHParser.h"
#include <string>
#include <cstring>

using namespace std;

//constructor
MSHParser::MSHParser(struct fileReader::meshStringData meshData) {
  this->meshData = meshData;
  
  nodeArrayGenerator();
  elementArrayGenerator();

}

void MSHParser::nodeArrayGenerator(){
  
  
  numericalData.nodes.reserve(meshData.nodeNumber);
    
  for (int i = 0; i < meshData.nodeNumber; i++) {
    //Realistically a limit of 80 is probably good enough given the exodus limits but better safe and nobody's gonna be missing 1KB of ram

    
    //Const char to char
    char line[512];
    strncpy(line, meshData.nodes[i].c_str(), sizeof(line));    
    char * substrings = strtok (line," -");
    int dimensions = (sizeof(substrings) / 2) - 1;
    numericalData.nodes[i].reserve(dimensions);
    
    //Discard the first datapoint (that is the node number)
    substrings = strtok (NULL, " -");
    
    for (int j = 0; j < dimensions; j++) {
      numericalData.nodes[i][j] = stod(substrings);
      substrings = strtok (NULL, " -");
    }
  }
  
  
}

void MSHParser::elementArrayGenerator(){
  
  numericalData.elements.reserve(meshData.elementNumber);
  
  for (int i = 0; i < meshData.elementNumber; i++) {
    //Realistically a limit of 80 is probably good enough given the exodus limits but better safe and nobody's gonna be missing 1KB of ram
    
    
    //Const char to char
    char line[512];
    strncpy(line, meshData.elements[i].c_str(), sizeof(line));    
    char * substrings = strtok (line," -");
    int properties = (sizeof(substrings) / 2);
    numericalData.elements[i].reserve(properties);
    
    //Discard the first datapoint (that is the element number)
    substrings = strtok (NULL, " -");
    
    /*
     * i = element number
     * j 0 = element type
     * j 1 = number of tags
     * j 2..(1+n) = tags
     * j 2+n..last = nodes
     */
    
    for (int j = 0; j < properties; j++) {
      numericalData.elements[i][j] = stoi(substrings);
      substrings = strtok (NULL, " -");
    }
  }
  

}
