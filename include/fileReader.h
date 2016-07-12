
//FileReader.h

#include <iostream>
#include <vector>


class fileReader {
public:
  fileReader(std::string input_File);
protected:
  struct meshStringData {
    int elementNumber;
    std::vector<std::string> elements;
    int nodeNumber;
    std::vector<std::string> nodes;
  } meshData;

private:
  void getElements(std::vector<std::string> inputVector);
  void getNodes(std::vector<std::string> inputVector);
  std::vector<std::string> splitInputString(const std::string &inputContent);
  std::vector<std::string> split(const std::__cxx11::string& inputContent, char delimiter, std::vector< std::__cxx11::string >& elems);
};