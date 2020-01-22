//
// Created by Andrey Solovyev on 04/01/2020.
//

#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <optional>

#include <random>
#include <chrono>

#include <utility>
#include <algorithm>
#include <cstdint>
#include <iostream>

#include <exception>
#ifndef DIJKSTRA_DIJKSTRA_H
#define DIJKSTRA_DIJKSTRA_H

namespace Dijkstra {

using namespace std::chrono;

class Randomer {
public:
	using UniformedInt = std::uniform_int_distribution<>;
	using UniformedReal = std::uniform_real_distribution<>;

	Randomer()
			:mtre{std::random_device{}()} { }//!ctor

	double operator()(double lowBound, double upperBound, [[maybe_unused]] int seed = std::random_device{}())
	{
		//std::mt19937_64 mtre(seed);
		return UniformedReal{lowBound, upperBound}(mtre);
	}//!func

	int operator()(int lowerBound, int upperBound, [[maybe_unused]] int seed = std::random_device{}())
	{
		//std::mt19937_64 mtre(seed);
		return UniformedInt{lowerBound, upperBound}(mtre);
	}//!func

	int SetSeed()
	{
		microseconds ms = duration_cast<microseconds>(system_clock::now().time_since_epoch());
		double rndBase = ms.count()/13.*3.141592653589793;
		int seed = unsigned((rndBase-int(rndBase)))*1e6;
		return seed;
	}//!func

private:
	std::mt19937 mtre;

};//!class randomer

struct Vertex {
  std::string name = "";
  std::string_view name_sv = "";
  Vertex* dijkstra_previous = nullptr;
  bool dijkstra_visited = false;
  unsigned int dijkstra_value = INT_MAX;
  bool operator==(const Vertex& rhs) const
  {
	  return this->name==rhs.name;
  }//!operator bool
};//!struct Vertex

struct VertexHasher {
  size_t operator()(const Vertex& vertex) const
  {
	  size_t primeNumber = 2017;
	  return (s_hash(vertex.name)*primeNumber+primeNumber);
  }//!operator
  std::hash<std::string> s_hash;
};//!struct Hasher
//using Vertexes =  std::vector<Vertex>;

using Vertexes =  std::unordered_set<Vertex, VertexHasher>;

std::ostream& operator<<(std::ostream& ostream, const Vertex& vertex)
{
	ostream << "Vertex: \"" << vertex.name << "\"";
	return ostream;
}//!ostream operator Vertex

std::ostream& operator<<(std::ostream& ostream, const Vertexes vertexes)
{
	for (const auto& vertex : vertexes) ostream << vertex << '\n';
	return ostream;
}//!ostream operator Vertexes

struct Edge {
  int value = 0;
  Vertex* from = nullptr;
  Vertex* to = nullptr;

  bool operator==(const Edge& rhs) const
  {
	  return this->from==rhs.from and this->to==rhs.to;
  }//!operator bool
};//!struct Edge

struct EdgeHasher {
  size_t operator()(const Edge& edge) const
  {
	  size_t primeNumber = 2017;
	  return (
			  vertexHasher(*edge.from)*primeNumber*primeNumber+
					  vertexHasher(*edge.to)*primeNumber+
					  primeNumber
	  );
  }//!operator
  VertexHasher vertexHasher;
};//!struct Hasher

using Edges = std::unordered_set<Edge, EdgeHasher>;
//using Edges = std::vector<Edge>;

std::ostream& operator<<(std::ostream& ostream, const Edge& edge)
{
	ostream << "from: " << (*edge.from) << ", to: " << (*edge.to) << "; value: " << edge.value;
	return ostream;
}//!ostream operator Edge

std::ostream& operator<<(std::ostream& ostream, const Edges edges)
{
	for (const auto& edge : edges) ostream << edge << '\n';
	return ostream;
}//!ostream operator Edges

struct Routes {
  using VertexEdges = std::unordered_map<Vertex*, std::vector<Edge*>>;
  VertexEdges vertex_edges;

};//!struct Routes

std::ostream& operator<<(std::ostream& ostream, const Routes& routes)
{

	for (const auto& vertex_edges : routes.vertex_edges)
		for (const auto& edge : vertex_edges.second)
			ostream << *edge << '\n';
	return ostream;
} //!ostream operator Routes


struct GraphInit {
  int from = 0, to = 0, value = 0;
};//!struct GraphInit

using GraphParameters = std::vector<GraphInit>;

class GraphForDijkstra {
public:
	using FromToIndexes = std::pair<int, int>;

