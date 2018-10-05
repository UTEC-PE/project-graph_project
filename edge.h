#ifndef EDGE_H
#define EDGE_H

#include "node.h"

template <typename G>
class Edge {
    public:
        typedef typename G::E E;
        typedef typename G::node node;

        Edge(node* from, node* to,E weight, bool dir = true):
        	data(weight),
        	dir(dir)
        {
        	nodes[0] = from;
        	nodes[1] = to;
            
            line[0].position = from->getPosition();
            line[0].color  = sf::Color::Black;
            line[1].position = to->getPosition();
            line[1].color = sf::Color::Black;
        }

        sf::Vertex line[2];

        node* nodes[2];

        E getData(){return data;}

    private:
        E data;
        bool dir;
};

#endif