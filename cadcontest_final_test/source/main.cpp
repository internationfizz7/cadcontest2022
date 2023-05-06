//edited Aug 24th by hsieh ^U^
#define _CRT_SECURE_NO_WARNINGS
#include "parser.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include "data_struct.h"

using namespace std;

void Plot(string);
//________________________^U^_____________________________//
void MergeSort(vector<int>&, vector<MODULE>&, int, int);
void MergeSort(vector<int>&, int, int);
REG OptimalRegion(MODULE&);
REG OptimalRegion(WIRE*, MODULE*);
REG WireRegion(WIRE*);
REG WireRegion(int, int, char);
REG find_space(REG, MODULE*);
int area(const REG&);
int HPWL(const REG&);
int HWPL_approx(WIRE*);
bool IsOcuppied(REG, MODULE);//find if the region is occupied by macros by mean of enum
vector<MODULE*> RegionEnum(REG);
void displayReg(REG);
//-------------------------------------------------------//
int main(int argc, char*argv[])
{

	//cout<<"please enter which case to execute(case01/case02/case03)\n";


	//verilog=argv[i];
	const char* tt = ".";
	char *caseee = argv[1];
	char* aa = strtok(caseee, tt);
	//string file=cs+"/lefdef/"+cs;
	/*const char *cstr = file.c_str();
	  char* aa=const_cast<char *>(cstr);*/
	  
	  
	  
	/*write first loop pl to def*/  
	  

	ReadLefFile(aa, nt);
	ReadMlistFile(aa, nt);
	ReadDefFile(aa, nt);
	ReadVerilogFile(aa, nt);
	/*for(int i=0;i<10;i++){
		cout<<nt.stds[i].modName<<" "<<nt.stds[i].typeName<<" "<<nt.stds[i].type<<" "<<nt.stds[i].LeftX<<" "<<nt.stds[i].LeftY<<"\n";
	}*/
	//--------------------------------------------------------------------------------------
	
	vector<MOD> module;
	vector<MOD_f> mod_f;
	string check="FIXED";
	ifstream ifs;
	ofstream ofs;
	bool start_store = false;
	char buf[256] = "1;2 3";
	string wwrite;
	//cout << sizeof(buf) << "\n";
	const char* d = " ;:";
	char* cut;
	string end;
	double move_length;  //ªí¥Ü¥i²¾°Êªº¶ZÂ÷¡A­n¥hÅªÀÉ44
	char* file_txt = argv[5];
	ifs.open(file_txt);
	ifs.getline(buf, sizeof(buf));
	cut = strtok(buf, d);
	cut = strtok(NULL, d);
	move_length = stod(cut);
	double unit=0;
	//cout << "length  " << move_length << "\n";
	ifs.close();
	
	
	char* file_mlist = argv[4];
	ifs.open(file_mlist);

	char* file_dmp = argv[6];
	
	
	
	
	
	
	ofs.open(file_dmp);
	//cout<<"here";
	if (!ifs.is_open()) {
		cout << "failed\n";

	}
	else {
		/*ifs.getline(buf, sizeof(buf));
		  cut = strtok(buf, d);
		  cout << cut << "\n";*/
		while (!ifs.eof()) {
			string write;
			ifs.getline(buf, sizeof(buf));
			if (buf[0] == '\0') {
				write = buf;
				ofs << write << "\n";
				continue;
			}
			else {

				string temp;
				string xp;
				string yp;
				write = buf;
				cut = strtok(buf, d);
				if (cut[0] == 'U' && cut[1] == 'N' && cut[2] == 'I' && cut[3] == 'T' && cut[4] == 'S'){
					ofs << write << "\n";
					cut = strtok(NULL, d);
					cut = strtok(NULL, d);
					cut = strtok(NULL, d);
					//unit = stod(cut);
					move_length = move_length*unit;
					//cout << "length  " << move_length << "\n";
				}
				else{
					if (start_store == false) {
						if (cut[0] == 'C' && cut[1] == 'O' && cut[2] == 'M' && cut[3] == 'P' && cut[4] == 'O') {
							start_store = true;
							ofs << write << "\n";
							continue;
						}
						else {
							ofs << write << "\n";
						}
					}
					else {
						if (cut[0] == 'E' && cut[1] == 'N' && cut[2] == 'D') {
							end = write;
							break;
						}
						else {
							MOD new_mod;
							new_mod.module_name = strtok(NULL, d);
							new_mod.module_type = strtok(NULL, d);
							ifs.getline(buf, sizeof(buf));
							cut = strtok(buf, d);
							new_mod.mobility = strtok(NULL, d);
							temp = strtok(NULL, d);
							xp = strtok(NULL, d);
							new_mod.x = stoi(xp);
							yp = strtok(NULL, d);
							new_mod.y = stoi(yp);
							temp = strtok(NULL, d);
							new_mod.turn = strtok(NULL, d);
							if(new_mod.mobility==check){
							 MOD_f new_modf;
							 new_modf.module_name = new_mod.module_name;
							 new_modf.module_type = new_mod.module_type;
							 new_modf.mobility=check;
							 new_modf.x=new_mod.x;
							 new_modf.y =new_mod.y;
							 new_modf.turn=new_mod.turn;
							 new_modf.num=module.size();
							 new_modf.is_write=false;
							 mod_f.push_back(new_modf);
							}
							else{module.push_back(new_mod);}
						}
					}
				}



			}
		}
		//ifs.close();
	}
	cout<<nt.mods[0].modName<<"\n\n";
	for(int i=0;i<nt.mods[0].macro_wires.size();i++){
		cout<<nt.mods[0].macro_wires[i]->wireName<<" ";
	}
	
	//---------------------------------------------------------------------------------------------
	//____________________________________std_cell__test____________________________________________
	int centx;
	int centy;
	string c1="case01";
	string c2="case02";
	string c3="case03";
	string c4="case04";
	string c5="case05";
	//add by condition
	if(aa==c1){
	centx = nt.ChipWidth / 2;
	centy= nt.ChipHeight / 2;
	}
	else if(aa==c2){
	centy=5700000;
	centx=5000000;
	}
	else if(aa==c3){
	centx=7000000;
	centy=6000000;
	}
	else if(aa==c4){
	centx=8000000;
	centy=5700000;
	}
	else if(aa==c5){
	//centx=4000000;
	centx=5000000;
	centy=4000000;
	//centy=5000000;
	}
	else{
	centx = nt.ChipWidth / 2;
	centy= nt.ChipHeight / 2;
	}
	//cout<<centx<<"   "<<centy<<"\n";
	vector<int> sorted_index(nt.nMod);
	for (int i = 0; i < nt.nMod; i++)
		sorted_index[i] = i;
	MergeSort(sorted_index, nt.mods, 0, nt.nMod - 1);
	//test macro64
	int all_degree=0;
	int all_y=0;
	
	/*for(int i=0;i<nt.mods[64].macro_pins.size();i++ ){
	WIRE* thisWire = nt.mods[64].macro_wires[i];
	int degree = thisWire->wire_macroID.size() + thisWire->wire_stdID.size();// +thisWire->wire_padID.size();
	 cout<<nt.mods[64].macro_pins[i]->x_shift+nt.mods[64].LeftX<<" "<<nt.mods[64].macro_pins[i]->y_shift<<" "<<nt.mods[64].LeftY<<" "<<nt.mods[64].macro_pins[i]->y_shift+nt.mods[64].LeftY<<" "<<nt.mods[64].macro_pins[i]->macropinName<<" "<<degree<<"\n";
	 all_degree+=degree;
	 cout<<degree<<"\n";
	 all_y+=(nt.mods[64].macro_pins[i]->y_shift+nt.mods[64].LeftY)*degree;
	 cout<<all_y<<"\n";
	}
	all_y=all_y/all_degree;
	cout<<"\n\n"<<all_y;*/
	
	
	//cout<<nt.mods[0].modName<<"\n"<<nt.mods[0].typeName<<"\n"<<nt.mods[0].type<<"\n"<<nt.mods[0].modW<<"\n"<<nt.mods[0].modH<<"\n"<<nt.mods[0].modL<<"\n"<<nt.mods[0].flagstd<<"\n"<<nt.mods[0].GleftX<<"\n"<<nt.mods[0].GleftY<<"\n"<<nt.mods[0].LeftX<<"\n"<<nt.mods[0].LeftY<<"\n"<<nt.mods[0].GcenterX<<"\n"<<nt.mods[0].GcenterY<<"\n"<<nt.mods[0].centerX<<"\n"<<nt.mods[0].centerY<<"\n"<<nt.mods[0].modOri<<"\n"<<nt.mods[0].nNet<<"\n"<<nt.mods[0].modArea<<"\n"<<nt.mods[0].macro_wires.size()<<"\n"<<nt.mods[0].macro_pins.size()<<"\n";
	
	
	
	for (int i = 0; i < (int)nt.nMod; i++)
	{	
		
		MODULE* thisMacro = &nt.mods[sorted_index[i]];
		//if(thisMacro->CenterX<4000000 ||thisMacro->CenterX>14000000 || thisMacro->CenterY<1000000 || thisMacro->CenterY>5500000){
		double cent_to_x = 0;
		double cent_to_y = 0;
		int oad=0;
		for (int j = 0; j < thisMacro->macro_wires.size(); j++)
		{
			WIRE* thisWire = thisMacro->macro_wires[j];
			int degree = thisWire->wire_macroID.size() + thisWire->wire_stdID.size();// +thisWire->wire_padID.size();
			oad+=degree;
			int orign = abs(centx - thisMacro->LeftX - thisMacro->macro_pins[j]->x_shift);
			int after = abs(centx - thisMacro->LeftX - thisMacro->modW + thisMacro->macro_pins[j]->x_shift);
			
			cent_to_x += (after - orign) * degree/ 2000;
			orign = abs(centy - thisMacro->LeftY - thisMacro->macro_pins[j]->y_shift);
			after = abs(centy - thisMacro->LeftY - thisMacro->modH + thisMacro->macro_pins[j]->y_shift);
			cent_to_y += (after - orign) * degree / 2000;
			
		}
		if (cent_to_x < 0 && cent_to_y < 0)
		{
			module[sorted_index[i]].turn = "S";
		}
		if (cent_to_x < 0 && cent_to_y > 0)
		{
			module[sorted_index[i]].turn = "FS";
		}
		if (cent_to_x > 0 && cent_to_y < 0)
		{
			module[sorted_index[i]].turn = "FN";
		}
		if(sorted_index[i]==64){
		//cout<<i<<" "<<sorted_index[i]<<" "<<oad<<" "<<cent_to_x<<" "<<cent_to_y<<"\n";
		}
		//}
	}
	//____________________________________________________________________________________________
	//--------------------------------------------------------------------------------------------
	
	int sdfsdfd=0;
	if(mod_f.empty()==false){
	for (int i = 0; i < module.size(); i++) {
	
	 if(i!=mod_f[sdfsdfd].num ){
		ofs << "   - " << module[i].module_name << " " << module[i].module_type << "\n";
		ofs << "      + " << module[i].mobility << " ( " << nt.mods[i].LeftX << " " << nt.mods[i].LeftY << " ) " << module[i].turn << " ;\n";
		
		}
	 else{
	       
	       i--;
	       
	       ofs << "   - " << mod_f[sdfsdfd].module_name << " " << mod_f[sdfsdfd].module_type << "\n";
	       ofs << "      + " << mod_f[sdfsdfd].mobility << " ( " << mod_f[sdfsdfd].x << " " << mod_f[sdfsdfd].y << " ) " << mod_f[sdfsdfd].turn << " ;\n";
	       sdfsdfd++;
	 }
	 }
	 
	}
        else{
        
        for (int i = 0; i < module.size(); i++) {
        ofs << "   - " << module[i].module_name << " " << module[i].module_type << "\n";
		ofs << "      + " << module[i].mobility << " ( " << nt.mods[i].LeftX << " " << nt.mods[i].LeftY << " ) " << module[i].turn << " ;\n";
		
        }
        }
	ofs << end << "\n";
	while (!ifs.eof()) {
		ifs.getline(buf, sizeof(buf));
		ofs << buf << "\n";
		continue;
	}
	ifs.close();
	ofs.close();
	
	//-----------------------------------------------------------------------------------------------
	string filename = "initial.m";
	Plot(filename);
      FILE * f;
	char pl[100]="\0";
	strcpy(pl,aa);
	strcat(pl,".pl");
	vector<string> wr;
	if((f = fopen(pl, "r")) == 0){
	 	cout<<"cant open .pl\n";
	}
	else{
	  fgets(buf,sizeof(buf),f);
	  wr.push_back(buf);
	  //cout<<buf;
	  fgets(buf,sizeof(buf),f);
	  wwrite=buf;
	  cut=strtok(buf,d);
	  for(int i=0;i<module.size();i++){
	    string write_in="\0";
	    write_in+=module[i].module_name;
	    write_in+=" ";
	    write_in+=to_string(nt.mods[i].LeftX);
	    write_in+=" ";
	    write_in+=to_string(nt.mods[i].LeftY);
	    write_in+=" : ";
	    write_in+=module[i].turn;
	    if(i==(module.size()-1)){
	    write_in+=" /FIXED\n";
	    }
	    else{
	    write_in+=" /FIXED\n";
	    }
	    
	    while(cut!=module[i].module_name){
	     	
	  	wr.push_back(wwrite);
	  	fgets(buf,sizeof(buf),f);
	  	wwrite=buf;
	  	if(buf[0]=='\0'){
	  	 wr.push_back(wwrite);
	  	 fgets(buf,sizeof(buf),f);
	  	 wwrite=buf;
	  	}
	  	cut=strtok(buf,d);
	    }
	    wr.push_back(write_in);
	    if(i==(module.size()-1)){
	     break;
	    }
	    fgets(buf,sizeof(buf),f);
	    wwrite=buf;
	    cut=strtok(buf,d);
	  }
	  while(!feof(f)){
	  	fgets(buf,sizeof(buf),f);
	  	wwrite=buf;
	  	wr.push_back(wwrite);
	  }
	}
	fclose(f);
	
	FILE *fp;
	fp=fopen(pl,"r+");
	for(int i=0;i<=wr.size();i++){
	if(i==wr.size()){
	break;
	}
	else{
	string wrr=wr[i];
	const char *wrrr=wrr.c_str();
	 fputs(wrrr,fp);
	}
	}
	fclose(fp);
        cout<<"done\n";
        
	return 0;
}

