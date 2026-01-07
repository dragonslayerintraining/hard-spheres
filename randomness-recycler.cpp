#include <iostream>
#include <random>
#include <numbers>
#include <ranges>
#include <optional>
#include <set>
#include <vector>
#include <cassert>
#include <algorithm>

class DiscreteGraph{
  std::vector<std::vector<int>> adj;
public:
  struct Vertex{int id;};
  friend bool operator<(Vertex p,Vertex q){
    return p.id<q.id;
  }
  DiscreteGraph(int n,std::vector<std::pair<int,int> > edges):adj(n){
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

//Vertex set is [0,1]x[0,1]
//Two vertices are adjacent if their distance is less than the range (2r)
class HardDiskGraph{
  double range;
public:
  struct Vertex{double x,y;};
  //Comparison is used to store vertices in a ordered set, but is otherwise unused by the algorithm
  friend bool operator<(Vertex p,Vertex q){
    return std::make_pair(p.x,p.y)<std::make_pair(q.x,q.y);
  }
  HardDiskGraph(double radius):range(radius*2){
  }
  //Output a uniformly random point from [0,1]x[0,1]
  template<class RNG>
  Vertex random_vertex(RNG& rng) const{
    std::uniform_real_distribution<> distr(0,1);
    double x=distr(rng);
    double y=distr(rng);
    return Vertex{x,y};
  }
  //total measure of vertices
  double size() const{
    return 1.0;
  }
  //Output a uniformly random point from B(p,2r)
  //https://mathworld.wolfram.com/DiskVertexPicking.html
  template<class RNG>
  Vertex random_neighbor(Vertex p,RNG& rng) const{
    std::uniform_real_distribution<> distr(0,1);
    double s=std::sqrt(distr(rng));
    double theta=distr(rng)*2*std::numbers::pi;
    double x=p.x+range*s*std::cos(theta);
    double y=p.y+range*s*std::sin(theta);
    while(x<0) x+=1;
    while(x>1) x-=1;
    while(y<0) y+=1;
    while(y>0) y-=1;
    return Vertex{x,y};
  }
  //measure of neighborhood
  double degree(Vertex p) const{
    return std::numbers::pi*range*range;
  }
  //Returns whether two points are adjacent (i.e. within 2r on the torus)
  bool is_adj(Vertex p,Vertex q) const{
    double dx=std::abs(p.x-q.x),dy=std::abs(p.y-q.y);
    dx=std::min(dx,1-dx);
    dy=std::min(dy,1-dy);
    return std::hypot(dx,dy)<range;
  }
};

//Generate a random independent set distributed according to the hard core model with activity target_lambda
template<class Graph,class RNG>
std::vector<typename Graph::Vertex> random_independent_set(const Graph& graph,const double target_lambda,RNG& rng){
  std::set<std::pair<double,typename Graph::Vertex> > independent_set;
  
  double current_lambda=0;
  //"dents" in the activity:
  // The activity at most places is current_lambda
  // The activity in the neighborhood of each vertex here is reduced to the corresponding value
  std::vector<std::pair<double,typename Graph::Vertex> > dents;
  auto try_add=[&](Graph::Vertex p,double lam){
    for(const auto& [l,q]:independent_set | std::views::reverse){
      if(graph.is_adj(p,q)){
	//q is highest labeled neighbor of p
	independent_set.erase({l,q});
	dents.push_back({l,p});
	dents.push_back({0,q});
	/*
	//REJECTION
	independent_set.clear();
	dents.clear();
	current_lambda=0;
	*/
	return;
      }
    }
    independent_set.insert({lam,p});
  };
  while(true){
    while(dents.size()>=1){
      //dent centered at p down to l
      auto [l,p]=dents.back();
      dents.pop_back();
      
      std::exponential_distribution<> next_for_vertex(graph.degree(p));
      l+=next_for_vertex(rng);
      if(l>current_lambda){
	continue;
      }
      dents.push_back({l,p});
      typename Graph::Vertex q = graph.random_neighbor(p,rng);
      
      bool ignore=false;
      for(const auto& [k,r]:dents){
	if(graph.is_adj(q,r)&&l>k){
	  ignore=true;
	  break;
	}
      }
      if(!ignore){
	try_add(q,l);
      }
    }
    //no dents
    std::exponential_distribution<> next_for_region(graph.size());
    current_lambda+=next_for_region(rng);
    if(current_lambda>target_lambda){
      break;
    }
    typename Graph::Vertex p = graph.random_vertex(rng);
    try_add(p,current_lambda);
  }
  std::vector<typename Graph::Vertex> output;
  for(const auto& [l,p]:independent_set){
    output.push_back(p);
  }
  return output;
}

/*
int main(){
  std::random_device rd;
  std::mt19937 rng(rd());
  HardDiskGraph graph(0.1);//discs of radius 0.1 on [0,1]x[0,1]
  std::vector<typename HardDiskGraph::Vertex> independent_set=random_independent_set(graph,100.0,rng);
  for(HardDiskGraph::Vertex p:independent_set){
    std::cout<<p.x<<" "<<p.y<<std::endl;
  }
}
*/

int main(){
  std::random_device rd;
  std::mt19937 rng(rd());
  //DiscreteGraph graph(4,{});//2.0
  //DiscreteGraph graph(4,{{0,1}});//1.666
  //DiscreteGraph graph(4,{{0,1},{0,2},{1,2}});//1.25
  //DiscreteGraph graph(4,{{0,1},{2,3}});//1.333
  //DiscreteGraph graph(4,{{0,1},{1,2},{2,3}});//1.25
  //DiscreteGraph graph(4,{{0,1},{0,2},{0,3}});//1.44
  //DiscreteGraph graph(4,{{0,1},{1,2},{2,3},{3,0}});//1.143
  DiscreteGraph graph(4,{{0,1},{0,2},{0,3},{1,2},{1,3},{2,3}});//0.80
  int total_size=0;
  for(int i=1;i<=10000000;i++){
    std::vector<typename DiscreteGraph::Vertex> independent_set=random_independent_set(graph,1.0,rng);
    total_size+=independent_set.size();
    if(i%100000==0){
      std::cout<<"Iteration "<<i<<std::endl;
      for(DiscreteGraph::Vertex p:independent_set){
	std::cout<<p.id<<std::endl;
      }
      std::cout<<"Average size: "<<(double)total_size/i<<std::endl;
    }
  }
}
