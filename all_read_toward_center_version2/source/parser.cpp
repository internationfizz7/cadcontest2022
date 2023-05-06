//edited Sep 2nd by hsieh ^U^
#include "parser.h"

#define AMP_PARA 2000
double dbuPerMicron;                     ///< database units per micron (usually in def)
map<string, int> mod_NameToID;	        ///< map for modName to idMod
map<string, int> pad_NameToID;	        ///< map for padName to idPad
map<string, MACRO*> comp_NameToMacro;    ///< find the MACRO of a COMPONENT(MODULE/PAD)
map<string, MACRO*> macro_NameToPtr;     ///< store MACRO info (in lef)
map<string, int> wire_NameToID;       ///< store Wire
vector<MACROPIN*> pins_tmp;///< temporarily store PINs info of CURRENT MACRO in lef and delete before parsering next MACRO
vector<BLOCKAGE*> obs_tmp;
NETLIST nt;//Collect all components
vector<ROUTING_LAYER*> routing_layer;

MACROPORT::MACROPORT() : llx_shift(0.0), lly_shift(0.0), urx_shift(0.0), ury_shift(0.0)
{ }

MACROPORT::~MACROPORT()
{ }

MACROPIN::MACROPIN() : x_shift(0.0), y_shift(0.0)
{ }

MACROPIN::~MACROPIN()
{ }

MACRO::MACRO() : macroW(0), macroH(0)
{ }

MACRO::~MACRO()
{ }

MODULE::MODULE()
{
	nNet=0;
	pNet=new unsigned int[1];
}
void ReadLefFile(char* filename, NETLIST& tmpnt)
{
	//cout << "*Now Parse LEF File" << endl;

	char* userData = 0;//NULL

	/* Initial Call Routine */
	lefrInit();

	lefrSetLayerCbk(LEF_LayerCB);
	lefrSetMacroCbk(LEF_MacroCB);
	lefrSetObstructionCbk(LEF_ObsCB);
	lefrSetPinCbk(LEF_PinCB);

	ifstream fin;
	char lefList[100] = "\0";

	strcpy(lefList, filename);
	strcat(lefList, ".list");
	fin.open(lefList);

	if (!fin)    ///< can't find {filename}.list file, then find {filename}.lef file
	{
		lefrReset();

		FILE* f;
		char lefFile[100] = "\0";
		strcpy(lefFile, filename);
		strcat(lefFile, ".lef");

		if ((f = fopen(lefFile, "r")) == 0) // r: read text file
		{
			cerr << "!error  : can't open file " << lefList << " and " << lefFile << endl;
			exit(EXIT_FAILURE);
		}

		int res = lefrRead(f, lefFile, (void*)userData); // res == 0: NO errors
		if (res)
		{
			cerr << "!error  : reader returns bad status in file " << lefFile << endl;
			exit(EXIT_FAILURE);
		}

		(void)lefrReleaseNResetMemory();
		fclose(f);
	}
	else        /// read {filename}.lef file in {filename}.list file
	{
		string lefFile;
		while (fin >> lefFile)
		{
			lefrReset();

			string benchPath = filename;
			benchPath = benchPath.substr(0, benchPath.find_last_of("/") + 1);
			lefFile = benchPath + lefFile;

			FILE* f;
			if ((f = fopen(lefFile.c_str(), "r")) == 0) // r: read text file
			{
				cerr << "!error  : can't open file " << lefFile << endl;
				continue;
			}

			int res = lefrRead(f, lefFile.c_str(), (void*)userData); // res == 0: NO errors
			if (res)
			{
				cerr << "!error  : reader returns bad status in file " << lefFile << endl;
				continue;
			}

			(void)lefrReleaseNResetMemory();
			fclose(f);
		}
	}

	fin.close();
}

int LEF_MacroCB(lefrCallbackType_e typ, lefiMacro* macro, lefiUserData ud)
{
	MACRO* thisMacro = new MACRO;
	thisMacro->macroName = macro->lefiMacro::name();
	thisMacro->macroW = macro->lefiMacro::sizeX();
	thisMacro->macroH = macro->lefiMacro::sizeY();


	if (macro->lefiMacro::hasClass())
	{
		string thisMacro_type = macro->lefiMacro::macroClass();
		if (thisMacro_type.substr(0, 5) == "COVER")       ///< fixed macro that can be overlaped but will affect the pin access
			thisMacro->macroType = "COVER";
		else if (thisMacro_type.substr(0, 5) == "BLOCK")  ///< macro
			thisMacro->macroType = "BLOCK";
		else if (thisMacro_type.substr(0, 3) == "PAD")    ///< i/o pad
			thisMacro->macroType = "PAD";
		else if (thisMacro_type.substr(0, 4) == "CORE")   ///< standard cell
			thisMacro->macroType = "CORE";
		//        else if(thisMacro_type.substr(0,4) == "RING")   ///< large macro that has an internal power mesh, and only exposes powerpin shapes that form a ring along the macro boundary
		//            thisMacro->macroType = "RING";
		//        else if(thisMacro_type.substr(0,6) == "ENDCAP") ///< macro(?) placed at the ends of core rows (to connect with power wiring)
		//            thisMacro->macroType = "ENDCAP";
		else
		{
			cout << "!warning: other CLASS type: " << thisMacro->macroName << " "
				<< thisMacro->macroW << " " << thisMacro->macroH << " " << thisMacro_type << endl;
		}
	}
	else ///< noClass => default: CORE (STD cell)
		thisMacro->macroType = "CORE";

	for (unsigned int i = 0; i < pins_tmp.size(); i++)
	{
		//        if((abs(pins_tmp[i]->x_shift) < thisMacro->w/2) && (abs(pins_tmp[i]->y_shift) < thisMacro->h/2)) ///< only remain the pins inside the macro region
		string str = pins_tmp[i]->macropinName;
		//cout<<pins_tmp[i]<<"=>";
		thisMacro->macropins.push_back(pins_tmp[i]);
		str.clear();
	}
	pins_tmp.clear();
	//cout<<"\n----------------------------\n";


	/*--------------------------------------------------------------------------------*/

	if (thisMacro->macroType != "CORE")
		for (unsigned i = 0; i < obs_tmp.size(); i++)
			thisMacro->macro_obs.push_back(obs_tmp[i]);

	obs_tmp.clear();

	/*--------------------------------------------------------------------------------*/

	macro_NameToPtr.insert(pair<string, MACRO*>(thisMacro->macroName, thisMacro));

	return 0;
}

