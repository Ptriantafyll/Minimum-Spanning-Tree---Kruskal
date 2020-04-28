#include "stdio.h"
#include "iostream"
#include "LEDA/graph/graph.h"
#include "LEDA/core/random_source.h"
#include "LEDA/graph/node_array.h"
#include "LEDA/graph/edge_array.h"
#include "LEDA/core/p_queue.h"

using namespace leda;

struct NodeInfo {					//keep info for every node
	bool visited;
	node first;
	list<node>* list_ptr;
};


list<edge> KRUSKAL(const graph& G, const edge_array<int>& cost, node_array<NodeInfo> &nodeInfo) {
	list<edge> mst;
	list<list<node>*> lists;

	p_queue<int, edge> P;
	edge e;
	forall_edges(e, G) {			//insert all edges in a priority queue (cost = priority)
		P.insert(cost[e], e);
	}

	while ( !P.empty() ) {
		 
		e = P.inf(P.find_min());	//min priority = min cost -> e is edge with minimum cost
		P.del_min();
		node s = G.source(e);		//starting node of edge e
		node t = G.target(e);		//ending node of edge e

		G.print_edge(e);

		if (s == t) continue;

		if (!nodeInfo[s].visited && !nodeInfo[t].visited ) //if both source and target have not been visited before
		{
			list<node>* temp = new list<node>;	//temporary list so that I can store into lists

			temp->append(s);			//insert both nodes into temp
			temp->append(t);

			nodeInfo[s].visited = true;
			nodeInfo[t].visited = true;
			nodeInfo[s].first = s;	//set first of both nodes as s (random)
			nodeInfo[t].first = s;
			nodeInfo[s].list_ptr = temp; //list pointers of both nodes point to the same list now
			nodeInfo[t].list_ptr = temp;

			lists.append(temp);		//store temp
			mst.append(e);

		}
		else if ( nodeInfo[s].visited && !nodeInfo[t].visited ) //if source has been visited and target has not
		{			
			nodeInfo[t].visited = true;			
			nodeInfo[t].list_ptr = nodeInfo[s].list_ptr;		//set target node's list pointer to be the same as source's
			nodeInfo[s].list_ptr->append(t);					//insert target (not visited node) into the list source (visited node) is in
			nodeInfo[t].first = nodeInfo[s].first;				//set first of target to be the same as the first of source
			mst.append(e);
		}
		else if (!nodeInfo[s].visited && nodeInfo[t].visited) //if target has been visited and source has not
		{
			nodeInfo[s].visited = true;
			nodeInfo[s].list_ptr = nodeInfo[t].list_ptr;		//set source node's list pointer to be the same as target's
			nodeInfo[t].list_ptr->append(s);					//insert source (not visited node) into the list target (visited node) is in
			nodeInfo[s].first = nodeInfo[t].first;				//set first of source to be the same as the first of target
			mst.append(e);
		}
		else											//if both have been visited
		{														//check if inserting e creates cycle
			if (nodeInfo[s].first != nodeInfo[t].first) //if it creates cycle
			{
				node u;
				if (nodeInfo[s].list_ptr->size() >= nodeInfo[t].list_ptr->size())  //if source node's list is bigger
				{
					forall(u, *nodeInfo[t].list_ptr)
					{
						nodeInfo[u].first = nodeInfo[s].first;
						if (u != t) nodeInfo[u].list_ptr = nodeInfo[s].list_ptr;
					}
					nodeInfo[s].list_ptr->conc(*nodeInfo[t].list_ptr, leda::behind);	//append target's list to node's and target's list becomes empty
					nodeInfo[t].list_ptr = nodeInfo[s].list_ptr;
				}
				else										//if target node's list is bigger
				{
					forall(u, *nodeInfo[s].list_ptr)
					{
						nodeInfo[u].first = nodeInfo[t].first;
						if (u != s) nodeInfo[u].list_ptr = nodeInfo[t].list_ptr;
					}
					nodeInfo[t].list_ptr->conc(*nodeInfo[s].list_ptr, leda::behind);	//append source's list to node's and source's list becomes empty	
					nodeInfo[s].list_ptr = nodeInfo[t].list_ptr;


				}
				mst.append(e);
			}
			else										//if it does not create cycle
			{
				continue;
			}
		}
	}
	return mst;
}


int main() {
	graph G;
	random_graph(G, 1000, 6000);
	Make_Connected(G);

	edge_array<int> cost(G);
	random_source S;
	edge e;
	forall_edges(e, G) {
		cost[e] = (S() % 9991) +10;
	}

	node_array<NodeInfo> nodeInfo(G);
	node v;
	forall_nodes(v, G) {
		nodeInfo[v].visited = false;	
		nodeInfo[v].first = nil;
		nodeInfo[v].list_ptr = nil;
	}

	list<edge> mst = KRUSKAL(G, cost, nodeInfo);
	std::cout << "\n\n";
	int counter = 0;
	forall(e, mst) {
		counter++;
		G.print_edge(e);
		std::cout << " with cost " << cost[e] << " \n";
	}

	std::cout << "number of nodes : " << G.number_of_nodes() << "\n";
	std::cout << "number of edges : " << G.number_of_edges() << "\n";
	std::cout << "number of edges in mst : " << counter << "\n";

	return 0;
}