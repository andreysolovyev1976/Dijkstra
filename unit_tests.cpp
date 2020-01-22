//
// Created by Andrey Solovyev on 05/01/2020.
//

#include"test_runner.h"
#include"dijkstra.h"
#include"graph.h"
#include"router.h"

template <typename T>
Graph::DirectedWeightedGraph<T> InitializeGraphsYandex(size_t vertexQty){
	std::vector<Graph::Edge<int>> graph_parameters = {
			{0, 1, 1},
			{1, 2, 2},
			{2, 0, 3},
			{2, 3, 4},
			{3, 1, 5},
			{3, 4, 6},
			{4, 2, 7},
			{4, 0, 8},
			{1, 4, 9},

			{2, 6, 10},
			{6, 3, 11},
			{3, 5, 16},
			{5, 4, 15},

			{6, 7, 12},
			{7, 5, 13},
			{5, 6, 14},
			{7, 8, 17},
			{8, 9, 18},
			{9, 7, 19},
			{9, 5, 20},
			{6, 9, 21},
			{5, 8, 22},
			{8, 6, 23},

	};

	Graph::DirectedWeightedGraph<T> graph(vertexQty);
	for (auto& edge : graph_parameters)
		graph.AddEdge(edge);


	return graph;

}//!func

void InitializeSelfWrittenGraph(Dijkstra::Dijkstra& dijkstra){
	Dijkstra::GraphParameters graph_params  = {
			{0, 1, 1},
			{1, 2, 2},
			{2, 0, 3},
			{2, 3, 4},
			{3, 1, 5},
			{3, 4, 6},
			{4, 2, 7},
			{4, 0, 8},
			{1, 4, 9},

			{2, 6, 10},
			{6, 3, 11},
			{3, 5, 16},
			{5, 4, 15},

			{6, 7, 12},
			{7, 5, 13},
			{5, 6, 14},
			{7, 8, 17},
			{8, 9, 18},
			{9, 7, 19},
			{9, 5, 20},
			{6, 9, 21},
			{5, 8, 22},
			{8, 6, 23},
	};

	dijkstra.GetGraphMutable().InitGraph(graph_params);

}//!func

void TestAccuracy() {
	size_t vertexesQty = 10;

	Graph::DirectedWeightedGraph<int> graph = InitializeGraphsYandex<int>(vertexesQty);
	Graph::Router router(graph);

	Dijkstra::Dijkstra dijkstra;
	InitializeSelfWrittenGraph(dijkstra);

	bool all_ok = true;
	for (unsigned int i = 0; i<vertexesQty; ++i)
		for (unsigned int j = 0; j<vertexesQty; ++j)
			if (i != j) {
				auto route = router.BuildRoute(i, j);
				if (route.value().weight!=dijkstra.GetPathLength(i, j))
				{
					std::cerr << i <<", " << j << '\n';
					all_ok = false;
				}
			}

	ASSERT(all_ok);

}//!func


void RunTests(){
	TestRunner tr;
	RUN_TEST(tr, TestAccuracy);

}//!func
