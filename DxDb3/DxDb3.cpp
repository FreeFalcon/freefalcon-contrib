// DxDb3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include "DxDb3.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "afxtempl.h"

#define	ShiAssert(x)

#include "d3dtypes.h"
#include "d3dxmath.h"


#include "polylib.h"
#include "DXDefines.h"
#include "DXFiles.h"
#include "encoder.h"


// The one and only application object

CWinApp theApp;

// The pointer to models
DWORD	nCT, PARENT, LOD, CLASS, LODLEVEL;
void		*Models[MAX_LOD];
E_Object	Obj;
CString		DBName,m_lpCmdLine;


bool	LogOn, DataModified;
bool	DbOpened;
bool	OpenDBFile(CString FileName);
void	Switches(CString);
void	Log(CString Text);
void	Commands(CString Command, char *Line);
void	SaveDb(void);



CString	Rename(char *Argument);
CString	Replace(char *Argument);
CString	SetData(char *Argument);
CString	UpdateData(char *Argument);
CString	ListData(char *Argument);
CString	AddModel(char *Argument);
CString	ExportModel(char *Argument);
CString	UnusedParents(char *Argument);
CString	UnusedTextures(char *Argument);

const	ParseItem	Coms[]={
							{ "RENAME", &Rename, true},
							{ "REPLACE", &Replace, true},
							{ "UPDATE", &UpdateData, true },
							{ "SET", &SetData, true },
							{ "LIST", &ListData, false },
							{ "ADD", &AddModel, false },
							{ "EXPORT", &ExportModel, false },
							{ "PARENTS", &UnusedParents, false },
							{ "TEXTURES", &UnusedTextures, false },
							{ "", NULL}
};


const	CString		VB_Classes[]={
								"FEATS",
								"GROUND",
								"AIR",
								""
};


int	Get1stFreeLod(void)
{
	for(DWORD a=0; a<MAX_LOD; a++)
		if(!Models[a]) return a;
	return -1;
}

//////////////////////////////////// THE COMMAND FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

CString	GetCommand(char *string=NULL);

CString	GetCommand(char *string)
{
	CString Command;
	// Get next command
	Command=strtok(string, " ,\n\r");
	Command.MakeUpper();
	if(Command=="") return Command;
		
	// if quoted, get till end of quotes
	if(Command[0]=='"' && Command[Command.GetLength()-1]!='"') Command=Command + " " + _strupr(strtok(NULL, "\"\n\r")) +"\"";
	if(Command[0]=='"') Command=Command.Right(Command.GetLength()-1);
	if(Command[Command.GetLength()-1]=='"') Command=Command.Left(Command.GetLength()-1);
	Command.Trim();
	return Command;
}	

CString	Rename(char *Argument)
{
	CString Name;
	CString t;
	// gets the new name to assign
	Name=GetCommand();
	if(LOD!=-1) {
		t.Format("MODEL %d : %s Renamed as %s", LOD, GetIndexName(LOD), Name);
		SetIndexName(LOD, Name); 
	} else {
		t="Unassigned #MODEL";
	}
	return t;
}




// Look for any parent using a LOD
DWORD	GetParentUsedByLod(DWORD LOD)
{
	static	DWORD	ParentSeek = -1;
	// if LOD = -1, then it's start
	if(LOD==-1) { ParentSeek = -1; return -1; }

	// rturn the seek
	return	ParentSeek = GetParentUsingLOD(ParentSeek+1, LOD);
}



