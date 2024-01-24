#include "scan.h"
#include <cstring>
#include <cereal/types/vector.hpp>
#include <vector>

void scan::execute(std::string src_label, std::string dst_label, edge_relation *relation, bool indexed) {
	std::cout<<"scan_execute\n";
    if (indexed) {
        execute_with_index(src_label, dst_label, relation);
    }
    else {
        execute_without_index(src_label, dst_label, relation);
    }
}

void scan::execute(std::vector<uint32_t> &src_candidate_set, std::vector<uint32_t> &dst_candidate_set,
                   edge_relation *relation) {
    for (auto u : dst_candidate_set) {
        flag_[u] = true;
    }

    uint32_t edge_count = 0;

    for (auto u : src_candidate_set) {
        uint32_t dst_count;
        const uint32_t* dst_list = data_graph_->getVertexNeighbors(u, dst_count);

        for (uint32_t j = 0; j < dst_count; ++j) {
            uint32_t v = dst_list[j];

            if (flag_[v]) {
                buffer_[edge_count].vertices_[0] = u;
                buffer_[edge_count].vertices_[1] = v;
                edge_count += 1;
            }
        }
    }

    relation->size_ = edge_count;
    relation->edges_ = new edge[edge_count];
    memcpy(relation->edges_, buffer_, sizeof(edge) * edge_count);

    for (auto u : dst_candidate_set) {
        flag_[u] = false;
    }
}

void scan::execute_with_index(std::string src_label, std::string dst_label, edge_relation *relation) {
    std::cout<<"scan_execute_index\n";
    std::string key = src_label+dst_label;

    if (data_graph_->getEdgeIndex()->contains(key)) {
        auto edges = data_graph_->getEdgeIndex()->at(key);
        relation->size_ = edges->size();
        relation->edges_ = new edge[relation->size_];
        memcpy(relation->edges_, edges->data(), sizeof(edge) * relation->size_);
    }
    else {
        relation->size_ = 0;
    }
	std::cout<<"scan_execute_index\n";
// 	ui u_match_cnt,v_match_cnt;
// 	const uint32_t* u_mc = data_graph_-> getVerticesByLabel(src_label, u_match_cnt);
// 	const uint32_t* v_mc = data_graph_-> getVerticesByLabel(dst_label, v_match_cnt);
// 	std::cout<<" 1111111111111111111\n";
// 	edge * edges=new edge[data_graph_->getEdgesCount()];
//     printf("edgecount==%d\n",data_graph_->getEdgesCount());
// 	ui cnt=0;
// 	//std::vector<edge> edges;
// 	for(int i=0;i<u_match_cnt;i++){
// 		ui u=u_mc[i];
// 		for(int j=0;j<v_match_cnt;j++){
// 			ui v=v_mc[j];
// 			//std::cout<<"  "<<u<<" "<<v<<std::endl;
// 			if(data_graph_->checkEdgeExistence(u,v)){
// 				edge t_edge;
// 				t_edge.vertices_[0]=u;
// 				t_edge.vertices_[1]=v;
// 				edges[cnt++]=t_edge;
// 			}
// 		}
// 	}

//     if (cnt!=0) {
//     	std::cout<<"cnt:"<<cnt<<std::endl;
// //        auto edges = data_graph_->getEdgeIndex()->at(key);
//         relation->size_ = cnt;
//         //edges.size();
//         relation->edges_ = new edge[relation->size_];
//         memcpy(relation->edges_, edges, sizeof(edge) * relation->size_);
//     }
//     else {
//         relation->size_ = 0;
//     }
//    // printf("over scan dex\n");
//     delete []edges;
//    // printf("over scan dex\n");
//     delete []u_mc;
//     delete []v_mc;
    //---------------------------------------------------------------------------------------
    std::cout<<"scan_execute_index over!\n";
}

void scan::execute_without_index(std::string src_label, std::string dst_label, edge_relation *relation) {
    uint32_t edge_count = 0;

    uint32_t src_count;
    const uint32_t* src_list = data_graph_->getVerticesByLabel(src_label, src_count);

    for (uint32_t i = 0; i < src_count; ++i) {
        uint32_t src = src_list[i];

        uint32_t dst_count;
        const uint32_t* dst_list = data_graph_->getVertexNeighbors(src, dst_count);

        for (uint32_t j = 0; j < dst_count; ++j) {
            uint32_t dst = dst_list[j];

            if (data_graph_->getVertexLabel(dst) != dst_label)
                continue;

            buffer_[edge_count].vertices_[0] = src;
            buffer_[edge_count].vertices_[1] = dst;
            edge_count += 1;
        }
    }

    relation->size_ = edge_count;
    relation->edges_ = new edge[edge_count];
    memcpy(relation->edges_, buffer_, sizeof(edge) * edge_count);
}