int LEF_PinCB(lefrCallbackType_e typ, lefiPin* pin, lefiUserData ud)
{
	string thisPin_type = pin->lefiPin::use();

	//    if((thisPin_type != "POWER") && (thisPin_type != "GROUND") && (thisPin_type != "CLOCK")) ///< only remain the pins which type is not POWER and GROUND and CLOCK
	{

		MACROPIN* thisPin = new MACROPIN;
		string n= pin->lefiPin::name();
		thisPin->macropinName = n.c_str();
		//string str = thisPin->macropinName;
		//cout << str << endl;
		thisPin->macropinType = thisPin_type.c_str();
		thisPin->macropinIo = pin->lefiPin::direction();

		/* get PORTS' info */
		for (int i = 0; i < pin->lefiPin::numPorts(); i++)
		{
			lefiGeometries* geometry = pin->lefiPin::port(i);
			lefiGeomRect* rect;

			/* find the center of the RECTs (a port has many RECTs) */
			double llx_shift = 9999999.0, urx_shift = -9999999.0, lly_shift = 9999999.0, ury_shift = -9999999.0;
			for (int j = 0; j < geometry->lefiGeometries::numItems(); j++)
			{
				switch (geometry->lefiGeometries::itemType(j))
				{
					case lefiGeomUnknown: break;
					case lefiGeomLayerE: break;
					case lefiGeomLayerExceptPgNetE: break;
					case lefiGeomLayerMinSpacingE: break;
					case lefiGeomLayerRuleWidthE: break;
					case lefiGeomWidthE: break;
					case lefiGeomPathE: break;
					case lefiGeomPathIterE: break;
					case lefiGeomRectE:
								rect = geometry->lefiGeometries::getRect(j);
								if (rect->xl < llx_shift)
									llx_shift = rect->xl;
								if (rect->xh > urx_shift)
									urx_shift = rect->xh;
								if (rect->yl < lly_shift)
									lly_shift = rect->yl;
								if (rect->yh > ury_shift)
									ury_shift = rect->yh;
								break;
					case lefiGeomRectIterE: break;
					case lefiGeomPolygonE: break;
					case lefiGeomPolygonIterE: break;
					case lefiGeomViaE: break;
					case lefiGeomViaIterE: break;
					case lefiGeomClassE: break;
					case lefiGeomEnd: break;
				}
			}
			MACROPORT* thisPort = new MACROPORT;
			thisPort->llx_shift = llx_shift;
			thisPort->lly_shift = lly_shift;
			thisPort->urx_shift = urx_shift;
			thisPort->ury_shift = ury_shift;

			thisPin->macroports.push_back(thisPort);
		}

		/* find the center of the PORTs (a pin has many PORTs) */
		double llx_shift = 9999999.0, urx_shift = -9999999.0, lly_shift = 9999999.0, ury_shift = -9999999.0;
		for (unsigned int i = 0; i < thisPin->macroports.size(); i++)
		{
			if (thisPin->macroports[i]->llx_shift < llx_shift)
				llx_shift = thisPin->macroports[i]->llx_shift;
			if (thisPin->macroports[i]->urx_shift > urx_shift)
				urx_shift = thisPin->macroports[i]->urx_shift;
			if (thisPin->macroports[i]->lly_shift < lly_shift)
				lly_shift = thisPin->macroports[i]->lly_shift;
			if (thisPin->macroports[i]->ury_shift > ury_shift)
				ury_shift = thisPin->macroports[i]->ury_shift;
		}
		thisPin->x_shift = (llx_shift + urx_shift) / 2 *AMP_PARA; //< shift from (0,0)
		thisPin->y_shift = (lly_shift + ury_shift) / 2 *AMP_PARA; //< shift from (0,0)

		//string str = thisPin->macropinName;
		//cout << str << endl;
		//cout<<thisPin->x_shift<<", "<<thisPin->y_shift;
		nt.macropins.push_back(*thisPin);
		pins_tmp.push_back(thisPin);
		for (unsigned int i = 0; i < pins_tmp.size(); i++)
		{
			//        if((abs(pins_tmp[i]->x_shift) < thisMacro->w/2) && (abs(pins_tmp[i]->y_shift) < thisMacro->h/2)) ///< only remain the pins inside the macro region
		}

	}

	return 0;
}

/*--------------------------------------------------------------------------------*/

int LEF_ObsCB(lefrCallbackType_e typ, lefiObstruction* obs, lefiUserData ud)
{
	lefiGeometries* geometry = obs->geometries();
	lefiGeomRect* rect;
	string layer = "";

	for (int i = 0; i < geometry->lefiGeometries::numItems(); i++)
	{
		BLOCKAGE* blockage = new BLOCKAGE;

		switch (geometry->lefiGeometries::itemType(i)) {
			case lefiGeomUnknown: break;
			case lefiGeomLayerExceptPgNetE: break;
			case lefiGeomLayerMinSpacingE: break;
			case lefiGeomLayerRuleWidthE: break;
			case lefiGeomEnd: break;
			case lefiGeomClassE: break;
			case lefiGeomLayerE:
					     layer = geometry->lefiGeometries::getLayer(i);
					     break;
			case lefiGeomWidthE: break;
			case lefiGeomPathE: break;
			case lefiGeomPathIterE: break;
			case lefiGeomRectE:
						blockage->blockageL = 0;
						for (unsigned int j = 0; j < routing_layer.size(); j++)
							if (layer == routing_layer[j]->name)
								blockage->blockageL = j + 1;

						rect = geometry->lefiGeometries::getRect(i);
						blockage->LeftX = (int)(rect->xl * AMP_PARA);
						blockage->LeftY = (int)(rect->yl * AMP_PARA);
						blockage->RightX = (int)(rect->xh * AMP_PARA);
						blockage->RightY = (int)(rect->yh * AMP_PARA);

						obs_tmp.push_back(blockage);
						break;
			case lefiGeomRectIterE: break;
			case lefiGeomPolygonE: break;
			case lefiGeomPolygonIterE: break;
			case lefiGeomViaE: break;
			case lefiGeomViaIterE: break;
		}
	}

	return 0;
}

/*--------------------------------------------------------------------------------*/

