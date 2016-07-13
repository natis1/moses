
//Simple header to tell main.cpp that is has a structure. It needs to be told since it is blind.

#include <string>
#include <vector>

//These are exoII globals not my globals
struct EXOIIGlobalVariables {
  std::string title;
  int nodes;
  int dimensions;
  int elements;
  int elementBlocks;
  int nodeSets;
  int sideSets;
  int databaseVersionNumber;
  int APIVersionNumber;
  int IOWordSize = 8;
  int CharacterStringLength = 32;
  int CharacterLineLength = 80;
} globals;

struct io_Files {
  std::vector<std::string> inputFiles;
  std::string qaFile;
  std::string outputFile;
} io;

int parseInput (int argc, char* argv[]);