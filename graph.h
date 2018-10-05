#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <list>
#include <fstream>
#include <string>
#include <sstream>

#include "node.h"
#include "edge.h"

using namespace std;

sf::Vector2f window_size(800,600);

class Traits {
	public:
		typedef int N;
		typedef int E;
};

template <typename Tr>
class Graph {
    public:
        typedef Graph<Tr> self;
        typedef Node<self> node;
        typedef Edge<self> edge;
        typedef vector<node*> NodeSeq;
        typedef vector<edge*> EdgeSeq;
        typedef typename Tr::N N;
        typedef typename Tr::E E;
        typedef typename NodeSeq::iterator NodeIte;
        typedef typename EdgeSeq::iterator EdgeIte;

        Graph()
        {
            ni = nodes.begin();
        };

        //add new edges
        void add_connection(N from, N to,E weight, bool dir)
        {
            node *firstNode,*secondNode;
            if(find(from,firstNode) && find(to,secondNode))
            {
                edge* newEdge = new edge(firstNode,secondNode,weight,dir);
                firstNode->addEdge(newEdge);
                return;
            }
            throw "Unable to create connection. Requested nodes aren't currently in graph.";
        }

        //add a new node
        void insert(N value, double posX, double posY)
        {   
            if(find(value))
            {
                throw "There is already a node with said value in the graph.";
            }
            node * temp = new node(value,posX,posY,window_size);
            nodes.push_back(temp);
        }


       
        bool find(N value)
        {
            //for (NodeIte it = nodes.begin(); *it; ++it)
            if(nodes.size()==0)
            {
                return false;
            }

            for(int i = 0; i < nodes.size(); i++)
            {
                if(nodes[i]->getData() == value)
                    return true;
            }
            return false;
        }

        //check if value is in graph and point to node with said value
        bool find(N value, node *& ptrNode)
        {
            if(nodes.size() == 0)
                return false;
            //for (NodeIte it = nodes.begin(); *it; it++)
            for(int i = 0; i < nodes.size();i++)
            {
                if(nodes[i]->getData() == value)
                {
                    ptrNode = nodes[i];
                    return true;
                }
            }
            return false;
        }
        

        void draw()
        {
            sf::RenderWindow window(sf::VideoMode(window_size.x,window_size.y), "Graph Drawing");
            sf::RectangleShape white_background(window_size);

            while (window.isOpen())
            {
                sf::Event event;
                while (window.pollEvent(event))
                {
                    if(event.type==sf::Event::Closed)
                    {
                        window.close();
                        return;
                    }
                }
                window.clear();
                window.draw(white_background);
                for(int i = 0; i < nodes.size(); i++)
                {
                    window.draw(*nodes[i]);
                    
                    for(int j = 0; j < nodes[i]->edges.size();j++)
                    {
                        window.draw(nodes[i]->edges[j]->line,2,sf::Lines);
                    }
                }
                window.display();
            }
        }

        void read(string name)
        {
            string line;
            ifstream file (name);
            
            //read number of nodes
            getline (file,line);
            int a = stoi(line);
            nodes.reserve(a);
            double posX,posY;
            
            for(int i = 0; i < a;i++)
            {
                getline(file,line);
                stringstream ssin(line);
                ssin >> posX >> posY;
                N data = i;

                insert(data,posX,posY);
                cout << data <<"\t" <<posX << "\t" <<  posY << endl;
            }
            
            //read empty line
            getline (file,line);
            cout << endl;
            
            //read edges
            N from,to;
            E weight;
            bool dir;
            while(getline(file,line))
            {
                stringstream ssin(line);
                ssin >> from >> to >> weight >> dir;
                cout << from << "\t" << to << "\t" << weight << "\t" << dir << endl; 
                add_connection(from,to,weight,dir);
            }
            
            file.close();        
        }

    private:
        NodeSeq nodes;
        NodeIte ni;
};

typedef Graph<Traits> graph;

#endif