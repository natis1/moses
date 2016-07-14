
//FileReader.h
#pragma once

#include <iostream>
#include <vector>

#include "meshStructures.h"

class fileReader {
public:
  fileReader(std::__cxx11::string input_File);
  meshStringData meshData;
  
  struct allImportedNumericalMeshData {
    std::vector<std::vector<double>> nodes;
    //Elements are stored as a series of nodes referenced by ID
    std::vector<std::vector<int>> elements;
  } numericalData;
  
protected:

  

private:
  void setElements(std::vector<std::string> inputVector);
  void setNodes(std::vector<std::string> inputVector);
  void nodeArrayGenerator();
  void elementArrayGenerator();
  std::vector<std::string> splitInputString(const std::string &inputContent);
  std::vector<std::string> split(const std::__cxx11::string& inputContent, char delimiter, std::vector< std::__cxx11::string >& elems);
  
};
