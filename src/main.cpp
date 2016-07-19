//
// Moses Version 0.01
//
// The All new way to create ExodusII files.
//


#include "main.h"
#include "fileReader.h"
#include "mergeData.h"
#include "exoConverter.h"
#include "sidesetFinder.h"


#include <iostream>
#include <cstring>




using namespace std;


EXOIIGlobalVariables globals = {};

int main (int argc, char* argv[]) {

  int errorCode = parseInput(argc, argv);
  if (errorCode != 0) {

    return 1;
  }
  cout << "Phase 1 complete" << endl;

  if (io.outputFile.empty()) {

    //This will attempt to remove the .msh from the input file. If no .msh is found then this will do nothing.
    size_t findLocation = io.inputFile.find(".msh");
    
    string iTemp = io.inputFile;
    // -1 is no location found.
    if (findLocation != -1) {
      iTemp.replace (findLocation, string(".msh").length(), "");
    }

    io.outputFile = iTemp + ".exoII";
  }

  NumericalMeshData importedMeshes;
  

  cout << "Phase 2 complete" << endl;

  fileReader file(io.inputFile);

  fileReader *fi;
  fi = &file;

  //vector<vector<int>> elements = fi->numericalData.elements;


  //importedMeshes.elements[i].reserve(fi->numericalData.elements.size());

  cout << fi->numericalData.elements.size() << endl;
  cout << fi->numericalData.elements[0].size() << endl;
  cout << fi->numericalData.nodes.size() << endl;
    
  globals.elements = fi->meshData.elementNumber;
  globals.nodes = fi->meshData.elementNumber;
  cout << "Vektor allocation DONE!!!!" << endl;
  
  
  importedMeshes.nodes = fi->numericalData.nodes;
  
  importedMeshes.elements.reserve(globals.elements);
  
  cout << importedMeshes.elements.capacity() << endl;
  
  cout << "Vektor allocation DONE!!!!" << endl;
  
  
  
  for (int i = 0; i < globals.elements; i++) {

    exoIIElement e = elementConverter(elementResolver(importedMeshes.nodes, fi->numericalData.elements[i]));
    importedMeshes.elements.push_back(e);
    
    //cout << getValue() << "KB Is current ram with " << i << "allocs" << endl;
  }
  
  
  cout << "Determining side and nodesets" << endl;
  importedMeshes.sidesetElements = sideSetExtractor(importedMeshes.elements, globals.includedTagMinimum, globals.includedTagMaximum);
  importedMeshes.nodesetElements = nodeSetExtractor(importedMeshes.elements, globals.includedTagMinimum, globals.includedTagMaximum);
  
  cout << "Removing side and nodeset element determiners from element vector" << endl;
  
  importedMeshes.elements = removeSets(importedMeshes.elements, globals.includedTagMinimum, globals.includedTagMaximum);
  
  
  exoIIInputData allInputs = {vector<exoIISideSet>(), vector<vector<int>>(), vector<exoIIElementBlock>()};
  
  
  cout << "Scanning elements for side and nodesets" << endl;
  allInputs.sideSets = automaticSidesetFinder(importedMeshes.elements, globals.includedTagMinimum, globals.includedTagMaximum, importedMeshes.sidesetElements);
  allInputs.nodeSets = automaticNodesetFinder(importedMeshes.nodes, globals.includedTagMinimum, globals.includedTagMaximum, importedMeshes.nodesetElements);
  
  cout << "Vektor allocation DONE!!!!" << endl;
  allInputs.elementBlocks = blockResolver(importedMeshes.elements);
  
  
  
  

  cout << "Merging Nodes" << endl;
  
  
  


  exit(0);
}




