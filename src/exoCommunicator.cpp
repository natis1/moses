
#include "exoCommunicator.h"

#include "exodusII.h"




using namespace std;


//Okay how does this whole library thing work anyway?
void exoCommunicator(exoIIInputData inputs) {
  
  //Note: for the most part, exoII uses 64 bit integers. However I find 64 bit integers for things like number of nodes absurd.
  //In a future patch I may add them but really, who needs more than 2 billion nodes in their simulation?
  
  int errorCode = 0;
  
  //Directory, mode == override (as an integer, see exodusii.inc)
  //computational and IO wordsize. Possible values: 4, 8
  
  //Final int is api version
  int fileID = ex_create_int(inputs.globalVariables.directory.c_str(), __SIZEOF_DOUBLE__, &inputs.globalVariables.IOWordSize,
                             &inputs.globalVariables.IOWordSize, inputs.globalVariables.databaseVersionNumber);
  
  
  //File ID, title, number of dimensions, number of nodes, number of elements
  //number of element blocks, number of nodesets, number of sidesets
  errorCode = ex_put_init(fileID, inputs.globalVariables.title.c_str(), inputs.globalVariables.dimensions,
                          inputs.globalVariables.nodes, inputs.globalVariables.elements, inputs.globalVariables.elementBlocks,
                          inputs.globalVariables.nodeSets, inputs.globalVariables.sideSets);
  
  //TODO Add QA information possibly.
  
  //If they get past here, they are probably good.
  if (errorCode != 0) {
    cout << "Failed to initialize exoII file.\nThis might be due to a lack of permissions." << endl;
    cout << "It also could mean a corrupted .msh file. Try regenerating it in GMSH" << endl;
    exit(errorCode);
  }
  //char *info[1];
  //strcpy(info[0], "Mesh autoconverted from gmsh with moses 0.04");
  //errorCode = ex_put_info(fileID, 1, info);
  
  if (inputs.globalVariables.dimensions == 3) {
    //HELP HOW DO I COMPUTER?!!!!!
    
    void* xBuffer = (void *) &(inputs.flippedNodes[0][0]);
    void* yBuffer = (void *) &(inputs.flippedNodes[1][0]);
    void* zBuffer = (void *) &(inputs.flippedNodes[2][0]);
    
    
    errorCode = ex_put_coord(fileID, xBuffer, yBuffer, zBuffer);
  } else if (inputs.globalVariables.dimensions == 2) {
    
    void* xBuffer = (void *) &(inputs.flippedNodes[0][0]);
    void* yBuffer = (void *) &(inputs.flippedNodes[1][0]);
    
    void* nullBuffer = NULL;
    
    
    errorCode = ex_put_coord(fileID, xBuffer, yBuffer, nullBuffer);
  } else {
    
    void* xBuffer = (void *) &(inputs.flippedNodes[0][0]);
    void* nullBuffer = NULL;

    errorCode = ex_put_coord(fileID, xBuffer, nullBuffer, nullBuffer);
  }
  
  if (errorCode != 0) {
    cout << "Failed to add nodes to exoII file." << endl;
    exit(errorCode);
  }
  
  
  //ex_put_elem_block requires "positive integers" and I don't expect them to be including zero in that.
  for (int i = 0; i < inputs.elementBlocks.size(); i++){
    //File ID, positive identifying integer, element name (see exoII docs), elements, nodes per element, attributes per element.
    errorCode = ex_put_elem_block(fileID, (i+1), inputs.elementBlocks[i].elementType.c_str(), inputs.elementBlocks[i].elements.size(),
                                  inputs.elementBlocks[i].nodesPerElement, 0);
    if (errorCode != 0) {
      cout << "Failed to add element blocks to exoII file." << endl;
      exit(errorCode);
    }
    
    //Connectivity array format: 0,1 0,2 0,3 0,4 1,1 1,2
    //where x is element and f(x) is node
    vector<int> connectivityArray;
    //connectivityArray.reserve( (inputs.elementBlocks[i].elements.size() *inputs.elementBlocks[i].elements[0].nodeIDs.size()) );
    for (int j = 0; j < inputs.elementBlocks[i].elements.size(); j++) {
      for (int k = 0; k < inputs.elementBlocks[i].elements[j].nodeIDs.size(); k++){
        //Get all nodeIDs from each element in order
        connectivityArray.push_back(inputs.elementBlocks[i].elements[j].nodeIDs[k]);
        
      }
    }
    void_int *connect = &connectivityArray[0];
    errorCode = ex_put_elem_conn(fileID, (i+1), connect);
    
    if (errorCode != 0) {
      cout << "Failed to add element connectivity to exoII file." << endl;
      exit(errorCode);
    }
  }
  
  //setting and allocating nodesets
  for (int i = 0; i < inputs.nodeSets.size(); i++) {
    errorCode = ex_put_node_set_param(fileID, (i+1), inputs.nodeSets[i].size(), 0);
    if (errorCode != 0) {
      cout << "Failed to add nodeset params" << endl;
      exit(errorCode);
    }
    
    int *nodeset = &(inputs.nodeSets[i][0]);
        
    errorCode = ex_put_node_set(fileID, (i+1), (void_int*) nodeset);
    
    if (errorCode != 0) {
      cout << "Failed to add nodeset nodes." << endl;
      exit(errorCode);
    }
  }
  
  //setting and allocating sidesets
  for (int i = 0; i < inputs.sideSets.size(); i++) {
    errorCode = ex_put_side_set_param(fileID, (i + 1), inputs.sideSets[i].components.size(), 0);
    if (errorCode != 0) {
      cout << "Failed to add sideset params" << endl;
      exit(errorCode);
    }
    int elements[inputs.sideSets[i].components.size()] = {};
    int sides[inputs.sideSets[i].components.size()] = {};
    
    for (int j = 0; j < inputs.sideSets[i].components.size(); j++) {
      elements[j] = (inputs.sideSets[i].components[j].elementID);
      sides[j] = (inputs.sideSets[i].components[j].elementSide);
    }
    int *elementList = &elements[0];
    int *sideList    = &sides[0];
    
    errorCode = ex_put_side_set(fileID, (i+1), elementList, sideList);
    
    if (errorCode != 0) {
      cout << "Failed to add sidesets" << endl;
      exit(errorCode);
    }
  }
  
  errorCode = ex_close(fileID);
  if (errorCode != 0) {
    cout << "Failed to close exodus file." << endl;
    exit(errorCode);
  }
  
  
}
