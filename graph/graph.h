//
// Created by ssunah on 6/22/18.
//

#ifndef SUBGRAPHMATCHING_GRAPH_H
#define SUBGRAPHMATCHING_GRAPH_H

#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include "utility/sparsepp/spp.h"
#include "configuration/types.h"
#include "configuration/config.h"

/**
 * A graph is stored as the CSR format.
 */
using spp::sparse_hash_map;
class Graph {
private:
    bool enable_label_offset_;

    ui vertices_count_;
    ui edges_count_;
    ui labels_count_;
    ui max_degree_;
    ui max_label_frequency_;

    ui* offsets_;
    VertexID * neighbors_;
    std::vector<std::string> labels_;//----------------------------------------
    ui* reverse_index_offsets_;
    ui* reverse_index_;

    int* core_table_;
    ui core_length_;

	
    std::unordered_map<std::string, ui> labels_frequency_;//---------------------------------������ 
    sparse_hash_map<std::string, std::vector<edge>* >* edge_index_;

#if OPTIMIZED_LABELED_GRAPH == 1
    ui* labels_offsets_;//-----------------------------------------------------
    sparse_hash_map<std::string, uint32_t>* nlf_;
#endif

private:
    void BuildReverseIndex();

#if OPTIMIZED_LABELED_GRAPH == 1
    void BuildNLF();
    void BuildLabelOffset();
#endif

public:
    Graph(const bool enable_label_offset) {
        enable_label_offset_ = enable_label_offset;

        vertices_count_ = 0;
        edges_count_ = 0;
        labels_count_ = 0;
        max_degree_ = 0;
        max_label_frequency_ = 0;
        core_length_ = 0;

        offsets_ = NULL;
        neighbors_ = NULL;
        //labels_ = NULL;
        reverse_index_offsets_ = NULL;
        reverse_index_ = NULL;
        core_table_ = NULL;
        labels_frequency_.clear();
        edge_index_ = NULL;
#if OPTIMIZED_LABELED_GRAPH == 1
        labels_offsets_ = NULL;
        nlf_ = NULL;
#endif
    }

    ~Graph() {
        delete[] offsets_;
        delete[] neighbors_;
        //delete[] labels_;
        delete[] reverse_index_offsets_;
        delete[] reverse_index_;
        delete[] core_table_;
        delete edge_index_;
#if OPTIMIZED_LABELED_GRAPH == 1
        delete[] labels_offsets_;
        delete[] nlf_;
#endif
    }

public:
    void loadGraphFromFile(const std::string& file_path);
    void loadGraphFromFileCompressed(const std::string& degree_path, const std::string& edge_path,
                                     const std::string& label_path);
    void storeComparessedGraph(const std::string& degree_path, const std::string& edge_path,
                               const std::string& label_path);
    void printGraphMetaData();
public:
    const ui getLabelsCount() const {
        return labels_count_;
    }

    const ui getVerticesCount() const {
        return vertices_count_;
    }

    const ui getEdgesCount() const {
        return edges_count_;
    }

    const ui getGraphMaxDegree() const {
        return max_degree_;
    }

    const ui getGraphMaxLabelFrequency() const {
        return max_label_frequency_;//--------------------------------------------
    }

    const ui getVertexDegree(const VertexID id) const {
        return offsets_[id + 1] - offsets_[id];
    }

    const ui getLabelsFrequency(const std::string label) const {//---------------------------------
        return labels_frequency_.find(label) == labels_frequency_.end() ? 0 : labels_frequency_.at(label);
    }

    const ui getCoreValue(const VertexID id) const {
        return core_table_[id];
    }

    const ui get2CoreSize() const {
        return core_length_;
    }
    const std::string getVertexLabel(const VertexID id) const {
        return labels_[id];
    }

    const ui * getVertexNeighbors(const VertexID id, ui& count) const {
        count = offsets_[id + 1] - offsets_[id];
        return neighbors_ + offsets_[id];
    }

    const sparse_hash_map<std::string, std::vector<edge>*>* getEdgeIndex() const {
        return edge_index_;
    }
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

    const ui * getVerticesByLabel(const std::string id, ui& count) const {//-----------------------------------------
        
        //std::cout<<" get!!!!!!!\n";
        ui * matchnode=new ui[vertices_count_];
        ui cnt=0;
        for(int i=0;i<vertices_count_;i++){
        	std::string label1=getVertexLabel(i);
        	//std::cout<<"jhhh\n";
        	if(distance(id,label1)<1){
        	//std::cout<<"2222\n";
				 matchnode[cnt++]=i;
				// std::cout<<id<<" "<<label1<<std::endl;
			}
		}
		count=cnt;
		std::cout<<"getverticesbylabel over\n";
		//count = reverse_index_offsets_[id + 1] - reverse_index_offsets_[id];
//        return reverse_index_ + reverse_index_offsets_[id];
		return matchnode;
    }

#if OPTIMIZED_LABELED_GRAPH == 1
    //const ui * getNeighborsByLabel(const VertexID id, const std::string label, ui& count) const {//--------------------------------
      //  ui offset = id * labels_count_ + label;
      //  count = labels_offsets_[offset + 1] - labels_offsets_[offset];
      //  return neighbors_ + labels_offsets_[offset];
   // }

    const sparse_hash_map<std::string, uint32_t>* getVertexNLF(const VertexID id) const {
        return nlf_ + id;
    }

    //bool checkEdgeExistence(const VertexID u, const VertexID v, const std::string u_label) const {
      //  ui count = 0;
       // const VertexID* neighbors = getNeighborsByLabel(v, u_label, count);
       // int begin = 0;
       // int end = count - 1;
       // while (begin <= end) {
         //   int mid = begin + ((end - begin) >> 1);
           // if (neighbors[mid] == u) {
             //   return true;
            //}
            //else if (neighbors[mid] > u)
              //  end = mid - 1;
            //else
              //  begin = mid + 1;
        //}

        //return false;
    //}
#endif

    bool checkEdgeExistence(VertexID u, VertexID v) const {
        if (getVertexDegree(u) < getVertexDegree(v)) {
            std::swap(u, v);
        }
        ui count = 0;
        const VertexID* neighbors =  getVertexNeighbors(v, count);

        int begin = 0;
        int end = count - 1;
        while (begin <= end) {
            int mid = begin + ((end - begin) >> 1);
            if (neighbors[mid] == u) {
                return true;
            }
            else if (neighbors[mid] > u)
                end = mid - 1;
            else
                begin = mid + 1;
        }

        return false;
    }

    void buildCoreTable();

    void buildEdgeIndex();
};


#endif //SUBGRAPHMATCHING_GRAPH_H
