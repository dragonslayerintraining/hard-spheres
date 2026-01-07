#include <iostream>
#include <random>
#include <numbers>
#include <ranges>
#include <optional>
#include <set>
#include <vector>


/* region is [0,1]x[0,1] */
//const double target_lambda = 100.0;
//const double range = 2*0.1;

std::random_device rd;
std::mt19937 rng(rd());


//Graph whose vertex set is 
class HardDiskGraph{
  double range;
public:
  struct Point{double x,y;};
  friend bool operator<(Point p,Point q){
    return std::make_pair(p.x,p.y)<std::make_pair(q.x,q.y);
  }
  friend std::ostream& operator<<(std::ostream& stream,const Point& p){
    return stream<<"("<<p.x<<","<<p.y<<")";
  }
  HardDiskGraph(double radius):range(radius*2){
  }
  Point random_vertex() const{
    std::uniform_real_distribution<> distr(0,1);
    double x=distr(rng);
    double y=distr(rng);
    return Point{x,y};
  }
  //total measure of vertices
  double size() const{
    return 1.0;
  }
  //https://mathworld.wolfram.com/DiskPointPicking.html
  //Output a uniformly random point from B(p,2r)
  Point random_neighbor(Point p) const{
    std::uniform_real_distribution<> distr(0,1);
    double s=std::sqrt(distr(rng));
    double theta=distr(rng)*2*std::numbers::pi;
    double x=p.x+range*s*std::cos(theta);
    double y=p.y+range*s*std::sin(theta);
    while(x<0) x+=1;
    while(x>1) x-=1;
    while(y<0) y+=1;
    while(y>0) y-=1;
    return Point{x,y};
  }
  double degree(Point p) const{
    return std::numbers::pi*range*range;
  }

  //Returns whether two points are adjacent (i.e. within 2r on the torus)
  bool is_adj(Point p,Point q) const{
    double dx=std::abs(p.x-q.x),dy=std::abs(p.y-q.y);
    dx=std::min(dx,1-dx);
    dy=std::min(dy,1-dy);
    return std::hypot(dx,dy)<range;
  }
};

//Generate a random independent set distributed according to the hard core model on [0,1]x[0,1] where points of distance <range are adjacent
template<class Graph>
std::vector<typename Graph::Point> random_independent_set(const Graph graph,const double target_lambda){
  std::set<std::pair<double,typename Graph::Point> > independent_set;

  //"dents" in the activity:
  // The ball of radius 2r around the point is reduced to this value
  std::set<std::pair<double,typename Graph::Point> > dents;
  double lambda=0;
  auto try_add=[&](Graph::Point p,double lam){
    std::cerr<<"try_add"<<std::endl;
    for(const auto& [l,q]:independent_set | std::views::reverse){
      std::cerr<<l<<" "<<q<<std::endl;
      if(graph.is_adj(p,q)){
	//q is highest labeled neighbor of p
	independent_set.erase({l,q});
	dents.insert({l,p});
	dents.insert({0,q});
	std::cerr<<"Recycling"<<std::endl;
	return;
      }
    }
    independent_set.insert({lam,p});
  };
  
  while(true){
    while(dents.size()>=1){
      //lift the lowest dent
      std::pair<double,typename Graph::Point> lowest_dent=*dents.begin();
      dents.erase(dents.begin());
      
      std::exponential_distribution<> next_for_vertex(graph.degree(lowest_dent.second));
      lowest_dent.first+=next_for_vertex(rng);
      if(lowest_dent.first>((dents.size()>=1)?dents.begin()->first:lambda)){
	continue;
      }
      dents.insert(lowest_dent);
      typename Graph::Point p = graph.random_neighbor(lowest_dent.second);
      try_add(p,lowest_dent.first);
    }
    //no dents
    std::exponential_distribution<> next_for_region(graph.size());
    lambda+=next_for_region(rng);
    if(lambda>target_lambda){
      break;
    }
    typename Graph::Point p = graph.random_vertex();
    try_add(p,lambda);
  }
  std::vector<typename Graph::Point> output;
  for(const auto& [l,p]:independent_set){
    output.push_back(p);
  }
  return output;
}

int main(){
  HardDiskGraph graph(0.1);//discs of radius 0.1 on [0,1]x[0,1]
  std::vector<typename HardDiskGraph::Point> independent_set=random_independent_set(graph,100.0);
  for(HardDiskGraph::Point p:independent_set){
    std::cout<<p.x<<" "<<p.y<<std::endl;
  }
}
