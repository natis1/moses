
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
