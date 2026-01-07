#ifndef RANDOMNESS_RECYCLER_HPP_
#define RANDOMNESS_RECYCLER_HPP_

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
	
	dents.push_back({0,q});
	dents.push_back({l,p});
	//std::cerr<<"Recycle "<<dents.size()<<std::endl;
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
    //std::cerr<<"current_lambda: "<<current_lambda<<std::endl;
    typename Graph::Vertex p = graph.random_vertex(rng);
    try_add(p,current_lambda);
  }
  std::vector<typename Graph::Vertex> output;
  for(const auto& [l,p]:independent_set){
    output.push_back(p);
  }
  return output;
}

#endif//RANDOMNESS_RECYCLER_HPP_
