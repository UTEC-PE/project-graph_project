#ifndef GRAPH_H
#define GRAPH_H

#include <vector>

#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <stdlib.h>

#include "node.h"
#include "edge.h"



using namespace std;

sf::Vector2f window_size(800,600);

class Traits {
	public:
		typedef int N;
		typedef int E;
};


//TODO: remove vertex, find edge, remove edge, strongly connected, bipartite, prim, kruskal, bfs, dfs

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

        Graph():
            directed(false),
            numberOfEdges(0)
        {
            ni = nodes.begin();
        };

        //add new edges
        void add_edge(N from, N to,E weight, bool dir)
        {
            if(dir)
                directed = true;

            node *firstNode,*secondNode;
            if(find(from,firstNode) && find(to,secondNode))
            {
                edge* newEdge = new edge(firstNode,secondNode,weight,dir);
                firstNode->addEdge(newEdge);
                secondNode-> indegree++;
                if (!dir)
                {
                    edge* newEdge2 = new edge(secondNode,firstNode,weight,dir);
                    secondNode->addEdge(newEdge2);
                    firstNode-> indegree++;
                }
                numberOfEdges++;
                return;
            }
            throw "Unable to create connection. Requested nodes aren't currently in graph.";
        }

        float getDensity()
        {
            float density = numberOfEdges/((nodes.size()*(nodes.size()-1)));
            if(!directed)
                density *= 2;
            return density;
        }

        void remove_edge(N from, N to, bool dir=false)
        {
            EdgeSeq* remove_from;
            int index;
            if(find_edge(from,to,remove_from,index))
            {

            }
            throw "couldn't find edge";
        }

        //add a new node
        void insert(N value, double posX, double posY)
        {   
            if(find(value))
            {
                return;
                //throw "There is already a node with said value in the graph.";
            }
            node * temp = new node(value,posX,posY,window_size);
            nodes.push_back(temp);
        }

        bool find_edge(N from, N to, EdgeSeq *&edges,int &index)
        {  
            node* node1;node* node2;
            if(find(from,node1) && find(to,node2))
            {
                for(int i = 0; i < node1->edges.size();i++)
                {
                    if(node1->edges[i]->nodes[1]->getData() == to)
                    {
                        edges = &node1->edges; 
                        index = i;
                        return true;
                    }
                }
                for(int i = 0; i < node2->edges.size();i++)
                {
                    if(node2->edges[i]->nodes[1]->getData() == from)
                    {
                        edges = &node1->edges;
                        index = i;
                        return true;
                    }
                }
                return false;
            }
            return false;
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
        
        void test()
        {
            //cout << getLightestEdge()->nodes[1]->getData()<<endl;
        }


        edge* getLightestEdge(NodeSeq visited={},NodeSeq base={})
        {
            if(base.size() == 0)
                return NULL;

            edge* lightest=NULL;
            edge* current_edge=NULL;
            E min = 999999;

            for(int i = 0; i < base.size(); i++)
            {
                for(int j = 0; j < base[i]->edges.size();j++)
                {
                    current_edge = base[i]->edges[j];
                
                    //if node has no edges
                    if(current_edge == NULL)
                        continue;

                    //if edge weight is smaller at leaste the node it goes to or comes from is not in visited
                    else if(current_edge->getData() < min && 
                        (std::find(visited.begin(), visited.end(), current_edge->nodes[1]) == visited.end() ||
                        std::find(visited.begin(), visited.end(), base[i]) == visited.end()))
                    {
                        min = current_edge->getData();
                        lightest = current_edge;
                    }
                
                }
            }
            return lightest;
                    /*
                    edge* current_edge = base[i]->getLightestEdge();
                    
                    //if node has no edges
                    if(current_edge == NULL)
                        continue;

                    //if edge weight is smaller at leaste the node it goes to or comes from is not in visited
                    else if(current_edge->getData() < min && 
                        (std::find(visited.begin(), visited.end(), current_edge->nodes[1]) == visited.end() ||
                        std::find(visited.begin(), visited.end(), nodes[i]) == visited.end()))// && 
                        //!find_edge(nodes[i]->getData(), current_edge->nodes[1]->getData()))
                    {
                        min = current_edge->getData();
                        lightest = current_edge;
                    }
                
            }
            return lightest;*/
        }

        Graph<Tr> prim()
        {
            NodeSeq visited; visited.reserve(nodes.size());
            Graph<Tr> prim_graph;

            edge* current = getLightestEdge(visited,nodes);
            prim_graph.insert(current->nodes[0]->getData(),current->nodes[0]->getPosX(),current->nodes[0]->getPosY());
            prim_graph.insert(current->nodes[1]->getData(),current->nodes[1]->getPosX(),current->nodes[1]->getPosY());
            prim_graph.add_edge(current->nodes[0]->getData(),current->nodes[1]->getData(),current->getData(),current->getDir());
            visited.push_back(current->nodes[0]);
            visited.push_back(current->nodes[1]);
            
            while(nodes.size()>prim_graph.nodes.size())
            {
                current = getLightestEdge(visited,nodes);
                prim_graph.insert(current->nodes[0]->getData(),current->nodes[0]->getPosX(),current->nodes[0]->getPosY());
                prim_graph.insert(current->nodes[1]->getData(),current->nodes[1]->getPosX(),current->nodes[1]->getPosY());
                prim_graph.add_edge(current->nodes[0]->getData(),current->nodes[1]->getData(),current->getData(),current->getDir());
                if(std::find(visited.begin(), visited.end(), current->nodes[0]) == visited.end())
                    visited.push_back(current->nodes[0]);
                if(std::find(visited.begin(), visited.end(), current->nodes[1]) == visited.end())
                    visited.push_back(current->nodes[1]);                   
            }

            return prim_graph;
        }

        Graph<Tr> kruskal()
        {
            int start = rand()%nodes.size();
            NodeSeq visited; visited.reserve(nodes.size());
            Graph<Tr> kruskal_graph;

            edge* current;
            kruskal_graph.insert(nodes[start]->getData(),nodes[start]->getPosX(),nodes[start]->getPosY());
            visited.push_back(nodes[start]);
            
            while(nodes.size()>kruskal_graph.nodes.size())
            {
                current = getLightestEdge(visited,visited);
                kruskal_graph.insert(current->nodes[0]->getData(),current->nodes[0]->getPosX(),current->nodes[0]->getPosY());
                kruskal_graph.insert(current->nodes[1]->getData(),current->nodes[1]->getPosX(),current->nodes[1]->getPosY());
                kruskal_graph.add_edge(current->nodes[0]->getData(),current->nodes[1]->getData(),current->getData(),current->getDir());
                
                if(std::find(visited.begin(), visited.end(), current->nodes[0]) == visited.end())
                    visited.push_back(current->nodes[0]);
                if(std::find(visited.begin(), visited.end(), current->nodes[1]) == visited.end())
                    visited.push_back(current->nodes[1]);
            }

            return kruskal_graph;
        }


        void dfs()
        {
            NodeSeq visited; visited.reserve(nodes.size());
            NodeSeq search; search.reserve(nodes.size());
            Graph dfs_graph;
            node* start = nodes[0];
            while(visited.size() != nodes.size())
            {

            }
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
                    for(int j = 0; j < nodes[i]->edges.size();j++)
                    {
                        nodes[i]->edges[j]->draw(window);
                    }
                    nodes[i]->draw(window);                    
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
            }
            
            //read empty line
            getline (file,line);
            
            //read edges
            N from,to;
            E weight;
            bool dir;
            while(getline(file,line))
            {
                stringstream ssin(line);
                ssin >> from >> to >> weight >> dir;
                add_edge(from,to,weight,dir);
            }
            
            file.close();        
        }



    private:
        NodeSeq nodes;
        NodeIte ni;
        bool directed;
        int numberOfEdges;
};


typedef Graph<Traits> graph;

#endif