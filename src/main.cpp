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




int main (int argc, char* argv[]) {

  int errorCode = parseInput(argc, argv);
  if (errorCode != 0) {

    return 1;
  }
  cout << "Phase 1 complete" << endl;

  if (io.outputFile.empty()) {

    //This will attempt to remove the .msh from the input file. If no .msh is found then this will do nothing.
    size_t findLocation = io.inputFiles[0].find(".msh");

    string iTemp = io.inputFiles[0];
    // -1 is no location found.
    if (findLocation != -1) {
      iTemp.replace (findLocation, string(".msh").length(), "");
    }

    io.outputFile = iTemp + ".exoII";
  }

  recursiveNumericalMeshData importedMeshes;

  importedMeshes.elements.reserve(io.inputFiles.size());
  importedMeshes.nodes.reserve(io.inputFiles.size());


  for (int i = 0; i < io.inputFiles.size(); i++) {
    cout << "Phase 2 complete" << endl;

    fileReader file(io.inputFiles[i]);

    fileReader *fi;
    fi = &file;

    //vector<vector<int>> elements = fi->numericalData.elements;


    //importedMeshes.elements[i].reserve(fi->numericalData.elements.size());

    cout << fi->numericalData.elements.size() << endl;
    cout << fi->numericalData.elements[0].size() << endl;
    cout << fi->numericalData.nodes.size() << endl;
    
    importedMeshes.elements[i] = fi->numericalData.elements;
    importedMeshes.nodes[i] = fi->numericalData.nodes;
/*
    for (int j = 0; j < fi->numericalData.elements.size(); j++){
    importedMeshes.elements[i][j].reserve(fi->numericalData.elements[j].size());
      for (int k = 0; k < fi->numericalData.elements[j].size(); k++){
        importedMeshes.elements[i][j][k] = fi->numericalData.elements[j][k];
      }
    }
    
    cout << "Half way there" << endl;

    importedMeshes.nodes[i].reserve(fi->numericalData.nodes.size());

    for (int j = 0; j < fi->numericalData.nodes.size(); j++){
    importedMeshes.nodes[i][j].reserve(fi->numericalData.nodes[j].size());
      for (int k = 0; k < fi->numericalData.nodes[j].size(); k++){
        importedMeshes.nodes[i][j][k] = fi->numericalData.nodes[j][k];
      }
    }
*/
    cout << "Vektor allocation code you wrote actually works" << endl;
    vector<vector<double>> nodes = fi->numericalData.nodes;
    importedMeshes.nodes.push_back(nodes);
    cout << "Vektor allocation DONE!!!!" << endl;
  }

  cout << "Phase 6 complete" << endl;
  
  


  return 0;
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
        while (current_arg < argc) {
          if ( *argv[current_arg] != '-' ){
            io.inputFiles.push_back(argv[current_arg]);
          } else {
            break;
          }
          current_arg++;
        }
        cout << "Input Files: " << endl;
        for (int i = 0; i < io.inputFiles.size(); i++){
          cout << io.inputFiles[i] << endl;
        }
          
        
        cout << "Phase 0 complete" << endl;


      } else if ( (strcmp(argv[current_arg], "-o") == 0 ) || (strcmp(argv[current_arg], "--output") == 0)) {
        cout << "Looking for an output file." << endl;
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
        cout << "Modifiers:" << endl;
        cout << "" << endl;
        cout << "-A INT\t\t--exoapi\tAPI version number" << endl;
        cout << "-d INT\t\t--dimensions\tNumber of dimensions " << endl;
        cout << "-D INT\t\t--database\tDatabase version number" << endl;
        cout << "-f \t\t--force\t\tUse standard input if no input file is given." << endl;
        cout << "-h \t\t--help\t\tView this help menu (see also man moses)." << endl;
        cout << "-i PATH\t\t--input\t\tOne of more input files." << endl;
        cout << "-I INT\t\t--io-size\tThe size (in bits) of floats (8)" << endl;
        cout << "-L INT\t\t--line-length\tThe Exodus Fortran character line length (80)" << endl;
        cout << "-n STRING\t--name\t\tExodusII mesh name" << endl;
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
      }

    }
  }

  if (io.inputFiles.empty() && !useStandardInput) {
    cout << "No input file specified. To read an input mesh from stdin use -f" << endl;
    return 1;
  } else if (io.inputFiles.empty()) {
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
