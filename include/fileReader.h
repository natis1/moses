
//FileReader.h

#include <iostream>
#include <vector>

#ifndef FILE_READER
#define FILE_READER


class fileReader {
public:
  fileReader(std::string input_File, int iter);
  
  struct meshStringData {
    int elementNumber;
    std::vector<std::string> elements;
    int nodeNumber;
    std::vector<std::string> nodes;
  } meshData;

  int getElementNumber(void);
  int getNodeNumber(void);
  std::vector<std::string> getElements(void);
  std::vector<std::string> getNodes(void);
  std::string getNode(int node);
  std::string getElement(int element);
  
  
protected:


private:
  void setElements(std::vector<std::string> inputVector);
  void setNodes(std::vector<std::string> inputVector);
  std::vector<std::string> splitInputString(const std::string &inputContent);
  std::vector<std::string> split(const std::__cxx11::string& inputContent, char delimiter, std::vector< std::__cxx11::string >& elems);

};

#endif