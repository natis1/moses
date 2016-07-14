
#pragma once


struct meshStringData {
  int elementNumber;
  std::vector<std::string> elements;
  int nodeNumber;
  std::vector<std::string> nodes;
};


struct recursiveNumericalMeshData {

  //Level 1 is input files
  //Level 2 is nodes and elements in input files
  //Level 3 is data about nodes and elements.
  std::vector<std::vector<std::vector<double>>> nodes;
  std::vector<std::vector<std::vector<int>>> elements;
};


struct exodusMeshInputData {
  
  //All node coordinates in order from 1 to n.
  //0 - x, 1 - y, 2 - z. Exodus supports 1 - 3 dimensions but this program is n dimensional.
  std::vector<std::vector<double>> nodeCoordinates;
  
  
};