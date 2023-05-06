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
#include <sstream>
#include <vector>
#include "data_struct.h"

using namespace std;
void plotpin_all_after(vector<MOD> , string );
void total_degree(MODULE* ,WIRE* ,double & ,double & ,double & );
void find_degree(MODULE* firstmod,WIRE* firstwire,double & cx,double & cy,double & std_num,vector<bool>,vector<bool>,vector<bool>);
void get_degree(double & cx,double & cy,double & std_num,vector<bool>& chosen_std,vector<bool>& chosen_mod,vector<bool>& chosen_wires,vector<WIRE*>& wv,vector<int>& layer,bool & ,double &);
void Plot(string);
void plotpin_all(string);
void plotpin(MODULE* ,string);
void Plot2(string ,vector<bool> ,vector<bool> );
//________________________^U^_____________________________//
void MergeSort(vector<int>&, vector<MODULE>&, int, int);
void Macro_Find_gravity(WIRE* ,double&,double& );
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
void Macro_Find_cent(MODULE*, double&, double&);
//-------------------------------------------------------//
int main(int argc, char*argv[])
{

	//cout
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
	double unit;
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
					unit = stod(cut);
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
	/*cout<<nt.mods[0].modName<<"\n\n";
	for(int i=0;i<nt.mods[0].macro_wires.size();i++){
		cout<<nt.mods[0].macro_wires[i]->wireName<<" ";
	}
	*/
	//---------------------------------------------------------------------------------------------
	//____________________________________std_cell__test____________________________________________
	double centx = 0;
	double centy = 0;
	//--------------------------------------method 1--------------------------------------------
	for (unsigned int i = 0; i<nt.stds.size(); i++)
	{
		centx += nt.stds[i].CenterX / nt.stds.size();
		centy += nt.stds[i].CenterY / nt.stds.size();
	}
	cout << endl << centx <<","<< centy << endl;
	//-------------------------------------------------------------------------------------------
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
	
	
	int num_moved=0;
	
	//check
	//cout<<"與wires[0]相接的:\n";
	//cout<<"nt.mods[0]:"<<nt.mods[0].modName<<endl;
	/*cout<<" nt.mods[0].macro_wires[0]:"<<nt.mods[0].macro_wires[0]->wireName<<endl;
	cout<<"  nt.mods[0].macro_wires[0]->wire_macroID[0]:"<<nt.mods[nt.mods[0].macro_wires[0]->wire_macroID[0]].modName<<endl;
	cout<<"  nt.mods[0].macro_wires[0]->wire_stdID[0]  :"<<nt.stds[nt.mods[0].macro_wires[0]->wire_stdID[0]].modName<<endl;
	cout<<"   nt.mods[0].macro_wires[0]->wire_macroID[0].macro_wires[0]:"<<nt.mods[nt.mods[0].macro_wires[0]->wire_macroID[0]].macro_wires[0]->wireName<<endl;
	cout<<"   nt.mods[0].macro_wires[0]->wire_stdID[0].macro_wires[0]  :"<<nt.stds[nt.mods[0].macro_wires[0]->wire_stdID[0]].macro_wires[0]->wireName<<endl;
	cout<<"    "<<nt.mods[nt.mods[nt.mods[0].macro_wires[0]->wire_macroID[0]].macro_wires[0]->wire_macroID[0]].modName<<endl;
	cout<<"    "<<nt.stds[nt.mods[nt.mods[0].macro_wires[0]->wire_macroID[0]].macro_wires[0]->wire_stdID[0]].modName<<endl;*/
	/*for(int i=0;i<nt.mods[0].macro_wires[0]->wire_macroID.size();i++){
		cout<<" mods["<<i<<"]:"<<nt.mods[nt.mods[0].macro_wires[0]->wire_macroID[i]].modName<<endl;
	}
	for(int i=0;i<nt.mods[0].macro_wires[0]->wire_stdID.size();i++){
		cout<<" stds["<<i<<"]:"<<nt.stds[nt.mods[0].macro_wires[0]->wire_stdID[i]].modName<<endl;
	}*/
	
	
	
	//get every wire's degree
	
	/*for(int i=0;i<nt.wires.size();i++){
		nt.wires[i].degree=(nt.wires[i].wire_stdID.size()+nt.wires[i].wire_macroID.size());
		
	}*/
	
	//cout<<"\n"<<macro_choose.size()<<" "<<std_choose.size()<<"\n";
	
	
	/*for(int i=0;i<nt.wires.size();i++){
		if(nt.wires[i].)
	}*/
	//cout<<nt.stds.size();
	/*for(int i=0;i<nt.mods.size();i++){		
	MODULE* thisMacro=&nt.mods[i];
	
	WIRE* thiswire=thisMacro->macro_wires[0];
	
	map<string,int>::iterator itt;
	
	
	
			double gx=0;
			double gy=0;
			double degree=0;
			total_degree(thisMacro,thiswire,gx,gy,degree);
			cout<<"\n\n"<<degree<<"\n\n";
			}*/
			
			/*for(int i=0;i<nt.wires.size();i++){
				if(nt.wires[i].wireName=="n173752"){
					WIRE* thiswire=&nt.wires[i];
					get_degree(thiswire,degree,gx,gy,std_num,std_choose,macro_choose);
				}
			}*/
	
	
	vector<bool> chosen_mod;
	for(int i=0;i<nt.mods.size();i++){
		chosen_mod.push_back(false);
	}
	vector<bool> chosen_std;
	for(int i=0;i<nt.stds.size();i++){
		chosen_std.push_back(false);
	}
	vector<bool> chosen_wires;
	for(int i=0;i<nt.wires.size();i++){
		chosen_wires.push_back(false);
	}
	
	for (int i = 0; i < (int)nt.nMod; i++){
		cout<<i;
		MODULE* thisMacro = &nt.mods[sorted_index[i]];
		double cent_to_x = 0;
		double cent_to_y = 0;
		for(int j=0;j<thisMacro->macro_wires.size();j++){
			WIRE* thiswire=thisMacro->macro_wires[j];
			
			
			double degree=0;
			double gx=0;
			double gy=0;
			
			find_degree(thisMacro,thiswire,gx,gy,degree,chosen_mod,chosen_std,chosen_wires);
			if(degree!=0){
			gx/=degree;
			gy/=degree;
			}
			
			
			double orign = abs(gx - thisMacro->LeftX - thisMacro->macro_pins[j]->x_shift);
			double after = abs(gx + thisMacro->LeftX -2* thisMacro->CenterX + thisMacro->macro_pins[j]->x_shift);
			
			cent_to_x += (after - orign) * degree/ 2000;
			orign = abs(gy - thisMacro->LeftY - thisMacro->macro_pins[j]->y_shift);
			after = abs(gy + thisMacro->LeftY - 2*thisMacro->CenterY + thisMacro->macro_pins[j]->y_shift);
			cent_to_y += (after - orign) * degree/ 2000;
		}
		if (cent_to_x < 0 && cent_to_y < 0)
		{
			num_moved++;
			if(module[sorted_index[i]].turn=="N"){
				module[sorted_index[i]].turn = "S";
			}
			else if(module[sorted_index[i]].turn=="FS"){
				module[sorted_index[i]].turn = "FN";
			}
			else if(module[sorted_index[i]].turn=="FN"){
				module[sorted_index[i]].turn = "FS";
			}
			else if(module[sorted_index[i]].turn=="S"){
				module[sorted_index[i]].turn = "N";
			}
		}
		else if (cent_to_x < 0 && cent_to_y > 0)
		{
			num_moved++;
			if(module[sorted_index[i]].turn=="N"){
				module[sorted_index[i]].turn = "FS";
			}
			else if(module[sorted_index[i]].turn=="FS"){
				module[sorted_index[i]].turn = "FN";
			}
			else if(module[sorted_index[i]].turn=="FN"){
				module[sorted_index[i]].turn = "S";
			}
			else if(module[sorted_index[i]].turn=="S"){
				module[sorted_index[i]].turn = "FN";
			}
		}
		else if (cent_to_x > 0 && cent_to_y < 0)
		{
			num_moved++;
			if(module[sorted_index[i]].turn=="N"){
				module[sorted_index[i]].turn = "FN";
			}
			else if(module[sorted_index[i]].turn=="FS"){
				module[sorted_index[i]].turn = "S";
			}
			else if(module[sorted_index[i]].turn=="FN"){
				module[sorted_index[i]].turn = "N";
			}
			else if(module[sorted_index[i]].turn=="S"){
				module[sorted_index[i]].turn = "FS";
			}
		}
		else{
			if(module[sorted_index[i]].turn=="N"){
				module[sorted_index[i]].turn = "N";
			}
			else if(module[sorted_index[i]].turn=="FS"){
				module[sorted_index[i]].turn = "FS";
			}
			else if(module[sorted_index[i]].turn=="FN"){
				module[sorted_index[i]].turn = "FN";
			}
			else if(module[sorted_index[i]].turn=="S"){
				module[sorted_index[i]].turn = "S";
			}
		}
	}
	
	  
	/*for (int i = 0; i < (int)nt.nMod; i++)
	{	
		bool ntt=false;
		for(int l=0;l<nt.mods.size();l++){
			macro_choose[l]=false;
		}
		for(int l=0;l<nt.stds.size();l++){
			std_choose[l]=false;
		}
		//if(sorted_index[i]==30){
		MODULE* thisMacro = &nt.mods[sorted_index[i]];
		//if(thisMacro->CenterX<4000000 ||thisMacro->CenterX>14000000 || thisMacro->CenterY<1000000 || thisMacro->CenterY>5500000){
		double cent_to_x = 0;
		double cent_to_y = 0;
		int oad=0;
		Macro_Find_cent(thisMacro, centx, centy); // the center of all cells this macro link
		//cout<<centx<<" "<<centy<<"\n";
		for (int j = 0; j < thisMacro->macro_wires.size(); j++)
		{
			WIRE* thisWire = thisMacro->macro_wires[j];
			double gx=0;
			double gy=0;
			Macro_Find_gravity(thisWire,gx,gy);
			cout<<gx<<" "<<gy<<"\n";
			double degree_x = abs((thisMacro->LeftX+thisMacro->macro_pins[j]->x_shift)-gx);
			double degree_y = abs((thisMacro->LeftY+thisMacro->macro_pins[j]->y_shift)-gy);
			//oad+=degree;
			double orign = abs(gx - thisMacro->LeftX - thisMacro->macro_pins[j]->x_shift);
			double after = abs(gx + thisMacro->LeftX -2* thisMacro->CenterX + thisMacro->macro_pins[j]->x_shift);
			
			cent_to_x += (after - orign) * degree_x/ 2000;
			orign = abs(gy - thisMacro->LeftY - thisMacro->macro_pins[j]->y_shift);
			after = abs(gy + thisMacro->LeftY - 2*thisMacro->CenterY + thisMacro->macro_pins[j]->y_shift);
			cent_to_y += (after - orign) * degree_y / 2000;
			
		}
		if (cent_to_x < 0 && cent_to_y < 0)
		{
			num_moved++;
			if(module[sorted_index[i]].turn=="N"){
				module[sorted_index[i]].turn = "S";
			}
			else if(module[sorted_index[i]].turn=="FS"){
				module[sorted_index[i]].turn = "FN";
			}
			else if(module[sorted_index[i]].turn=="FN"){
				module[sorted_index[i]].turn = "FS";
			}
			else if(module[sorted_index[i]].turn=="S"){
				module[sorted_index[i]].turn = "N";
			}
		}
		else if (cent_to_x < 0 && cent_to_y > 0)
		{
			num_moved++;
			if(module[sorted_index[i]].turn=="N"){
				module[sorted_index[i]].turn = "FS";
			}
			else if(module[sorted_index[i]].turn=="FS"){
				module[sorted_index[i]].turn = "FN";
			}
			else if(module[sorted_index[i]].turn=="FN"){
				module[sorted_index[i]].turn = "S";
			}
			else if(module[sorted_index[i]].turn=="S"){
				module[sorted_index[i]].turn = "FN";
			}
		}
		else if (cent_to_x > 0 && cent_to_y < 0)
		{
			num_moved++;
			if(module[sorted_index[i]].turn=="N"){
				module[sorted_index[i]].turn = "FN";
			}
			else if(module[sorted_index[i]].turn=="FS"){
				module[sorted_index[i]].turn = "S";
			}
			else if(module[sorted_index[i]].turn=="FN"){
				module[sorted_index[i]].turn = "N";
			}
			else if(module[sorted_index[i]].turn=="S"){
				module[sorted_index[i]].turn = "FS";
			}
		}
		else{
			if(module[sorted_index[i]].turn=="N"){
				module[sorted_index[i]].turn = "N";
			}
			else if(module[sorted_index[i]].turn=="FS"){
				module[sorted_index[i]].turn = "FS";
			}
			else if(module[sorted_index[i]].turn=="FN"){
				module[sorted_index[i]].turn = "FN";
			}
			else if(module[sorted_index[i]].turn=="S"){
				module[sorted_index[i]].turn = "S";
			}
		}
		//if(sorted_index[i]==30){
			
				cout<<"\n";
				macro_choose[sorted_index[i]]=true;
				//cout<<"macro"<<sorted_index[i]<<"  \ncenter:("<<thisMacro->CenterX<<","<<thisMacro->CenterY<<")\nW/H:"<<thisMacro->modW<<","<<thisMacro->modH<<"\n\n";
				for(int j = 0; j < thisMacro->macro_wires.size(); j++){
					WIRE* thisWire = thisMacro->macro_wires[j];
					//cout<<" net"<<j<<":"<<thisMacro->macro_pins[j]->x_shift<<","<<thisMacro->macro_pins[j]->y_shift<<"\n";
					for(int k=0;k<thisWire->wire_macroID.size();k++){
						macro_choose[thisWire->wire_macroID[k]]=true;
						//cout<<"   macro"<<thisWire->wire_macroID[k]<<":(c)"<<nt.mods[thisWire->wire_macroID[k]].CenterX<<" "<<nt.mods[thisWire->wire_macroID[k]].CenterY<<" (W/H)"<<nt.mods[thisWire->wire_macroID[k]].modW<<" "<<nt.mods[thisWire->wire_macroID[k]].modH<<"\n";
					}
					for(int k=0;k<thisWire->wire_stdID.size();k++){
						std_choose[thisWire->wire_stdID[k]]=true;
						//cout<<"   stand"<<thisWire->wire_stdID[k]<<":(c)"<<nt.stds[thisWire->wire_stdID[k]].CenterX<<" "<<nt.stds[thisWire->wire_stdID[k]].CenterY<<" (W/H)"<<nt.stds[thisWire->wire_stdID[k]].modW<<" "<<nt.stds[thisWire->wire_stdID[k]].modH<<"\n";
					}
				}
				
				
				
				cout<<"\n";
				string filename="initial";
				int a=sorted_index[i];
				stringstream ss;
				ss<<a;
				filename+=ss.str();
				filename+=".m";
				Plot2(filename,macro_choose,std_choose);
				filename="initial";
				a=sorted_index[i];
				filename+=ss.str();
				filename+="_pin";
				filename+=".m";	
				//plotpin(thisMacro,filename);
			
		//}
		
		//}
		//}
	}*/
	//____________________________________________________________________________________________
	//--------------------------------------------------------------------------------------------
	
	int sdfsdfd=0;
	if(mod_f.empty()==false){
	for (int i = 0; i < module.size(); i++) {
	
	 if(i!=mod_f[sdfsdfd].num ){
		ofs << "   - " << module[i].module_name << " " << module[i].module_type << "\n";
		ofs << "      + " << module[i].mobility << " ( " << module[i].x << " " << module[i].y << " ) " << module[i].turn << " ;\n";
		
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
		ofs << "      + " << module[i].mobility << " ( " << module[i].x << " " << module[i].y << " ) " << module[i].turn << " ;\n";
		
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
	/*string filename = "initial.m";
	Plot(filename);
	filename="pin.m";
	plotpin_all(filename);
	filename="pin_after.m";
	plotpin_all_after(module,filename);*/
	
		
	//cout<<"\nnum_moved="<<num_moved;
	
	
	
	//cout<<"\n\n\n\n";
	double hpwl=0;
	
	std::map<std::string, int>::iterator it = wire_NameToID.begin();
	while(it != wire_NameToID.end() ){
		bool first=true;
		//nt.wires
		double lx=0;
		double ly=0;
		double rx=0;
		double ry=0;
		for(int i=0;i<nt.wires[it->second].wire_macroID.size();i++){
			if(first==true){
				lx=nt.mods[nt.wires[it->second].wire_macroID[i]].CenterX;
				rx=nt.mods[nt.wires[it->second].wire_macroID[i]].CenterX;
				ly=nt.mods[nt.wires[it->second].wire_macroID[i]].CenterY;
				ry=nt.mods[nt.wires[it->second].wire_macroID[i]].CenterY;
				
				first=false;
			}
			else{
				if(nt.mods[nt.wires[it->second].wire_macroID[i]].CenterX<lx){
					lx=nt.mods[nt.wires[it->second].wire_macroID[i]].CenterX;
				}
				if(nt.mods[nt.wires[it->second].wire_macroID[i]].CenterX>rx){
					rx=nt.mods[nt.wires[it->second].wire_macroID[i]].CenterX;
				}
				if(nt.mods[nt.wires[it->second].wire_macroID[i]].CenterY<ly){
					ly=nt.mods[nt.wires[it->second].wire_macroID[i]].CenterY;
				}
				if(nt.mods[nt.wires[it->second].wire_macroID[i]].CenterY>ry){
					ry=nt.mods[nt.wires[it->second].wire_macroID[i]].CenterY;
				}
			}
		}
		for(int i=0;i<nt.wires[it->second].wire_stdID.size();i++){
			if(first==true){
				lx=nt.stds[nt.wires[it->second].wire_stdID[i]].CenterX;
				rx=nt.stds[nt.wires[it->second].wire_stdID[i]].CenterX;
				ly=nt.stds[nt.wires[it->second].wire_stdID[i]].CenterY;
				ry=nt.stds[nt.wires[it->second].wire_stdID[i]].CenterY;
				
				first=false;
			}
			else{
				if(nt.stds[nt.wires[it->second].wire_stdID[i]].CenterX<lx){
					lx=nt.stds[nt.wires[it->second].wire_stdID[i]].CenterX;
				}
				if(nt.stds[nt.wires[it->second].wire_stdID[i]].CenterX>rx){
					rx=nt.stds[nt.wires[it->second].wire_stdID[i]].CenterX;
				}
				if(nt.stds[nt.wires[it->second].wire_stdID[i]].CenterY<ly){
					ly=nt.stds[nt.wires[it->second].wire_stdID[i]].CenterY;
				}
				if(nt.stds[nt.wires[it->second].wire_stdID[i]].CenterY>ry){
					ry=nt.stds[nt.wires[it->second].wire_stdID[i]].CenterY;
				}
			}
		}
		hpwl+=((rx-lx)+(ry-ly));
		//for(int i=0;i<nt.wires[it->second].wire_padID.size()){}
		++it;
	}
	cout<<hpwl<<"\n";
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
	    write_in+=to_string(module[i].x);
	    write_in+=" ";
	    write_in+=to_string(module[i].y);
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
void plotpin(MODULE* m,string filename_pin){
	ofstream fout(filename_pin.c_str());
	fout << "figure;\nclear;\nclf;\naxis equal;\nhold on;\ngrid on;\n" << endl;

	//chip boundary
	fout << "block_x=[" << nt.ChipBoundary_llx << " " << nt.ChipBoundary_llx << " " << nt.ChipBoundary_urx << " " << nt.ChipBoundary_urx << " " << nt.ChipBoundary_llx << " ];" << endl;
	fout << "block_y=[" << nt.ChipBoundary_lly << " " << nt.ChipBoundary_ury << " " << nt.ChipBoundary_ury << " " << nt.ChipBoundary_lly << " " << nt.ChipBoundary_lly << " ];" << endl;
	fout << "fill(block_x, block_y, 'w');" << endl;
	
	
		double llx = m->LeftX;
		double lly = m->LeftY;
		double urx = m->LeftX + m->modW;
		double ury = m->LeftY + m->modH;

		fout << "block_x=[" << llx << " " << llx << " " << urx << " " << urx << " " << llx << " ];" << endl;
		fout << "block_y=[" << lly << " " << ury << " " << ury << " " << lly << " " << lly << " ];" << endl;
		//fout << "text(" << (llx + urx) / 2.0 << ", " << (lly + ury) / 2.0 << ", '" << i << "');" << endl << endl;
		fout << "fill(block_x,block_y,'g','facealpha',0.5);" << endl;
	
	
	for(int i=0;i<m->macro_pins.size();i++){
		llx = m->LeftX+m->macro_pins[i]->x_shift;
		lly = m->LeftY+m->macro_pins[i]->y_shift;
		urx = m->LeftX+m->macro_pins[i]->x_shift+100;
		ury = m->LeftY+m->macro_pins[i]->y_shift+100;
		fout << "block_x=[" << llx << " " << llx << " " << urx << " " << urx << " " << llx << " ];" << endl;
		fout << "block_y=[" << lly << " " << ury << " " << ury << " " << lly << " " << lly << " ];" << endl;
		//fout << "text(" << (llx + urx) / 2.0 << ", " << (lly + ury) / 2.0 << ", '" << i << "');" << endl << endl;
		fout << "fill(block_x,block_y,'g','facealpha',0.5);" << endl;
	}
	fout.close();
}

void plotpin_all(string filename){
	ofstream fout(filename.c_str());
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
		for(int j=0;j<nt.mods[i].macro_pins.size();j++){
			llx = nt.mods[i].LeftX+nt.mods[i].macro_pins[j]->x_shift;
			lly = nt.mods[i].LeftY+nt.mods[i].macro_pins[j]->y_shift;
			urx = nt.mods[i].LeftX+nt.mods[i].macro_pins[j]->x_shift+100;
			ury = nt.mods[i].LeftY+nt.mods[i].macro_pins[j]->y_shift+100;
			fout << "block_x=[" << llx << " " << llx << " " << urx << " " << urx << " " << llx << " ];" << endl;
			fout << "block_y=[" << lly << " " << ury << " " << ury << " " << lly << " " << lly << " ];" << endl;
			//fout << "text(" << (llx + urx) / 2.0 << ", " << (lly + ury) / 2.0 << ", '" << i << "');" << endl << endl;
			fout << "fill(block_x,block_y,'g','facealpha',0.5);" << endl;
		}
	}
	fout.close();
}
void plotpin_all_after(vector<MOD> m, string filename){
	ofstream fout(filename.c_str());
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
		for(int j=0;j<nt.mods[i].macro_pins.size();j++){
			llx = nt.mods[i].LeftX+nt.mods[i].macro_pins[j]->x_shift;
			lly = nt.mods[i].LeftY+nt.mods[i].macro_pins[j]->y_shift;
			
			if(m[i].turn=="S"){
				llx=(2*nt.mods[i].CenterX)-nt.mods[i].LeftX-nt.mods[i].macro_pins[j]->x_shift;
				lly=(2*nt.mods[i].CenterY)-nt.mods[i].LeftY-nt.mods[i].macro_pins[j]->y_shift;
			}
			else if(m[i].turn=="FS"){
				llx=(2*nt.mods[i].CenterX)-nt.mods[i].LeftX-nt.mods[i].macro_pins[j]->x_shift;
			}
			else if(m[i].turn=="FN"){
				lly=(2*nt.mods[i].CenterY)-nt.mods[i].LeftY-nt.mods[i].macro_pins[j]->y_shift;
			}
			urx = llx+100;
			ury = lly+100;
			fout << "block_x=[" << llx << " " << llx << " " << urx << " " << urx << " " << llx << " ];" << endl;
			fout << "block_y=[" << lly << " " << ury << " " << ury << " " << lly << " " << lly << " ];" << endl;
			//fout << "text(" << (llx + urx) / 2.0 << ", " << (lly + ury) / 2.0 << ", '" << i << "');" << endl << endl;
			fout << "fill(block_x,block_y,'g','facealpha',0.5);" << endl;
		}
	}
	fout.close();
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
void Plot2(string fileName,vector<bool> macro_choose,vector<bool> std_choose){
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
		if(macro_choose[i]==true){
		double llx = nt.mods[i].LeftX;
		double lly = nt.mods[i].LeftY;
		double urx = nt.mods[i].LeftX + nt.mods[i].modW;
		double ury = nt.mods[i].LeftY + nt.mods[i].modH;

		fout << "block_x=[" << llx << " " << llx << " " << urx << " " << urx << " " << llx << " ];" << endl;
		fout << "block_y=[" << lly << " " << ury << " " << ury << " " << lly << " " << lly << " ];" << endl;
		fout << "text(" << (llx + urx) / 2.0 << ", " << (lly + ury) / 2.0 << ", '" << i << "');" << endl << endl;
		fout << "fill(block_x,block_y,'g','facealpha',0.5);" << endl;
		}
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
	for (unsigned int i = 0; i < nt.stds.size(); i++)
	{
		if(std_choose[i]==true){
		double llx = nt.stds[i].LeftX;
		double lly = nt.stds[i].LeftY;
		double urx = nt.stds[i].LeftX + nt.stds[i].modW;
		double ury = nt.stds[i].LeftY + nt.stds[i].modH;

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
	REG wire_region = {0, 0, 0, 0};
	for (int i = 0; i < thisWire->wire_macroID.size(); i++)
	{
		MODULE* thisMacro = &nt.mods[thisWire->wire_macroID[i]];
		if (i == 0)
		{
			wire_region.left_x = thisMacro->CenterX;
			wire_region.right_x = thisMacro->CenterX;
			wire_region.bottom_y = thisMacro->CenterY;
			wire_region.up_y = thisMacro->CenterY;
		}
		else
		{
			if (thisMacro->CenterX < wire_region.left_x)
				wire_region.left_x = thisMacro->CenterX;
			if (thisMacro->CenterX > wire_region.right_x)
				wire_region.right_x = thisMacro->CenterX;
			if (thisMacro->CenterX < wire_region.bottom_y)
				wire_region.bottom_y = thisMacro->CenterY;
			if (thisMacro->CenterX > wire_region.up_y)
				wire_region.up_y = thisMacro->CenterY;
		}
	}
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

void Macro_Find_cent(MODULE* thisMacro, double& centx, double& centy){
	centx = 0;
	centy = 0;
	int num_cells = 0;
	for (unsigned int i = 0; i<thisMacro->macro_wires.size(); i++)
	{
		WIRE* thisNet = thisMacro->macro_wires[i];
		for (unsigned int i = 0; i<thisNet->wire_stdID.size(); i++){
			MODULE* thisCell = &nt.stds[thisNet->wire_stdID[i]];
			centx += thisCell->CenterX / 10000;
			centy += thisCell->CenterY / 10000;
			num_cells++;
		}
	}
	centx = centx*(10000 / num_cells);
	centy = centy*(10000 / num_cells);
}
void Macro_Find_gravity(WIRE* thisWire,double& gravityx,double& gravityy){
	gravityx=0;
	gravityy=0;
	double area_sum=0;
	for(unsigned int i=0;i<thisWire->wire_stdID.size();i++){
		MODULE* thiscell=&nt.stds[thisWire->wire_stdID[i]];
		//cout<<thiscell->modW<<" "<<thiscell->modH<<" "<<thiscell->CenterX<<" "<<thiscell->CenterY<<"\n";
		area_sum=area_sum+((((double)thiscell->modW)*((double)thiscell->modH))/10000);
		
		gravityx=gravityx+(((double)thiscell->CenterX/10000)*(((double)thiscell->modW)*((double)thiscell->modH)));
		gravityy=gravityy+(((double)thiscell->CenterY/10000)*(((double)thiscell->modW)*((double)thiscell->modH)));
		//cout<<gravityx<<" "<<gravityy<<"\n";
		
	}
	for(unsigned int i=0;i<thisWire->wire_macroID.size();i++){
		MODULE* thiscell=&nt.mods[thisWire->wire_macroID[i]];
		//cout<<thiscell->modW<<" "<<thiscell->modH<<" "<<thiscell->CenterX<<" "<<thiscell->CenterY<<"\n";
		area_sum=area_sum+((((double)thiscell->modW)*((double)thiscell->modH))/10000);
		
		gravityx=gravityx+(((double)thiscell->CenterX/10000)*(((double)thiscell->modW)*((double)thiscell->modH)));
		gravityy=gravityy+(((double)thiscell->CenterY/10000)*(((double)thiscell->modW)*((double)thiscell->modH)));
		//cout<<gravityx<<" "<<gravityy<<"\n";
		
	}
	//cout<<"\n";
	gravityx=gravityx/area_sum;
	gravityy=gravityy/area_sum;
	//cout<<gravityx<<" "<<gravityy<<"\n\n";
}
bool far_divide_near_is_high_x(MODULE* thismacro){
	//nt.ChipBoundary_llx << " " << nt.ChipBoundary_llx << " " << nt.ChipBoundary_urx 
	double Chip_center_dis=(-nt.ChipBoundary_llx+nt.ChipBoundary_urx)/2;
	double far=0;
	double near=0;
	for(unsigned int i = 0; i<thismacro->macro_wires.size(); i++){
		WIRE* thisNet = thismacro->macro_wires[i];
		for (unsigned int i = 0; i<thisNet->wire_stdID.size(); i++){
			MODULE* thisCell = &nt.stds[thisNet->wire_stdID[i]];
			if(abs(thisCell->CenterX-thismacro->CenterX)>Chip_center_dis){
				far=far+1;
			}
			else{
				near=near+1;
			}
		}
	}
	double ratio=(far/near);
	if(ratio>=5){
		return true;
	}
	else{
		return false;
	}
}
bool far_divide_near_is_high_y(MODULE* thismacro){
	//nt.ChipBoundary_llx << " " << nt.ChipBoundary_llx << " " << nt.ChipBoundary_urx 
	double Chip_center_dis=(-nt.ChipBoundary_lly+nt.ChipBoundary_ury)/2;
	double far=0;
	double near=0;
	for(unsigned int i = 0; i<thismacro->macro_wires.size(); i++){
		WIRE* thisNet = thismacro->macro_wires[i];
		for (unsigned int i = 0; i<thisNet->wire_stdID.size(); i++){
			MODULE* thisCell = &nt.stds[thisNet->wire_stdID[i]];
			if(abs(thisCell->CenterY-thismacro->CenterY)>Chip_center_dis){
				far=far+1;
			}
			else{
				near=near+1;
			}
		}
	}
	double ratio=(far/near);
	if(ratio>=5){
		return true;
	}
	else{
		return false;
	}
}


void get_degree(double & cx,double & cy,double & std_num,vector<bool>& chosen_std,vector<bool>& chosen_mod,vector<bool>& chosen_wires,vector<WIRE*>& wv,vector<int>& layer,bool & sign,double & amount){
	
	amount++;
	if(amount<100){	
		
	if(layer[0]>0){
		map<string,int>::iterator itt;
		WIRE* thiswire=new WIRE;
		thiswire=wv[0];
		//cout<<thiswire->wireName;
		itt=wire_NameToID.find(thiswire->wireName);
		chosen_wires[itt->second]=true;
		wv.erase(wv.begin());
		int t_layer=layer[0];
		layer.erase(layer.begin());
		
		for(int i=0;i<thiswire->wire_macroID.size();i++){
			if(chosen_mod[thiswire->wire_macroID[i]]==false){
				chosen_mod[thiswire->wire_macroID[i]]=true;
				std_num=std_num+t_layer;
				cx+=(nt.mods[thiswire->wire_macroID[i]].CenterX);
				cy+=(nt.mods[thiswire->wire_macroID[i]].CenterY);
				
			}
		}
		for(int i=0;i<thiswire->wire_stdID.size();i++){
			if(chosen_std[thiswire->wire_stdID[i]]==false){
				chosen_std[thiswire->wire_stdID[i]]=true;
				std_num=std_num+t_layer;
				cx+=(nt.stds[thiswire->wire_stdID[i]].CenterX);
				cy+=(nt.stds[thiswire->wire_stdID[i]].CenterY);
				for(int j=0;j<nt.stds[thiswire->wire_stdID[i]].macro_wires.size();j++){
					map<string,int>::iterator ittt;
					ittt=wire_NameToID.find(nt.stds[thiswire->wire_stdID[i]].macro_wires[j]->wireName);
					if(chosen_wires[ittt->second]==false){
						chosen_wires[ittt->second]=true;
						wv.push_back(nt.stds[thiswire->wire_stdID[i]].macro_wires[j]);
						layer.push_back(t_layer-1);
					}
				}
			}
		}
		if(wv.size()>0){
		get_degree(cx,cy,std_num,chosen_std,chosen_mod,chosen_wires,wv,layer,sign,amount);
		}
		
	}
	}
	else{
		sign=true;
	}
	
}
void total_degree(MODULE* firstmod,WIRE* firstwire,double & cx,double & cy,double & std_num){
	
	vector<bool> chosen_mod;
	for(int i=0;i<nt.mods.size();i++){
		chosen_mod.push_back(false);
	}
	vector<bool> chosen_std;
	for(int i=0;i<nt.stds.size();i++){
		chosen_std.push_back(false);
	}
	vector<bool> chosen_wires;
	for(int i=0;i<nt.wires.size();i++){
		chosen_wires.push_back(false);
	}
	vector<WIRE*> wv;
	vector<int> layer;
	layer.push_back(2);
	wv.push_back(firstwire);
	chosen_mod[firstmod->idMod]=true;
	double amount=0;
	bool sign=true;
	while(sign==true){
	sign=false;
	amount=0;
	get_degree(cx,cy,std_num,chosen_std,chosen_mod,chosen_wires,wv,layer,sign,amount);
	}
		
	
}
void find_degree(MODULE* firstmod,WIRE* firstwire,double & cx,double & cy,double & std_num,vector<bool> chosen_mod,vector<bool> chosen_std,vector<bool> chosen_wires){
	
	vector<WIRE*> wv;
	vector<int> layer;
	layer.push_back(10);
	
	wv.push_back(firstwire);
	chosen_mod[firstmod->idMod]=true;
	while(layer[0]>0){
		WIRE* thiswire=new WIRE;
		thiswire=wv[0];
		map<string,int>::iterator itt;
		itt=wire_NameToID.find(thiswire->wireName);
		chosen_wires[itt->second]=true;
		int t_layer=layer[0];
		wv.erase(wv.begin());
		layer.erase(layer.begin());
		for(int i=0;i<thiswire->wire_macroID.size();i++){
			if(chosen_mod[thiswire->wire_macroID[i]]==false){
				chosen_mod[thiswire->wire_macroID[i]]=true;
				std_num=std_num+t_layer;
				cx+=(nt.mods[thiswire->wire_macroID[i]].CenterX);
				cy+=(nt.mods[thiswire->wire_macroID[i]].CenterY);
				
			}
		}
		for(int i=0;i<thiswire->wire_stdID.size();i++){
			if(chosen_std[thiswire->wire_stdID[i]]==false){
				chosen_std[thiswire->wire_stdID[i]]=true;
				std_num=std_num+t_layer;
				cx+=(nt.stds[thiswire->wire_stdID[i]].CenterX);
				cy+=(nt.stds[thiswire->wire_stdID[i]].CenterY);
				for(int j=0;j<nt.stds[thiswire->wire_stdID[i]].macro_wires.size();j++){
					map<string,int>::iterator ittt;
					ittt=wire_NameToID.find(nt.stds[thiswire->wire_stdID[i]].macro_wires[j]->wireName);
					if(chosen_wires[ittt->second]==false){
						chosen_wires[ittt->second]=true;
						wv.push_back(nt.stds[thiswire->wire_stdID[i]].macro_wires[j]);
						layer.push_back(t_layer-1);
					}
				}
			}
		}
		if(wv.size()<=0){
			break;
		}
	}
}
