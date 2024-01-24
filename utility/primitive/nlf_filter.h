#ifndef SUBGRAPHMATCHING_NLF_FILTER_H
#define SUBGRAPHMATCHING_NLF_FILTER_H

#include "graph/graph.h"
#include "utility/relation/catalog.h"
#include <vector>

class nlf_filter {
private:
    const Graph* query_graph_;
    const Graph* data_graph_;
    std::vector<char> status_;
    std::vector<uint32_t> updated_;

private:
    void filter_ordered_relation(uint32_t u, edge_relation* relation);
    void filter_unordered_relation(uint32_t u, edge_relation* relation);
public:
    nlf_filter(const Graph* query_graph, const Graph* data_graph) {
        query_graph_ = query_graph;
        data_graph_ = data_graph;
        status_.resize(data_graph->getVerticesCount(), 'u');
        updated_.reserve(1024);
    }

    void execute(std::vector<std::vector<uint32_t>> &candidate_sets);
    void execute(catalog* storage);
    const ui distance(std::string source,std::string target) const{
    
	    //step 1
	
		int n=source.length();
		int m=target.length();
		if (m==0) return n;
		if (n==0) return m;
		//Construct a matrix
		typedef std::vector< std::vector<int> >  Tmatrix;
		Tmatrix matrix(n+1);
		for(int i=0; i<=n; i++)  matrix[i].resize(m+1);
	
	    //step 2 Initialize

    	for(int i=1;i<=n;i++) matrix[i][0]=i;
    	for(int i=1;i<=m;i++) matrix[0][i]=i;

     //step 3
     	for(int i=1;i<=n;i++)
     	{
        const char si=source[i-1];
        //step 4
	        for(int j=1;j<=m;j++)
	        {
	
	            const char dj=target[j-1];
	            //step 5
	            int cost;
	            if(si==dj){
	                cost=0;
	            }
	            else{
	                cost=1;
	            }
	            //step 6
	            const int above=matrix[i-1][j]+1;
	            const int left=matrix[i][j-1]+1;
	            const int diag=matrix[i-1][j-1]+cost;
	            matrix[i][j]=std::min(above,std::min(left,diag));
	
	        }
     	}//step7
      return matrix[n][m];
    
     }
};


#endif //SUBGRAPHMATCHING_NLF_FILTER_H
