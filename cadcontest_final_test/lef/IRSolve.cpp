#include "IRSolver.h"
double dlt;
static double getDoubleTime() {
	struct timeval tm_tv;
	gettimeofday(&tm_tv, 0);
	return (double)tm_tv.tv_sec + (1e-6)*tm_tv.tv_usec;
}
void IRSOLVER::CheckIR(NetList *nt_info, IRSOLVER *irsolver)
{
	cout << "[Info] Check IR constraint ..." << endl;
	IRSOLVER::ReConstructSegmentFlow(nt_info, irsolver);
	IRSOLVER::ConstructPowerCrossNode(nt_info, irsolver);
	IRSOLVER::LinkRelationship(nt_info, irsolver);
	IRSOLVER::SetPadNode(nt_info, irsolver);
	IRSOLVER::LoadCurrent(nt_info, irsolver);
	IRSOLVER::NodeBaseSolver(nt_info, irsolver);
	//IRSOLVER::LasPack(nt_info, irsolver);
	IRSOLVER::DisplayIR(nt_info, irsolver);
	IRSOLVER::ReleaseMem(irsolver);
}
bool IRSOLVER::CmpByY(SEGMENT *a, SEGMENT *b)
{
	return a->ori_y <  b->ori_y;
}
bool IRSOLVER::CmpByX(SEGMENT *a, SEGMENT *b)
{
	return a->ori_x <  b->ori_x;
}
void IRSOLVER::ReConstructSegmentFlow(NetList *nt_info, IRSOLVER *irsolver)
{
	cout << "[Info] ReConstruct Segment Flow  ..." << endl;
	IRSOLVER::SortSegment(nt_info, irsolver, nt_info->RingStrapH_VDD);
	IRSOLVER::SortSegment(nt_info, irsolver, nt_info->RingStrapV_VDD);
	fstream fout("PowerLine.m", fstream::out);
	map<double, vector<SEGMENT* > >::iterator iter;
	fout << "figure;\naxis equal;\nhold on;grid on;\n";
	for (iter = irsolver->Vsnd_power.begin(); iter != irsolver->Vsnd_power.end(); iter++)
	{
		for (unsigned int p = 0; p < iter->second.size(); p++)
		{
			fout << "rectangle('Position',[" << iter->second.at(p)->ori_x - iter->second.at(p)->width / 2 << " " <<
				iter->second.at(p)->ori_y << " " << iter->second.at(p)->width << " " << iter->second.at(p)->length << "],'EdgeColor','b')" << endl;
		}
	}
	for (iter = irsolver->Htop_power.begin(); iter != irsolver->Htop_power.end(); iter++)
	{
		for (unsigned int p = 0; p < iter->second.size(); p++)
		{
			fout << "rectangle('Position',[" << iter->second.at(p)->ori_x << " " << iter->second.at(p)->ori_y - iter->second.at(p)->width / 2
				<< " " << iter->second.at(p)->length << " " << iter->second.at(p)->width << "],'EdgeColor','r')" << endl;
		}
	}
}

