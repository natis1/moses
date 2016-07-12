
#include "fileReader.h"


using namespace std;



void fileReader(string input_File) {
  
  cout << input_File << endl;
  ifstream inputStream(input_File);
  string inputContent( (istreambuf_iterator<char>(inputStream)), (istreambuf_iterator<char>()) );
  
  double[][] double* elementList= new double[3][getElementNumber(inputContent)];
  double[][] double* nodeList   = new double[3][getElementNumber(inputContent)];
  
}


int getElementNumber(string inputContent) {
  
  
  
  return 1;
}

int getNodeNumber(string inputContent) {
  
  
  
  return 1;
}