int parseInput (int argc, char* argv[]) {
  bool useStandardInput = false;

  if (argc == 1){
    cout << "Moses converts GMSH files into ExodusII files for use with Sandia software." << endl;
    cout << "Please use 'moses -h' to find out how to use this program, or 'man moses'." << endl;
    return 1;
  }

  for (int current_arg = 1; current_arg < argc; current_arg++){
    if (*argv[current_arg] == '-') {
      if ( (strcmp(argv[current_arg], "-i") == 0 ) || (strcmp(argv[current_arg], "--input") == 0)) {
        current_arg++;
        io.inputFile = argv[current_arg];
        cout << "Input File: " << io.inputFile << endl;
        
          
        

      } else if ( (strcmp(argv[current_arg], "-o") == 0 ) || (strcmp(argv[current_arg], "--output") == 0)) {

        current_arg++;
        io.outputFile=argv[current_arg];

      } else if ((strcmp(argv[current_arg], "-f") == 0 )) {
        useStandardInput = true;

      } else if ((strcmp(argv[current_arg], "-h") == 0 ) || (strcmp(argv[current_arg], "--help") == 0 )) {
        cout << "Usage: moses [options] -i [FILE]" << endl;
        cout << "Moses converts GMSH files into ExodusII files for use with Sandia software." << endl;
        cout << "" << endl;
        cout << "Example Use:" << endl;
        cout << "moses -i foo.msh            \t# Create foo.exoII from data inside foo.msh." << endl;
        cout << "moses -i foo.msh -n fooie   \t# Create foo.exoII with the name 'fooie'" << endl;
        cout << "" << endl;
        //                                                                                V Any text hitting this line will get cut off in an 80 char
        cout << "Modifiers:" << endl;//                                                   |
        cout << "" << endl;//                                                             |Frankly I think this is kind of a stupid limitation
        cout << "-A INT\t\t--exoapi\tAPI version number" << endl;//                       |I mean I am not writing fortran code
        cout << "-d INT\t\t--dimensions\tNumber of dimensions " << endl;//                |But like you never know when someone on their
        cout << "-D INT\t\t--database\tDatabase version number" << endl;//                |3 inch raspberry pi screen is gonna be doing
        cout << "-f \t\t--force\t\tUse standard input if no input file is given." << endl;// finite element simulations and they need
        cout << "-h \t\t--help\t\tView this help menu (see also man moses)." << endl;//   | to see what they are doing.
        cout << "-i PATH\t\t--input\t\tOne of more input files." << endl;//               |
        cout << "-I INT\t\t--io-size\tThe size (in bits) of floats (8)" << endl;//        | Barely threading the needle
        cout << "-L INT\t\t--line-length\tThe Exodus Fortran character line length (80)" << endl;
        cout << "-m INT\t\t--minimum-grp\tThe first group converted to node/side sets (10)" << endl;
        cout << "-M INT\t\t--maximum-grp\tThe last group converted to node/side sets (100)" << endl;
        cout << "-n STRING\t--name\t\tExodusII mesh name" << endl;//                      |
        cout << "-o PATH\t\t--output\tSpecify a file to write to." << endl;
        cout << "-S INT\t\t--string-length\tThe Exodus Fortran max string length (32)" << endl;
        cout << "-Q PATH\t\t--qa-inpath\tRead QA info from file (see man moses)" << endl;

        cout << "" << endl;
        cout << "" << endl;
        cout << "Please look at \"man moses\" for more information" << endl;

        return 1;

      } else if ((strcmp(argv[current_arg], "-d") == 0 ) || (strcmp(argv[current_arg], "--dimensions") == 0 )) {
        current_arg++;
        globals.dimensions = atoi(argv[current_arg]);
      } else if ((strcmp(argv[current_arg], "-A") == 0 ) || (strcmp(argv[current_arg], "--exoapi") == 0 )) {
        current_arg++;
        globals.APIVersionNumber = atoi(argv[current_arg]);
      } else if ((strcmp(argv[current_arg], "-D") == 0 ) || (strcmp(argv[current_arg], "--database") == 0 )) {
        current_arg++;
        globals.databaseVersionNumber = atoi(argv[current_arg]);
      } else if ((strcmp(argv[current_arg], "-I") == 0 ) || (strcmp(argv[current_arg], "--io-size") == 0 )) {
        current_arg++;
        globals.IOWordSize = atoi(argv[current_arg]);
      } else if ((strcmp(argv[current_arg], "-D") == 0 ) || (strcmp(argv[current_arg], "--line-length") == 0 )) {
        current_arg++;
        globals.CharacterLineLength = atoi(argv[current_arg]);
      } else if ((strcmp(argv[current_arg], "-S") == 0 ) || (strcmp(argv[current_arg], "--string-length") == 0 )) {
        current_arg++;
        globals.CharacterStringLength = atoi(argv[current_arg]);
      } else if ((strcmp(argv[current_arg], "-n") == 0 ) || (strcmp(argv[current_arg], "--name") == 0 )) {
        current_arg++;
        globals.title = argv[current_arg];
      } else if ((strcmp(argv[current_arg], "-Q") == 0 ) || (strcmp(argv[current_arg], "--qa-inpath") == 0 )) {
        current_arg++;
        io.qaFile = argv[current_arg];
      } else if ((strcmp(argv[current_arg], "-m") == 0 ) || (strcmp(argv[current_arg], "--minimum-grp") == 0 )) {
        current_arg++;
        globals.includedTagMinimum = atoi(argv[current_arg]);
      } else if ((strcmp(argv[current_arg], "-M") == 0 ) || (strcmp(argv[current_arg], "--maximum-grp") == 0 )) {
        current_arg++;
        globals.includedTagMaximum = atoi(argv[current_arg]);
      }

    }
  }

  if (io.inputFile.empty() && !useStandardInput) {
    cout << "No input file specified." << endl;
    return 1;
  } else if (io.inputFile.empty()) {
    if (io.outputFile.empty()) {
      cout << "No output specified and no input file to draw on, please append -o [output file]" << endl;
      return 1;
    }

    cout << "Attempting to read from standard input" << endl;
    cout << "standard input is not yet supported." << endl;
    return 1;
  }
  return 0;
}








//WARNING WARNING WARNING WARNING WARNING WARNING WARNING
//DEBUGGING ONLY

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int parseLine(char* line){
  // This assumes that a digit will be found and the line ends in " Kb".
  int i = strlen(line);
  const char* p = line;
  while (*p <'0' || *p > '9') p++;
  line[i-3] = '\0';
  i = atoi(p);
  return i;
}

int getValue(){ //Note: this value is in KB!
  FILE* file = fopen("/proc/self/status", "r");
  int result = -1;
  char line[128];
  
  while (fgets(line, 128, file) != NULL){
    if (strncmp(line, "VmRSS:", 6) == 0){
      result = parseLine(line);
      break;
    }
  }
  fclose(file);
  return result;
}