void Plot(string fileName){
	//cout << "   Write " << fileName << endl;
	//if(fileName == "") fileName = "GP.m";
	ofstream fout(fileName.c_str());


	fout << "figure;\nclear;\nclf;\naxis equal;\nhold on;\ngrid on;\n" << endl;

	//chip boundary
	fout << "block_x=[" << nt.ChipBoundary_llx << " " << nt.ChipBoundary_llx << " " << nt.ChipBoundary_urx << " " << nt.ChipBoundary_urx << " " << nt.ChipBoundary_llx << " ];" << endl;
	fout << "block_y=[" << nt.ChipBoundary_lly << " " << nt.ChipBoundary_ury << " " << nt.ChipBoundary_ury << " " << nt.ChipBoundary_lly << " " << nt.ChipBoundary_lly << " ];" << endl;
	fout << "fill(block_x, block_y, 'w');" << endl;

	for (unsigned int i = 0; i < (int)nt.mods.size(); i++)
	{
		double llx = nt.mods[i].LeftX;
		double lly = nt.mods[i].LeftY;
		double urx = nt.mods[i].LeftX + nt.mods[i].modW;
		double ury = nt.mods[i].LeftY + nt.mods[i].modH;

		fout << "block_x=[" << llx << " " << llx << " " << urx << " " << urx << " " << llx << " ];" << endl;
		fout << "block_y=[" << lly << " " << ury << " " << ury << " " << lly << " " << lly << " ];" << endl;
		fout << "text(" << (llx + urx) / 2.0 << ", " << (lly + ury) / 2.0 << ", '" << i << "');" << endl << endl;
		fout << "fill(block_x,block_y,'g','facealpha',0.5);" << endl;
	}
	for (unsigned int i = 0; i < nt.pads.size(); i++)
	{
		double llx = nt.pads[i].LeftX;
		double lly = nt.pads[i].LeftY;
		double urx = nt.pads[i].LeftX + nt.pads[i].modW;
		double ury = nt.pads[i].LeftY + nt.pads[i].modH;

		/*if(nt.pads[i].padOri == 4 || nt.pads[i].padOri == 6)
		  {
		  urx = nt.pads[i].LeftX + nt.pads[i].padH;
		  ury = nt.pads[i].LeftY + nt.pads[i].padW;
		  }*/

		fout << "block_x=[" << llx << " " << llx << " " << urx << " " << urx << " " << llx << " ];" << endl;
		fout << "block_y=[" << lly << " " << ury << " " << ury << " " << lly << " " << lly << " ];" << endl;
		//        fout << "fill(block_x,block_y,'c');" << endl;
		fout << "text(" << (llx + urx) / 2.0 << ", " << (lly + ury) / 2.0 << ", '" << i << "');" << endl << endl;
		fout << "fill(block_x,block_y,'c','facealpha',0.5);" << endl;
	}

	fout.close();
}

