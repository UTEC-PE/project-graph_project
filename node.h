#ifndef NODE_H
#define NODE_H
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>


template <typename G>
class Node:public sf::CircleShape 
{
    public:
        typedef typename G::N N;
        typedef typename G::E E;
        typedef typename G::edge edge;
        typedef typename G::EdgeSeq EdgeSeq;
        typedef typename EdgeSeq::iterator EdgeIte;

        Node(N data,double posX = 0, double posY = 0,sf::Vector2f window_size = sf::Vector2f(800,600),sf::Vector2f max = sf::Vector2f(50,50)):
            data(data),posX(posX),posY(posY)
        {
            this->setRadius(20);
            this->setPosition(sf::Vector2f(posX*window_size.x/max.x,posY*window_size.y/max.y));
            this->setOrigin(this->getRadius(),this->getRadius());
            this->setFillColor(sf::Color::Black);
        }

        EdgeSeq edges;

        void addEdge(edge* connection)
        {
            edges.push_back(connection);
        }

        edge* getLightestEdge()
        {
            if(edges.begin() == NULL)
            {
                throw "Node isn't connected to anything. Unable to retrieve lightest edge.";
            }

            edge* lightest = edges[0];
            E min = lightest->data;
            for (EdgeIte it = edges.begin()+1; it != edges.end(); ++it)
            {
                if (it->getData() < data)
                {
                    min = it->getData();
                    lightest = *it;
                }
            }
            return lightest;

        }

        N getData(){return data;}
    private:
        N data;
        double posX;
        double posY;
};

#endif