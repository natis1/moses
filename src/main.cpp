//
// Moses Version 0.01
//
// The All new way to create ExodusII files.
//


#include "main.h"
#include "fileReader.h"


#include <iostream>
#include <cstring>

using namespace std;


struct io_Files {
  string input_File;
  string output_File;
};

int main (int argc, char* argv[]) {
  io_Files io;
  bool useStandardInput = false;
  
  for (int current_arg = 1; current_arg < argc; current_arg++){
    if (*argv[current_arg] == '-') {
      if ( (strcmp(argv[current_arg], "-i") == 0 ) || (strcmp(argv[current_arg], "--input") == 0)) {
        cout << "I found your input file" << endl;
        current_arg++;
        io.input_File=argv[current_arg];
        
      } else if ( (strcmp(argv[current_arg], "-o") == 0 ) || (strcmp(argv[current_arg], "--output") == 0)) {
        cout << "Looking for an output file." << endl;
        current_arg++;
        io.output_File=argv[current_arg];
        
      } else if ((strcmp(argv[current_arg], "-f") == 0 )) {
        useStandardInput = true;
        
      } else if ((strcmp(argv[current_arg], "-h") == 0 ) || (strcmp(argv[current_arg], "--help") == 0 )) {
        cout << "Usage: moses [options] -i [FILE]" << endl;
        cout << "Moses converts GMSH files into ExodusII files for use with Sandia software." << endl;
        cout << "" << endl;
        cout << "Example Use:" << endl;
        cout << "moses -i foo.msh         # Create foo.msh.exoII from data inside foo.msh." << endl;
        cout << "moses -f -o bar.exoII    # Create bar.exoII from standard input." << endl;
        cout << "" << endl;
        cout << "Modifiers:" << endl;
        cout << "" << endl;
        cout << "-f - Use standard input if no input file is specified." << endl;
        cout << "-h - View this help menu (see also man moses)." << endl;
        cout << "-i - Specify an input file." << endl;
        cout << "-o - Specify an output file." << endl;
        cout << "" << endl;
        cout << "" << endl;
        cout << "Please look at \"man moses\" for more information" << endl;
        
        return 0;
        
      }
      
    }
  }
  
  if (io.input_File.empty() && !useStandardInput) {
    cout << "No input file specified. To read an input mesh from stdin use -f" << endl;
    return 1;
  } else if (io.input_File.empty()) {
    if (io.output_File.empty()) {
      cout << "No output specified and no input file to draw on, please append -o [output file]" << endl;
      return 1;
    }
    
    cout << "Attempting to read from standard input" << endl;
    cout << "standard input is not yet supported." << endl;
    return 1;
    
  }
  
  if (io.output_File.empty()) {
    
    //This will attempt to remove the .msh from the input file. If no .msh is found then this will do nothing.
    size_t findLocation = io.input_File.find(".msh");
    
    string iTemp = io.input_File;
    // -1 is no location found.
    if (findLocation != -1) {
      iTemp.replace (findLocation, string(".msh").length(), "");
    }
    
    io.output_File = iTemp + ".exoII";
  }
  
  fileReader(io.input_File);
  
  cout << "Testing" << endl;
  cout << "Input file " << io.input_File << " output file " << io.output_File << endl;
  
  string input_Mesh = "test.msh";
  return 0;
}







