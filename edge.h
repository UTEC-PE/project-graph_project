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

            if (!font.loadFromFile("arial.ttf"))
            {
                throw "Couldn't find font.";
            }

            text.setFont(font);            
            text.setString(std::to_string(data));
            text.setCharacterSize(15);
            text.setColor(sf::Color::Black);
            text.setPosition(sf::Vector2f((line[0].position.x+line[1].position.x)/2,(line[0].position.y+line[1].position.y)/2));
        }

        sf::Vertex line[2];

        node* nodes[2];

        E getData(){return data;}
        bool getDir(){return dir;}

        void draw(sf::RenderWindow &window)
        {
            window.draw(line,2,sf::Lines);
            window.draw(text);
        }

    private:
        E data;
        bool dir;

        sf::Font font;
        sf::Text text;
};

#endif