void ReadDefFile(char* filename, NETLIST& tmpnt)
{
	//cout << "*Now Parse DEF File" << endl;

	char* userData = 0;

	/* Initial Call Routine */
	defrInit();

	defrSetUnitsCbk(DEF_UnitCB);
	defrSetDieAreaCbk(DEF_DieAreaCB);
	defrSetComponentCbk(DEF_ComponentCB);
	//defrSetPinCbk(DEF_PinCB);
	//defrSetNetCbk(DEF_NetCB);
	//defrSetBlockageCbk(DEF_BlockageCB);
	//defrSetGcellGridCbk(DEF_GCellGridCB);

	defrReset();

	FILE* f;
	char defFile[100] = "\0";
	strcpy(defFile, filename);
	strcat(defFile, ".def");
	if ((f = fopen(defFile, "r")) == 0) // r: read text file
	{
		cerr << "!error  : can't open file " << defFile << endl;
		exit(EXIT_FAILURE);
	}

	int res = defrRead(f, defFile, (void*)userData, 1); // res == 0: NO errors
	if (res)
	{
		cerr << "!error  : reader returns bad status in file " << defFile << endl;
		exit(EXIT_FAILURE);
	}

	(void)defrReleaseNResetMemory();
	fclose(f);
}

int DEF_UnitCB(defrCallbackType_e typ, double d, defiUserData ud)
{
	dbuPerMicron = d;

	return 0;
}

int DEF_DieAreaCB(defrCallbackType_e typ, defiBox* diebox, defiUserData ud)
{
	//    double llx = (double) diebox->xl() / dbuPerMicron;  //< let unit be Micron
	//    double urx = (double) diebox->xh() / dbuPerMicron;  //< let unit be Micron
	//    double lly = (double) diebox->yl() / dbuPerMicron;  //< let unit be Micron
	//    double ury = (double) diebox->yh() / dbuPerMicron;  //< let unit be Micron

	double llx = 999999999.0, urx = -999999999.0, lly = 999999999.0, ury = -999999999.0;

	struct defiPoints points = diebox->getPoint();
	for (int i = 0; i < points.numPoints; i++)
	{
		if (points.x[i] < llx)
			llx = points.x[i];
		if (points.x[i] > urx)
			urx = points.x[i];

		if (points.y[i] < lly)
			lly = points.y[i];
		if (points.y[i] > ury)
			ury = points.y[i];
	}

	nt.ChipBoundary_llx = llx; nt.ChipBoundary_lly = lly;
	nt.ChipBoundary_urx = urx; nt.ChipBoundary_ury = ury;

	llx = llx / (double)dbuPerMicron;  //< let unit be Micron
	urx = urx / (double)dbuPerMicron;  //< let unit be Micron
	lly = lly / (double)dbuPerMicron;  //< let unit be Micron
	ury = ury / (double)dbuPerMicron;  //< let unit be Micron

	nt.ChipWidth = (int)fabs((urx - llx) * AMP_PARA);
	nt.ChipHeight = (int)fabs((ury - lly) * AMP_PARA);
	nt.ChipArea = (float)(fabs(urx - llx) * fabs(ury - lly));      //< == ((nt.ChipWidth)*(nt.ChipHeight)) / ((AMP_PARA)*(AMP_PARA))
	nt.aR = nt.ChipWidth / (float)nt.ChipHeight;

	return 0;
}

int DEF_ComponentCB(defrCallbackType_e typ, defiComponent* co, defiUserData ud)
{
	if ((co->defiComponent::isFixed()) || (co->defiComponent::isCover())) ///< fixed or cover (PAD)
	{
		/*if (macro_NameToPtr.find(co->defiComponent::name()) != macro_NameToPtr.end()) // co->defiComponent::name(): MACRO name
		  {
		  MODULE* thisPad = new PAD;
		  strcpy(thisPad->padName, co->defiComponent::id()); // thisPad->padName = co->defiComponent::id()
		  thisPad->idPad = nt.pads.size();

		  MACRO* thisMacro = (macro_NameToPtr.find(co->defiComponent::name()))->second;
		  strcpy(thisPad->typeName, co->defiComponent::name()); // thisPad->typeName = co->defiComponent::name()
		  thisPad->modL = 0;                                    ///< for 2-D case: no layer
		//thisPad->padOri = co->defiComponent::placementOrient(); ///< 0(N)/1(W)/2(S)/3(E)/4(FN)/5(FW)/6(FS)/7(FE)

		//--------------------------------------------------------------------------------

		for (unsigned int i = 0; i < thisMacro->macro_obs.size(); i++)
		thisPad->blockage.push_back(*thisMacro->macro_obs[i]);

		//--------------------------------------------------------------------------------

		if ((thisPad->modOri % 2) == 0) /// N, S, FN, FS
		{
		thisPad->modW = (int)(thisMacro->macroW * AMP_PARA);
		thisPad->modH = (int)(thisMacro->macroH * AMP_PARA);
		}
		else /// W, E, FW, FE
		{
		thisPad->modW = (int)(thisMacro->macroH * AMP_PARA);
		thisPad->modH = (int)(thisMacro->macroW * AMP_PARA);
		}

		thisPad->LeftX = (int)((co->defiComponent::placementX() / (double)dbuPerMicron) * AMP_PARA);
		thisPad->LeftY = (int)((co->defiComponent::placementY() / (double)dbuPerMicron) * AMP_PARA);

		if (co->defiComponent::isCover())
		thisPad->flagCover = 1;
		else
		thisPad->flagCover = 0;

		nt.totalPadArea = nt.totalPadArea + (((float)thisPad->padW / AMP_PARA) * ((float)thisPad->padH / AMP_PARA));

		nt.nPad++;
		nt.pads.push_back(*thisPad);

		pad_NameToID.insert(pair<string, int>(thisPad->padName, thisPad->idPad));
		comp_NameToMacro.insert(pair<string, MACRO*>(thisPad->padName, thisMacro));

		delete thisPad;
		}
		else
		{
		cout << "!warning: can't find MACRO " << co->defiComponent::name()
		<< " for PAD (COMPONENT) " << co->defiComponent::id() << endl;
		}*/
	}
	else ///< unfixed, movable (MODULE)
	{
		if (macro_NameToPtr.find(co->defiComponent::name()) != macro_NameToPtr.end()) // co->defiComponent::name(): MACRO name
		{
			MODULE* thisMod = new MODULE;
			strcpy(thisMod->modName, co->defiComponent::id()); // thisMod->modName = co->defiComponent::id()


			MACRO* thisMacro = macro_NameToPtr.find(co->defiComponent::name())->second;
			strcpy(thisMod->typeName, co->defiComponent::name()); // thisMod->typeName = co->defiComponent::name()
			thisMod->type = '0';      ///< hard module
			thisMod->modW = (int)(thisMacro->macroW * AMP_PARA);
			thisMod->modH = (int)(thisMacro->macroH * AMP_PARA);
			thisMod->modL = 0;        ///< for 2-D case: no layer
			thisMod->modArea = ((float)thisMod->modW / AMP_PARA) * ((float)thisMod->modH / AMP_PARA);
			/*//add pins=============================================
			  thisMod->macro_pins.resize(thisMacro->macropins.size());
			  string str;
			  for (int i = 0; i < thisMacro->macropins.size(); i++)
			  {
			  thisMod->macro_pins[i] = thisMacro->macropins[i];	
			  }
			//======================================================*/
			if (thisMacro->macroType == "CORE")
			{
				thisMod->flagStd = 1;
			}
			else if (thisMacro->macroType == "BLOCK")
			{
				thisMod->flagStd = 0;
			}
			else
				cout << "!warning: other macroType " << thisMacro->macroType << " is in class MODULE" << endl;

			if (co->defiComponent::isPlaced())
			{
				thisMod->GLeftX = thisMod->LeftX = (int)((co->defiComponent::placementX() / (double)dbuPerMicron) * AMP_PARA);
				thisMod->GLeftY = thisMod->LeftY = (int)((co->defiComponent::placementY() / (double)dbuPerMicron) * AMP_PARA);
				thisMod->GCenterX = thisMod->CenterX = thisMod->GLeftX + (thisMod->modW / 2);
				thisMod->GCenterY = thisMod->CenterY = thisMod->GLeftY + (thisMod->modH / 2);
			}
			else if (co->defiComponent::isUnplaced())
			{
				thisMod->GLeftX = thisMod->LeftX = 0;
				thisMod->GLeftY = thisMod->LeftY = 0;
				thisMod->GCenterX = thisMod->CenterX = thisMod->GLeftX + (thisMod->modW / 2);
				thisMod->GCenterY = thisMod->CenterY = thisMod->GLeftY + (thisMod->modH / 2);
			}


			nt.totalModArea = nt.totalModArea + thisMod->modArea;//totalModeArea+=modArea
			if (thisMod->flagStd == 1)//std cells
			{
				thisMod->idMod = nt.nStd;
				nt.nStd++;
				nt.totalStdArea = nt.totalStdArea + thisMod->modArea;
				nt.stds.push_back(*thisMod);
				//cout << "CELL:(" << thisMod->idMod << ")" << thisMod->modName << endl;//
			}
			nt.nHard++;


			mod_NameToID.insert(pair<string, int>(thisMod->modName, thisMod->idMod));
			comp_NameToMacro.insert(pair<string, MACRO*>(thisMod->modName, thisMacro));

			delete thisMod;
		}
		else
		{
			cout << "!warning: can't find MACRO " << co->defiComponent::name()
				<< " for MODULE (COMPONENT) " << co->defiComponent::id() << endl;
		}
	}

	return 0;
}