void IRSOLVER::SortSegment(NetList *nt_info, IRSOLVER *irsolver, map<double, vector<SEGMENT* > > &input)
{
	double center_x, center_nx, center_y, center_ny;
	vector<double> map_key;
	map<double, vector<SEGMENT* > >::iterator iter;
	map<double, vector<SEGMENT* > >::iterator n_iter;
	iter = input.begin();
	if (iter->second.at(0)->vertical == 1)
	{
		for (iter = input.begin(); iter != input.end(); iter++)
		{
			if (iter->second.at(0)->powerline_tag == true)
				continue;
			else
			{
				map_key.push_back(iter->first);
				iter->second.at(0)->powerline_tag = true;
				center_x = iter->second.at(0)->ori_x;
				for (n_iter = input.begin(); n_iter != input.end(); n_iter++)
				{
					if (n_iter->second.at(0)->powerline_tag == true || iter->second.at(0)->layer != n_iter->second.at(0)->layer)
						continue;
					else
					{
						center_nx = n_iter->second.at(0)->ori_x;
						if (center_x + iter->second.at(0)->width / 2 < center_nx - n_iter->second.at(0)->width / 2 && center_x < center_nx)
							break;
						if (fabs(center_x - center_nx) == fabs(iter->second.at(0)->width / 2 - n_iter->second.at(0)->width / 2))
						{
							map_key.push_back(n_iter->first);
							n_iter->second.at(0)->powerline_tag = true;
						}
					}
				}
				IRSOLVER::ConstructPowerLine(nt_info, irsolver, map_key, input);
				map_key.clear();
			}
		}
	}
	else if (iter->second.at(0)->vertical == 0)
	{
		for (iter = input.begin(); iter != input.end(); iter++)
		{
			if (iter->second.at(0)->powerline_tag == true)
				continue;
			else
			{
				map_key.push_back(iter->first);
				iter->second.at(0)->powerline_tag = true;
				center_y = iter->second.at(0)->ori_y;
				for (n_iter = input.begin(); n_iter != input.end(); n_iter++)
				{
					if (n_iter->second.at(0)->powerline_tag == true || iter->second.at(0)->layer != n_iter->second.at(0)->layer)
						continue;
					else
					{
						center_ny = n_iter->second.at(0)->ori_y;
						if (fabs(center_y - center_ny) == fabs(iter->second.at(0)->width / 2 - n_iter->second.at(0)->width / 2))
						{
							map_key.push_back(n_iter->first);
							n_iter->second.at(0)->powerline_tag = true;
						}
						if (center_y + iter->second.at(0)->width / 2 < center_ny - n_iter->second.at(0)->width / 2 && center_y < center_ny)
							break;
					}
				}
				IRSOLVER::ConstructPowerLine(nt_info, irsolver, map_key, input);
				map_key.clear();
			}
		}
	}
}
void IRSOLVER::PowerLine(NetList *nt_info, IRSOLVER *irsolver, double x1, double y1, double x2, double y2, LAYER* curr_layer, bool vertical)
{
	SEGMENT *power_line;
	power_line = new SEGMENT();
	map<double, vector<SEGMENT* > >::iterator iter;
	if (vertical == 1)
	{
		power_line->ori_x = (x1 + x2) / 2;
		power_line->ori_y = y1;
		power_line->end_x = power_line->ori_x;
		power_line->end_y = y2;
		power_line->layer = curr_layer->layer_name;
		power_line->layer_ptr = curr_layer;
		power_line->vertical = true;
		power_line->length = y2 - y1;
		power_line->width = x2 - x1;
		if (curr_layer == nt_info->top_layer1)
		{
			iter = irsolver->Vtop_power.find(power_line->ori_x);
			if (iter != irsolver->Vtop_power.end())
				iter->second.push_back(power_line);
			else
			{
				vector<SEGMENT* > segments;
				segments.push_back(power_line);
				irsolver->Vtop_power.insert(pair<double, vector<SEGMENT* > >(power_line->ori_x, segments));
			}
		}
		else if (curr_layer == nt_info->top_layer2)
		{
			iter = irsolver->Vsnd_power.find(power_line->ori_x);
			if (iter != irsolver->Vsnd_power.end())
				iter->second.push_back(power_line);
			else
			{
				vector<SEGMENT* > segments;
				segments.push_back(power_line);
				irsolver->Vsnd_power.insert(pair<double, vector<SEGMENT* > >(power_line->ori_x, segments));
			}
		}
	}
	else
	{
		power_line->ori_x = x1;
		power_line->ori_y = (y1 + y2) / 2;
		power_line->end_x = x2;
		power_line->end_y = power_line->ori_y;
		power_line->layer = curr_layer->layer_name;
		power_line->layer_ptr = curr_layer;
		power_line->vertical = false;
		power_line->length = x2 - x1;
		power_line->width = y2 - y1;
		if (curr_layer == nt_info->top_layer1)
		{
			iter = irsolver->Htop_power.find(power_line->ori_y);
			if (iter != irsolver->Htop_power.end())
				iter->second.push_back(power_line);
			else
			{
				vector<SEGMENT* > segments;
				segments.push_back(power_line);
				irsolver->Htop_power.insert(pair<double, vector<SEGMENT* > >(power_line->ori_y, segments));
			}
		}
		else if (curr_layer == nt_info->top_layer2)
		{
			iter = irsolver->Hsnd_power.find(power_line->ori_y);
			if (iter != irsolver->Hsnd_power.end())
				iter->second.push_back(power_line);
			else
			{
				vector<SEGMENT* > segments;
				segments.push_back(power_line);
				irsolver->Hsnd_power.insert(pair<double, vector<SEGMENT* > >(power_line->ori_y, segments));
			}
		}
	}

}
void IRSOLVER::ConstructPowerLine(NetList *nt_info, IRSOLVER *irsolver, vector<double> &input_key, map<double, vector<SEGMENT* > > &input)
{
	string layer;
	LAYER *curr_layer = NULL;
	map<double, vector<SEGMENT* > >::iterator iter;
	bool vertical = false;
	double map_key;
	double max_x = -1, max_y = -1, min_x = -1, min_y = -1;
	vector<SEGMENT * >tmp_segments;
	for (unsigned int i = 0; i < input_key.size(); i++)
	{
		map_key = input_key.at(i);
		iter = input.find(map_key);
		if (iter->second.at(0)->vertical == 1)
		{
			//sort(iter->second.begin(), iter->second.end(), CmpByY);
			if (i == 0)
			{
				tmp_segments.assign(iter->second.begin(), iter->second.end());
				vertical = true;
				layer = iter->second.at(0)->layer;
				curr_layer = (layer == nt_info->top_layer1->layer_name) ? nt_info->top_layer1 : nt_info->top_layer2;
				min_x = iter->second.at(0)->ori_x - iter->second.at(0)->width / 2;
				max_x = iter->second.at(0)->end_x + iter->second.at(0)->width / 2;
			}
			else
			{
				tmp_segments.insert(tmp_segments.end(), iter->second.begin(), iter->second.end());
				if (iter->second.at(0)->ori_x - iter->second.at(0)->width / 2 < min_x)
					min_x = iter->second.at(0)->ori_x - iter->second.at(0)->width / 2;
				if (iter->second.at(0)->end_x + iter->second.at(0)->width / 2 > max_x)
					max_x = iter->second.at(0)->end_x + iter->second.at(0)->width / 2;
			}
		}
		else
		{
			//sort(iter->second.begin(), iter->second.end(), CmpByX);
			if (i == 0)
			{
				tmp_segments.assign(iter->second.begin(), iter->second.end());
				vertical = false;
				layer = iter->second.at(0)->layer;
				curr_layer = (layer == nt_info->top_layer1->layer_name) ? nt_info->top_layer1 : nt_info->top_layer2;
				min_y = iter->second.at(0)->ori_y - iter->second.at(0)->width / 2;
				max_y = iter->second.at(0)->end_y + iter->second.at(0)->width / 2;
			}
			else
			{
				tmp_segments.insert(tmp_segments.end(), iter->second.begin(), iter->second.end());
				if (iter->second.at(0)->ori_y - iter->second.at(0)->width / 2 < min_y)
					min_y = iter->second.at(0)->ori_y - iter->second.at(0)->width / 2;
				if (iter->second.at(0)->end_y + iter->second.at(0)->width / 2 > max_y)
					max_y = iter->second.at(0)->end_y + iter->second.at(0)->width / 2;
			}
		}
	}

	if (vertical == 1)
	{
		sort(tmp_segments.begin(), tmp_segments.end(), CmpByY);
		if (tmp_segments.size() == 1)
		{
			IRSOLVER::PowerLine(nt_info, irsolver, min_x, tmp_segments.at(0)->ori_y, max_x, tmp_segments.at(0)->end_y, curr_layer, vertical);
		}
		else
		{
			min_y = tmp_segments.at(0)->ori_y;
			max_y = tmp_segments.at(0)->end_y;
			for (unsigned int v = 1; v < tmp_segments.size(); v++)
			{
				if (v == tmp_segments.size() - 1)
				{
					if (tmp_segments.at(v)->ori_y <= max_y && tmp_segments.at(v)->end_y > max_y)
					{
						max_y = tmp_segments.at(v)->end_y;
						IRSOLVER::PowerLine(nt_info, irsolver, min_x, min_y, max_x, max_y, curr_layer, vertical);
					}
					else if (tmp_segments.at(v)->ori_y > max_y)
					{
						IRSOLVER::PowerLine(nt_info, irsolver, min_x, min_y, max_x, max_y, curr_layer, vertical);
						IRSOLVER::PowerLine(nt_info, irsolver, min_x, tmp_segments.at(v)->ori_y, max_x, tmp_segments.at(v)->end_y, curr_layer, vertical);
					}
					else
						IRSOLVER::PowerLine(nt_info, irsolver, min_x, min_y, max_x, max_y, curr_layer, vertical);
				}
				else
				{
					if (tmp_segments.at(v)->ori_y <= max_y && tmp_segments.at(v)->end_y > max_y)
					{
						max_y = tmp_segments.at(v)->end_y;
					}
					else if (tmp_segments.at(v)->ori_y > max_y)
					{
						IRSOLVER::PowerLine(nt_info, irsolver, min_x, min_y, max_x, max_y, curr_layer, vertical);
						min_y = tmp_segments.at(v)->ori_y;
						max_y = tmp_segments.at(v)->end_y;
					}
				}
			}
		}
	}
	else
	{
		sort(tmp_segments.begin(), tmp_segments.end(), CmpByX);
		if (tmp_segments.size() == 1)
		{
			IRSOLVER::PowerLine(nt_info, irsolver, tmp_segments.at(0)->ori_x, min_y, tmp_segments.at(0)->end_x, max_y, curr_layer, vertical);
		}
		else
		{
			min_x = tmp_segments.at(0)->ori_x;
			max_x = tmp_segments.at(0)->end_x;
			for (unsigned int v = 1; v < tmp_segments.size(); v++)
			{
				if (v == tmp_segments.size() - 1)
				{
					if (tmp_segments.at(v)->ori_x <= max_x && tmp_segments.at(v)->end_x > max_x)
					{
						max_x = tmp_segments.at(v)->end_x;
						IRSOLVER::PowerLine(nt_info, irsolver, min_x, min_y, max_x, max_y, curr_layer, vertical);
					}
					else if (tmp_segments.at(v)->ori_x > max_x)
					{
						IRSOLVER::PowerLine(nt_info, irsolver, min_x, min_y, max_x, max_y, curr_layer, vertical);
						IRSOLVER::PowerLine(nt_info, irsolver, tmp_segments.at(v)->ori_x, min_y, tmp_segments.at(v)->end_x, max_y, curr_layer, vertical);
					}
					else
						IRSOLVER::PowerLine(nt_info, irsolver, min_x, min_y, max_x, max_y, curr_layer, vertical);
				}
				else
				{
					if (tmp_segments.at(v)->ori_x <= max_x && tmp_segments.at(v)->end_x > max_x)
					{
						max_x = tmp_segments.at(v)->end_x;
					}
					else if (tmp_segments.at(v)->ori_x > max_x)
					{
						IRSOLVER::PowerLine(nt_info, irsolver, min_x, min_y, max_x, max_y, curr_layer, vertical);
						min_x = tmp_segments.at(v)->ori_x;
						max_x = tmp_segments.at(v)->end_x;
					}
				}
			}
		}
	}

	tmp_segments.clear();
	vector<SEGMENT *>().swap(tmp_segments);
}

