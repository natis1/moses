//TODO: Use hpp instead of h files so people don't think I am secretly coding in C. I'm not but who could tell
//
//C, C++, Fortran, they really are all the same once compiled. The thing I don't get is how you get them playing nice with eachother.
//


#pragma once

#include <iostream>
#include <vector>

//Will return 1 if nodes are mergable, and a 0 otherwise.
int mergeNodes(std::vector<double> node, std::vector<std::vector<double>> nodeArrayList, double epsilon);

//Will return 1 if elements are mergable, and a 0 otherwise.
int mergeElements(std::vector<int> element, std::vector<std::vector<int>> elementArrayList);