void	GetParentData(DWORD Parent, DWORD *nSwitches, DWORD *nSlots, DWORD *nDofs, float *Radius, float *Radar, float *IR, D3DXVECTOR3	*SlotsArray, D3DXVECTOR3 *HitBox)
{
	DWORD	Level=0, Lod=0;
	D3DXVECTOR3	tSlotsArray[100];

	// Get the lod in the parent
	while((Lod=GetParentLodID(Parent, Level))!=-1){
		if(Lod!=-1){
			// Get the model features
			DWORD	tSwitches=0, tDofs=0, tSlots=0;
			float	tRadius = 0.0f;
			memset(tSlotsArray, 0, sizeof(tSlotsArray));
			GetModelFeatures(Models[Lod], &tSwitches, &tDofs, &tSlots, tSlotsArray);
			// Keep only bigger parameters
			if(nSwitches) *nSwitches=max(tSwitches, *nSwitches);
			if(nSlots) *nSlots=max(tSlots, *nSlots);
			if(nDofs) *nDofs=max(tDofs, *nDofs);
			if(Radius) *Radius= max(*Radius, GetModelRadius(Models[Lod]));

			// only from LOD 0
			if(Level==0){
				if(SlotsArray){
					for(DWORD a=0; a<100; a++){
						if(!SlotsArray[a].x && !SlotsArray[a].y && !SlotsArray[a].z)
							SlotsArray[a] = tSlotsArray[a];
					}
				}
			
				D3DXVECTOR3	tHitBox[2];
				memset(tHitBox, 0, sizeof(tHitBox));
				GetModelHitBox(Models[Lod], &tHitBox[0], &tHitBox[1]);

				if(HitBox) HitBox[0] = tHitBox[0], HitBox[1] = tHitBox[1] ;

				if(Radar) *Radar = GetRadarSignature(&tHitBox[0], &tHitBox[1]);
				if( IR ) *IR = GetRadarSignature(&tHitBox[0], &tHitBox[1]);

			}
		}
		Level++;
	}
}

// This function updated STOLT/SWITCHES/DOFS ( no Radius ) for any paernt using a LOD
void	UpdatePRecords(DWORD LodId)
{
	DWORD	Parent=-1;
	D3DXVECTOR3	SlotsArray[100], tSlotsArray[100];
	memset(SlotsArray, 0, sizeof(SlotsArray));

	// Thru all parents using such Lod
	while((Parent=GetParentUsingLOD(Parent+1, LodId))!=-1){
		if(Parent!=-1){
			CString t;
			t.Format("Updating PARENT #%d ", Parent);
			Log(t);
			DWORD	Level=0;
			DWORD	Lod=0;
			DWORD	nSwitches=0, nDofs=0, nSlots=0;
			float	Radius = 0.0f;
			// Get the lod in the parent
			while((Lod=GetParentLodID(Parent, Level))!=-1){
				if(Lod!=-1){
					// Get the model features
					DWORD	tSwitches=0, tDofs=0, tSlots=0;
					float	tRadius = 0.0f;
					memset(tSlotsArray, 0, sizeof(tSlotsArray));
					GetModelFeatures(Models[Lod], &tSwitches, &tDofs, &tSlots, tSlotsArray);
					// Keep only bigger parameters
					nSwitches=max(tSwitches, nSwitches);
					nSlots=max(tSlots, nSlots);
					nDofs=max(tDofs, nDofs);
					tRadius = GetModelRadius(Models[Lod]);
					Radius = max(Radius, tRadius);
					for(DWORD a=0; a<100; a++){
						if(!SlotsArray[a].x && !SlotsArray[a].y && !SlotsArray[a].z)
							SlotsArray[a] = tSlotsArray[a];
					}
				}
				Level++;
			}
			// Force an Update
			UpdateParentRecord(LodId, &nSwitches, &nDofs, &nSlots, &Radius, true, SlotsArray);
		}
	}


}