int DEF_PinCB(defrCallbackType_e typ, defiPin* pin, defiUserData ud)
{
	/*//cout<<"calling DEF_PinCB...."<<endl;
	  if ((pin->defiPin::isFixed()) || (pin->defiPin::isCover())) ///< fixed or cover (PAD)
	  {
	//cout<<"fix or cover..."<<endl;
	PAD* thisPad = new PAD;
	//getchar();
	strcpy(thisPad->padName, pin->defiPin::pinName()); // thisPad->padName = pin->defiPin::pinName()
	//cout<<"thisPad->pinName="<<thisPad->padName;
	thisPad->idPad = nt.pads.size();

	strcpy(thisPad->typeName, "PIN");
	//cout<<"thisPad->typeName="<<thisPad->typeName<<endl;
	thisPad->padL = 0;                        ///< for 2-D case: no layer
	thisPad->padOri = pin->defiPin::orient();   ///< 0(N)/1(W)/2(S)/3(E)/4(FN)/5(FW)/6(FS)/7(FE)

	int xl = 0, yl = 0, xh = 0, yh = 0;
	pin->defiPin::bounds(0, &xl, &yl, &xh, &yh);
	if ((thisPad->padOri % 2) == 0) /// N, S, FN, FS
	{
	thisPad->padW = (int)(((xh - xl) / (double)dbuPerMicron) * AMP_PARA);
	thisPad->padH = (int)(((yh - yl) / (double)dbuPerMicron) * AMP_PARA);
	}
	else /// W, E, FW, FE
	{
	thisPad->padW = (int)(((yh - yl) / (double)dbuPerMicron) * AMP_PARA);
	thisPad->padH = (int)(((xh - xl) / (double)dbuPerMicron) * AMP_PARA);
	}

	thisPad->LeftX = (int)((pin->defiPin::placementX() / (double)dbuPerMicron) * AMP_PARA);
	thisPad->LeftY = (int)((pin->defiPin::placementY() / (double)dbuPerMicron) * AMP_PARA);

	if (pin->defiPin::isCover())
	thisPad->flagCover = 1;
	else
	thisPad->flagCover = 0;

	//        nt.totalPadArea = nt.totalPadArea + (((float)thisPad->padW/AMP_PARA) * ((float)thisPad->padH/AMP_PARA));

	nt.nPad++;
	nt.pads.push_back(*thisPad);

	pad_NameToID.insert(pair<string, int>(thisPad->padName, thisPad->idPad));

	delete thisPad;
	}

	return 0;*/
}