void IRSOLVER::ConstructPowerCrossNode(NetList *nt_info, IRSOLVER *irsolver)
{
	cout << "[Info] Construct Power CrossNode ..." << endl;
	SEGMENT *curr_h, *curr_v;
	CROSSNODE *tmpnode, *left_node, *right_node;
	vector<int>tmptopID, tmpsndID;
	int id = 0;
	bool first_node = false;
	double h_low_y, h_up_y, v_left_x, v_right_x, n_low_y, n_up_y;
	map<double, vector<SEGMENT* > >::iterator h_iter, v_iter, n_iter;
	fstream fout("CrossNode.m", fstream::out);
	/*************************Check Connect but Not Overlap*************************/
	for (h_iter = irsolver->Htop_power.begin(); h_iter != irsolver->Htop_power.end(); h_iter++)
	{
		h_low_y = h_iter->first - h_iter->second.at(0)->width / 2;
		h_up_y = h_iter->first + h_iter->second.at(0)->width / 2;
		v_left_x = h_iter->second.at(0)->ori_x;
		for (n_iter = irsolver->Htop_power.begin(); n_iter != irsolver->Htop_power.end(); n_iter++)
		{
			if (h_iter == n_iter)
				continue;
			if (h_iter != n_iter)
			{
				n_low_y = n_iter->first - n_iter->second.at(0)->width / 2;
				n_up_y = n_iter->first + n_iter->second.at(0)->width / 2;
				v_right_x = n_iter->second.back()->end_x;
				if (h_low_y > n_up_y || h_up_y < n_low_y || v_right_x!= v_left_x)
					continue;
				else if(v_right_x >= v_left_x)
				{
					n_iter->second.back()->right = h_iter->second.at(0);
					h_iter->second.at(0)->left = n_iter->second.back();
				}
			}
		}
	}
	/*************************Construct Node*************************/
	for (h_iter = irsolver->Htop_power.begin(); h_iter != irsolver->Htop_power.end(); h_iter++)
	{
		for (unsigned int i = 0; i < h_iter->second.size(); i++)
		{
			curr_h = h_iter->second.at(i);
			first_node = false;
			h_low_y = h_iter->first - h_iter->second.at(i)->width / 2;
			h_up_y = h_iter->first + h_iter->second.at(i)->width / 2;
			for (v_iter = irsolver->Vsnd_power.begin(); v_iter != irsolver->Vsnd_power.end(); v_iter++)
			{
				v_left_x = v_iter->first - v_iter->second.at(0)->width / 2;
				v_right_x = v_iter->first + v_iter->second.at(0)->width / 2;
				if (v_left_x > curr_h->end_x || v_right_x < curr_h->ori_x)
					continue;
				else
				{
					for (unsigned int v = 0; v < v_iter->second.size(); v++)
					{
						curr_v = v_iter->second.at(v);
						if (curr_v->end_y < h_low_y || curr_v->ori_y > h_up_y)
							continue;
						else
						{
							tmpnode = new CROSSNODE();
							tmpnode->id = id;
							tmpnode->center_x = curr_v->ori_x;
							tmpnode->center_y = curr_h->ori_y;
							tmpnode->h_strap = curr_h;
							tmpnode->v_strap = curr_v;
							tmpnode->top_current = 0.000001;
							tmpnode->top_voltage = nt_info->GLOBAL_VOLTAGE;
							//tmpnode->top_voltage = nt_info->GLOBAL_VOLTAGE*0.997;
							tmpnode->snd_voltage = nt_info->GLOBAL_VOLTAGE*0.97;
							irsolver->CrossNodeList.push_back(tmpnode);
							tmptopID.push_back(id);
							tmpsndID.push_back(id);
							id++;
							if (curr_h->left != NULL&& first_node==false)
							{
								first_node = true;
								right_node = tmpnode;
							}
							fout << "plot(" << tmpnode->center_x << ", " << tmpnode->center_y << ", 'g*');\nhold on" << endl;
							fout << "text(" << tmpnode->center_x << ", " << tmpnode->center_y << ", '" << tmpnode->id << "');\n" << endl;
							break;
						}
					}
				}
			}
			if (curr_h->right != NULL)
			{
				left_node = tmpnode;
			}
			if (tmptopID.size() != 0)
			{
				irsolver->topNodeID.push_back(tmptopID);
				tmptopID.clear();
			}
		}
		if (tmpsndID.size() != 0)
		{
			irsolver->sndNodeID.push_back(tmpsndID);
			tmpsndID.clear();
		}
	}
	if (left_node != NULL&&right_node != NULL)
	{
		cout << "right " << right_node->center_x << "  " << right_node->center_y << endl;
		cout << "left " << left_node->center_x << "  " << left_node->center_y << endl;
		left_node->right_node = right_node;
		right_node->left_node = left_node;
	}

}
void IRSOLVER::LinkRelationship(NetList *nt_info, IRSOLVER *irsolver)
{
	cout << "[Info] Link Top Layer CrossNode Relationship ..." << endl;
	CROSSNODE *curr_node, *neighbor_node;
	int vindex, index, n_index;
	bool record = false;
	///***************************For top layer nodes->Link left and right relationship***************************///
	for (unsigned int t = 0; t < irsolver->topNodeID.size(); t++)
	{
		for (unsigned int h = 0; h < irsolver->topNodeID.at(t).size() - 1; h++)
		{
			index = irsolver->topNodeID[t][h];
			n_index = irsolver->topNodeID[t][h + 1];
			curr_node = irsolver->CrossNodeList[index];
			neighbor_node= irsolver->CrossNodeList[n_index];
			curr_node->right_node = neighbor_node;
			neighbor_node->left_node = curr_node;
		}
	}
	///***************************For snd layer nodes->Link up and low relationship***************************///
	cout << "[Info] Link Snd Layer CrossNode Relationship ..." << endl;
	for (unsigned int s = 0; s < irsolver->sndNodeID.size() - 1; s++)
	{
		for (unsigned int v = 0; v < irsolver->sndNodeID.at(s).size(); v++)
		{
			vindex = s + 1;
			record = false;
			index = irsolver->sndNodeID[s][v];
			curr_node = irsolver->CrossNodeList[index];
			while (record != true && vindex < (int)irsolver->sndNodeID.size())
			{
				for (unsigned int n = 0; n < irsolver->sndNodeID.at(vindex).size(); n++)
				{
					n_index = irsolver->sndNodeID[vindex][n];
					neighbor_node = irsolver->CrossNodeList[n_index];
					if (curr_node->v_strap != neighbor_node->v_strap)
						continue;
					else if (curr_node->v_strap == neighbor_node->v_strap)
					{
						if (curr_node->center_x == neighbor_node->center_x)
						{
							curr_node->up_node = neighbor_node;
							neighbor_node->low_node = curr_node;
							record = true;
							break;
						}
						else if (curr_node->center_x < neighbor_node->center_x)
							break;
					}
				}
				vindex++;
			}
		}
	}
	/*fstream fout("Relation.txt", fstream::out);
	for (unsigned int i = 0; i < irsolver->CrossNodeList.size()/2; i++)
	{
		fout << irsolver->CrossNodeList[i]->id << " : ";
		if (irsolver->CrossNodeList[i]->up_node != NULL)
			fout << "up : " << irsolver->CrossNodeList[i]->up_node->id;
		if (irsolver->CrossNodeList[i]->low_node != NULL)
			fout << "low : " << irsolver->CrossNodeList[i]->low_node->id;
		if (irsolver->CrossNodeList[i]->left_node != NULL)
			fout << "left : " << irsolver->CrossNodeList[i]->left_node->id;
		if (irsolver->CrossNodeList[i]->right_node != NULL)
			fout << "right : " << irsolver->CrossNodeList[i]->right_node->id;
		fout << endl;
	}*/
}
void IRSOLVER::SetPadNode(NetList *nt_info, IRSOLVER *irsolver)
{
	cout << "[Info] Set the Node that is pointed to Power Pad ..." << endl;
	PAD *curr_pad;
	CROSSNODE *curr_node, *pad_node = NULL;
	double pad_cenX, pad_cenY, dist;
	for (unsigned int p = 0; p < nt_info->vdd_power_pad.size(); p++)
	{
		curr_pad = nt_info->vdd_power_pad.at(p);
		pad_cenX = (curr_pad->llx + curr_pad->urx)*0.5;
		pad_cenY = (curr_pad->lly + curr_pad->ury)*0.5;
		int mindist = INT_MAX;
		for (unsigned int i = 0; i < irsolver->CrossNodeList.size(); i++)
		{
			curr_node = irsolver->CrossNodeList[i];
			dist = fabs(pad_cenX - curr_node->center_x) + fabs(pad_cenY - curr_node->center_y);
			if ((int)dist < mindist && curr_node->power_pad == false)
			{
				mindist = (int)dist;
				pad_node = curr_node;
			}
		}
		pad_node->power_pad = true;
		cout << pad_node->id << " " << pad_node->center_x << ", " << pad_node->center_y << endl;
		pad_node->top_voltage = nt_info->GLOBAL_VOLTAGE;
	}
}
void IRSOLVER::LoadCurrent(NetList *nt_info, IRSOLVER *irsolver)
{
	cout << "[Info] Load STD Cell Current ..." << endl;
	CROSSNODE *curr_node, *tmpnode = NULL;
	STDCELL *curr_cell;
	int mindist = INT_MAX;
	double dist;
	double center_x, center_y;
	for (unsigned int s = 0; s < nt_info->std_cell.size(); s++)
	{
		curr_cell = nt_info->std_cell.at(s);
		center_x = (curr_cell->llx + curr_cell->urx)*0.5;
		center_y = (curr_cell->lly + curr_cell->ury)*0.5;
		mindist = INT_MAX;
		for (unsigned int i = 0; i < irsolver->CrossNodeList.size(); i++)
		{
				curr_node = irsolver->CrossNodeList[i];
				if (curr_node->power_pad == true)
					continue;
				dist = fabs(center_x - curr_node->center_x) + fabs(center_y - curr_node->center_y);
				if ((int)dist < mindist)
				{
					mindist = (int)dist;
					tmpnode = curr_node;
				}
		}
		tmpnode->snd_current += max(curr_cell->static_power, 0.0) / nt_info->GLOBAL_VOLTAGE;
	}
	cout << "[Info] Load Macro Current ..." << endl;
	MODULE *curr_module;
	double width = 10 * 1000;
	double height = 10 * 1000;
	double llx, lly, urx, ury, power;
	for (unsigned int m = 0; m < nt_info->memory.size(); m++)
	{
		curr_module = nt_info->module.at(m);
		int x_col_num = (int)(1 + (curr_module->urx - curr_module->llx) / width);
		int y_row_num = (int)(1 + (curr_module->ury - curr_module->lly) / height);
		int macro_power_bin_num = x_col_num*y_row_num;
		for (int x = 0; x < x_col_num; x++)
		{
			for (int y = 0; y < y_row_num; y++)
			{
				llx = curr_module->llx + (x*width);
				lly = curr_module->lly + (y*width);
				urx = llx + width;
				ury = lly + height;
				center_x = (int)(0.5*(llx + urx));
				center_y = (int)(0.5*(lly + ury));
				power = curr_module->static_power / macro_power_bin_num;
				mindist = INT_MAX;
				for (unsigned int i = 0; i < irsolver->CrossNodeList.size(); i++)
				{
					curr_node = irsolver->CrossNodeList[i];
					if (curr_node->power_pad == true)
						continue;
					dist = fabs(center_x - curr_node->center_x) + fabs(center_y - curr_node->center_y);
					if (dist < mindist)
					{
						tmpnode = curr_node;
						mindist = (int)dist;
					}
				}
				tmpnode->snd_current += max(power, 0.0) / nt_info->GLOBAL_VOLTAGE;
			}
		}
	}
}