CString	Replace(char *Argument)
{
	CString Name;
	CString t;
	// gets the filename to load
	Name=GetCommand();
	if(LOD!=-1) {
		// Check fopr extension
		if(Name.Right(4)!=".DXM") Name+=".DXM";
		CFile F;
		// if not found
		if(!F.Open(Name, CFile::modeRead)) return "Failed To Open " + Name;
		DWORD Size=(DWORD)F.GetLength();
		// Allocate new buffer
		void *New=malloc(Size);
		if(!F.Read(New, Size)) return "Failed to Read "+ Name;
		F.Close();
		// Get original class
		DWORD Class=((DxDbHeader*)Models[LOD])->VBClass;
		// ok... deindex and eventually update the model, kill any static light
		DeIndexModel(New, &Obj, false);
		Obj.Nodes=(void*)E_SortModel((DXNode*)Obj.Nodes);
		Obj.Name="";
		// Check for Max Textures Used and Update eventually
		if(Obj.MaxTexNr > GetMaxDbTexture()) SetMaxDbTexture(Obj.MaxTexNr);
		Encoder	E(Obj.Name);
		E.Initialize(&Obj);
		E.Encode(LOD, Class);
		ClearModel(&Obj);
		free(New);
		New=malloc(E.GetModelSize());
		memcpy(New, E.GetModelBuffer(), E.GetModelSize());

		// eventuially unallocate old model
		if(Models[LOD]) free(Models[LOD]);
		Models[LOD]=New;


		// Missing texture set fix
		if(!GetParentTexSets(PARENT)) SetParentTexSets(PARENT, 1);

		t.Format("LOD %d : %s Replaced by file %s", LOD, GetIndexName(LOD), Name);
		Log(t);
		UpdatePRecords(LOD);
		t.Format("LOD %d : %s *OK*\n", LOD, GetIndexName(LOD));;

	} else {
		t="Unassigned #MODEL";
	}
	return t;
}



CString	AddModel(char *Argument)
{

	CString Name;
	CString t;

	//check if a LOD already present, if so, go to do a replace
	if(LOD!=-1){
		t.Format("ERROR : Parent %d : Lod %d has already lod #%d assigned.", PARENT, LODLEVEL, LOD);
		return t;
	}

	// gets the filename to load
	Name=GetCommand();

	// Check fopr extension
	if(Name.Right(4)!=".DXM") Name+=".DXM";
	CFile F;
	// if not found
	if(!F.Open(Name, CFile::modeRead)) return "Failed To Open " + Name;
	DWORD Size=(DWORD)F.GetLength();
	// Allocate new buffer
	void *New=malloc(Size);
	if(!F.Read(New, Size)) return "Failed to Read "+ Name;
	F.Close();

	// Get a free lod slot
	LOD = Get1stFreeLod();
	if(LOD == -1) { free(New) ; ClearModel(&Obj); return("Failed, no more LOD slots available"); }

	// Get original class
	DWORD Class=((DxDbHeader*)New)->VBClass;
	// ok... deindex and eventually update the model, kill any static light
	ClearModel(&Obj);
	DeIndexModel(New, &Obj, false);
	Obj.Nodes=(void*)E_SortModel((DXNode*)Obj.Nodes);
	Obj.Name="";
	// Check for Max Textures Used and Update eventually
	if(Obj.MaxTexNr > GetMaxDbTexture()) SetMaxDbTexture(Obj.MaxTexNr);
	Encoder	E(Obj.Name);
	E.Initialize(&Obj);
	E.Encode(LOD, Class);
	ClearModel(&Obj);
	free(New);
	New=malloc(E.GetModelSize());
	memcpy(New, E.GetModelBuffer(), E.GetModelSize());

	// assign it
	Models[LOD]=New;
	// Ok, update the parent
	SetParentLod( PARENT, LODLEVEL, LOD, Name.Left(Name.StringLength(Name) - 4));

	if(LODLEVEL == 0){
		SetParentBox(PARENT, &Obj.UpLeft, &Obj.DnRight);
		float	RadarSign = GetRadarSignature(&Obj); 
		SetParentSignatures(PARENT, &RadarSign, &RadarSign);
		SetParentTexSets(PARENT, 1);

	}

	UpdatePRecords(LOD);

	SetParentDistance(PARENT, LODLEVEL, 260000.0f);
	ClearModel(&Obj);

	t.Format("Parent %d : Lod %d : Assigned Lod#%d - %s", PARENT, LODLEVEL, LOD, Name);

	// Check for Max Parent
	if((PARENT + 1) > GetMaxDbParent())
		SetMaxDbParent(PARENT + 1);

	DataModified = true;

	return t;

}