int DEF_NetCB(defrCallbackType_e typ, defiNet* net, defiUserData ud)
{
	/*//cout<<"calling NetCB..."<<endl;
	  NET* thisNet = new NET;
	//cout<<"construct NET successful...."<<endl;
	//cout<<"net::name()="<<net->defiNet::name()<<endl;
	strcpy(thisNet->netName, net->defiNet::name()); 
	//thisNet->netName = net->defiNet::name();
	thisNet->idNet = nt.nets.size();
	thisNet->head = nt.pins.size();
	thisNet->nPin = 0;    // initialize
	//cout<<"thisNet->Name="<<thisNet->netName<<endl;
	//getchar();
	for (int i = 0; i < net->defiNet::numConnections(); i++)
	{
	string strPIN = "PIN";
	if (net->defiNet::instance(i) != strPIN) ///< NOT external pins ("PIN pinName" in .def NETS)
	{
	if (mod_NameToID.find(net->defiNet::instance(i)) != mod_NameToID.end())  ///< unfixed, movable (MODULE)
	{
	PIN* thisPin = new PIN;
	strcpy(thisPin->pinName, net->defiNet::pin(i)); 
	//thisPin->pinName = net->defiNet::pin(i);
	thisPin->idPin = nt.pins.size();

	int idMod = mod_NameToID[net->defiNet::instance(i)];  // net->defiNet::instance(i): component name
	thisPin->index = idMod;
	thisPin->offsetX = 0;    // initialize
	thisPin->offsetY = 0;    // initialize

	if (comp_NameToMacro.find(net->defiNet::instance(i)) != comp_NameToMacro.end())
	{
	MACRO* thisMacro = comp_NameToMacro.find(net->defiNet::instance(i))->second;
	for (unsigned int j = 0; j < thisMacro->macropins.size(); j++)
	{
	if (thisPin->pinName == thisMacro->macropins[j]->macropinName) ///< orient: all "N"
	{
	thisPin->offsetX = (int)((thisMacro->macropins[j]->x_shift * AMP_PARA) - (nt.mods[idMod].modW / 2));
	thisPin->offsetY = (int)((thisMacro->macropins[j]->y_shift * AMP_PARA) - (nt.mods[idMod].modH / 2));
	}
	}
	}

	thisNet->nPin++;
	nt.nPin++;
	nt.pins.push_back(*thisPin);
	delete thisPin;

	// updated nNet & *pNet in class MODULE (use dynamic array) 
	nt.mods[idMod].nNet++;
	unsigned int* newpNet;                                  // create new *pNet
	newpNet = new unsigned int[nt.mods[idMod].nNet];
	for (unsigned int j = 0; j < (nt.mods[idMod].nNet - 1); j++)   // copy info from old *pNet to new *pNet (~ nNet-1)
	newpNet[j] = nt.mods[idMod].pNet[j];
	newpNet[nt.mods[idMod].nNet - 1] = thisNet->idNet;        // store the newest net id into new *pNet
	delete[]  nt.mods[idMod].pNet;                          // delete old dynamical array
	nt.mods[idMod].pNet = newpNet;                          // updated *pNet in class MODULE
	}
	else if (pad_NameToID.find(net->defiNet::instance(i)) != pad_NameToID.end()) ///< fixed or cover (PAD) => notice orient!!!
	{
	PIN* thisPin = new PIN;
	strcpy(thisPin->pinName, net->defiNet::pin(i));
	//thisPin->pinName = net->defiNet::pin(i);
	thisPin->idPin = nt.pins.size();

	int idPad = pad_NameToID[net->defiNet::instance(i)];  // net->defiNet::instance(i): component name
	thisPin->index = nt.nMod + idPad;
	thisPin->offsetX = 0;    // initialize
	thisPin->offsetY = 0;    // initialize

	if (comp_NameToMacro.find(net->defiNet::instance(i)) != comp_NameToMacro.end())
	{
	MACRO* thisMacro = comp_NameToMacro.find(net->defiNet::instance(i))->second;
	for (unsigned int j = 0; j < thisMacro->macropins.size(); j++)
	{
		if (thisPin->pinName == thisMacro->macropins[j]->macropinName)
		{
			if ((nt.pads[idPad].padOri % 2) == 0) /// N, S, FN, FS
			{
				int xlength = (int)((thisMacro->macropins[j]->x_shift * AMP_PARA) - (nt.pads[idPad].padW / 2));
				int ylength = (int)((thisMacro->macropins[j]->y_shift * AMP_PARA) - (nt.pads[idPad].padH / 2));

				if (nt.pads[idPad].padOri == 0) /// N
				{
					thisPin->offsetX = xlength;
					thisPin->offsetY = ylength;
				}
				else if (nt.pads[idPad].padOri == 2) /// S
				{
					thisPin->offsetX = -xlength;
					thisPin->offsetY = -ylength;
				}
				else if (nt.pads[idPad].padOri == 4) /// FN
				{
					thisPin->offsetX = -xlength;
					thisPin->offsetY = ylength;
				}
				else if (nt.pads[idPad].padOri == 6) /// FS
				{
					thisPin->offsetX = xlength;
					thisPin->offsetY = -ylength;
				}
			}
			else /// W, E, FW, FE
			{
				int xlength = (int)((thisMacro->macropins[j]->y_shift * AMP_PARA) - (nt.pads[idPad].padW / 2));
				int ylength = (int)((thisMacro->macropins[j]->x_shift * AMP_PARA) - (nt.pads[idPad].padH / 2));

				if (nt.pads[idPad].padOri == 1) /// W
				{
					thisPin->offsetX = -xlength;
					thisPin->offsetY = ylength;
				}
				else if (nt.pads[idPad].padOri == 3) /// E
				{
					thisPin->offsetX = xlength;
					thisPin->offsetY = -ylength;
				}
				else if (nt.pads[idPad].padOri == 5) /// FW
				{
					thisPin->offsetX = xlength;
					thisPin->offsetY = ylength;
				}
				else if (nt.pads[idPad].padOri == 7) /// FE
				{
					thisPin->offsetX = -xlength;
					thisPin->offsetY = -ylength;
				}
			}
		}
	}
}

thisNet->nPin++;
nt.nPin++;
nt.pins.push_back(*thisPin);
delete thisPin;
}
else
{
	cout << "!warning: can't find MODULE/PAD (COMPONENT) " << net->defiNet::instance(i)
		<< " for NET " << net->defiNet::name() << endl;
}
}
else                                    ///< IS external pins ("PIN pinName" in .def NETS)
{
	// (1) let FIXED external PINS (in .def) be pads, so search "pad_NameToID" (padName = pinName) \n
	// (2) format for pins of NETS (in .def) is "(componentName pinName) & (PIN pinName)", \n
	//     so use "net->defiNet::pin(i)" as key to find
	if (pad_NameToID.find(net->defiNet::pin(i)) != pad_NameToID.end())
	{
		PIN* thisPin = new PIN;
		strcpy(thisPin->pinName, net->defiNet::pin(i)); 
		//thisPin->pinName = net->defiNet::pin(i);
		thisPin->idPin = nt.pins.size();

		int idPad = pad_NameToID[net->defiNet::pin(i)];  // net->defiNet::pin(i): external pin name
		thisPin->index = nt.nMod + idPad;
		thisPin->offsetX = 0;
		thisPin->offsetY = 0;

		thisNet->nPin++;
		nt.nPin++;
		nt.pins.push_back(*thisPin);
		delete thisPin;
	}
	else
	{
		//cout << "!warning: can't find PAD (FIXED external PIN) " << net->defiNet::pin(i)
		//     << " for NET " << net->defiNet::name() << endl;
	}
}
}

if (thisNet->nPin > 0)
{
	nt.nNet++;
	nt.nets.push_back(*thisNet);
}
delete thisNet;
return 0;*/
}

