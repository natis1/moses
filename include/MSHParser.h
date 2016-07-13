
//This file left blank intentionally for some reason.
//JK It is because I cannot code.

#include <vector>
#include "fileReader.h"

class MSHParser{
public:
  MSHParser(struct fileReader::meshStringData meshData);
protected:
  
  
private:
  void nodeArrayGenerator();
  void elementArrayGenerator();
  
  fileReader::meshStringData meshData;
  
  
  struct allImportedNumericalMeshData {
    std::vector<std::vector<double>> nodes;
    //Elements are stored as a series of nodes referenced by ID
    std::vector<std::vector<int>> elements;
    
    //Node groups allow nodes and elements to be used by computer modelling software
    std::vector<int> elementGroups;
    std::vector<int> nodeGroups;
    
    
  } numericalData;
  
};