CString	ExportModel(char *Argument)
{

	CString Name;
	CString t;

	//check if a LOD already present, if so, go to do a replace
	if(LOD==-1) return "ERROR : LOD not specified";

	// Get the model Name
	Name = GetIndexName(LOD);

	if(Name=="") return "ERROR : LOD not assigned";

	// add the extension
	Name+=".DXM";

	CFile F;
	if(!F.Open(Name, CFile::modeCreate | CFile::modeWrite)) return "Failed To Open " + Name;

	DxDbHeader *head =  (DxDbHeader*) Models[LOD];
	DWORD	Size = head->ModelSize;

	F.Write(Models[LOD], Size);
	F.Close();


	return Name + " successfully exported!";
}


CString	SetVBClass(char *Argument, CString Name)
{
	DWORD	ClassNr =	0;

	if(LOD==-1) return "SET VBCLASS - Unassigned #MODEL";

	while(VB_Classes[ClassNr][0]){
		if(VB_Classes[ClassNr] == Name){
			((DxDbHeader*)Models[LOD])->VBClass = 1<<ClassNr;
			Name.Format("LOD %d Assigned VB Class %s", LOD, Name);
			return Name;
		}
		ClassNr++;
	}


	return "Invalid VB CLASS";


}



CString	SetData(char *Argument)
{
	CString Name;
	CString t;
	D3DXVECTOR3	HitBox[2];
	memset(HitBox, 0, sizeof(HitBox));

	if(LOD == -1 && PARENT == -1) return("SET Error - Missing operation target");

	// gets the TARGET of the Set
	while((Name=GetCommand())!=""){
		float	fValue;
		DWORD	dValue;
		DWORD	Parent;
		CString Value=GetCommand();
		fValue=(float)atof(Value);
		dValue=atoi(Value);

		if(Name=="VBCLASS") { SetVBClass(Argument, Value); continue; }

		// Distance requires a LOD
		if(Name=="DISTANCE"){
			if(LOD==-1) { "SET DISTANCE - Unassigned #MODEL"; continue; }
			SetLodDistance(LOD, fValue);
			t.Format("LOD %d : Distance %f", LOD, fValue, true);
			puts(t);
			continue;
		}

		// HitBox is form ed by many values
		if(Name=="HITBOX"){
			HitBox[0].x = fValue;
			HitBox[0].y = (float)atof(GetCommand());
			HitBox[0].z = (float)atof(GetCommand());
			HitBox[1].x = (float)atof(GetCommand());
			HitBox[1].y = (float)atof(GetCommand());
			HitBox[1].z = (float)atof(GetCommand());
		}


		// Initialize Parent Scan
		GetParentUsedByLod(-1);

		// till work to do...
		while(true){
			// if we have a LOD, look for any parent using it, all parents are to be set
			if(LOD!=-1) Parent = GetParentUsedByLod(LOD);
			else Parent = PARENT; PARENT = -1;

			if(Parent!=-1) {

				// FORCE the Parameter
				if(Name=="RADIUS") 	{ UpdateSinglePRecord(Parent, NULL, NULL, NULL, &fValue); t.Format("PARENT %d : Set Radius to %f", Parent, fValue); }
				if(Name=="SWITCHES"){ UpdateSinglePRecord(Parent, &dValue, NULL, NULL, NULL); t.Format("PARENT %d : Set switches to %d", Parent, dValue); }
				if(Name=="SLOTS")	{ UpdateSinglePRecord(Parent, NULL, &dValue, NULL, NULL); t.Format("PARENT %d : Set slots to %d", Parent, dValue); }
				if(Name=="DOFS")	{ UpdateSinglePRecord(Parent, NULL, NULL, &dValue, NULL); t.Format("PARENT %d : Set Dofs to %d", Parent, dValue); }
				if(Name=="TEXSET") 	{ SetParentTexSets(Parent, dValue); t.Format("PARENT %d : Texture Sets %d", Parent, dValue); }
				if(Name=="RADAR") 	{ SetParentSignatures(Parent, &fValue, NULL); t.Format("PARENT %d : Radar Signature %f", Parent, fValue); }
				if(Name=="IR") 	    { SetParentSignatures(Parent, NULL, &fValue); t.Format("PARENT %d : IR Signature %f", Parent, fValue); }
				if(Name=="HITBOX")  { UpdateSinglePRecord(Parent, NULL, NULL, NULL, NULL, NULL, HitBox);  t.Format("PARENT %d : HitBox", Parent); }
					
				// Missing texture set fix
				if(!GetParentTexSets(Parent)) SetParentTexSets(Parent, 1);
			
				puts(t);

			} else {
				// if here, end of job
				break;
			}
		} 
	}
	return t;	
}