void IRSOLVER::NodeBaseSolver(NetList *nt_info, IRSOLVER *irsolver)
{
	cout << "[Info] NodeBaseSolver ..." << endl;
	fstream fout("IR_drop.txt", fstream::out);
	CROSSNODE *curr_node;
	int iteration = 1;
	bool convergence = false;
	double length, width;
	double error = 0.0000001, prev_V, tmp_V;
	/******************************First Step: Compute conductor for each node******************************/
	for (unsigned int i = 0; i < irsolver->CrossNodeList.size(); i++)
	{
		curr_node=irsolver->CrossNodeList[i];
		if (curr_node->snd_current == 0.0)
			curr_node->snd_current = 0.000001;
		if (curr_node->left_node != NULL)
		{
			length = fabs(curr_node->center_x - curr_node->left_node->center_x);
			width = curr_node->h_strap->width;
			curr_node->top_conductor += (1 / (nt_info->UnitNomResistance_PDN_layer_H*length / width));
			curr_node->left_conductor = (1 / (nt_info->UnitNomResistance_PDN_layer_H*length / width));
		}
		if (curr_node->right_node != NULL)
		{
			length = fabs(curr_node->center_x - curr_node->right_node->center_x);
			width = curr_node->h_strap->width;
			curr_node->top_conductor += (1 / (nt_info->UnitNomResistance_PDN_layer_H*length / width));
			curr_node->right_conductor = (1 / (nt_info->UnitNomResistance_PDN_layer_H*length / width));
		}
		if (curr_node->up_node != NULL)
		{
			length = fabs(curr_node->center_y - curr_node->up_node->center_y);
			width = curr_node->v_strap->width;
			curr_node->snd_conductor += (1 / (nt_info->UnitNomResistance_PDN_layer_V*length / width));
			curr_node->up_conductor = (1 / (nt_info->UnitNomResistance_PDN_layer_V*length / width));
		}
		if (curr_node->low_node != NULL)
		{
			length = fabs(curr_node->center_y - curr_node->low_node->center_y);
			width = curr_node->v_strap->width;
			curr_node->snd_conductor += (1 / (nt_info->UnitNomResistance_PDN_layer_V*length / width));
			curr_node->low_conductor = (1 / (nt_info->UnitNomResistance_PDN_layer_V*length / width));
		}
		length = nt_info->via_RDL->height;
		width = nt_info->via_RDL->width;
		curr_node->top_conductor += (1 / (nt_info->via_RDL->unitNomResistance*length / width));
		curr_node->snd_conductor += (1 / (nt_info->via_RDL->unitNomResistance*length / width));
		curr_node->via_conductor = (1 / (nt_info->via_RDL->unitNomResistance*length / width));
	}
	/******************************Second Step: Start IR Solver******************************/
	dlt = getDoubleTime();
	while (convergence != true && iteration < 1000000)
	{
		iteration++;
		int count = 0;
		convergence = true;
		for (unsigned int i = 0; i < irsolver->CrossNodeList.size(); i++)
		{
			curr_node = irsolver->CrossNodeList[i];
			if (curr_node->power_pad != true)
			{
				tmp_V = 0.0;
				prev_V = curr_node->top_voltage;
				if (curr_node->left_node != NULL)
				{
					tmp_V = tmp_V + (curr_node->left_conductor / curr_node->top_conductor)*curr_node->left_node->top_voltage;
				}
				if (curr_node->right_node != NULL)
				{
					tmp_V = tmp_V + (curr_node->right_conductor / curr_node->top_conductor)*curr_node->right_node->top_voltage;
				}
				tmp_V = tmp_V + (curr_node->via_conductor / curr_node->top_conductor)*curr_node->snd_voltage;	//for top-1 node
				tmp_V = tmp_V - (curr_node->top_current / curr_node->top_conductor);
				curr_node->top_voltage = tmp_V;
				if (fabs(curr_node->top_voltage - prev_V) < error)
				{
					count++;
				}
				else
					convergence = false;
			}
			tmp_V = 0.0;
			prev_V = curr_node->snd_voltage;
			if (curr_node->low_node != NULL)
			{
				tmp_V = tmp_V + (curr_node->low_conductor / curr_node->snd_conductor)*curr_node->low_node->snd_voltage;
			}
			if (curr_node->up_node != NULL)
			{
				tmp_V = tmp_V + (curr_node->up_conductor / curr_node->snd_conductor)*curr_node->up_node->snd_voltage;
			}
			tmp_V = tmp_V + (curr_node->via_conductor / curr_node->snd_conductor)*curr_node->top_voltage;
			tmp_V = tmp_V - (curr_node->snd_current / curr_node->snd_conductor);
			curr_node->snd_voltage = tmp_V;
			if (fabs(curr_node->snd_voltage - prev_V) < error)
			{
				count++;
			}
			else
				convergence = false;
		}
	}
	dlt = getDoubleTime() - dlt;
	/*cout << "Time: " << setw(5) << dlt << " s" << endl;
	cout << "iteration " << iteration << endl;*/
	/******************************Last Step: Find the Min/Max IR Drop******************************/
	irsolver->Max_V = 0.0;
	irsolver->Min_V = nt_info->GLOBAL_VOLTAGE;
	int maxid = -1;
	for (unsigned int i = 0; i < irsolver->CrossNodeList.size(); i++)
	{
		curr_node = irsolver->CrossNodeList[i];
		if (curr_node->power_pad == true)
			continue;
		if (curr_node->snd_voltage > irsolver->Max_V)
		{
			irsolver->Max_V = curr_node->snd_voltage;
			maxid = curr_node->id;
		}
		if (curr_node->snd_voltage < irsolver->Min_V)
		{
			irsolver->Min_V = curr_node->snd_voltage;
		}
	}
	cout << "maxid " << maxid << endl;
	cout << "Max node voltage = " << setprecision(7) << irsolver->Max_V << " , Min IR Drop Voltage= " << (nt_info->GLOBAL_VOLTAGE - irsolver->Max_V) << endl;
	cout << "Min node voltage = " << setprecision(7) << irsolver->Min_V << " , Max IR Drop Voltage = " << (nt_info->GLOBAL_VOLTAGE - irsolver->Min_V) << endl;
	fout << "Max node voltage = " << setprecision(7) << irsolver->Max_V << " , Min IR Drop Voltage = " << (nt_info->GLOBAL_VOLTAGE - irsolver->Max_V) << endl;
	fout << "Min node voltage = " << setprecision(7) << irsolver->Min_V << " , Max IR Drop Voltage = " << (nt_info->GLOBAL_VOLTAGE - irsolver->Min_V) << endl;
}

