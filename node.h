#ifndef NODE_H
#define NODE_H
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>

#include <iostream>

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

    private:
        N data;
        double posX;
        double posY;
        bool dir;

        sf::Font font;
        sf::Text text;

    public:
        
        Node(N data,double posX = 0, double posY = 0,sf::Vector2f window_size = sf::Vector2f(800,600),sf::Vector2f max = sf::Vector2f(50,50)):
            data(data),posX(posX),posY(posY), indegree(0), outdegree(0), dir(false)
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

        N getData(){return data;}
        double getPosX(){return posX;}
        double getPosY(){return posY;}

        int getDegree()
        {
            if(!dir)
                return edges.size();
            return indegree + outdegree; 
        }


        void printEdges()
        {
            for(int i = 0; i < edges.size(); i++)
            {
                std::cout  << " " << edges[i]->nodes[1]->getData();
            }
            std::cout << std::endl;
        }

        void addEdge(edge* connection)
        {
            if(isConnectedTo(connection->nodes[1]->getData()))
                throw "Can't add edge";   
            dir = connection->getDir();                
            edges.push_back(connection);
            outdegree++;
        }

        void removeEdge(N to)
        {
            edge* to_remove;
            if(isConnectedTo(to,to_remove))
            {
                to_remove->nodes[1]->indegree--;
                outdegree--;
                
                int index = getConnectionIndex(to);
                
                edges.erase(edges.begin()+index);
                delete to_remove;
                
            }
            return;
        }

        void removeEdge(N to, bool &dir)
        {
            edge* to_remove;
            if(isConnectedTo(to,to_remove))
            {
                dir = to_remove->getDir();
                to_remove->nodes[1]->indegree--;
                outdegree--;
                
                int index = getConnectionIndex(to);
                
                edges.erase(edges.begin()+index);
                delete to_remove;
                
            }
            return;
        }


        void clearEdges()
        {
            edge* temp;
            for(int i = 0; i < edges.size(); i++)
            {
                 temp = edges[i];
                 edges[i] = NULL;
                 delete temp;
            }
            edges.clear();
        }

        

        int getConnectionIndex(N data)
        {
            for(int i = 0; i < edges.size();i++)
            {
                if(edges[i]->nodes[1]->getData() == data)
                    return i;
            }
            throw "Couldn't find connection";
        }


        bool isConnectedTo(N data)
        {
            for(int i = 0; i < edges.size();i++)
            {
                if (edges[i]->nodes[1]->getData() == data)
                {
                    return true;
                }
            }
            return false;
        }

        bool isConnectedTo(N data, edge *&search)
        {
            for(int i = 0; i < edges.size();i++)
            {
                if (edges[i]->nodes[1]->getData() == data)
                {
                    search = edges[i];
                    return true;
                }
            }
            return false;
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

    
};

#endif