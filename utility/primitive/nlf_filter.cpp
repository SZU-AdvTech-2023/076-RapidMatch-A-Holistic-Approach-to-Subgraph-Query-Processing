#include "nlf_filter.h"

void nlf_filter::execute(std::vector<std::vector<uint32_t>> &candidate_sets) {
    uint32_t n = query_graph_->getVerticesCount();
    candidate_sets.resize(n);

    for (uint32_t u = 0; u < n; ++u) {
        string label = query_graph_->getVertexLabel(u);
        uint32_t degree = query_graph_->getVertexDegree(u);
#if OPTIMIZED_LABELED_GRAPH == 1
        auto u_nlf = query_graph_->getVertexNLF(u);
#endif

        uint32_t data_vertex_num;
        //-----------------------------------------------------
        const uint32_t* data_vertices = data_graph_->getVerticesByLabel(label, data_vertex_num);
        auto& candidate_set = candidate_sets[u];

        for (uint32_t j = 0; j < data_vertex_num; ++j) {
            uint32_t v = data_vertices[j];
            if (data_graph_->getVertexDegree(v) >= degree) {

                // NLF check
#if OPTIMIZED_LABELED_GRAPH == 1
                auto v_nlf = data_graph_->getVertexNLF(v);

                if (v_nlf->size() >= u_nlf->size()) {
                    bool is_valid = true;

                    for (auto element : *u_nlf) {
                        auto iter = v_nlf->find(element.first);
                        if (iter == v_nlf->end() || iter->second < element.second) {
                            is_valid = false;
                            break;
                        }
                    }

                    if (is_valid) {
                        candidate_set.push_back(v);
                    }
                }
#endif
            }
        }
    }
}

void nlf_filter::execute(catalog *storage) {
	std::cout<<"nlf_filter\n";
    for (uint32_t u = 0; u < query_graph_->getVerticesCount(); ++u) {
        uint32_t u_nbrs_cnt;
        const uint32_t* u_nbrs = query_graph_->getVertexNeighbors(u, u_nbrs_cnt);
        uint32_t uu = u_nbrs[u_nbrs_cnt - 1];

        if (u < uu) {
        	std::cout<<"enter filter_order_relation==="<<u<<" "<<uu<<" "<<storage->edge_relations_[u][uu].size_<<std::endl;
            filter_ordered_relation(u, &storage->edge_relations_[u][uu]);
            std::cout<<"finally, filter_order_relation==="<<u<<" "<<uu<<" "<<storage->edge_relations_[u][uu].size_<<std::endl;
        }
        else {
            filter_unordered_relation(u, &storage->edge_relations_[uu][u]);
        }
        //------------------------------------------unorder->order
    }
    std::cout<<"nlf_filter over\n";
}