void IRSOLVER::ReleaseMem(IRSOLVER *irsolver)
{
	cout << "[Info] Release Memory ..." << endl;
	///***************************Release Memory***************************///
	for (unsigned int t = 0; t < irsolver->topNodeID.size(); t++)
	{
		irsolver->topNodeID[t].clear();
		vector<int>().swap(irsolver->topNodeID[t]);
	}
	for (unsigned int t = 0; t < irsolver->sndNodeID.size(); t++)
	{
		irsolver->sndNodeID[t].clear();
		vector<int>().swap(irsolver->sndNodeID[t]);
	}
	irsolver->topNodeID.clear();
	irsolver->sndNodeID.clear();
	vector<vector<int> >().swap(irsolver->topNodeID);
	vector<vector<int> >().swap(irsolver->sndNodeID);
	irsolver->CrossNodeList.clear();
	vector< CROSSNODE *>().swap(irsolver->CrossNodeList);

}
void IRSOLVER::DisplayIR(NetList *nt_info, IRSOLVER *irsolver)
{
	cout << "[Info] Draw IR.map ..." << endl;
	fstream fout("IR_drop.m", fstream::out);
	fout << "figure;\naxis equal;\nhold on;\nset(gca,'xtick',[])\nset(gca,'ytick',[])\n";
	fout << "rectangle('Position',[" << nt_info->die_llx << " " << nt_info->die_lly << " "
		<< nt_info->die_urx - nt_info->die_llx << " " << nt_info->die_ury - nt_info->die_lly << "])" << endl;
	/*********************Draw Node*********************/
	//irsolver->Min_V = nt_info->GLOBAL_VOLTAGE*0.9;
	double diff_v = irsolver->Max_V - irsolver->Min_V;
	IRSOLVER::CROSSNODE *curr_node;
	vector<double> left, right, low, up;
	fout << "colorbar('YTickLabel',{'" << irsolver->Min_V + diff_v << "', '" << irsolver->Min_V + (diff_v * 6) / 7 << "', '" <<
		irsolver->Min_V + (diff_v * 5) / 7 << "', '" << irsolver->Min_V + (diff_v * 4) / 7 << "', '" << irsolver->Min_V + (diff_v * 3) / 7 << "', '" <<
		irsolver->Min_V + (diff_v * 2) / 7 << "', '" << irsolver->Min_V + diff_v / 7 << "'})" << endl;
	for (unsigned int i = 0; i < irsolver->CrossNodeList.size(); i++)
	{
		curr_node = irsolver->CrossNodeList[i];
		left.clear(); right.clear(); low.clear(); up.clear();
		low.push_back(curr_node->center_y - curr_node->h_strap->width / 2);
		up.push_back(curr_node->center_y + curr_node->h_strap->width / 2);
		left.push_back(curr_node->center_x - curr_node->v_strap->width / 2);
		right.push_back(curr_node->center_x + curr_node->v_strap->width / 2);
		if (curr_node->low_node != NULL)
		{
			low.push_back((curr_node->center_y + curr_node->low_node->center_y) / 2);
			up.push_back(curr_node->center_y + curr_node->h_strap->width / 2);
			if (curr_node->left_node != NULL&&curr_node->low_node->left_node != NULL)
				left.push_back((curr_node->center_x + curr_node->left_node->center_x) / 2);
			else
				left.push_back(curr_node->center_x - curr_node->v_strap->width / 2);
			if (curr_node->right_node != NULL&&curr_node->low_node->right_node != NULL)
				right.push_back((curr_node->center_x + curr_node->right_node->center_x) / 2);
			else
				right.push_back(curr_node->center_x + curr_node->v_strap->width / 2);
		}
		if (curr_node->up_node != NULL)
		{
			low.push_back(curr_node->center_y - curr_node->h_strap->width / 2);
			up.push_back((curr_node->center_y + curr_node->up_node->center_y) / 2);
			if (curr_node->left_node != NULL&&curr_node->up_node->left_node != NULL)
				left.push_back((curr_node->center_x + curr_node->left_node->center_x) / 2);
			else
				left.push_back(curr_node->center_x - curr_node->v_strap->width / 2);
			if (curr_node->right_node != NULL&&curr_node->up_node->right_node != NULL)
				right.push_back((curr_node->center_x + curr_node->right_node->center_x) / 2);
			else
				right.push_back(curr_node->center_x + curr_node->v_strap->width / 2);
		}
		if (curr_node->left_node != NULL)
		{
			left.push_back((curr_node->center_x + curr_node->left_node->center_x) / 2);
			right.push_back(curr_node->center_x + curr_node->v_strap->width / 2);
			if (curr_node->up_node != NULL&&curr_node->left_node->up_node != NULL)
				up.push_back((curr_node->center_y + curr_node->up_node->center_y) / 2);
			else
				up.push_back(curr_node->center_y + curr_node->h_strap->width / 2);
			if (curr_node->low_node != NULL&&curr_node->left_node->low_node != NULL)
				low.push_back((curr_node->center_y + curr_node->low_node->center_y) / 2);
			else
				low.push_back(curr_node->center_y - curr_node->h_strap->width / 2);
		}
		if (curr_node->right_node != NULL)
		{
			right.push_back((curr_node->center_x + curr_node->right_node->center_x) / 2);
			left.push_back(curr_node->center_x - curr_node->v_strap->width / 2);
			if (curr_node->up_node != NULL&&curr_node->right_node->up_node != NULL)
				up.push_back((curr_node->center_y + curr_node->up_node->center_y) / 2);
			else
				up.push_back(curr_node->center_y + curr_node->h_strap->width / 2);
			if (curr_node->low_node != NULL&&curr_node->right_node->low_node != NULL)
				low.push_back((curr_node->center_y + curr_node->low_node->center_y) / 2);
			else
				low.push_back(curr_node->center_y - curr_node->h_strap->width / 2);
		}
		for (unsigned b = 0; b < left.size(); b++)
		{
			if (curr_node->snd_voltage < irsolver->Min_V + diff_v / 7)
			{
				fout << "rectangle('Position',[" << left[b] << " " << low[b] << " " << right[b] - left[b] << " " << up[b] - low[b] << "], 'EdgeColor','r', 'FaceColor','r')" << endl;
			}
			else if (curr_node->snd_voltage < irsolver->Min_V + (diff_v * 2) / 7)
			{
				fout << "rectangle('Position',[" << left[b] << " " << low[b] << " " << right[b] - left[b] << " " << up[b] - low[b] << "], 'EdgeColor',[1 0.5 0], 'FaceColor',[1 0.5 0])" << endl; //orange
			}
			else if (curr_node->snd_voltage < irsolver->Min_V + (diff_v * 3) / 7)
			{
				fout << "rectangle('Position',[" << left[b] << " " << low[b] << " " << right[b] - left[b] << " " << up[b] - low[b] << "], 'EdgeColor','y', 'FaceColor','y')" << endl;
			}
			else if (curr_node->snd_voltage < irsolver->Min_V + (diff_v * 4) / 7)
			{
				fout << "rectangle('Position',[" << left[b] << " " << low[b] << " " << right[b] - left[b] << " " << up[b] - low[b] << "], 'EdgeColor','g', 'FaceColor','g')" << endl;
			}
			else if (curr_node->snd_voltage < irsolver->Min_V + (diff_v * 5) / 7)
			{
				fout << "rectangle('Position',[" << left[b] << " " << low[b] << " " << right[b] - left[b] << " " << up[b] - low[b] << "], 'EdgeColor','c', 'FaceColor','c')" << endl;
			}
			else if (curr_node->snd_voltage < irsolver->Min_V + (diff_v * 6) / 7)
			{
				fout << "rectangle('Position',[" << left[b] << " " << low[b] << " " << right[b] - left[b] << " " << up[b] - low[b] << "], 'EdgeColor','b', 'FaceColor','b')" << endl;
			}
			else
			{
				fout << "rectangle('Position',[" << left[b] << " " << low[b] << " " << right[b] - left[b] << " " << up[b] - low[b] << "], 'EdgeColor',[0.67 0 1], 'FaceColor',[0.67 0 1])" << endl; //purple
			}
		}
	}
	/*********************Draw Module*********************/
	for (unsigned int m = 0; m < nt_info->module.size(); m++)
	{
		fout << "rectangle('Position',[" << nt_info->module[m]->llx << " " << nt_info->module[m]->lly << " "
			<< nt_info->module[m]->urx - nt_info->module[m]->llx << " " << nt_info->module[m]->ury - nt_info->module[m]->lly << "])" << endl;
	}
	/*********************Draw Pad*********************/
	for (unsigned int p = 0; p < nt_info->vdd_power_pad.size(); p++)
	{
		fout << "rectangle('Position',[" << nt_info->vdd_power_pad[p]->llx << " " << nt_info->vdd_power_pad[p]->lly << " "
			<< nt_info->vdd_power_pad[p]->urx - nt_info->vdd_power_pad[p]->llx << " " << nt_info->vdd_power_pad[p]->ury - nt_info->vdd_power_pad[p]->lly << "],'FaceColor',[0.8 0.8 0.8])" << endl;
	}
}
void IRSOLVER::LasPack(NetList *nt_info, IRSOLVER *irsolver)
{
	dlt = getDoubleTime();
	vector< vector<double> > temp_matrix;	/// coefficeient matrix
	vector < double > temp_current_vector;	/// current vector
	int countt = 0, node_s = 0;
	fstream fs("solve.m", fstream::out);
	for (unsigned int i = 0; i < irsolver->CrossNodeList.size(); i++)
	{
		if (irsolver->CrossNodeList.at(i)->power_pad == false)
		{
			irsolver->CrossNodeList.at(i)->id = node_s;
			node_s++;
		}
	}
	cout << node_s; 
	for (unsigned int i = 0; i < irsolver->CrossNodeList.size(); i++)
	{
		if (irsolver->CrossNodeList.at(i)->power_pad == false)
		{
			vector<double> temp_node(node_s * 2, 0), temp_node2(node_s * 2, 0);        /// matrix pointer corresponding to temp conductivity
			double total_conductivity = 0.0;		///element for exist node
			double total_current = 0.0;
			double length, width, resistance, conductivity, left_r = -1, right_r = -1, low_r = -1, up_r = -1;
			if (irsolver->CrossNodeList.at(i)->left_node != NULL)
			{
				length = fabs(irsolver->CrossNodeList.at(i)->center_x - irsolver->CrossNodeList.at(i)->left_node->center_x);
				width = irsolver->CrossNodeList.at(i)->h_strap->width;
				resistance = nt_info->UnitNomResistance_PDN_layer_H*length / width;
				conductivity = 1.0 / resistance;
				total_conductivity += conductivity;
				if (irsolver->CrossNodeList.at(i)->left_node->power_pad == true)
				{
					total_current+=nt_info->GLOBAL_VOLTAGE / resistance;
				}
				else
				{
					left_r = -conductivity;
				}
			}
			if (irsolver->CrossNodeList.at(i)->right_node != NULL)
			{
				length = fabs(irsolver->CrossNodeList.at(i)->center_x - irsolver->CrossNodeList.at(i)->right_node->center_x);
				width = irsolver->CrossNodeList.at(i)->h_strap->width;
				resistance = nt_info->UnitNomResistance_PDN_layer_H*length / width;
				conductivity = 1.0 / resistance;
				total_conductivity += conductivity;
				if (irsolver->CrossNodeList.at(i)->right_node->power_pad == true)
				{
					total_current += nt_info->GLOBAL_VOLTAGE / resistance;
				}
				else
					right_r = -conductivity;
			}
			length = nt_info->via_RDL->height;
			width = nt_info->via_RDL->width;
			conductivity = (1 / (nt_info->via_RDL->unitNomResistance*length / width));
			total_conductivity += conductivity;
			temp_node[irsolver->CrossNodeList.at(i)->id] = total_conductivity;
			if (left_r != -1)
			{
				temp_node[irsolver->CrossNodeList.at(i)->left_node->id] = left_r;
				//cout << "Left " << irsolver->CrossNodeList.at(i)->left_node->id <<" : "<<left_r << " ";
			}
			if (right_r != -1)
			{
				temp_node[irsolver->CrossNodeList.at(i)->right_node->id] = right_r;
				/*cout << "Right " << irsolver->CrossNodeList.at(i)->right_node->id << " : " << right_r << " ";
				getchar();*/
			}
			temp_node[irsolver->CrossNodeList.at(i)->id + node_s] = -conductivity;
			temp_current_vector.push_back(total_current);
			total_conductivity = 0.0;
			temp_matrix.push_back(temp_node);
			if (irsolver->CrossNodeList.at(i)->low_node != NULL)
			{
				length = fabs(irsolver->CrossNodeList.at(i)->center_y - irsolver->CrossNodeList.at(i)->low_node->center_y);
				width = irsolver->CrossNodeList.at(i)->v_strap->width;
				resistance = nt_info->UnitNomResistance_PDN_layer_V*length / width;
				conductivity = 1.0 / resistance;
				total_conductivity += conductivity;
				low_r = -conductivity;
			}
			if (irsolver->CrossNodeList.at(i)->up_node != NULL)
			{
				length = fabs(irsolver->CrossNodeList.at(i)->center_y - irsolver->CrossNodeList.at(i)->up_node->center_y);
				width = irsolver->CrossNodeList.at(i)->v_strap->width;
				resistance = nt_info->UnitNomResistance_PDN_layer_V*length / width;
				conductivity = 1.0 / resistance;
				total_conductivity += conductivity;
				up_r = -conductivity;
			}
			length = nt_info->via_RDL->height;
			width = nt_info->via_RDL->width;
			conductivity = (1 / (nt_info->via_RDL->unitNomResistance*length / width));
			total_conductivity += conductivity;
			temp_node2[irsolver->CrossNodeList.at(i)->id +node_s] = total_conductivity;
			temp_node2[irsolver->CrossNodeList.at(i)->id] = -conductivity;
			total_current = irsolver->CrossNodeList.at(i)->snd_current;
			temp_current_vector.push_back(total_current);
			if (low_r != -1)
			{
				temp_node2[irsolver->CrossNodeList.at(i)->low_node->id] = low_r;
			}
			if (up_r != -1)
			{
				temp_node2[irsolver->CrossNodeList.at(i)->up_node->id] = up_r;
			}
			temp_matrix.push_back(temp_node2);
		}
	}
	cout << "    temp matrix done" << endl;
	fs << "A=[";
	for (unsigned int i = 0; i < temp_matrix.size(); i++)
	{
		for (unsigned int j = 0; j < temp_matrix[i].size(); j++)
		{
			fs << temp_matrix[i][j] << " ";
		}
		fs << ";";
	}
	fs << "];C=[";
	for (unsigned int i = 0; i < temp_current_vector.size(); i++)
	{
		fs << temp_current_vector[i] << " ";
	}
	fs << "]';";
	dlt = getDoubleTime() - dlt;
	cout << "Time: " << setw(5) << dlt << " s" << endl;
}
