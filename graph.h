#ifndef GRAPH_H
#define GRAPH_H

#include <vector>

#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <stdlib.h>
#include <map>
#include <iomanip>
#include "node.h"
#include "edge.h"

#define INFINITE 999

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

    private:
        
        NodeIte ni;
        int numberOfEdges;
        sf::Font font;
        sf::Text text;
        NodeSeq nodes;

    public:

        bool directed;
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
            float density = (float)numberOfEdges/(float)((nodes.size()*(nodes.size()-1)));
            if(!directed)
                density *= 2;
            return density;
        }

        bool isDense(float limit)
        {
            return (getDensity() > limit);
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

        //add new edges
        void add_edge(N from, N to,E weight, bool dir,bool warning = true)
        {
            if(dir)
                directed = true;

            node *firstNode,*secondNode;
            if(find_node(from,firstNode) && find_node(to,secondNode))
            {
                edge* newEdge = new edge(firstNode,secondNode,weight,dir);
                firstNode->addEdge(newEdge,warning);
                secondNode-> indegree++;
                if (!dir)
                {
                    edge* newEdge2 = new edge(secondNode,firstNode,weight,dir);
                    secondNode->addEdge(newEdge2,warning);
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


        //prim starting in random vertex
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

            for(int i = 0; i < current_node->edges.size();i++)
                dfs_recursiveFunction(current_node,current_node->edges[0]->nodes[1],visited,dfs_graph);
            return dfs_graph;
        }


        void dfs_recursiveFunction(node *previous_node,node *current_node, NodeSeq &visited, Graph<Tr> &dfs_graph)
        {

            if(std::find(visited.begin(), visited.end(), current_node) == visited.end())
            {
                dfs_graph.insert(current_node->getData(),current_node->getPosX(),current_node->getPosY(),false);

                edge* old_edge;
                find_edge(previous_node->getData(),current_node->getData(),old_edge);

                dfs_graph.add_edge(previous_node->getData(),current_node->getData(),old_edge->getData(),old_edge->getDir(),false);
                visited.push_back(current_node);
                
                if(current_node->edges.size() == 0)
                    return;


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


        bool isStronglyConnected()
        {
            Graph<Tr> sc_graph;
            sc_graph.text.setString("Inverted");
            for(int i = 0; i < nodes.size(); i++)
            {
                sc_graph.insert(nodes[i]->getData(),nodes[i]->getPosX(),nodes[i]->getPosY());
            }

            for(int i = 0; i < nodes.size(); i++)
            {
                for(int j = 0; j < nodes[i]->edges.size(); j++)
                {
                    edge *current_edge = nodes[i]->edges[j];
                    node *node_from = current_edge->nodes[0];
                    node *node_to = current_edge->nodes[1];
                    sc_graph.add_edge(node_to->getData(),node_from->getData(),current_edge->getData(),current_edge->getDir(),false);
                }
            }

            for(int i = 0; i < nodes.size();i++)
            {
                if(sc_graph.dfs(nodes[i]->getData()).nodes.size() == sc_graph.nodes.size())
                    return true;
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




        Graph<Tr> greedy_bfs(N value, N end)
        {
            int index = getNodeIndex(value);
            NodeSeq visited; visited.reserve(nodes.size());
            Graph<Tr> greedy_bfs_graph;

            greedy_bfs_graph.text.setString("Greedy BFS");

            edge* current = NULL;
            greedy_bfs_graph.insert(nodes[index]->getData(),nodes[index]->getPosX(),nodes[index]->getPosY());
            visited.push_back(nodes[index]);
            
            NodeSeq path;

            while(nodes.size()>greedy_bfs_graph.nodes.size())
            {

                current = getLightestEdge(visited,visited);

                if(current == NULL)
                    break;

                greedy_bfs_graph.insert(current->nodes[0]->getData(),current->nodes[0]->getPosX(),current->nodes[0]->getPosY(),false);
                greedy_bfs_graph.insert(current->nodes[1]->getData(),current->nodes[1]->getPosX(),current->nodes[1]->getPosY(),false);
                greedy_bfs_graph.add_edge(current->nodes[0]->getData(),current->nodes[1]->getData(),current->getData(),current->getDir());
             
                if(current->nodes[0]->getData() == value)
                {    
                    path.clear();
                    path.push_back(current->nodes[0]);
                }

                path.push_back(current->nodes[1]);

                if(current->nodes[1]->getData() == end)
                    break;

                if(std::find(visited.begin(), visited.end(), current->nodes[0]) == visited.end())
                    visited.push_back(current->nodes[0]);
                if(std::find(visited.begin(), visited.end(), current->nodes[1]) == visited.end())
                    visited.push_back(current->nodes[1]);
            }
            
            cout <<endl << "Greedy BFS:"<<endl;
            for(int i = 0; i < path.size();i++)
            {
                cout << path[i]->getData() << " ";
            }
            cout << endl;
            return greedy_bfs_graph;
        }


        E getWeightBetween(int start, int end)
        {
            if(start == end)
                return 0;
            edge* search;
            if(find_edge(start,end,search))
            {
                return search->getData();
            }
            return INFINITE;
        }

        vector<vector<int>> floyd_warshall()
        {
            vector<int> infinites(nodes.size(),INFINITE);
            
            vector<vector<int>> directPaths;
            vector<vector<int>> path;
            for(int i = 0; i < nodes.size();i++)
            {
                directPaths.push_back(infinites);
                
                for(int j = 0;j < nodes.size();j++)
                {
                    directPaths[i][j] = getWeightBetween(i,j);
                }
                path.push_back(directPaths[i]);
            }



            for(int i = 0; i < nodes.size(); i++)
            {
                for(int j = 0; j < nodes.size(); j++)
                {
                    for(int k = 0; k < nodes.size(); k++)
                    {
                        int distance = path[j][i] + path[i][k];
                        if(path[j][k] > distance )
                        {
                            path[j][k] = distance;    
                        }
                    }
                }
            }            

            //print
            cout << endl << "Floyd Warshall Matrix 1:" << endl;
            cout << setw(2)<<" ";
            cout << "|";
                
            for(int i = 0; i < directPaths.size();i++)
            {
                cout << setw(3) << i<< " "; 
            }
            cout << endl;
            for(int i = 0; i < directPaths.size();i++)
            {
                cout << setw(2) << i;
                cout << "|";
                for(int j = 0; j < directPaths[i].size();j++)
                {
                    if(directPaths[i][j] == INFINITE)
                    {
                        cout<< setw(3) << -1 << " ";
                        continue;    
                    }
                    cout<< setw(3) << directPaths[i][j] << " ";
                }
                cout << endl;
            }



            cout << endl << "Floyd Warshall Matrix 2:" << endl;
            cout << setw(2)<<" ";
            cout << "|";
                
            for(int i = 0; i < path.size();i++)
            {
                cout << setw(3) << i<< " "; 
            }
            cout << endl;
            for(int i = 0; i < path.size();i++)
            {
                cout << setw(2) << i;
                cout << "|";
                for(int j = 0; j < path[i].size();j++)
                {
                    if(path[i][j] == INFINITE)
                    {
                        cout<< setw(3) << -1 << " ";
                        continue;    
                    }
                    cout<< setw(3) << path[i][j] << " ";
                }
                cout << endl;
            }

            return path;

        }

        edge* getLightestEdgeDjikstra(map<N,E> distances,NodeSeq visited={},NodeSeq base={})
        {
            if(base.size() == 0)
                return NULL;

            edge* lightest=NULL;
            edge* current_edge=NULL;
            E min = INFINITE+1;

            bool empty = true;

            for(int i = 0; i < base.size(); i++)
            {
                if(base[i]->edges.size() == 0)
                    continue;

                int relativeDist = distances[base[i]->getData()];

                for(int j = 0; j < base[i]->edges.size();j++)
                {
                    empty = false;
                    current_edge = base[i]->edges[j];
                
                    //if node has no edges
                    if(current_edge == NULL)
                        continue;

                    //if edge weight is smaller at leaste the node it goes to or comes from is not in visited
                    else if(current_edge->getData()+relativeDist < min && 
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

        map<N,E> Djikstra(N start)
        {
            map<N,E> distances;

            for(int i = 0; i < nodes.size();i++)
            {
                distances[nodes[i]->getData()] = INFINITE;
            }

            distances[start] = 0;

            map<N,bool> visited;
            for(int i = 0; i < nodes.size();i++)
            {
                visited[nodes[i]->getData()] = false;
            }
            int number_visited = 0;

            vector<node*> visitedVect;

            node* first;
            find_node(start,first);

            node* current = first;

            for(int i = 0; i < current->edges.size();i++)
            {
                int second_data = current->edges[i]->nodes[1]->getData();
                if(distances[second_data] == INFINITE)
                {
                    distances[second_data] = distances[current->getData()] + current->edges[i]->getData();   
                }
            }
            visited[current->getData()] = true;
            number_visited++;
            visitedVect.push_back(first);

            while(number_visited != nodes.size())
            {
                edge* currentLightest = getLightestEdgeDjikstra(distances,visitedVect,visitedVect);
                if( currentLightest == NULL)
                {
                    break;
                }

                current = currentLightest->nodes[1];
                for(int i = 0; i < current->edges.size();i++)
                {
                    int second_data = current->edges[i]->nodes[1]->getData();
                    if(distances[second_data] == INFINITE)
                    {
                        distances[second_data] = distances[current->getData()] + current->edges[i]->getData();   
                    }
                    else if(distances[second_data] > distances[current->getData()] + current->edges[i]->getData())
                    {
                        distances[second_data] = distances[current->getData()] + current->edges[i]->getData();   
                    }
                }
                visited[current->getData()] = true;
                visitedVect.push_back(current);
                number_visited++;
            }


            //print
            cout << endl << "Djikstra:" << endl;
            for(int i = 0; i < nodes.size(); i++)
            {
                cout << setw(2) << i;
                cout << "|";
                if(distances[i] == INFINITE)
                {
                    cout << -1 << endl;
                    continue;    
                }
                cout << distances[i] << endl;

            }

            return distances;
        }


        map<N,E> bellmanFord(N start)
        {
            cout << endl;
            map<N,E> distances;

            for(int i = 0; i < nodes.size();i++)
            {
                distances[nodes[i]->getData()] = INFINITE;
            }

            distances[start] = 0;

            //repeat V-1 times
            for(int i = 0; i < nodes.size()-1;i++)
            {
                //for every edge
                for(int j = 0; j < nodes.size();j++)
                {
                    for(int k = 0;k < nodes[j]->edges.size();k++)
                    {
                        edge* currentEdge = nodes[j]->edges[k];
                        
                        node* nodeFrom = currentEdge->nodes[0];
                        node* nodeTo = currentEdge->nodes[1];

                        if(distances[nodeFrom->getData()] + currentEdge->getData() < distances[nodeTo->getData()])
                        {
                            distances[nodeTo->getData()] =  distances[nodeFrom->getData()] + currentEdge->getData();
                        }
                    }
                }
            }


            for(int j = 0; j < nodes.size();j++)
                {
                    for(int k = 0;k < nodes[j]->edges.size();k++)
                    {
                        edge* currentEdge = nodes[j]->edges[k];
                        
                        node* nodeFrom = currentEdge->nodes[0];
                        node* nodeTo = currentEdge->nodes[1];

                        if(distances[nodeFrom->getData()] + currentEdge->getData() < distances[nodeTo->getData()])
                        {
                            cout << "ERROR: negative weight cycle"; 
                        }
                    }
                }


            //print
            cout << endl << "Bellman Ford:" << endl;
            for(int i = 0; i < nodes.size(); i++)
            {
                cout << setw(2) << i;
                cout << "|";
                if(distances[i] == INFINITE)
                {
                    cout << -1 << endl;
                    continue;    
                }
                cout << distances[i] << endl;

            }
            return distances;
        }

        vector<N> buildPath(map<N,N> cameFrom, N current)
        {
            vector<N> path; 
            path.push_back(current);
            while ( cameFrom.find(current) != cameFrom.end() )
            {
                current = cameFrom[current];
                path.push_back(current);
            }
            return path;
        }

        vector<N> a_star(N start, N end)
        {
            node* first;
            find_node(start,first);

            node* last;
            find_node(end,last);

            vector<N> open;
            vector<N> closed;
            map<N,N> cameFrom;

            open.push_back(start);


            vector<vector<E>>distanceFW = floyd_warshall();

            map<N,double> valorG;
            for(int i = 0; i < nodes.size();i++)
            {
                valorG[nodes[i]->getData()] = INFINITE;
            }
            valorG[start] = 0;



            map <N,double> heuristic;
            for(int i = 0; i < nodes.size();i++)
            {
                //heuristic[nodes[i]->getData()] = sqrt(2*(nodes[i]->getPosX()-last->getPosX())+2*(nodes[i]->getPosY()-last->getPosY()));
                heuristic[nodes[i]->getData()] = distanceFW[nodes[i]->getData()][end];
            }

            cout<< endl << "A *: " << endl;
            cout<< endl << "heuristic: " << endl;


            for(int i = 0; i < nodes.size();i++)
            {
                cout << nodes[i]->getData() << " " << heuristic[nodes[i]->getData()];
                cout << endl;
            }
            
            map<N,double> valorF;
            for(int i = 0; i < nodes.size();i++)
            {
                valorF[nodes[i]->getData()] = INFINITE;
            }
            valorF[start] = heuristic[start];


            while (open.size() > 0)
            {
                N current = getKeywithMin(valorF,open);
                if (current == end)
                {
                    vector<N> path = buildPath(cameFrom,current);

                    for(int i = 0; i< path.size();i++)
                    {
                        cout << path[path.size()-1-i] << " ";
                    } 
                    cout << endl;
                    return path;
                    
                }


                open.erase(std::remove(open.begin(), open.end(), current), open.end());
                closed.push_back(current);

                node* currentNode;
                find_node(current,currentNode);

                for(int i = 0; i < currentNode->edges.size();i++)
                {
                    node* neighbour = currentNode->edges[i]->nodes[1];

                    if(std::find(closed.begin(), closed.end(), neighbour->getData()) != closed.end())
                    {
                        continue;
                    }

                    double tempValorG = valorG[current] + currentNode->edges[i]->getData();


                    if(std::find(open.begin(), open.end(), neighbour->getData()) == open.end())
                    {
                        open.push_back(neighbour->getData());
                    }

                    else if (tempValorG >= valorG[neighbour->getData()])
                    {
                        continue;
                    }            

                    cameFrom[neighbour->getData()] = current;
                    valorG[neighbour->getData()] = tempValorG;
                    valorF[neighbour->getData()] = valorG[neighbour->getData()] + heuristic[neighbour->getData()];         

                }

            }


        }


    private:
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

        N getKeywithMin(map<N,double> valorF, vector<N> open)
        {
            if (open.size() == 0)
                throw "ERROR";


            N min = valorF[open[0]];
            N key = open[0]; 

            for(int i = 1; i < open.size(); i++)
            {
                if(valorF[open[i]] < min)
                {
                    min = valorF[open[i]];
                    key = open[i];
                }

            }
            return key;
        }








        
};


typedef Graph<Traits> graph;

#endif