void nlf_filter::filter_ordered_relation(uint32_t u, edge_relation *relation) {
	std::cout<<"filter_ordered_relation\n";
    uint32_t u_deg = query_graph_->getVertexDegree(u);

#if OPTIMIZED_LABELED_GRAPH == 1
    auto u_nlf = query_graph_->getVertexNLF(u);
    std::vector<std::pair<std::string, uint32_t>> nlf_array;
    for (auto element : *u_nlf) {
        nlf_array.emplace_back(element);
    }
#endif

    uint32_t valid_edge_count = 0;
    uint32_t prev_v = std::numeric_limits<uint32_t>::max();
    bool add = false;
    for (uint32_t i = 0; i < relation->size_; ++i) {
        uint32_t v = relation->edges_[i].vertices_[0];
        // std::cout<<"i="<<i<<" u="<<u<<" v="<<v<<std::endl;
		//vÊÇu-uuÆ¥Åä±ßµÄu'µã 
        if (v != prev_v) {
            prev_v = v;
            add = false;
            uint32_t v_deg = data_graph_->getVertexDegree(v);
			
			//³õ²œŒì²éuÓëvÊÇ·ñÆ¥Åä£ºvµÄ¶ÈÒªŽóÓÚµÈÓÚuµÄ¶È 
            if (v_deg >= u_deg) {
                add = true;
#if OPTIMIZED_LABELED_GRAPH == 1
                auto v_nlf = data_graph_->getVertexNLF(v);
					
                if (v_nlf->size() >= nlf_array.size()) {
                //uµÄÁÚœÓµã£¬vÒ²ÒªÓÐ£¬²¢ÇÒÊýÁ¿²»ÄÜÐ¡ÓÚu 
                // std::cout<<"enter-for,add="<<add<<std::endl;
                    for (auto element : nlf_array) {
                    	bool flag=true;
                    	ui count=std::numeric_limits<uint32_t>::max();
                    	for(auto it : *v_nlf){
                    	// std::cout<<"element----it"<<std::endl;
                    	// std::cout<<element.first<<" "<<it.first<<std::endl;
                    		if(nlf_filter::distance(element.first,it.first)<=1){
                    		flag=false;
                    		if(count!=std::numeric_limits<uint32_t>::max())count+=it.second;
                    		else count=it.second;
             
                    		//break;
                    		}
                    	}
                    	// std::cout<<"element.second="<<element.second<<" count="<<count<<std::endl;
                    	if(flag==true || count<element.second){
                    		add=false;
                    		break;
                    	}
                     }
                }
            }
#endif
       }
        
		//³õ²œ·ûºÏ 
	// std::cout<<"i="<<i<<" u="<<u<<" v="<<v<<" add="<<add<<std::endl;
        if (add) {
        	//Ö®Ç°ŽæÔÚ¹ýinvalid 
            if(valid_edge_count != i)
                relation->edges_[valid_edge_count] = relation->edges_[i];
            valid_edge_count += 1;
        }
    }

    relation->size_ = valid_edge_count;
    std::cout<<"valid_edge_count"<<valid_edge_count<<std::endl;
    std::cout<<"filter_ordered_relation over\n";
}

void nlf_filter::filter_unordered_relation(uint32_t u, edge_relation *relation) {
    uint32_t u_deg = query_graph_->getVertexDegree(u);

#if OPTIMIZED_LABELED_GRAPH == 1
    auto u_nlf = query_graph_->getVertexNLF(u);
    std::vector<std::pair<std::string, uint32_t>> nlf_array;
    for (auto element : *u_nlf) {
        nlf_array.emplace_back(element);
    }
#endif

    uint32_t valid_edge_count = 0;
    for (uint32_t i = 0; i < relation->size_; ++i) {
        uint32_t v = relation->edges_[i].vertices_[1];

        // v is not checked.
        if (status_[v] == 'u') {
            status_[v] = 'r';
            updated_.push_back(v);
            uint32_t v_deg = data_graph_->getVertexDegree(v);
            if (v_deg >= u_deg) {
                status_[v] = 'a';
#if OPTIMIZED_LABELED_GRAPH == 1
                auto v_nlf = data_graph_->getVertexNLF(v);

                if (v_nlf->size() >= nlf_array.size()) {
                    for (auto element : nlf_array) {
                    	bool flag=true;
                    	ui count=std::numeric_limits<uint32_t>::max();
                        //auto iter = v_nlf->find(element.first);
                        for(auto it : *v_nlf){
                    	//std::cout<<"element----it"<<std::endl;
                    	//std::cout<<element.first<<" "<<it.first<<std::endl;
                    		if(nlf_filter::distance(element.first,it.first)<=1){
                    		flag=false;
                    		if(count!=std::numeric_limits<uint32_t>::max())count+=it.second;
                    		else count=it.second;
             
                    		//break;
                    		}
                    	}
                        if (flag==true || count<element.second) {
                            status_[v] = 'r';
                            break;
                        }
                    }
                }
#endif
            }
        }

        if (status_[v] == 'a') {
            if (valid_edge_count != i)
                relation->edges_[valid_edge_count] = relation->edges_[i];
            valid_edge_count += 1;
        }
    }

    for (auto v : updated_)
        status_[v] = 'u';
    updated_.clear();

    relation->size_ = valid_edge_count;
}