CString	UpdateData(char *Argument)
{
	CString Name;
	CString t;
	DWORD	Parent;

	// Check for LOD
	if(LOD == -1 && PARENT == -1) return("UPDATE Error - Missing operation target");

	while((Name=GetCommand())!=""){


		// Initialize Parent Scan
		GetParentUsedByLod(-1);

		t.Format("UPDATE %s", Name);
		puts(t);

		// till work to do...
		while(true){
			// if we have a LOD, look for any parent using it, all parents are to be set
			if(LOD!=-1) Parent = GetParentUsedByLod(LOD);
			else Parent = PARENT; PARENT = -1;


			if(Parent!=-1){

				t.Format("UPDATED PARENT #%d", Parent);
				puts(t);

				// Zeroize all data to get
				DWORD	nSwitches=0, nDofs=0, nSlots=0;
				float	Radius = 0.0f, Radar =0.0f, IR = 0.0f;

				D3DXVECTOR3	HitBox[2];
				memset(HitBox, 0, sizeof(HitBox));
				
				D3DXVECTOR3	SlotsArray[100];
				memset(SlotsArray, 0, sizeof(SlotsArray));

				GetParentData(Parent, &nSwitches, &nSlots, &nDofs, &Radius, &Radar, &IR, SlotsArray, HitBox);

				if(Name=="SLOTS" || Name=="ALL")	UpdateSinglePRecord(Parent, NULL, NULL, &nSlots, NULL, SlotsArray);
				if(Name=="SWITCHES" || Name=="ALL") UpdateSinglePRecord(Parent, &nSwitches, NULL, NULL, NULL);
				if(Name=="DOFS" || Name=="ALL")		UpdateSinglePRecord(Parent, NULL, &nDofs, NULL, NULL);
				if(Name=="RADIUS" || Name=="ALL")	UpdateSinglePRecord(Parent, NULL, NULL, NULL, &Radius);
				if(Name=="RADAR" || Name=="ALL") 	SetParentSignatures(Parent, &Radar, NULL);
				if(Name=="IR" || Name=="ALL") 	    SetParentSignatures(Parent, NULL, &IR);
				if(Name=="HITBOX" || Name=="ALL")   UpdateSinglePRecord(Parent, NULL, NULL, NULL, NULL, NULL, HitBox);
				// Missing texture set fix
				if(!GetParentTexSets(Parent)) SetParentTexSets(Parent, 1);
			} else {
				break; 
			}
		}
	}
	return "";
	
}



