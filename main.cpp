#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include "graph.h"

using namespace std;


int main(int argc, char *argv[]) {
	srand((unsigned)time(0));
    Graph<Traits> a;
    a.read("grafo3.txt");
    
    //std::cout<< "Density: " << a.getDensity() << endl;

    a.print();
    
    a.draw();
    /*
    //will color nodes acording to the set it would belong if it was bipartite
    cout << "Is bipartite(1 = yes)? : "<< a.isBipartite(true)<< endl;
    a.draw();

    cout << "Is strongly connected(1 = yes)? : " << a.isStronglyConnected() << endl;
    
    if(!a.directed)
    {
        a.prim(0).draw();
        a.kruskal().draw();
    }*/

    a.greedy_bfs(1,5).draw();
    
    a.floyd_warshall();
    a.Djikstra(0);
    /*a.dfs(5).draw();
    a.dfs(0).print();
    a.bfs(5).draw();*/

    a.bellmanFord(0);
    a.Aast(0,1);

    return EXIT_SUCCESS;
}