int DEF_BlockageCB(defrCallbackType_e typ, defiBlockage* blockage, defiUserData ud)
{
	/*--------------------------------------------------------------------------------*/

	for (int i = 0; i < blockage->defiBlockage::numRectangles(); i++)
	{
		BLOCKAGE* new_blockage = new BLOCKAGE;
		new_blockage->idBlockage = nt.blockages.size();

		new_blockage->LeftX = (int)((blockage->defiBlockage::xl(i) / (double)dbuPerMicron) * AMP_PARA);
		new_blockage->LeftY = (int)((blockage->defiBlockage::yl(i) / (double)dbuPerMicron) * AMP_PARA);
		new_blockage->RightX = (int)((blockage->defiBlockage::xh(i) / (double)dbuPerMicron) * AMP_PARA);
		new_blockage->RightY = (int)((blockage->defiBlockage::yh(i) / (double)dbuPerMicron) * AMP_PARA);

		if (blockage->defiBlockage::hasPlacement())
			new_blockage->blockageL = 0;
		else if (blockage->defiBlockage::hasLayer())
		{
			for (unsigned int j = 0; j < routing_layer.size(); j++)
			{
				if (routing_layer[j]->name == (string)blockage->defiBlockage::layerName())
				{
					new_blockage->blockageL = j + 1;
					break;
				}
			}
		}

		nt.nBlockage++;
		nt.blockages.push_back(*new_blockage);
		delete new_blockage;
	}

	/*--------------------------------------------------------------------------------*/

	return 0;
}

void ReadMlistFile(char* filename, NETLIST& tmpnt){
	char* userData = 0;

	/* Initial Call Routine */
	defrInit();

	defrSetUnitsCbk(DEF_UnitCB);
	defrSetDieAreaCbk(DEF_DieAreaCB);
	defrSetComponentCbk(MLIST_ComponentCB);
	//defrSetGcellGridCbk(DEF_GCellGridCB);

	defrReset();

	FILE* f;
	char defFile[100] = "\0";
	strcpy(defFile, filename);
	strcat(defFile, ".mlist");
	if ((f = fopen(defFile, "r")) == 0) // r: read text file
	{
		cerr << "!error  : can't open file " << defFile << endl;
		exit(EXIT_FAILURE);
	}

	int res = defrRead(f, defFile, (void*)userData, 1); // res == 0: NO errors
	if (res)
	{
		cerr << "!error  : reader returns bad status in file " << defFile << endl;
		exit(EXIT_FAILURE);
	}

	(void)defrReleaseNResetMemory();
	fclose(f);
}

int MLIST_ComponentCB(defrCallbackType_e typ, defiComponent* co, defiUserData ud)
{
	if ((co->defiComponent::isFixed()) || (co->defiComponent::isCover())) ///< fixed or cover (PAD)
	{
		if (macro_NameToPtr.find(co->defiComponent::name()) != macro_NameToPtr.end()) // co->defiComponent::name(): MACRO name
		{
			MODULE* thisPad = new MODULE;
			strcpy(thisPad->modName, co->defiComponent::id()); // thisPad->padName = co->defiComponent::id()
			thisPad->idMod = nt.pads.size();

			MACRO* thisMacro = (macro_NameToPtr.find(co->defiComponent::name()))->second;
			strcpy(thisPad->typeName, co->defiComponent::name()); // thisPad->typeName = co->defiComponent::name()
			thisPad->modL = 0;                                    ///< for 2-D case: no layer
			//thisPad->modOri = co->defiComponent::placementOrient(); ///< 0(N)/1(W)/2(S)/3(E)/4(FN)/5(FW)/6(FS)/7(FE)

			/*--------------------------------------------------------------------------------*/

			//for (unsigned int i = 0; i < thisMacro->macro_obs.size(); i++)
			//thisPad->blockage.push_back(*thisMacro->macro_obs[i]);

			/*--------------------------------------------------------------------------------*/

			if ((thisPad->modOri % 2) == 0) /// N, S, FN, FS
			{
				thisPad->modW = (int)(thisMacro->macroW * AMP_PARA);
				thisPad->modH = (int)(thisMacro->macroH * AMP_PARA);
			}
			else /// W, E, FW, FE
			{
				thisPad->modW = (int)(thisMacro->macroH * AMP_PARA);
				thisPad->modH = (int)(thisMacro->macroW * AMP_PARA);
			}

			thisPad->LeftX = (int)((co->defiComponent::placementX() / (double)dbuPerMicron) * AMP_PARA);
			thisPad->LeftY = (int)((co->defiComponent::placementY() / (double)dbuPerMicron) * AMP_PARA);

			/*if (co->defiComponent::isCover())
			  thisPad->flagCover = 1;
			  else
			  thisPad->flagCover = 0;*/

			nt.totalPadArea = nt.totalPadArea + (((float)thisPad->modW / AMP_PARA) * ((float)thisPad->modH / AMP_PARA));

			nt.nPad++;
			nt.pads.push_back(*thisPad);

			pad_NameToID.insert(pair<string, int>(thisPad->modName, thisPad->idMod));
			comp_NameToMacro.insert(pair<string, MACRO*>(thisPad->modName, thisMacro));

			delete thisPad;
		}
		else
		{
			cout << "!warning: can't find MACRO " << co->defiComponent::name()
				<< " for PAD (COMPONENT) " << co->defiComponent::id() << endl;
		}
	}
	else ///< unfixed, movable (MODULE)
	{
		if (macro_NameToPtr.find(co->defiComponent::name()) != macro_NameToPtr.end()) // co->defiComponent::name(): MACRO name
		{
			MODULE* thisMod = new MODULE;
			strcpy(thisMod->modName, co->defiComponent::id()); // thisMod->modName = co->defiComponent::id()


			MACRO* thisMacro = macro_NameToPtr.find(co->defiComponent::name())->second;
			strcpy(thisMod->typeName, co->defiComponent::name()); // thisMod->typeName = co->defiComponent::name()
			thisMod->type = '0';      ///< hard module
			thisMod->modW = (int)(thisMacro->macroW * AMP_PARA);
			thisMod->modH = (int)(thisMacro->macroH * AMP_PARA);
			thisMod->modL = 0;        ///< for 2-D case: no layer
			thisMod->modArea = ((float)thisMod->modW / AMP_PARA) * ((float)thisMod->modH / AMP_PARA);
			//add pins=============================================
			thisMod->macro_pins.resize(thisMacro->macropins.size());
			string str;
			for (int i = 0; i < thisMacro->macropins.size(); i++)
			{
				thisMod->macro_pins[i] = thisMacro->macropins[i];
			}
			//======================================================
			if (thisMacro->macroType == "CORE")
			{
				thisMod->flagStd = 1;
			}
			else if (thisMacro->macroType == "BLOCK")
			{
				thisMod->flagStd = 0;
			}
			else
				cout << "!warning: other macroType " << thisMacro->macroType << " is in class MODULE" << endl;

			if (co->defiComponent::isPlaced())
			{
				thisMod->GLeftX = thisMod->LeftX = (int)((co->defiComponent::placementX() / (double)dbuPerMicron) * AMP_PARA);
				thisMod->GLeftY = thisMod->LeftY = (int)((co->defiComponent::placementY() / (double)dbuPerMicron) * AMP_PARA);
				thisMod->GCenterX = thisMod->CenterX = thisMod->GLeftX + (thisMod->modW / 2);
				thisMod->GCenterY = thisMod->CenterY = thisMod->GLeftY + (thisMod->modH / 2);
			}
			else if (co->defiComponent::isUnplaced())
			{
				thisMod->GLeftX = thisMod->LeftX = 0;
				thisMod->GLeftY = thisMod->LeftY = 0;
				thisMod->GCenterX = thisMod->CenterX = thisMod->GLeftX + (thisMod->modW / 2);
				thisMod->GCenterY = thisMod->CenterY = thisMod->GLeftY + (thisMod->modH / 2);
			}


			nt.totalModArea = nt.totalModArea + thisMod->modArea;//totalModeArea+=modArea
			thisMod->idMod = nt.nMod;
			nt.mods.push_back(*thisMod);
			nt.nMod++;
			nt.nHard++;

			//cout <<"MACRO:("<<thisMod->idMod <<")"<< thisMod->modName << endl;//

			mod_NameToID.insert(pair<string, int>(thisMod->modName, thisMod->idMod));
			comp_NameToMacro.insert(pair<string, MACRO*>(thisMod->modName, thisMacro));

			delete thisMod;
		}
		else
		{
			cout << "!warning: can't find MACRO " << co->defiComponent::name()
				<< " for MODULE (COMPONENT) " << co->defiComponent::id() << endl;
		}
	}

	return 0;
}