CString	ListData(char *Argument)
{
	CString		Name;
	CString		Data;
	HDRPARENT	Parent;

	if(PARENT==-1) return "Unassigned PARENT";

	Name.Format("Parent #%d\n", PARENT);
	puts(Name);
	
	GetParentData(PARENT, &Parent);
	
	Data.Format("\tTexture Sets      : %d", Parent.TextureSets), puts(Data);
	Data.Format("\tDofs              : %d", Parent.nDOFs), puts(Data);
	Data.Format("\tSwitches          : %d", Parent.nSwitches), puts(Data);
	Data.Format("\tSlots             : %d", Parent.nSlots), puts(Data);
	Data.Format("\tRadius            : %f", Parent.Radius), puts(Data);
	Data.Format("\tBox Max( X, Y ,Z ): %f, %f, %f", Parent.XMax, Parent.YMax, Parent.ZMax), puts(Data);
	Data.Format("\tBox Min( X, Y ,Z ): %f, %f, %f", Parent.XMin, Parent.YMin, Parent.ZMin), puts(Data);
	Data.Format("\tRadar Signature   : %f", Parent.RadarSignature), puts(Data);
	Data.Format("\tIR Signature      : %f\n", Parent.IRSignature), puts(Data);

	puts("\t----------------------------");
	Data.Format("\tLODs              : %d\n", Parent.nLODs), puts(Data);
	

	for(DWORD a=0; a<7; a++){
		DWORD LOD=GetParentLodID(PARENT, a);
		float Dist= GetParentLodDistance(PARENT, a);

		CString	VbClass;
		VbClass = "";
		if(LOD != -1){
			DWORD VbC = ((DxDbHeader*)Models[LOD])->VBClass;
			for(DWORD c=0; c<3; c++){
				if(VbC& 1<<c) VbClass += VB_Classes[c] + " ";
			}
			if(VbClass=="") VbClass="! NO VB CLASS !";
			VbClass = " : " + VbClass;
		}
		Name="";
		if(LOD!=-1)	Name.Format("\t%d  %6.0f :  %4d - %32s", a, Dist, LOD, GetIndexName(LOD));
		else 	Name.Format("\t%d  %6.0f :  * Not assigned *", a, 0.0f);
		puts(Name  + VbClass);
	}

	if(Parent.nSlots){
		puts("\n\t----------------------------");
		puts("\tSlots Coords ( X, Y ,Z )\n");
		D3DXVECTOR3	Coords;
		for(DWORD a=0; a<Parent.nSlots; a++){
			Coords = GetParentSlots(PARENT, a);
			Data.Format("\t%d\t%12f,  %12f,  %12f", a, Coords.x, Coords.y, Coords.z), puts(Data);
		}
	}



	puts("\n");
	return "End\n\n\n";
}


CString	UnusedTextures(char *Argument)
{
	bool	Hits = false, Free = false;
	
	puts("TEXTURES\n\n");

	// Get what is requested in the report
	CString Value;
	
	// Max textures
	DWORD	MaxT = GetMaxDbTexture();
	// retport it
	Value.Format("DB Supported Textures : %d\n", MaxT);
	puts(Value);
	
	// Used texturs array
	DWORD	*TexUsed = (DWORD*)malloc(sizeof(DWORD) * MaxT);
	// Defaults to 0 ( not used )
	memset(TexUsed, 0, sizeof(DWORD) * MaxT);

	
	
	// evaluate the opertors	
	while((Value=GetCommand())!=""){
		if(Value == "USED") Hits = true;
		if(Value == "FREE") Free = true;
	};
	
	// if no operatur, fail
	if(!Hits && ! Free) return("Missing operator \"USED | FREE\"");

	// thru all Lods
	for(DWORD i=0; i<MAX_LOD; i++){
		DxDbHeader *Hdr = (DxDbHeader*)Models[i];

		if(i==2700)
			_asm nop;
		// if a Model is present
		if(Hdr){
			// for each texture present in the Model
			for(DWORD t=0; t < Hdr->dwTexNr; t++){
				DWORD	TexNr = GetModelTexture(Hdr, t);
				if(TexNr!=-1) TexUsed[TexNr]++;
			}
		}
	}

	// Report results

	// * USED TEXTURES *
	if(Hits){
		puts("Textures Hits -------------------------------------------------------------\n");
		DWORD	Count = 0;
		for( DWORD t = 0; t<MaxT; t++){
			if(TexUsed[t]) Count++, Value.Format("%04d : Texture %d - %d Hits", Count, t, TexUsed[t]), puts(Value);
		}
		puts("\nTextures Hits - END -------------------------------------------------------\n");
	}

	// * UNUSED / FREE TEXTURES *
	if(Free){
		puts("Textures Free -------------------------------------------------------------\n");
		DWORD Count = 0;
		for( DWORD t = 0; t<MaxT; t++){
			if(!TexUsed[t]) Count++, Value.Format("%04d : Texture %d", Count, t), puts(Value);
		}
		puts("\nTextures Free - END -------------------------------------------------------\n");
	}



	free(TexUsed);

	return "";
}



