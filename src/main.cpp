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
#include "exoCommunicator.h"


#include <iostream>
#include <cstring>
#include <thread>
#include <dlib/threads.h>

//Debugging only
#include <ctime>
#include <chrono>


using namespace std;


EXOIIGlobalVariables globals = {};
exoIIInputData allInputs;
NumericalMeshData importedMeshes;


int main (int argc, char* argv[]) {
  
  
  
  int errorCode = parseInput(argc, argv);
  if (errorCode != 0) {

    return 1;
  }
  
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
  globals.directory = io.outputFile;
  
  

  
  
  
  chrono::time_point<chrono::system_clock> start, end;
  
  start = chrono::system_clock::now();
  
  fileReader file(io.inputFile);
  
  fileReader *fi;
  fi = &file;
  
  end = chrono::system_clock::now();
  chrono::duration<double> elapsedSeconds = end - start;
  
  
  cout << elapsedSeconds.count() << " To read file " << endl;
  
  globals.elements = fi->meshData.elementNumber;
  globals.nodes = fi->meshData.nodeNumber;
  importedMeshes.nodes = fi->numericalData.nodes;
  importedMeshes.elements.reserve(globals.elements);
  
  start = chrono::system_clock::now();
  importedMeshes.elements.assign(globals.elements,{});
  dlib::parallel_for(globals.threads, 0, globals.elements, [&](long i) {
    importedMeshes.elements[i] = elementConverter(elementResolver(&importedMeshes.nodes, fi->numericalData.elements[i]));
    
  });
  end = chrono::system_clock::now();
  elapsedSeconds = end - start;
  cout << elapsedSeconds.count() << " To convert elements" << endl;
  
  
  start = chrono::system_clock::now();
  
  
  cout << endl << "Allocating side and nodesets" << endl;
  
  
    
  start = chrono::system_clock::now();
  
  importedMeshes.actualElementNumber = removeSets( &(importedMeshes.elements), globals.nodesetTagMin, globals.sidesetTagMax).size();
  
  thread sidesetAlloc(sidesetsJob);
  thread nodesetAlloc(nodesetsJob);
  sidesetAlloc.join(); nodesetAlloc.join();
  
  end = chrono::system_clock::now();
  elapsedSeconds = end - start;
  cout << elapsedSeconds.count() << " To extract side and nodesets" << endl;
  
  
  
  importedMeshes.elements = removeSets( &(importedMeshes.elements), globals.nodesetTagMin, globals.sidesetTagMax);
  allInputs.elementBlocks = blockResolver(importedMeshes.elements, globals.dimensions, globals.elementBlockSize);
  allInputs.flippedNodes = flipNodes(importedMeshes.nodes);
  globals.elements   = importedMeshes.elements.size();
  
  //globals.dimensions = allInputs.flippedNodes.size();
  
  globals.elementBlocks = allInputs.elementBlocks.size();
  globals.sideSets = allInputs.sideSets.size();
  globals.nodeSets = allInputs.nodeSets.size();
  
  end = chrono::system_clock::now();
  elapsedSeconds = end - start;
  cout << elapsedSeconds.count() << " To allocate allInputs" << endl;
  
  start = chrono::system_clock::now();
  
  
  cout << "Creating ExodusII library with " << importedMeshes.nodes.size() << ", " << globals.nodes << " nodes" <<  endl;
  allInputs.globalVariables = globals;
  exoCommunicator(allInputs);
  
  end = chrono::system_clock::now();
  elapsedSeconds = end - start;
  cout << elapsedSeconds.count() << " To run exodus creation tools" << endl;
  
  
  cout << "Everything completed successfully." << endl;
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
        cout << "Output File: " << io.outputFile << endl;
        
      } else if ((strcmp(argv[current_arg], "-f") == 0 )) {
        useStandardInput = true;

      } else if ((strcmp(argv[current_arg], "-h") == 0 ) || (strcmp(argv[current_arg], "--help") == 0 )) {
        cout << "Usage: moses [options] -i [FILE]" << endl;
        cout << "Moses converts GMSH files into ExodusII files for use with Sandia software." << endl;
        cout << "" << endl;
        cout << "Example Use:" << endl;
        cout << "moses -i foo.msh            \t# Create 3D foo.exoII from data inside foo.msh." << endl;
        cout << "moses -i bar.msh -m fooie -d 2\t# Create 2D bar.exoII with the name 'fooie'" << endl;
        cout << "" << endl;
        //                                                                                V Any text hitting this line will get cut off in an 80 char
        cout << "Modifiers:" << endl;//                                                   |
        cout << "" << endl;//                                                             |Frankly I think this is kind of a stupid limitation
        cout << "-B INT\t\t--block-size\tElement block size (100) (see man moses)" << endl;//I mean I am not writing fortran code
        cout << "-d INT\t\t--dimensions\tNumber of dimensions " << endl;//                |But like you never know when someone on their
//                                                                                        |3 inch raspberry pi screen is gonna be doing
        cout << "-f \t\t--force\t\tUse standard input if no input file is given." << endl;// finite element simulations and they need
        cout << "-h \t\t--help\t\tView this help menu (see also man moses)." << endl;//   | to see what they are doing.
        cout << "-i PATH\t\t--input\t\tOne of more input files." << endl;//               
        cout << "-I INT\t\t--io-size\tThe size (in bits) of floats (8)" << endl;//        
        cout << "-m STRING\t--name\t\tExodusII mesh name" << endl;
        cout << "-n INT\t\t--ns-min\tThe first physical group nodeset detector (10)" << endl;
        cout << "-N INT\t\t--ns-max\tThe last physical group nodeset detector (100)" << endl;
        cout << "-o PATH\t\t--output\tSpecify a file to write to." << endl;
        cout << "-t INT\t\t--threads\tThe number of threads to use when possible (1)" << endl;
        cout << "-s INT\t\t--ss-min\tThe first physical group sideset detector (101)" << endl;
        cout << "-S INT\t\t--ss-max\tThe last physical group sideset detector (190)" << endl;

        cout << "The sideset physical group tags must not overlap" << endl;
        cout << "and be greater than the nodeset physical group tags." << endl;
        cout << endl;
        cout << endl;
        cout << "Please look at \"man moses\" for more information" << endl;

        return 1;

      } else if ((strcmp(argv[current_arg], "-d") == 0 ) || (strcmp(argv[current_arg], "--dimensions") == 0 )) {
        current_arg++;
        globals.dimensions = atoi(argv[current_arg]);
        cout << "Mesh dimensions: " << globals.dimensions << endl;
      } else if ((strcmp(argv[current_arg], "-I") == 0 ) || (strcmp(argv[current_arg], "--io-size") == 0 )) {
        current_arg++;
        globals.IOWordSize = atoi(argv[current_arg]);
      } else if ((strcmp(argv[current_arg], "-s") == 0 ) || (strcmp(argv[current_arg], "--ss-min") == 0 )) {
        current_arg++;
        globals.sidesetTagMin = atoi(argv[current_arg]);
        cout << "Sideset minimum tag: " << globals.sidesetTagMin;
      } else if ((strcmp(argv[current_arg], "-S") == 0 ) || (strcmp(argv[current_arg], "--ss-max") == 0 )) {
        current_arg++;
        globals.sidesetTagMax = atoi(argv[current_arg]);
        cout << "Sideset maximum tag: " << globals.sidesetTagMax;
      } else if ((strcmp(argv[current_arg], "-n") == 0 ) || (strcmp(argv[current_arg], "--ns-min") == 0 )) {
        current_arg++;
        globals.nodesetTagMin = atoi(argv[current_arg]);
        cout << "Sideset minimum tag: " << globals.sidesetTagMin;
      } else if ((strcmp(argv[current_arg], "-N") == 0 ) || (strcmp(argv[current_arg], "--ns-max") == 0 )) {
        current_arg++;
        globals.nodesetTagMax = atoi(argv[current_arg]);
        cout << "Sideset maximum tag: " << globals.sidesetTagMax;
      } else if ((strcmp(argv[current_arg], "-m") == 0 ) || (strcmp(argv[current_arg], "--name") == 0 )) {
        current_arg++;
        globals.title = argv[current_arg];
        cout << "Mesh name: " << globals.title << endl;
      } else if ((strcmp(argv[current_arg], "-t") == 0 ) || (strcmp(argv[current_arg], "--threads") == 0 )) {
        current_arg++;
        globals.threads = atoi(argv[current_arg]);
        cout << "Using " << globals.threads << " threads" << endl;
      } else if ((strcmp(argv[current_arg], "-B") == 0 ) || (strcmp(argv[current_arg], "--block-size") == 0 )) {
        current_arg++;
        globals.threads = atoi(argv[current_arg]);
        cout << "Block size: " << globals.elementBlockSize << "" << endl;
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




void sidesetsJob() {
  vector<exoIIElement> sidesets = sideSetExtractor( &(importedMeshes.elements), globals.sidesetTagMin, globals.sidesetTagMax);
  allInputs.sideSets = automaticSidesetFinder(&(importedMeshes.elements), globals.sidesetTagMin, globals.sidesetTagMax,
&sidesets, globals.threads, importedMeshes.actualElementNumber, globals.dimensions);  
}


void nodesetsJob() {
  vector<exoIIElement> nodesets = nodeSetExtractor( &(importedMeshes.elements), globals.nodesetTagMin, globals.nodesetTagMax);
  allInputs.nodeSets = automaticNodesetFinder(&(importedMeshes.nodes), globals.nodesetTagMin, globals.nodesetTagMax, &nodesets);
}


//WARNING WARNING WARNING WARNING WARNING WARNING WARNING
//DEBUGGING ONLY


double cpuTimeTaken(clock_t start, clock_t end) {
  return double(end - start);
}


