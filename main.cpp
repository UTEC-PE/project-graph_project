#include <cstdlib>
#include <iostream>

#include "graph.h"

using namespace std;


int main(int argc, char *argv[]) {
	srand((unsigned)time(0));
    Graph<Traits> a;
    a.read("graph1.txt");
    a.test();
    
    a.print();
    a.isBipartite(true);
    a.draw();


    a.prim(0).draw();
    a.kruskal().draw();
    a.dfs(0).draw();
    a.bfs(0).draw();
    return EXIT_SUCCESS;
}