CString	UnusedParents(char *Argument)
{
	puts("PARENTS Test\n");
	CString Text;
	HDRPARENT	p;
	DWORD		Unused = 0;

	Text.Format("Supported Parents %d\n", GetMaxDbParent());
	puts(Text);
	puts("**************************************************\n");
	puts("UNUSED PARENTS... Analizing...\n");
	// Get the Max available Parents
	DWORD	Parents = GetMaxDbParent();
	// Look thru all supported parents
	for(DWORD i=0; i<Parents; i++){
		//Get the Parent Data
		GetParentData(i, &p);
		// if no Lods assigned
		if(!p.nLODs){
			Text.Format("# %04d : NO LODs assigned", i);
			puts(Text);
			Unused++;
		}
	}

	Text.Format("\nTotal Unused : %d\n", Unused);
	puts(Text);

	puts("**************************************************");
	return "END\n";
}


///////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\







void	Log(CString Text)
{
	 if(LogOn) puts(Text);
	// { ((CDxDb2Dlg*)m_pMainWnd)->Log(Text); 	Sleep(10); }

}




void	Commands(CString Command, char *Line)
{
	const ParseItem	*ptr=&Coms[0];
	while(ptr->Exe){
		if(Command==ptr->CommandName) {Log(ptr->CommandName + " : " + ptr->Exe(Line)); DataModified|=ptr->Modify; return; }
		ptr++;
	}

}



// Returns true if a passed string is composed onlyby digits
bool	CheckDigit(CString Value)
{
	for(int a=0; a<Value.GetLength(); a++) if(!isdigit(Value[a])) return false;
	return true;
}


void	Switches(CString Command)
{
	char	Switch=Command[0];

	switch(Switch){
						// LOG ON, create the window
		case	'V'	:	LogOn=true;
						Log("\nDxDb3 - Version 1.21 - [R]edViper 2008\n");
						break;

						// nCT Number - Name
		case	'C'	:	nCT=PARENT=LOD-1;
						Command=GetCommand();
						if(CheckDigit(Command)) nCT=atoi(Command);
						break;

		case	'P'	:	PARENT=LOD=-1;
						Command=GetCommand();
						if(Command[0]=='#'){
							Command=Command.Right(Command.StringLength(Command)-1);
							PARENT=atoi(Command);
						} else {
							if(nCT==-1) { Log("ERROR - nCT Not specified") ;return;}
							PARENT=GetParentOfCT(nCT, atoi(Command));
						}
						break;

		case	'L'	:	LOD=-1;
						Command=GetCommand();
						if(Command[0]=='#'){
							Command=Command.Right(Command.StringLength(Command)-1);
							LOD=atoi(Command);
						} else {
							if(CheckDigit(Command)){
								if(PARENT==-1) { Log("ERROR - PARENT Not specified") ;return;}
								// this is used for NEW Lods
								LODLEVEL = atoi(Command);
								LOD=GetParentLodID(PARENT, atoi(Command));
							} else {
								LOD=GetLodIDFromName(Command);
								if(LOD!=-1){
									CString t;
									t.Format("LOD %d : %s", LOD, GetIndexName(LOD)); 
									Log(t);
								} else {
									Log("Invalid #MODEL");
								}
							}
						}
						break;
	}
}





bool	OpenDBFile(CString FileName)
{
	CString	fn=FileName;
	// rrmove eventually extensions
	if(fn.Right(4)==".DXH") fn=fn.Left(fn.StringLength(fn)-4);
	if(fn.Right(4)==".DXL") fn=fn.Left(fn.StringLength(fn)-4);
	
	// The DXH File
	Log("Opening file " + fn + ".DXH");
	if(!LoadHDR(fn+".DXH")) { Log("File Not found "+fn+".DXH"); return false; }

	// The DXL File
	Log("Opening file " + fn + ".DXL");
	if(!InitDbaseIN(fn+".DXL"))  { Log("File Not found "+fn+".DXL"); return false; }

	DBName=fn;

	memset(Models, 0, sizeof(Models));

	// Allocate models in memory
	Log("Loading Models...");
	DWORD	Idx=0;
	void	*ptr;
	while(ptr=GetNextModel()){
		Models[((DxDbHeader*)ptr)->Id]=ptr;
		Idx++;
	}
	CloseDBaseIN();
	CString	t; t.Format("Loaded Models %d\n", Idx);
	Log(t);



	return true;

}