void MergeSort(vector<int>& arr, vector<MODULE>& ref, int left, int right){
	if (left == right)
	{
		return;
	}
	//divide-------------------------
	int new_right = left + (right - left - 1) / 2;
	int new_left = right - (right - left) / 2;
	MergeSort(arr, ref, left, new_right);
	MergeSort(arr, ref, new_left, right);
	//merge--------------------------
	int i = left, j = new_left, k = 0;
	vector<int> temp(right - left + 1);
	while (k < right - left + 1)
	{
		if (i > new_right)// left array done
		{
			temp[k] = arr[j];
			k++;
			j++;
			continue;
		}
		if (j > right)// right array done
		{
			temp[k] = arr[i];
			k++;
			i++;
			continue;
		}
		if (ref[arr[i]].macro_wires.size() < ref[arr[j]].macro_wires.size())//right array [i] < left array[j]
		{
			temp[k] = arr[j];
			k++;
			j++;
		}
		else ////right array [i] >= left array[j]
		{
			temp[k] = arr[i];
			k++;
			i++;
		}
	}
	for (int m = left; m < right + 1; m++)
	{
		arr[m] = temp[m - left];
	}
}
void MergeSort(vector<int>& arr, int left, int right){
	if (left == right)
	{
		return;
	}
	//divide-------------------------
	int new_right = left + (right - left - 1) / 2;
	int new_left = right - (right - left) / 2;
	MergeSort(arr, left, new_right);
	MergeSort(arr, new_left, right);
	//merge--------------------------
	int i = left, j = new_left, k = 0;
	vector<int> temp(right - left + 1);
	while (k < right - left + 1)
	{
		if (i > new_right)// left array done
		{
			temp[k] = arr[j];
			k++;
			j++;
			continue;
		}
		if (j > right)// right array done
		{
			temp[k] = arr[i];
			k++;
			i++;
			continue;
		}
		if (arr[i] < arr[j])//right array [i] < left array[j]
		{
			temp[k] = arr[j];
			k++;
			j++;
		}
		else ////right array [i] >= left array[j]
		{
			temp[k] = arr[i];
			k++;
			i++;
		}
	}
	for (int m = left; m < right + 1; m++)
	{
		arr[m] = temp[m - left];
	}
}
REG OptimalRegion(MODULE& thisMacro){
	int left = 0, right = 0, bottom = 0, up = 0;
	vector<int> forX;
	vector<int> forY;
	for (int i = 0; i < thisMacro.macro_wires.size(); i++)
	{
		WIRE* thisWire = thisMacro.macro_wires[i];
		if (thisWire->wire_macroID.size() < 2){//¨S±µ¨ì¨ä¥Lmacroªºwire
			continue;
		}
		//cout << thisWire->wireName<<"|";
		REG thisReg = OptimalRegion(thisWire, &thisMacro); //¦©°£±¼¦Û¤v«áªºwire region
		if (thisReg.right_x == 0 && thisReg.left_x == 0){
			continue;
		}
		forX.push_back(thisReg.right_x);
		forX.push_back(thisReg.left_x);
		forY.push_back(thisReg.bottom_y);
		forY.push_back(thisReg.up_y);
	}
	//sort---------------------------------
	if (forX.empty()){
		REG noReg = { 0, 0, 0, 0 };
		return noReg;
	}
	//cout << endl;
	vector<int> sort_x(forX);
	vector<int> sort_y(forY);
	MergeSort(sort_x, 0, sort_x.size() - 1);
	MergeSort(sort_y, 0, sort_y.size() - 1);
	int count_x = 1;
	int count_y = 1;
	forX[0] = sort_x[0];
	forY[0] = sort_y[0];
	for (int i = 1; i < sort_x.size(); i++)
	{
		if (sort_x[i] == sort_x[i])
			continue;
		forX[count_x] = sort_x[i];
		count_x++;
		//cout <<", "<< sort_x[i] ;
	}//¦©°£­«½Æx
	forX.resize(count_x);
	//cout << endl;
	for (int i = 1; i < sort_y.size(); i++)
	{
		if (sort_y[i] == sort_y[i - 1])
			continue;
		forY[count_y] = sort_y[i];
		count_y++;
		//cout <<", "<< sort_y[i] ;
	}//¦©°£­«½Æy
	forY.resize(count_y);
	//cout << endl;
	//-------------------------------------
	left = forX[(count_x) / 2];
	right = forX[(count_x - 1) / 2];
	bottom = forY[(count_y) / 2];
	up = forY[(count_y - 1) / 2];
	//--------------------------------------
	REG opt_region = { left, right, bottom, up };
	return opt_region;
}
REG OptimalRegion(WIRE* thisWire, MODULE* macro){
	int left = 0, right = 0, bottom = 0, up = 0;
	bool first = true;
	for (int i = 0; i < thisWire->wire_macroID.size(); i++)
	{
		MODULE* thisMacro = &nt.mods[thisWire->wire_macroID[i]];
		if (thisMacro == macro)
		{
			continue;
		}
		if (first)
		{
			left = thisMacro->CenterX;
			right = thisMacro->CenterX;
			bottom = thisMacro->CenterY;
			up = thisMacro->CenterY;
			first = false;
		}
		else
		{
			if (thisMacro->CenterX < left)
				left = thisMacro->CenterX;
			if (thisMacro->CenterX > right)
				right = thisMacro->CenterX;
			if (thisMacro->CenterX < bottom)
				bottom = thisMacro->CenterY;
			if (thisMacro->CenterX > up)
				up = thisMacro->CenterY;
		}
	}
	REG wire_region = { left, right, bottom, up };
	//cout << "[(" << wire_region.left_x << ", " << wire_region.bottom_y << "), (" << wire_region.right_x << ", " << wire_region.up_y << ")], ";
	return wire_region;
}
REG WireRegion(WIRE* thisWire){
	int left = 0, right = 0, bottom = 0, up = 0;
	for (int i = 0; i < thisWire->wire_macroID.size(); i++)
	{
		MODULE* thisMacro = &nt.mods[thisWire->wire_macroID[i]];
		if (i == 0)
		{
			left = thisMacro->CenterX;
			right = thisMacro->CenterX;
			bottom = thisMacro->CenterY;
			up = thisMacro->CenterY;
		}
		else
		{
			if (thisMacro->CenterX < left)
				left = thisMacro->CenterX;
			if (thisMacro->CenterX > right)
				right = thisMacro->CenterX;
			if (thisMacro->CenterX < bottom)
				bottom = thisMacro->CenterY;
			if (thisMacro->CenterX > up)
				up = thisMacro->CenterY;
		}
	}
	REG wire_region = { left, right, bottom, up };
	return wire_region;
}
REG WireRegion(int thisModId, int thisWireId, char dir){
	MODULE* thisMacro = &nt.mods[thisModId];
	WIRE*  thisWire = thisMacro->macro_wires[thisWireId];
	int llx, urx, lly, ury;
	switch (dir)
	{
	case 'N':
		llx = thisMacro->LeftX + thisMacro->macro_pins[thisWireId]->x_shift;
		urx = llx;
		lly = thisMacro->LeftY + thisMacro->macro_pins[thisWireId]->y_shift;
		ury = lly;
		break;
	case 'X':
		llx = thisMacro->LeftX + thisMacro->modW - thisMacro->macro_pins[thisWireId]->x_shift;
		urx = llx;
		lly = thisMacro->LeftY + thisMacro->macro_pins[thisWireId]->y_shift;
		ury = lly;
		break;
	case 'Y':
		llx = thisMacro->LeftX + thisMacro->macro_pins[thisWireId]->x_shift;
		urx = llx;
		lly = thisMacro->LeftY + thisMacro->modH - thisMacro->macro_pins[thisWireId]->y_shift;
		ury = lly;
		break;
	case 'S':
		llx = thisMacro->LeftX + thisMacro->modW - thisMacro->macro_pins[thisWireId]->x_shift;
		urx = llx;
		lly = thisMacro->LeftY + thisMacro->modH - thisMacro->macro_pins[thisWireId]->y_shift;
		ury = lly;
		break;
	default:
		cout << "error, invalid charactor";
		return{ 0, 0, 0, 0 };
		break;
	}
	for (int i = 0; i < thisWire->wire_macroID.size(); i++)
	{
		if (thisWire->wire_macroID[i] == thisModId)
			continue;
		MODULE* Macro_i = &nt.mods[thisWire->wire_macroID[i]];
		if (Macro_i->CenterX < llx)
			llx = Macro_i->CenterX;
		if (Macro_i->CenterX > urx)
			urx = Macro_i->CenterX;
		if (Macro_i->CenterY < lly)
			lly = Macro_i->CenterY;
		if (Macro_i->CenterY < urx)
			ury = Macro_i->CenterY;
	}
	REG reg;
	reg.left_x = llx;
	reg.right_x = urx;
	reg.bottom_y = lly;
	reg.up_y = ury;
	return reg;
}
int HWPL_approx(WIRE* thisWire){
	REG wire_region = WireRegion(thisWire);
	return wire_region.right_x - wire_region.left_x + wire_region.up_y - wire_region.bottom_y;
}
bool IsOcuppied(REG thisReg, MODULE* thisMacro){
	bool ocped = false;
	for (int i = 0; i < nt.mods.size(); i++)
	{
		if (&nt.mods[i] == thisMacro){
			continue;
		}
		double llx = nt.mods[i].LeftX;
		double lly = nt.mods[i].LeftY;
		double urx = nt.mods[i].LeftX + nt.mods[i].modW;
		double ury = nt.mods[i].LeftY + nt.mods[i].modH;
		if ((llx < thisReg.right_x && urx > thisReg.left_x) || (lly < thisReg.up_y && ury > thisReg.bottom_y))
		{
			ocped = true;
			break;
		}
	}
	return ocped;
}
vector<MODULE*> RegionEnum(REG thisReg){
	vector<MODULE*> macros(0);
	for (int i = 0; i < nt.mods.size(); i++)
	{
		MODULE* thisMacro = &nt.mods[i];
		double llx = nt.mods[i].LeftX;
		double lly = nt.mods[i].LeftY;
		double urx = nt.mods[i].LeftX + nt.mods[i].modW;
		double ury = nt.mods[i].LeftY + nt.mods[i].modH;
		if ((llx < thisReg.right_x && urx > thisReg.left_x) || (lly > thisReg.up_y && ury < thisReg.bottom_y))
		{
			macros.resize(macros.size() + 1, thisMacro);
			break;
		}
	}
	return macros;
}
REG find_space(REG thisReg, MODULE* thisMacro) {
	displayReg(thisReg);
	int MW = thisMacro->modW;
	int MH = thisMacro->modH;
	int RW = thisReg.right_x - thisReg.left_x;
	int RH = thisReg.up_y - thisReg.bottom_y;
	bool empty = true;
	if (RW < MW || RH < MH)
	{
		return{ 0, 0, 0, 0 };// ===the region is inavalible(too small)===
	}
	for (int i = 0; i < nt.mods.size(); i++){
		if (&nt.mods[i] == thisMacro){
			cout << "!";
			continue;
		}
		int llx = nt.mods[i].LeftX;
		int lly = nt.mods[i].LeftY;
		int urx = nt.mods[i].LeftX + nt.mods[i].modW;
		int ury = nt.mods[i].LeftY + nt.mods[i].modH;
		vector<REG> regs;
		if ((llx < thisReg.right_x && urx > thisReg.left_x) && (lly < thisReg.up_y && ury > thisReg.bottom_y))
		{
			cout << "^o^\n";
			empty = 0;
			//divide remaining region into 4 parts;
			if (thisReg.left_x < llx)
			{
				REG reg_left = { thisReg.left_x, llx, thisReg.bottom_y, thisReg.up_y };
				regs.push_back(reg_left);
			}
			if (thisReg.bottom_y < lly)
			{
				REG reg_bottom = { thisReg.left_x, thisReg.right_x, thisReg.bottom_y, lly };
				regs.push_back(reg_bottom);
			}
			if (thisReg.right_x > urx)
			{
				REG reg_right = { urx, thisReg.right_x, thisReg.bottom_y, thisReg.up_y };
				regs.push_back(reg_right);
			}
			if (thisReg.up_y > ury)
			{
				REG reg_up = { thisReg.left_x, thisReg.right_x, ury, thisReg.up_y };
				regs.push_back(reg_up);
			}
			if (regs.empty())//the region is full
				return{ 0, 0, 0, 0 };// ====the regin is occupied by a single macro====
			//findspace()
			for (int i = 0; i < regs.size(); i++)
			{
				cout << "  >>  ";
				REG subreg = find_space(regs[i], thisMacro);
				if (subreg.up_y == 0 && subreg.right_x == 0)
				{
					continue;
				}
				cout << "^^\n";
				return subreg;//=== return sub_region if it is availible
			}
			break;
		}
	}
	if (empty)
	{
		cout << "XD";
		return thisReg;// ===return this region if it is avalible(empty)===
	}
	else
	{
		return{ 0, 0, 0, 0 };//=== the whole region is not avalible
	}

}
int area(const REG& r) {
	return (r.right_x - r.left_x) * (r.up_y - r.bottom_y);
}
int HPWL(const REG& thisReg){
	return thisReg.right_x - thisReg.left_x + thisReg.up_y - thisReg.bottom_y;
}
void displayReg(REG thisReg){
	cout << "{(" << thisReg.left_x << ", " << thisReg.bottom_y << "), (" << thisReg.right_x << ", " << thisReg.up_y << ")}";
	return;
}