	explicit GraphForDijkstra(int vertexesQty = 0, int edgesQty = 0)
			:vertexesQty(vertexesQty), edgesQty(edgesQty)
	{
		if (vertexesQty*(vertexesQty-1)<edgesQty)
			throw std::invalid_argument(
					"Wrong params, a unique vertex can't have more than 2 edges to another unique vertex. \n");
		else if (vertexesQty!=0 and edgesQty!=0) {
			InitVertexes();
			InitEdges();
			InitRoutes();
		}
	}//!ctor
	const Vertexes& GetVertexesConst() const { return vertexes; }//!func
	Vertexes& GetVertexesMutable() { return vertexes; }//!func
	const Edges& GetEdgesConst() const { return edges; }//!func
	Edges& GetEdgesMutable() { return edges; }//!func
	const Routes& GetRoutesConst() const { return routes; }//!func
	Routes& GetRoutesMutable() { return routes; }//!func

	void InitGraph(const GraphParameters& params)
	{
		routes.vertex_edges.clear();
		edges.clear();
		vertexes.clear();

		for (auto& param : params) {
			Vertex vertex;
			vertex.name = std::to_string(param.from);
			vertex.name_sv = vertex.name;
			vertexes.emplace(std::move(vertex));
		}//!for Vertex Init

		for (auto& param : params)
			AddEdge(CreateEdge(param.from, param.to, param.value));

		InitRoutes();

	}//!func

private:
	const int vertexesQty, edgesQty;

	Randomer randomer;

	Vertexes vertexes;
	Edges edges;
	Routes routes;

	void InitVertexes()
	{
		for (int i = 0; i<vertexesQty; ++i) {
			Vertex vertex;
			vertex.name = std::to_string(i);
			vertex.name_sv = vertex.name;
			vertexes.emplace(std::move(vertex));
		}
	}//!func
	FromToIndexes GetVertexesIndexes()
	{
		int
				from = randomer(0, vertexesQty-1),
				to = randomer(0, vertexesQty-1);
		while (from==to) to = randomer(0, vertexesQty-1);

		return {from, to};
	}//!func
	Edge CreateEdge(int low_bound = 1, int upper_bound = 20)
	{
		FromToIndexes vertexes_indexes = GetVertexesIndexes();

		Vertex* from = const_cast<Vertex*>(&(*vertexes.find({std::to_string(vertexes_indexes.first)})));
		Vertex* to = const_cast<Vertex*>(&(*vertexes.find({std::to_string(vertexes_indexes.second)})));

		return {randomer(low_bound, upper_bound), from, to};
	}//!func
	Edge CreateEdge(int from, int to, int value)
	{

		Vertex* vertex_from = const_cast<Vertex*>(&(*vertexes.find({std::to_string(from)})));
		Vertex* vertex_to = const_cast<Vertex*>(&(*vertexes.find({std::to_string(to)})));

		return {value, vertex_from, vertex_to};
	}//!func
	bool CheckEdgeAlreadyExists(const Edge& edge)
	{
		auto it = edges.find(edge);
		return it==edges.end() ? false : true;
	}//!func
	void AddEdge(Edge edge)
	{
		edges.emplace(edge);
	}//!func
	void InitEdges()
	{
		for (int i = 0; i<edgesQty; ++i) {
			Edge edge = CreateEdge();
			while (CheckEdgeAlreadyExists(edge)) edge = CreateEdge();
			AddEdge(std::move(edge)); //TODO wtf?
		}//!for
	}//!func
	void InitRoutes()
	{
		for (const auto& edge : edges)
			routes.vertex_edges[edge.from].push_back(const_cast<Edge*> (&edge));
	}//!func

};//!class graph

class Dijkstra {
public:
	using PathLength = int;

