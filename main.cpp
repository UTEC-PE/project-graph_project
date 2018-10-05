#include <cstdlib>
#include <iostream>

#include "graph.h"

using namespace std;


int main(int argc, char *argv[]) {
    Graph<Traits> a;
    a.read("graph1.txt");
    a.draw();
    return EXIT_SUCCESS;
}