//void ReadParaFile(char* filename)
//{
//	ifstream fin;
//	char paraFile[100] = "\0";
//	strcpy(paraFile, filename);
//	strcat(paraFile, ".para");
//	fin.open(paraFile);
//
//	if (!fin)
//	{
//		cout << "!warning: can't open file " << paraFile << ", use initial parameters" << endl;
//	}
//	else
//	{
//		string strIn;
//		while (fin >> strIn)
//		{
//			if (strIn == "clusAlpha")
//				fin >> strIn >> clusAlpha;
//			else if (strIn == "timesClusScore")
//				fin >> strIn >> timesClusScore;
//			else if (strIn == "timesStdArea")
//				fin >> strIn >> timesStdArea;
//			else if (strIn == "timesMacroArea")
//				fin >> strIn >> timesMacroArea;
//			else if (strIn == "timesChipArea")
//				fin >> strIn >> timesChipArea;
//		}
//	}
//}

/*--------------------------------------------------------------------------------*/

int LEF_LayerCB(lefrCallbackType_e typ, lefiLayer* layer, lefiUserData ud)
{
	if (strcmp(layer->lefiLayer::type(), "ROUTING") == 0)
	{
		ROUTING_LAYER* new_routing_layer = new ROUTING_LAYER();
		new_routing_layer->name = layer->lefiLayer::name();
		new_routing_layer->direction = layer->lefiLayer::direction();
		new_routing_layer->min_width = layer->lefiLayer::minwidth();
		new_routing_layer->min_spacing = layer->lefiLayer::pitch() - layer->lefiLayer::minwidth();
		routing_layer.push_back(new_routing_layer);
	}

	return 0;
}

//int DEF_GCellGridCB(defrCallbackType_e typ, defiGcellGrid* gcell, defiUserData ud)
//{
//	if (strcmp(gcell->defiGcellGrid::macro(), "X") == 0)
//	{
//		V_track_start = 0;
//		//VTrackStart = gcell->defiGcellGrid::x() / (double)dbuPerMicron * AMP_PARA;
//		num_V_track = gcell->defiGcellGrid::xNum();
//		V_track_spacing = (int)((gcell->defiGcellGrid::xStep() / (double)dbuPerMicron) * AMP_PARA);
//	}
//	else
//	{
//		H_track_start = 0;
//		//HTrackStart = gcell->defiGcellGrid::x() / (double)dbuPerMicron * AMP_PARA;
//		num_H_track = gcell->defiGcellGrid::xNum();
//		H_track_spacing = (int)((gcell->defiGcellGrid::xStep() / (double)dbuPerMicron) * AMP_PARA);
//	}
//
//	return 0;
//}
//
/*--------------------------------------------------------------------------------*/