	explicit Dijkstra(int vertexQty = 0, int edgesQty = 0)
			:GraphForDijkstra(vertexQty, edgesQty),
			 vertexes (graph.GetVertexesMutable()) {
	}//!ctor
	int GetPathLength(int from, int to)
	{

		[[maybe_unused]] Vertex* vertex_from = const_cast<Vertex*> ( &(*(graph.GetVertexesMutable().find(
				{std::to_string(from)}))));
		[[maybe_unused]] Vertex* vertex_to = const_cast<Vertex*> (&(*(graph.GetVertexesMutable().find(
				{std::to_string(to)}))));

		if (vertex_from == vertex_to) path_length = 0;
		else GetPath(vertex_from, vertex_to);

/*
		std::cout << "From: " << *vertex_from << ", To: " << *vertex_to << ", length is: " << path_length << '\n';
		for (auto* vertex : vertexes)
			if (vertex->dijkstra_previous) std::cout << *vertex << ": " << *vertex->dijkstra_previous << ' ';
			else std::cout << *vertex << ": " << '-' << ' ';
		std::cout << '\n';
*/

		return path_length;

	}//!func
	GraphForDijkstra& GetGraphMutable() {return graph;}//!func
private:
	GraphForDijkstra graph;
	PathLength path_length;
//	std::vector<Vertex*> vertexes;
	Vertexes vertexes;

	std::vector<Edge*> GetNextEdges (Vertex* current_vertex, Routes& routes) {
		std::vector<Edge*> edges_next;

		if (auto r = routes.vertex_edges.find(current_vertex); r != routes.vertex_edges.end()) {
			if (!r->second.empty()) {

				for (auto& edge : r->second)
					if ((current_vertex->dijkstra_value + edge->value) < edge->to->dijkstra_value ) {
						edge->to->dijkstra_value = (current_vertex->dijkstra_value + edge->value);
						edge->to->dijkstra_previous = current_vertex;
						edges_next.push_back(edge);
					}
			}//!if empty connections lists
			std::sort(edges_next.begin(), edges_next.end(),
					[](const Edge* a, const Edge* b){return a->to->dijkstra_value > b->to->dijkstra_value;});
		}//!if vertex found in graph

		current_vertex->dijkstra_visited = true;
		return edges_next;
	}//!func

	void UpdateSPT (std::vector<Edge*>& edges_sorted, Routes& routes_input, Routes& routes_opt) {

		if (edges_sorted.empty()) return;

		while (!edges_sorted.empty()) {
			Vertex* current_vertex = edges_sorted.back()->to;
			edges_sorted.pop_back();

			auto edges_next = GetNextEdges(current_vertex, routes_input);
			if (!edges_next.empty()) {
				routes_opt.vertex_edges[current_vertex].push_back(edges_next.back());
//				edges_next.pop_back();
				UpdateSPT(edges_next, routes_input, routes_opt);
			}
		}//!while

	}//!func

	void GetPath(Vertex* from, Vertex* to) {

		Routes opt;

		Routes r = graph.GetRoutesMutable();

		//get entire Start from original graph, remove Start from original graph
		from->dijkstra_visited = true;
		from->dijkstra_value = 0;
		from->dijkstra_previous = nullptr;

		Vertex* current_vertex = from;

		while (current_vertex) {
			auto edges_next = GetNextEdges(current_vertex, r);

			if (edges_next.empty()) break;
			current_vertex = edges_next.back()->to;
//			edges_next.pop_back();

			UpdateSPT (edges_next, r, opt);
		}//!while

		path_length = to->dijkstra_value;

		for (auto& vertex_edges : r.vertex_edges) {
			vertex_edges.first->dijkstra_visited = false;
			vertex_edges.first->dijkstra_previous = nullptr;
			vertex_edges.first->dijkstra_value = INT_MAX;
		}//!for

	}//!func

};//!class Dijkstra

}//!namespace

#endif //DIJKSTRA_DIJKSTRA_H
