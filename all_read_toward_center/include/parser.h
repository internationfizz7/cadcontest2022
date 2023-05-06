//edited july 21th by hsieh ^U^
//WIRE added
//REG added
#pragma once
#include <map>
#include <string.h>
#include <iostream>
#include <vector>
#include <fstream>
#include "defrReader.hpp"
#include "lefrReader.hpp"
#include <math.h>
#include <stdio.h>
#include <cstdlib>
using namespace std;
class MODULE;

class WIRE  // edited0_713 ^U^
{
public:
	WIRE(){};
	WIRE(const WIRE& w){
		this->wireName = w.wireName;
	};
	~WIRE(){};
	string wireName;
	vector<int> wire_macroID;
	vector<int> wire_stdID;
	vector<int> wire_padID;
	//int degree;
	//the macro this wire connected ^U^
};
class PIN
{
public:
	PIN() {};
	~PIN() {};
	int index, offsetX, offsetY,idPin;
	//const char* pinName;
	char pinName[500];
};
class BLOCKAGE
{
public:
	BLOCKAGE() {};
	~BLOCKAGE() {};
	int LeftX, LeftY, RightX, RightY;
	int blockageL, idBlockage;
};

class MACROPORT
{
public:
	MACROPORT();
	~MACROPORT();
	double llx_shift, lly_shift, urx_shift, ury_shift;
};

class MACROPIN
{
public:
	MACROPIN();
	~MACROPIN();
	float x_shift, y_shift;
	string macropinName, macropinType, macropinIo;
	vector<MACROPORT*> macroports;
};

class MACRO
{
public:
	MACRO();
	~MACRO();
	float macroW, macroH;
	const char* macroName;
	const char* macroType;
	vector<MACROPIN*> macropins;
	vector<BLOCKAGE*> macro_obs;
	vector<WIRE*> macro_wires; //the wires macro connected to ^U^
};


class PAD
{
public:
	char padName[500];
	char typeName[500];
	unsigned int idPad;
	int padL, padOri, padW, padH, LeftX, LeftY, flagCover;	/// padOri: 0(N)/1(W)/2(S)/3(E)/4(FN)/5(FW)/6(FS)/7(FE)
	vector<BLOCKAGE> blockage;
};

class MODULE
{
public:
	//MODULE():nNet(0),pNet(NULL) {};
	MODULE();
	~MODULE() {};
	char modName[500], typeName[500];
	char type;
	int modW, modH, modL;
	int flagStd;
	int GLeftX, GLeftY, LeftX, LeftY;
	int GCenterX, GCenterY, CenterX, CenterY;
	int modOri;
	int nNet;
	float modArea;
	unsigned int* pNet;
	//char* typeName;
	unsigned int idMod;
	vector<WIRE*> macro_wires; //the wires macro(std cell) connected to ^U^
	vector<MACROPIN*>macro_pins; //the wires macro connected to ^U^
};


class NET
{
public:
	NET() :nPin(0) {};
	~NET() {};
	char netName[100];
	int idNet;//nets.size
	int head;//pins.size
	int nPin;
}; 

class ROUTING_LAYER
{
public:
	ROUTING_LAYER(){};
	~ROUTING_LAYER(){};
	const char* name;
	const char* direction;
	double min_width, min_spacing;
};
class NETLIST
{
public:
	NETLIST() :totalModArea(0), totalStdArea(0), totalPadArea(0), nStd(0), nPad(0),
		nHard(0), nMod(0), nPin(0), nBlockage(0) {};
	~NETLIST() {};
	int ChipBoundary_llx, ChipBoundary_lly, ChipBoundary_urx, ChipBoundary_ury; // chip boundary lower lef xy, upper right xy
	int ChipWidth, ChipHeight;
	float ChipArea;
	float aR;
	float totalModArea, totalStdArea, totalPadArea;
	unsigned int nStd, nPad, nHard, nMod, nPin, nNet, nBlockage;
	vector<MODULE> pads;
	vector<MODULE> mods;
	vector<MODULE> stds;
	vector<PIN> pins;
	vector<NET> nets;
	vector<BLOCKAGE> blockages;
	vector<WIRE> wires;//^U^
	vector<MACROPIN> macropins;
};
struct REG{ // edited0_721 ^U^
	int left_x;
	int right_x;
	int bottom_y;
	int up_y;
};
//*----------------------------LEF--------------------------------------//
void ReadLefFile(char* filename, NETLIST& tmpnt);
int LEF_MacroCB(lefrCallbackType_e typ, lefiMacro* macro, lefiUserData ud);
int LEF_PinCB(lefrCallbackType_e typ, lefiPin* pin, lefiUserData ud);
int LEF_ObsCB(lefrCallbackType_e typ, lefiObstruction* obs, lefiUserData ud);
int LEF_LayerCB(lefrCallbackType_e typ,lefiLayer* layer,lefiUserData ud);
//*----------------------------DEF--------------------------------------//
void ReadDefFile(char* filename, NETLIST& tmpnt);
int DEF_UnitCB(defrCallbackType_e typ, double d, defiUserData ud);
int DEF_DieAreaCB(defrCallbackType_e typ, defiBox* diebox, defiUserData ud);
int DEF_ComponentCB(defrCallbackType_e typ, defiComponent* co, defiUserData ud);
int DEF_NetCB(defrCallbackType_e typ, defiNet* net, defiUserData ud);
int DEF_BlockageCB(defrCallbackType_e typ, defiBlockage* blockage, defiUserData ud);
int DEF_PinCB(defrCallbackType_e typ,defiPin* pin,defiUserData  ud);
//--------------------------------verilog------------------------------//
void ReadVerilogFile(char* filename, NETLIST& tmpnt);//^U^
//--------------------------------mlist--------------------------------//
void ReadMlistFile(char* filename, NETLIST& tmpnt);
int MLIST_ComponentCB(defrCallbackType_e typ, defiComponent* co, defiUserData ud);
//---------------------------------------------------------------------//
extern NETLIST nt;
extern vector<ROUTING_LAYER*> routing_layer;
extern map<string, int> mod_NameToID;	        ///< map for modName to idMod
extern map<string, int> pad_NameToID;
extern map<string, int> wire_NameToID;
