
#include "fileReader.h"
#include <fstream>
#include <streambuf>
#include <sstream>
#include "MSHParser.h"

using namespace std;




fileReader::fileReader(string input_File, int iter) {
  
  cout << input_File << endl;
  
  ifstream inputStream(input_File);
  stringstream inputBuffer;
  inputBuffer << inputStream.rdbuf();
  
  vector<string> inputVector = splitInputString(inputBuffer.str());
  setNodes (inputVector);
  setElements (inputVector);
  
  MSHParser(this->meshData);
  
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




void fileReader::setNodes(vector<string> inputVector) {
int nodeStart = -1;
  for (int i = 0; i < inputVector.size(); i++){
    if (inputVector[i].compare("$Nodes") == 0) {
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


void fileReader::setElements(vector<string> inputVector) {
  
  int elementStart = -1;
  for (int i = 0; i < inputVector.size(); i++){
    if (inputVector[i].compare("$Elements") == 0) {
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

int fileReader::getElementNumber(){
  return meshData.elementNumber;
}

vector< string > fileReader::getElements(){
  return meshData.elements;
}

int fileReader::getNodeNumber(){
  return meshData.nodeNumber;
}

vector< string > fileReader::getNodes(){
  return meshData.nodes;
}

string fileReader::getElement(int element){
  return meshData.elements[element];
}

string fileReader::getNode(int node){
  return meshData.nodes[node];
}
