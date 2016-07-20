
#include "fileReader.h"
#include <fstream>
#include <streambuf>
#include <sstream>
#include <cstring>



using namespace std;




fileReader::fileReader(string input_File) {

  this->meshData = meshData;
  
  ifstream inputStream(input_File);
  stringstream inputBuffer;
  inputBuffer << inputStream.rdbuf();

  vector<string> inputVector = splitInputString(inputBuffer.str());

  setNodes (inputVector);
  setElements (inputVector);
  
  nodeArrayGenerator();
  elementArrayGenerator();
  
  
  

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



void fileReader::nodeArrayGenerator(){
  
  
  numericalData.nodes.reserve(meshData.nodeNumber);
  
  
  
  for (int i = 0; i < meshData.nodeNumber; i++) {
    //Realistically a limit of 80 is probably good enough given the exodus limits but better safe and nobody's gonna be missing 1KB of ram
    
    
    //Const char to char
    char line[128];
    strncpy(line, meshData.nodes[i].c_str(), sizeof(line));
    char * substrings = strtok (line," -");
    //Actually gets number - 1 because of the way its programmed.
    int dimensions = getNumberOfSubstrings(meshData.nodes[i]);
    
    
    //Don't touch this, it doesn't work any other way (for some reason)
    
    vector<double> v;
    v.reserve(dimensions);
    for (int j = 0; j < dimensions; j++) {
      substrings = strtok (NULL, " -");
      v.push_back(stod(substrings));
    }
    numericalData.nodes.push_back(v);
    
  }
  
  
}

void fileReader::elementArrayGenerator(){
  
  numericalData.elements.reserve(meshData.elementNumber);
  
  for (int i = 0; i < meshData.elementNumber; i++) {
    //Realistically a limit of 80 is probably good enough given the exodus limits but better safe and nobody's gonna be missing 1KB of ram
    
    
    //Const char to char
    char line[128];
    strncpy(line, meshData.elements[i].c_str(), sizeof(line));
    char * substrings = strtok (line," -");
    int properties = getNumberOfSubstrings(meshData.elements[i]);
    
    
    /*
     * i = element number
     * j 0 = element type
     * j 1 = number of tags
     * j 2..(1+n) = tags
     * j 2+n..last = nodes
     */
    
    vector<int> v;
    v.reserve(properties);
    for (int j = 0; j < properties; j++) {
      substrings = strtok (NULL, " -");
      v.push_back(stod(substrings));
    }
    numericalData.elements.push_back(v);
  }
  
}


int fileReader::getNumberOfSubstrings(string line){
  
  int subs = 0;
  bool previousWasChar = true;
  for (int i = 0; i < line.length(); i++) {
    if (previousWasChar){
      if (line.at(i) == ' '){
        //Space after some char
        previousWasChar=false;
      }
    } else {
      if (line.at(i) == ' '){
        //2 spaces in a row
        break;
      } else {
        //Space to char :)
        subs++;
        previousWasChar=true;
      }
    }
  }
  
  return subs;
}


