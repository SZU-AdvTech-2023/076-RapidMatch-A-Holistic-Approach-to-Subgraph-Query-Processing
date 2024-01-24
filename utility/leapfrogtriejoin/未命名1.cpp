 //---------------------------------------------
        priority_queue< pair<uint32_t,uint32_t> > Pu;
		uint32_t* data=local_candidates_[cur_depth];
	    uint32_t score;
		std::string s_label,node_label;
		s_label=catalog->query_graph_->getVertexLabel(vertex_ordering_[cur_depth]);
		for(int i=0;i<num_local_candidates_[cur_depth];i++){
			uint32_t node=data[i];
			node_label=catalog->query_graph_->getVertexLabel(node);
			score= distance(s_label,node_label);
			Pu.push(pair<uint32_t,uint32_t>(score,node));
		}
		uint32_t cnt=0;
		while(!Pu.empty()){
			pair<uint32_t,uint32_t> p=Pu.top();
			local_candidates_[cur_depth][cnt]=p.second;
			local_candidates_score_[cur_depth][cnt++]=p.first;
			Pu.pop();
		}
		//-----------------------------------------------------
		
		nowbound+=local_candidates_score_[cur_depth+1][j];
		//---------------------add or not----------
								
		if(nowbound>Pm.top().score){
			Pm.pop();
			Pm.push(M);
		}
		lowerbound=Pm.top().score;
		//-----------------add or not--------------
		
		nowbound-=local_candidates_score_[cur_depth+1][j];
		
		
		nowbound+=local_candidates_score[start_depth][idx_[start_depth]];
        upperbound=nowbound;
        for(int i=start_depth+1;i<max_depth;i++){
        	upperbound+=max_score[i];
		}
		if(upperbound<=lowerbound){
			idx_[start_depth]=0;
			break;
			//£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿ 
		}
		
		
		
		
		nowbound+=local_candidates_score[cur_depth][idx_[cur_depth]];
        upperbound=nowbound;
        for(int i=cur_depth+1;i<max_depth;i++){
        	upperbound+=max_score[i];
		}
		if(upperbound<=lowerbound){
			nowbound-=local_candidates_score[cur_depth][idx_[cur_depth]];
			idx_[cur_depth]=0;
			cur_depth--;
			continue;
		}
