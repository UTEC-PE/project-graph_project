#ifndef NODE_H
#define NODE_H
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>



template <typename G>
class Node:public sf::CircleShape 
{
    public:
        typedef typename G::N N;
        typedef typename G::E E;
        typedef typename G::edge edge;
        typedef typename G::EdgeSeq EdgeSeq;
        typedef typename EdgeSeq::iterator EdgeIte;

        EdgeSeq edges;
        int outdegree;
        int indegree;
        

        Node(N data,double posX = 0, double posY = 0,sf::Vector2f window_size = sf::Vector2f(800,600),sf::Vector2f max = sf::Vector2f(50,50)):
            data(data),posX(posX),posY(posY), indegree(0), outdegree(0)
        {
            this->setRadius(20);
            this->setPosition(sf::Vector2f(posX*window_size.x/max.x,posY*window_size.y/max.y));
            this->setOrigin(this->getRadius(),this->getRadius());
            this->setFillColor(sf::Color::Black);
            if (!font.loadFromFile("arial.ttf"))
            {
                throw "Couldn't find font.";
            }
            text.setFont(font);
            
            text.setString(std::to_string(data));
            text.setCharacterSize(15);
            text.setColor(sf::Color::White);
            text.setPosition(sf::Vector2f(getPosition().x-getRadius()/2,getPosition().y-getRadius()/2));
        }

        void addEdge(edge* connection)
        {
            edges.push_back(connection);
            outdegree++;
        }

        edge* getLightestEdge()
        {
            if(edges.size() == 0)
            {
                return NULL;
            }

            edge* lightest = edges[0];
            E min = lightest->getData();
            for (int i = 1; i < edges.size(); i++)
            {

                if (edges[i]->getData() < min)
                {
                    min = edges[i]->getData();
                    lightest = edges[i];
                }
            }
            return lightest;
        }

        int degree()
        {
            return edges.size(); 
        }

        std::string type()
        {
            std::string answer = "";
            if (indegree == 0)
            {
                if(outdegree == 0)
                    answer += "isolated ";
                else
                    answer += "source ";
            }
            else if(outdegree == 0)
            {
                answer += "sink ";
            }
            else if(edges.size() == 1)
            {
                answer+= "leaf ";
            }
            else            
            {
                answer += "normal ";
            }
            return answer + "vertex";

        }

        void draw(sf::RenderWindow &window)
        {
            window.draw(*this);
            window.draw(text);
        }

        N getData(){return data;}
        double getPosX(){return posX;}
        double getPosY(){return posY;}
    private:
        N data;
        double posX;
        double posY;

        sf::Font font;
        sf::Text text;
};

#endif