char	*SplitLine(char *CommandLine)
{
	// Get the Line till end
	char	*Line = strchr(CommandLine, '\n');
	
	Line[0]=0;
	return &Line[1];
}

void	CommandParse()
{
	char	*CommandList, *Pool;
	
	DbOpened=false;
	bool	Start=true;
	bool	Script=false;
	// Get the command line
	CommandList=(char*) malloc(strlen(m_lpCmdLine)+32);
	Pool = CommandList;
	strcpy(CommandList, m_lpCmdLine);
	strcat(CommandList, "\n #END#\n");


	// Parsing of Command Line
	CString	Delimit=" \n";
	CString Quotes="\"";
	CString	Command;
	CFile F;
	nCT=LOD=PARENT=-1;


	char	*Line;
	
	do{
		// Assign the Line start
		Line = CommandList;
		// and Split te line from rest of commands list
		CommandList = SplitLine(CommandList);
		// New line
		Start = true;
		do{
			// Get next command
			Command=GetCommand(Start ? Line : NULL), Start=false;

			switch(Command[0]){

								// * COMMANDS FORM A FILE *
								// Get the FileName
				case	'@'	:	Command=Command.Right(Command.StringLength(Command)-1);
								// Unallocate the previous list
								free(Pool);
								// Read the File
								if(!F.Open(Command, CFile::modeRead)) return;
								CommandList=(char*)malloc((DWORD)F.GetLength()+32);
								Pool = CommandList;
								memset(CommandList, 0, (DWORD)F.GetLength()+1);
								F.Read((void*)CommandList, (DWORD)F.GetLength());
								F.Close();
								strcat(CommandList, "\n #END#\n");
								Command = "",

								Start=true;
								Script=true;

								break;

								// * DATABASE FILE & PATH *
								// get the fileName
				case	'+'	:	Command=Command.Right(Command.StringLength(Command)-1);
								// If DB already opened exit
								if(DbOpened) break;
								// File Open failure
								if(!OpenDBFile(Command)) return; 

								break;

								// * SWITCHES *
				case	'-'	:	Command=Command.Right(Command.StringLength(Command)-1);
								Switches(Command);
								break;

								// * END OF COMMANDS *
				case	0	:	break;

				default		:	Commands(Command, Line);


			

			}


		} while(Command!="#END#" && Command!="");

	} while(Command!="#END#" && CommandList!="");

	free(Pool);

}


void SaveDb(void)
{
	CFile	F;
	DWORD	Index=0;
	
	if(!InitDbaseOUT(DBName+".__l")) return;
	Log("Saving DXL...");
	while(Index<MAX_LOD){
		if(Models[Index]) WriteNextModel(Models[Index]);
		Index++;
	}
	CloseDBaseOUT();
	Log("Saving DXH...");
	if(!SaveHDR(DBName+".__h")) return;

	DeleteFile(DBName+".dxl");
	DeleteFile(DBName+".dxh");

	rename(DBName+".__l",DBName+".dxl");
	rename(DBName+".__h",DBName+".dxh");
	Log("Done...");

}









int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	DataModified=false;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{

		char	x[1024];
		if(argc==1) { puts("Missing commands...."); return 1; }
		strcpy(x, argv[1]);
		m_lpCmdLine=x;
		CommandParse();
		if(DataModified) SaveDb();
		// Unallocate memory
		DWORD	Idx=0;
		while(Idx<MAX_LOD) {  if(Models[Idx]) free(Models[Idx]); Idx++; }
	}

	return nRetCode;
}


void	IdleMode(void)
{
}