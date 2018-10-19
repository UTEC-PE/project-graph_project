#ifndef GRAPH_H
#define GRAPH_H

#include <vector>

#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <stdlib.h>
#include <map>

#include "node.h"
#include "edge.h"



using namespace std;

sf::Vector2f window_size(800,600);

class Traits {
	public:
		typedef int N;
		typedef int E;
};


//TODO: strongly connected, prim unconnected graphs

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

        NodeSeq nodes;

        Graph():
            directed(false),
            numberOfEdges(0)
        {
            ni = nodes.begin();

            if (!font.loadFromFile("arial.ttf"))
            {
                throw "Couldn't find font.";
            }
            text.setFont(font);
            text.setString("Base");
            text.setCharacterSize(40);
            text.setColor(sf::Color::Black);
            text.setPosition(sf::Vector2f(10,10));
        };

        float getDensity()
        {
            float density = numberOfEdges/((nodes.size()*(nodes.size()-1)));
            if(!directed)
                density *= 2;
            return density;
        }

        bool isDense(float limit)
        {
            return getDensity>limit;
        }


        bool find_node(N value)
        {
            if(nodes.size()==0)
                return false;

            for(int i = 0; i < nodes.size(); i++)
            {
                if(nodes[i]->getData() == value)
                    return true;
            }
            return false;
        }

        //check if value is in graph and point to node with said value
        bool find_node(N value, node *& ptrNode)
        {
            if(nodes.size() == 0)
                return false;

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

        bool find_edge(N from, N to)
        {  
            node* node1;
            if(find_node(from,node1))
                return node1->isConnectedTo(to);
            return false;
        }

        bool find_edge(N from, N to, edge *&search)
        {  
            node* node1;
            if(find_node(from,node1))
                return node1->isConnectedTo(to,search);
            return false;
        }


        //add a new node
        void insert(N value, double posX, double posY, bool warning = true)
        {   
            if(find_node(value))
            {
                if(warning)
                    throw "There is already a node with said value in the graph.";
                return;
            }
            node * temp = new node(value,posX,posY,window_size);
            nodes.push_back(temp);
        }

        void remove_node(N value)
        {
            node *to_remove;
            //erase all edges going to node
            if(!find_node(value,to_remove))
                return;
            for(int i = 0; i < nodes.size(); i++)
            {
                if(nodes[i]->getData() != value && nodes[i]->isConnectedTo(value))
                {
                    nodes[i]->removeEdge(value);
                }
                
            }
            to_remove->clearEdges();
            //erase node;
            int index = getNodeIndex(value);
            nodes.erase(nodes.begin()+index);
            delete to_remove;
        }   

        //add new edges
        void add_edge(N from, N to,E weight, bool dir,bool warning = true)
        {
            if(dir)
                directed = true;

            node *firstNode,*secondNode;
            if(find_node(from,firstNode) && find_node(to,secondNode))
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
            if(warning)
                throw "Unable to create connection. Requested nodes aren't currently in graph.";
        }

        void remove_edge(N from, N to)
        {
            node* node_from,*node_to;
            if (find_node(from,node_from) && find_node(to,node_to))
            {
                bool dir = false;
                
                node_from->removeEdge(to,dir);
                if(!dir)
                    node_to->removeEdge(from);        
                return;
            }
            throw "Couldn't find node";
        }

        edge* getLightestEdge(NodeSeq visited={},NodeSeq base={})
        {
            if(base.size() == 0)
                return NULL;

            edge* lightest=NULL;
            edge* current_edge=NULL;
            E min = 999999;

            bool empty = true;

            for(int i = 0; i < base.size(); i++)
            {
                if(base[i]->edges.size() == 0)
                    continue;

                for(int j = 0; j < base[i]->edges.size();j++)
                {
                    empty = false;
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

            if(empty)
                return NULL;
            return lightest;
        }



        edge* getLightestEdgeKruskal(Graph<Tr>*kruskal_graph,NodeSeq visited={},NodeSeq base={})
        {
            if(base.size() == 0)
                return NULL;

            edge* lightest=NULL;
            edge* current_edge=NULL;
            E min = 999999;

            bool empty = true;

            for(int i = 0; i < base.size(); i++)
            {
                if(base[i]->edges.size() == 0)
                    continue;

                for(int j = 0; j < base[i]->edges.size();j++)
                {
                    empty = false;
                    current_edge = base[i]->edges[j];
                
                    node *kruskal_node_from = NULL,*kruskal_node_to = NULL;
                    kruskal_graph->find_node(base[i]->getData(),kruskal_node_to);
                    kruskal_graph->find_node(current_edge->nodes[1]->getData(),kruskal_node_from);

                    //if node has no edges
                    if(current_edge == NULL)
                        continue;

                    //if edge weight is smaller at leaste the node it goes to or comes from is not in visited
                    else if(current_edge->getData() < min && !areConnected(kruskal_node_from,kruskal_node_to))
                    {
                        min = current_edge->getData();
                        lightest = current_edge;
                    }
                }
            }

            if(empty)
                return NULL;
            return lightest;
        }




        Graph<Tr> kruskal()
        {
            NodeSeq visited; visited.reserve(nodes.size());
            Graph<Tr> kruskal_graph;

            kruskal_graph.text.setString("Kruskal");

            edge* current = getLightestEdge(visited,nodes);
            if(current != NULL)
            {
                kruskal_graph.insert(current->nodes[0]->getData(),current->nodes[0]->getPosX(),current->nodes[0]->getPosY());
                kruskal_graph.insert(current->nodes[1]->getData(),current->nodes[1]->getPosX(),current->nodes[1]->getPosY());
                kruskal_graph.add_edge(current->nodes[0]->getData(),current->nodes[1]->getData(),current->getData(),current->getDir());
                visited.push_back(current->nodes[0]);
                visited.push_back(current->nodes[1]);
            }

            while(nodes.size()>kruskal_graph.nodes.size())
            {
                current = getLightestEdgeKruskal(&kruskal_graph,visited,nodes);

                if(current == NULL)
                    break;

                kruskal_graph.insert(current->nodes[0]->getData(),current->nodes[0]->getPosX(),current->nodes[0]->getPosY(),false);
                kruskal_graph.insert(current->nodes[1]->getData(),current->nodes[1]->getPosX(),current->nodes[1]->getPosY(),false);
                kruskal_graph.add_edge(current->nodes[0]->getData(),current->nodes[1]->getData(),current->getData(),current->getDir());
                if(std::find(visited.begin(), visited.end(), current->nodes[0]) == visited.end())
                    visited.push_back(current->nodes[0]);
                if(std::find(visited.begin(), visited.end(), current->nodes[1]) == visited.end())
                    visited.push_back(current->nodes[1]);                   
            }

            return kruskal_graph;
        }

        //kruskal startng at random
        Graph<Tr> prim()
        {
            int start = rand()%nodes.size();
            NodeSeq visited; visited.reserve(nodes.size());
            Graph<Tr> prim_graph;

            prim_graph.text.setString("Prim");

            edge* current = NULL;
            prim_graph.insert(nodes[start]->getData(),nodes[start]->getPosX(),nodes[start]->getPosY());
            visited.push_back(nodes[start]);
            
            while(nodes.size()>prim_graph.nodes.size())
            {
                current = getLightestEdge(visited,visited);

                if(current == NULL)
                    break;

                prim_graph.insert(current->nodes[0]->getData(),current->nodes[0]->getPosX(),current->nodes[0]->getPosY(),false);
                prim_graph.insert(current->nodes[1]->getData(),current->nodes[1]->getPosX(),current->nodes[1]->getPosY(),false);
                prim_graph.add_edge(current->nodes[0]->getData(),current->nodes[1]->getData(),current->getData(),current->getDir());
                
                if(std::find(visited.begin(), visited.end(), current->nodes[0]) == visited.end())
                    visited.push_back(current->nodes[0]);
                if(std::find(visited.begin(), visited.end(), current->nodes[1]) == visited.end())
                    visited.push_back(current->nodes[1]);
            }
            

            return prim_graph;
        }

        Graph<Tr> prim(N value)
        {
            int index = getNodeIndex(value);
            NodeSeq visited; visited.reserve(nodes.size());
            Graph<Tr> prim_graph;

            prim_graph.text.setString("Prim");

            edge* current = NULL;
            prim_graph.insert(nodes[index]->getData(),nodes[index]->getPosX(),nodes[index]->getPosY());
            visited.push_back(nodes[index]);
            
            while(nodes.size()>prim_graph.nodes.size())
            {
                current = getLightestEdge(visited,visited);

                if(current == NULL)
                    break;

                prim_graph.insert(current->nodes[0]->getData(),current->nodes[0]->getPosX(),current->nodes[0]->getPosY(),false);
                prim_graph.insert(current->nodes[1]->getData(),current->nodes[1]->getPosX(),current->nodes[1]->getPosY(),false);
                prim_graph.add_edge(current->nodes[0]->getData(),current->nodes[1]->getData(),current->getData(),current->getDir());
                
                if(std::find(visited.begin(), visited.end(), current->nodes[0]) == visited.end())
                    visited.push_back(current->nodes[0]);
                if(std::find(visited.begin(), visited.end(), current->nodes[1]) == visited.end())
                    visited.push_back(current->nodes[1]);
            }
            

            return prim_graph;
        }


        Graph<Tr> dfs(N value)
        {
            node* current_node;
            
            if(!find_node(value,current_node))
            {
                throw "Couldn't find node";
            } 

            Graph<Tr> dfs_graph;

            dfs_graph.text.setString("DFS");

            NodeSeq visited; visited.reserve(nodes.size()); 
            dfs_graph.insert(current_node->getData(),current_node->getPosX(),current_node->getPosY());
            visited.push_back(current_node);

            if(current_node->edges.size() == 0)
                return dfs_graph;

            dfs_recursiveFunction(current_node,current_node->edges[0]->nodes[1],visited,dfs_graph);
            return dfs_graph;
        }


        void dfs_recursiveFunction(node *previous_node,node *current_node, NodeSeq &visited, Graph<Tr> &dfs_graph)
        {
            if(current_node->edges.size() == 0)
                return;
            if(std::find(visited.begin(), visited.end(), current_node) == visited.end())
            {
                dfs_graph.insert(current_node->getData(),current_node->getPosX(),current_node->getPosY(),false);

                edge* old_edge;
                find_edge(previous_node->getData(),current_node->getData(),old_edge);

                dfs_graph.add_edge(previous_node->getData(),current_node->getData(),old_edge->getData(),old_edge->getDir(),false);
                visited.push_back(current_node);
                for(int i = 0; i < current_node->edges.size();i++)
                    dfs_recursiveFunction(current_node,current_node->edges[i]->nodes[1],visited,dfs_graph);   
            }
        }

        Graph<Tr> bfs(N value)
        {
            node* current_node;
            
            if(!find_node(value,current_node))
            {
                cout << " b " << endl;
                throw "Couldn't find node";
            } 

            Graph<Tr> bfs_graph;

            bfs_graph.text.setString("BFS");

            NodeSeq visited; visited.reserve(nodes.size()); 
            bfs_graph.insert(current_node->getData(),current_node->getPosX(),current_node->getPosY());
            visited.push_back(current_node);

            NodeSeq current_nodes; current_nodes.push_back(current_node);

            if(current_node->edges.size() == 0)
                return bfs_graph;

            bfs_recursiveFunction(current_nodes,visited,bfs_graph);
            return bfs_graph;
        }


        void bfs_recursiveFunction(NodeSeq current_nodes, NodeSeq &visited, Graph<Tr> &bfs_graph)
        {

            bool complete = true;
            NodeSeq new_current_nodes;
            for(int k = 0; k < current_nodes.size(); k++)
            {
                for(int i = 0; i < current_nodes[k]->edges.size();i++)
                {
                    node* current = current_nodes[k]->edges[i]->nodes[1];
                    if(std::find(visited.begin(), visited.end(),current) == visited.end())
                    {
                        complete = false;
                        bfs_graph.insert(current->getData(),current->getPosX(),current->getPosY(),false);
                        visited.push_back(current);
                        new_current_nodes.push_back(current);
                        edge* old_edge;
                        find_edge(current_nodes[k]->getData(),current->getData(),old_edge);
                        bfs_graph.add_edge(current_nodes[k]->getData(),current->getData(),old_edge->getData(),old_edge->getDir(),false);    
                    }
                }
            }

            if(!complete)
                bfs_recursiveFunction(new_current_nodes,visited,bfs_graph);
        }


        bool isBipartite(bool apply_color=false)
        {
            node* current_node = nodes[0];

            if(nodes.size()==0)
                return true;

            bool answer = true;
            map<N,bool> colors;

            bool current_color = true;
            
            NodeSeq visited; visited.reserve(nodes.size()); 
            colors[current_node->getData()] = current_color;

            if(apply_color)
            {
                if(current_color)
                    current_node->setFillColor(sf::Color::Blue);
                else
                    current_node->setFillColor(sf::Color::Red);
            }

            NodeSeq current_nodes; current_nodes.push_back(current_node);

            bipartiteRecursiveFunction(current_color,current_nodes,visited,colors,answer,apply_color);

            if(visited.size()!=nodes.size() && answer)
            {
                return checkBipartitionWithAllNodes(visited,colors,apply_color);
            }
                        
            return answer;
            
        }

        bool checkBipartitionWithAllNodes(NodeSeq &visited,map<N,bool> &colors,bool apply_color)
        {
            node* current_node;
            for(int i = 0; i < nodes.size();i++)
            {
                if(std::find(visited.begin(), visited.end(),nodes[i]) == visited.end())
                {
                    current_node = nodes[i];
                    break;        
                }
            }

            bool answer = true;

            bool current_color = true;
            
            colors[current_node->getData()] = current_color;

            if(apply_color)
            {
                if(current_color)
                    current_node->setFillColor(sf::Color::Blue);
                else
                    current_node->setFillColor(sf::Color::Red);
            }                  

            NodeSeq current_nodes; current_nodes.push_back(current_node);

            bipartiteRecursiveFunction(current_color,current_nodes,visited,colors,answer,apply_color);

            if(visited.size()!=nodes.size())
            {
                answer = checkBipartitionWithAllNodes(visited,colors,apply_color);            
            }
                        
            return answer;
            
        }

        void bipartiteRecursiveFunction(bool &current_color,NodeSeq current_nodes,NodeSeq &visited,map<N,bool> &colors, bool &answer,bool apply_color)
        {
            for(int i = 0; i < current_nodes.size();i++)
            {
                visited.push_back(current_nodes[i]);
            }

            if(!answer)
            {
                return;
            }

            bool complete = true;
            current_color = !current_color;
            NodeSeq new_current_nodes;
            for(int k = 0; k < current_nodes.size(); k++)
            {
                for(int i = 0; i < current_nodes[k]->edges.size();i++)
                {
                    node* current = current_nodes[k]->edges[i]->nodes[1];
                    if(std::find(visited.begin(), visited.end(),current) == visited.end())
                    {
                        complete = false;
                        colors[current->getData()] = current_color;
                        
                        if(apply_color)
                        {
                            if(current_color)
                                current->setFillColor(sf::Color::Blue);
                            else
                                current->setFillColor(sf::Color::Red);
                        }

                        new_current_nodes.push_back(current);
                    }
                    else
                    {
                        if(current_color != colors[current->getData()])
                        {
                            if(apply_color)
                                current->setFillColor(sf::Color::Green);
                            answer = false;
                            return;
                        }
                    }
                }
            }

            if(!complete)
                bipartiteRecursiveFunction(current_color,new_current_nodes,visited,colors,answer,apply_color);            
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
                window.draw(text);
                window.display();
            }
        }


        void print()
        {
            for(int i = 0; i < nodes.size(); i++)
            {
                std::cout << nodes[i]->getData() << " | ";
                nodes[i]->printEdges();
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



        void test()
        {
            //cout << areConnected(nodes[0],nodes[5]) << endl; 
        }


    private:
        
        NodeIte ni;
        bool directed;
        int numberOfEdges;
        sf::Font font;
        sf::Text text;

        int getNodeIndex(N value)
        {
            for(int i = 0; i < nodes.size(); i++)
            {
                if(nodes[i]->getData() == value)
                    return i;
            }
            throw "Couldn't find node";
        }

        bool areConnected(node* from,node* to)
        {
            if(from == NULL || to == NULL)
                return false;
            bool answer = false;
            NodeSeq visited; visited.push_back(from);
            bool found = false;
            for(int i = 0;i < from->edges.size();i++)
            {
                searchConnectionTo(from->edges[i]->nodes[1],visited,to,answer);
                if(answer)
                    return answer;
            }
            return answer;
        }

        void searchConnectionTo(node* current_node,NodeSeq &visited, node *search,bool &answer)
        {
            if(current_node->edges.size() == 0)
                return;
            
            if(std::find(visited.begin(), visited.end(), current_node) == visited.end())
            {
                if(current_node == search)
                {     
                    answer = true;
                    return;
                }
            
                visited.push_back(current_node);
                for(int i = 0; i < current_node->edges.size();i++)
                    searchConnectionTo(current_node->edges[i]->nodes[1],visited,search,answer);   
            }
        }
        
};


typedef Graph<Traits> graph;

#endif