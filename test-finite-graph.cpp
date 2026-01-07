#include <iostream>
#include <random>
#include <numbers>
#include <ranges>
#include <optional>
#include <set>
#include <vector>
#include <cassert>
#include <algorithm>

#include "randomness-recycler.hpp"

class FiniteGraph{
  std::vector<std::vector<int>> adj;
public:
  struct Vertex{int id;};
  friend bool operator<(Vertex p,Vertex q){
    return p.id<q.id;
  }
  FiniteGraph(int n,std::vector<std::pair<int,int> > edges):adj(n){
    for(int i=0;i<n;i++){
      adj[i].push_back(i);
    }
    for(auto e:edges){
      adj[e.first].push_back(e.second);
      adj[e.second].push_back(e.first);
    }
  }
  template<class RNG>
  Vertex random_vertex(RNG& rng) const{
    assert(!adj.empty());
    std::uniform_int_distribution<> distr(0,adj.size()-1);
    return Vertex{distr(rng)};
  }
  double size() const{
    return adj.size();
  }
  //Random vertex in closed neighborhood
  template<class RNG>
  Vertex random_neighbor(Vertex p,RNG& rng) const{
    std::uniform_int_distribution<> distr(0,adj[p.id].size()-1);
    return Vertex{adj[p.id][distr(rng)]};
  }
  double degree(Vertex p) const{
    return adj[p.id].size();
  }
  bool is_adj(Vertex p,Vertex q) const{
    return std::find(adj[p.id].begin(),adj[p.id].end(),q.id)!=adj[p.id].end();
  }
};

template<class Graph,class RNG>
double estimate_density(Graph graph,double activity,int num_trials,RNG& rng){
  int total_size=0;
  for(int i=1;i<=num_trials;i++){
    std::vector<typename Graph::Vertex> independent_set=random_independent_set(graph,activity,rng);
    total_size+=independent_set.size();
  }
  return 1.0*total_size/num_trials;
}

template<class Graph,class RNG>
void verify_density(Graph graph,double activity,int num_trials,RNG& rng,double truth){
  double est = estimate_density(graph,activity,num_trials,rng);
  std::cout<<"Estimate: "<<est<<" Truth: "<<truth<<std::endl;
  assert(std::abs(est-truth)<0.01);
}

int main(){
  //Estimate the expected size of the independent set by Monte Carlo and check that it is close to ground truth
  std::random_device rd;
  std::mt19937 rng(rd());
  
  verify_density(FiniteGraph(4,{}),1.0,1000000,rng,2.0);
  verify_density(FiniteGraph(4,{{0,1}}),1.0,1000000,rng,5.0/3);
  verify_density(FiniteGraph(4,{{0,1},{0,2},{1,2}}),1.0,1000000,rng,1.25);
  verify_density(FiniteGraph(4,{{0,1},{2,3}}),1.0,1000000,rng,4.0/3);
  verify_density(FiniteGraph(4,{{0,1},{1,2},{2,3}}),1.0,1000000,rng,1.25);
  verify_density(FiniteGraph(4,{{0,1},{0,2},{0,3}}),1.0,1000000,rng,13.0/9);
  verify_density(FiniteGraph(4,{{0,1},{1,2},{2,3},{3,0}}),1.0,1000000,rng,8.0/7);
  verify_density(FiniteGraph(4,{{0,1},{0,2},{0,3},{1,2},{1,3},{2,3}}),1.0,1000000,rng,0.80);
  verify_density(FiniteGraph(4,{{0,1},{1,2},{2,3},{3,0}}),2.0,10000,rng,24.0/17);
}
