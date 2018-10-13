#include <cstdlib>
#include <iostream>

#include "graph.h"

using namespace std;


int main(int argc, char *argv[]) {
	srand((unsigned)time(0));
    Graph<Traits> a;
    a.read("graph1.txt");
    a.test();
    a.draw();
    a.kruskal().draw();
    a.prim().draw();
    return EXIT_SUCCESS;
}