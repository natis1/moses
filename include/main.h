
//Simple header to tell main.cpp that is has a structure or two. It needs to be told since it is blind.

#include <string>
#include <vector>



struct io_Files {
  std::string inputFile;
  std::string qaFile;
  std::string outputFile;
} io;


struct qa_Data {
  int applicationCode;  
  std::string applicationCodeDescriptor;
  //Date and time in 64 bit unix time format
  long dateTime;
};


int parseInput (int argc, char* argv[]);
int getValue();
int parseLine(char* line);



double cpuTimeTaken(clock_t start, clock_t end);