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



int main(int argc,char *argv[]){
  double activity=35;
  double radius=0.1;
  if(argc>=2){
    activity=std::stod(argv[1]);
  }
  if(argc>=3){
    radius=std::stod(argv[2]);
  }
  std::random_device rd;
  std::mt19937 rng(rd());
  HardDiskGraph graph(radius);//discs of radius (0.1) on torus [0,1]x[0,1]
  //Runtime jumps up drastically around activity=35
  std::vector<typename HardDiskGraph::Vertex> independent_set=random_independent_set(graph,activity,rng);
  for(HardDiskGraph::Vertex p:independent_set){
    std::cout<<p.x<<" "<<p.y<<std::endl;
  }
}

