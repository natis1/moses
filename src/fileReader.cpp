
#include "fileReader.h"
#include <fstream>
#include <streambuf>
#include <sstream>

using namespace std;




fileReader::fileReader(string input_File) {
  
  cout << input_File << endl;
  
  ifstream inputStream(input_File);
  stringstream inputBuffer;
  inputBuffer << inputStream.rdbuf();
  
  vector<string> inputVector = splitInputString(inputBuffer.str());
  getNodes (inputVector);
  getElements (inputVector);
  
  for (int i = 0; i < meshData.elementNumber; i++) {
    cout << meshData.elements[i] << endl; 
  }
  
  for (int i = 0; i < meshData.nodeNumber; i++) {
    cout << meshData.nodes[i] << endl; 
  }
  
  
}

vector<string> fileReader::split(const string &inputContent, char delimiter, vector<string> &elems) {
  stringstream inputStream(inputContent);
  string item;
  while (getline(inputStream, item, delimiter)) {
    elems.push_back(item);
  }
  return elems;
}

vector<string> fileReader::splitInputString(const string &inputContent) {
  vector<string> elements;
  split(inputContent, '\n', elements);
  return elements;
}




void fileReader::getNodes(vector<string> inputVector) {
int nodeStart = -1;
  for (int i = 0; i < inputVector.size(); i++){
    if (inputVector[i].compare("$Nodes")) {
      nodeStart = (i + 1);
      break;
    }
  }
  if (nodeStart == -1) {
    cout << "Invalid msh file, unable to find $Nodes" << endl;
    exit(1);
  }
  meshData.nodeNumber = stoi(inputVector[nodeStart]);
  nodeStart++;
  
  //Getting a subvector in linear time.
  vector<string> s(&inputVector[nodeStart],&inputVector[(nodeStart + meshData.nodeNumber)]);
  meshData.nodes = s;
}


void fileReader::getElements(vector<string> inputVector) {
  
  int elementStart = -1;
  for (int i = 0; i < inputVector.size(); i++){
    if (inputVector[i].compare("$Elements")) {
      elementStart = (i + 1);
      break;
    }
  }
  if (elementStart == -1) {
    cout << "Invalid msh file, unable to find $Elements" << endl;
    exit(1);
  }
  meshData.elementNumber = stoi(inputVector[elementStart]);
  elementStart++;
  
  //Getting a subvector in linear time.
  vector<string> s(&inputVector[elementStart],&inputVector[ (elementStart + meshData.elementNumber)]);
  meshData.elements = s;
}