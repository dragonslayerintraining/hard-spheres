#include <iostream>
#include <random>
#include <numbers>
#include <ranges>
#include <optional>
#include <set>

struct Point{double x,y;};

bool operator<(Point p,Point q){
  return std::make_pair(p.x,p.y)<std::make_pair(q.x,q.y);
}

std::ostream& operator<<(std::ostream& stream,const Point& p){
  return stream<<"("<<p.x<<","<<p.y<<")";
}

/* region is [0,1]x[0,1] */

const double target_lambda = 10.0;
const double radius = 0.1;

std::random_device rd;
std::mt19937 rng(rd());

std::exponential_distribution<> next_for_disk(4*std::numbers::pi*radius*radius);
std::exponential_distribution<> next_for_region(1);

Point random_point_in_region(){
  std::uniform_real_distribution<> distr(0,1);
  double x=distr(rng);
  double y=distr(rng);
  return Point{x,y};
}

//https://mathworld.wolfram.com/DiskPointPicking.html
Point random_point_in_disk(Point p){
  std::uniform_real_distribution<> distr(0,1);
  double r=std::sqrt(distr(rng));
  double theta=distr(rng)*2*std::numbers::pi;
  return Point{p.x+r*std::cos(theta),p.y+r*std::sin(theta)};
}

bool is_adj(Point p,Point q){
  double dx=std::abs(p.x-q.x),dy=std::abs(p.y-q.y);
  dx=std::min(dx,1-dx);
  dy=std::min(dy,1-dy);
  return std::hypot(dx,dy)<2*radius;
}

std::set<std::pair<double,Point> > independent_set;

//"dents" in the activity: the ball of radius 2r around the point is reduced to this value
std::set<std::pair<double,Point> > dents;
double lambda=0;

void try_add(Point p,double lam){
  std::cerr<<"try_add"<<std::endl;
  for(const auto& [l,q]:independent_set | std::views::reverse){
    std::cerr<<l<<" "<<q<<std::endl;
    if(is_adj(p,q)){
      //q is highest labeled neighbor of p
      independent_set.erase({l,q});
      dents.insert({l,p});
      dents.insert({0,q});
      std::cerr<<"Recycling"<<std::endl;
      return;
    }
  }
  independent_set.insert({lam,p});
}
  
int main(){
  while(true){
    while(dents.size()>=1){
      //lift the lowest dent
      std::pair<double,Point> lowest_dent=*dents.begin();
      dents.erase(dents.begin());
      
      double next_lambda=(dents.size()>=1)?dents.begin()->first:lambda;
      double t=lowest_dent.first+next_for_disk(rng);
      if(t>next_lambda){
	continue;
      }
      lowest_dent.first=t;
      Point p = random_point_in_region();
      try_add(p,lowest_dent.first);
    }
    //no dents
    double t=lambda+next_for_region(rng);
    if(t>target_lambda){
      lambda=target_lambda;
      break;
    }
    lambda=t;
    Point p = random_point_in_region();
    try_add(p,lambda);
  }
  for(const auto& [l,p]:independent_set){
    std::cout<<p.x<<" "<<p.y<<" "<<l<<std::endl;
  }
}