void ReadVerilogFile(char* filename, NETLIST& tmpnt)
{
	cout << "*Now Parse verilog File" << endl;

	char* userData = 0;//NULL

	/* Initial Call Routine */

	ifstream fin;
	string vFile = "\0";

	vFile = filename;
	vFile += ".v";
	fin.open(vFile);
	if (!fin)
	{
		cerr << "!error  : reader returns bad status in file " << vFile << endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
	else        /// read {filename}.lef file in {filename}.list file
	{
		string tmpstr;
		int wire_num = 0;
		while (fin >> tmpstr)
		{
			//cout << "<<"<<tmpstr<<">>\n";
			if(tmpstr=="module"){
				getline(fin, tmpstr);	
				continue;
			}
			if (tmpstr == "//"){
				getline(fin, tmpstr);
				cout << "comment:" << tmpstr << "\n";
				continue;
			}
			if (tmpstr == "input"){
				fin >> tmpstr;
				continue;
			}
			if(tmpstr == "output"){
				fin >> tmpstr;
				continue;
			}
			if (tmpstr == "wire")
			{
				fin >> tmpstr;
				//cout << "WIRE" << tmpstr<<endl;
				string wire_name = tmpstr.substr(0,tmpstr.size()-1); 
				WIRE* thisWire = new WIRE; 
				thisWire-> wireName = wire_name;
				tmpnt.wires.resize(tmpnt.wires.size() + 1, *thisWire);
				// map insert
				wire_NameToID.insert(pair<string, int>(wire_name, wire_num));
				wire_num++;
				// new wire
			}
			if (tmpstr.size() >= 5)
			{
				if (tmpstr.substr(0, 5) == "block")
				{
					//cout << "Macro:" << tmpstr <<" | ";
					string Macro_name;
					fin >> Macro_name;
					int thisModId = mod_NameToID.find(Macro_name)->second;
					//cout<<"ID="<<thisModId;
					if(pad_NameToID.find(Macro_name)!=pad_NameToID.end()){
						//cout<<"here";
						pad_NameToID.find(Macro_name)->second;
						MODULE* thisPad = &tmpnt.pads[thisModId];
						cout << "Marco:" << Macro_name << endl;
						while (fin >> tmpstr){
							//cout << "-" << tmpstr << endl;
							if (tmpstr[0] == '(')
								continue;
							else if (tmpstr[tmpstr.size() - 1] == ';')
								break;
							//parse wire name
							string wire_name = "wire";
							string pin_name = "pin";
							int wireh = 0, wiret = 0, pinh = 0, pint = 0;
							for (int i = 0; i < tmpstr.size(); i++)
							{
								switch (tmpstr[i])
								{
									case '.':
										pinh = i;
										break;
									case '(':
										pint = i - 1;
										wireh = i + 1;
										break;
									case ')':
										wiret = i - 1;
										break;
									default:
										break;
								}
							}
							wire_name = tmpstr.substr(wireh, wiret - wireh + 1);
							pin_name = tmpstr.substr(pinh + 1, pint - pinh);
							//wire insertion
							//1.maping macro_name to macro* 
							//2.maping wire_name to wire*
							int thisWireId = wire_NameToID.find(wire_name)->second;
							WIRE* thisWire = &(tmpnt.wires[thisWireId]);
							thisPad->macro_wires.resize(thisPad->macro_wires.size() + 1, thisWire);
							//3.save wire* in macro_wire in this macro
							thisWire->wire_padID.resize(thisWire->wire_padID.size() + 1, thisModId);
							//pin reordering
							//cout<<"find:"<<pin_name<<"\n";
						}
						continue;
					}
					MODULE* thisMod = &tmpnt.mods[thisModId];
					//cout <<" "<< Macro_name<<" "<<thisMod->modName << endl;
					while (fin >> tmpstr){
						//cout << "-" << tmpstr << endl;
						if (tmpstr[0] == '(')
							continue;
						else if (tmpstr[tmpstr.size() - 1] == ';')
							break;
						//parse wire name
						string wire_name = "wire";
						string pin_name = "pin";
						int wireh = 0, wiret = 0, pinh = 0, pint = 0;
						for (int i = 0; i < tmpstr.size(); i++)
						{
							switch (tmpstr[i])
							{
								case '.':
									pinh = i;
									break;
								case '(':
									pint = i - 1;
									wireh = i + 1;
									break;
								case ')':
									wiret = i - 1;
									break;
								default:
									break;
							}
						}
						wire_name = tmpstr.substr(wireh, wiret - wireh + 1);
						pin_name = tmpstr.substr(pinh+1 , pint - pinh );
						//wire insertion
						//1.maping macro_name to macro* 
						//2.maping wire_name to wire*
						int thisWireId = wire_NameToID.find(wire_name)->second;
						WIRE* thisWire = &(tmpnt.wires[thisWireId]);
						thisMod->macro_wires.resize( thisMod->macro_wires.size()+1, thisWire);
						//3.save wire* in macro_wire in this macro
						thisWire->wire_macroID.resize(thisWire->wire_macroID.size()+1,thisModId);
						//pin reordering
						//cout<<"find:"<<pin_name<<"\n";
						for (int i = 0; i < tmpnt.mods[thisModId].macro_pins.size(); i++)
						{
							if (thisMod->macro_pins[i]->macropinName == pin_name){ //wire to pins for a module
								//cout<<thisMod->macro_pins[i]->macropinName;
								//cout<<"   "<<thisMod->macro_pins[i]->x_shift<<", "<< thisMod->macro_pins[i]-> y_shift;
								//swap
								int j = thisMod->macro_wires.size()-1;
								MACROPIN* macropin_tmp = thisMod->macro_pins[i];
								thisMod->macro_pins[i] = thisMod->macro_pins[j];
								thisMod->macro_pins[j] = macropin_tmp;
								break;
							}
						}
						//cout<<"\n--------------"<<endl;
						//------------Aug 19 added
					}
				}// read macro to wire
				else if(tmpstr == "endmodule")
				{
					break;
				}
				else
				{
					if(tmpstr[tmpstr.size()-1]==';'||tmpstr[tmpstr.size()-1]==',')
						continue;
					string Cell_name;
					fin >> Cell_name;
					int thisModId = mod_NameToID.find(Cell_name)->second;
					//1.maping macro_name to cell* 
					if(mod_NameToID.find(Cell_name)==mod_NameToID.end()){
						cout<<"[404! Cell]"<<tmpstr<<" "<<Cell_name<<endl;
						continue;
					}
					MODULE* thisMod = &tmpnt.stds[thisModId];
					//cout << "Cell:" << Cell_name << endl;
					while (fin >> tmpstr){
						if (tmpstr[0] == '(')
							continue;
						else if (tmpstr[tmpstr.size() - 1] == ';')
							break;
						//parse wire name
						string wire_name = "wire";
						string pin_name = "pin";
						int wireh = 0, wiret = 0, pinh = 0, pint = 0;
						for (int i = 0; i < tmpstr.size(); i++)
						{
							switch (tmpstr[i])
							{
								case '.':
									pinh = i;
									break;
								case '(':
									pint = i - 1;
									wireh = i + 1;
									break;
								case ')':
									wiret = i - 1;
									break;
								default:
									break;
							}
						}
						wire_name = tmpstr.substr(wireh, wiret - wireh + 1);
						pin_name = tmpstr.substr(pinh + 1, pint - pinh);
						//wire insertion
						//2.maping wire_name to wire*
						int thisWireId = wire_NameToID.find(wire_name)->second;
						WIRE* thisWire = &(tmpnt.wires[thisWireId]);
						thisMod->macro_wires.resize(thisMod->macro_wires.size() + 1, thisWire);
						//3.save wire* in macro_wire in this cell
						thisWire->wire_stdID.resize(thisWire->wire_stdID.size() + 1, thisModId);
					}
				}//read standard cell to wire
			}//read component
		}
	}
	fin.close();
}
//edited july 20th by hsieh ^U^
