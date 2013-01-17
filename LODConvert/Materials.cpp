#include	"stdafx.h"
#include	"ObjectInstance.h"
#include	"LodConvert.h"
#include	"BSPNodes.h"
#include	"DXnodes.h"
#include	"Encoder.h"
#include	"d3d9types.h"
#include	"Materials.h"
#include	"DXEngine.h"
#include	"ClassTbl.h"

DXLightType	ModelLights[128];
DWORD		AssignedLights;
DWORD		LastLightSwitch;
DWORD		PNumber,ModelType;
bool		CheckedVertex[65536], AssignedTaxi;

void	TransformAlpha (D3DVERTEXEX *VPool, DWORD nV, Int16 *Vp, float AlphaMax, float AlphaCx)
{

	memset(CheckedVertex, false, sizeof(CheckedVertex));
	while(nV){
		if(!CheckedVertex[*Vp]){
			CheckedVertex[*Vp]=true;
			DWORD	col=(VPool+(*Vp))->dwColour;
			float	a=((float)((col>>24)&0xff))/255.0f;
			if(a>=AlphaMax){
				a*=AlphaCx;//LANDING_LIGHT_ALPHA_CX;
				if(a>1.0f) a=1.0f;
				(VPool+(*Vp))->dwColour=(VPool+(*Vp))->dwColour&0x00ffffff|((DWORD)(a*255.9)<<24);
			}
		}
		Vp++;
		nV--;
	}
}

void	TransformSpecularity (D3DVERTEXEX *VPool, DWORD nV, Int16 *Vp, float rCx, float gCx, float bCx)
{

	memset(CheckedVertex, false, sizeof(CheckedVertex));
	while(nV){
		if(!CheckedVertex[*Vp]){
			CheckedVertex[*Vp]=true;
			DWORD	col=(VPool+(*Vp))->dwSpecular;
			float	r=((float)((col>>16)&0xff))/255.0f;
			float	g=((float)((col>>8)&0xff))/255.0f;
			float	b=((float)((col)&0xff))/255.0f;

			r*=rCx;
			g*=gCx;
			b*=bCx;

			if(r>1.0f) r=1.0f;
			if(g>1.0f) g=1.0f;
			if(b>1.0f) b=1.0f;

			(VPool+(*Vp))->dwSpecular=(VPool+(*Vp))->dwSpecular&0xff000000|((DWORD)(r*255.9)<<16)|((DWORD)(g*255.9)<<8)|((DWORD)(b*255.9));
		}
		Vp++;
		nV--;
	}
}


void	TransformDiffuse(D3DVERTEXEX *VPool, DWORD nV, Int16 *Vp, float SpecCx)
{

	memset(CheckedVertex, false, sizeof(CheckedVertex));
	while(nV){
		if(!CheckedVertex[*Vp]){
			CheckedVertex[*Vp]=true;
			DWORD	col=(VPool+(*Vp))->dwColour;
			float	r=((float)((col>>16)&0xff))/255.0f;
			float	g=((float)((col>>8)&0xff))/255.0f;
			float	b=((float)((col)&0xff))/255.0f;

			r*=SpecCx;
			g*=SpecCx;
			b*=SpecCx;

			if(r>1.0f) r=1.0f;
			if(g>1.0f) g=1.0f;
			if(b>1.0f) b=1.0f;

			(VPool+(*Vp))->dwColour=(VPool+(*Vp))->dwSpecular&0xff000000|((DWORD)(r*255.9)<<16)|((DWORD)(g*255.9)<<8)|((DWORD)(b*255.9));
		}
		Vp++;
		nV--;
	}
}


void	AssignSurfaces(NodeScannerType	NODE, D3DVERTEXEX *VPool, DWORD PresentSwitch, DWORD PresentDof)
{
	Int16	*VList=(Int16*)(NODE.BYTE+sizeof(DxSurfaceType));

	//Default Specularity
	ASSIGN_SURFACE_FEATURE((true), MATERIAL);

	// EXHAUST NOZZLE FEATURES
	ASSIGN_SURFACE_FEATURE((PresentDof==EXHAUST_NOZZLE_DOF), EXHAUST_NOZZLE);

	// LEFS
	ASSIGN_SURFACE_FEATURE((PresentDof==LT_LEF_DOF || PresentDof==RT_LEF_DOF), LEF);

	// TEFS
	ASSIGN_SURFACE_FEATURE((PresentDof==LT_TEF_DOF || PresentDof==RT_TEF_DOF), TEF);

	// FLAPS
	ASSIGN_SURFACE_FEATURE((PresentDof==LT_FLAP_DOF || PresentDof==RT_FLAP_DOF), FLAP);

	// AIRBRAKES
	ASSIGN_SURFACE_FEATURE((PresentDof==LT_AB_TOP_DOF || PresentDof==LT_AB_BOT_DOF ||
		PresentDof==RT_AB_TOP_DOF || PresentDof==RT_AB_BOT_DOF), AIRBRAKE);

	// CANOPY
	if( gClass==7 && (PresentDof==CANOPY_DOF || PresentSwitch==CANOPY_DOF || 
		(PresentDof==-1 && PresentSwitch==-1 && NODE.SURFACE->dwFlags.b.Lite && NODE.SURFACE->dwFlags.b.VColor )) && NODE.SURFACE->dwFlags.b.Alpha){
		NODE.SURFACE->SpecularIndex=CANOPY_INDEX;
		DWORD	nV=NODE.SURFACE->dwVCount;
		Int16	*Vp=VList;
		while(nV--){
			if(NODE.SURFACE->dwFlags.b.Texture){
				(VPool+(*Vp++))->dwSpecular=CANOPY_SPECULARITY;
			} else {
				DWORD	col=(VPool+(*Vp))->dwColour;

				float	a=((float)((col>>24)&0xff))/255.0f;
				float	r=((float)((col>>16)&0xff))/255.0f;
				float	g=((float)((col>>8)&0xff))/255.0f;
				float	b=((float)(col&0xff))/255.0f;
				
				float	m=1/r;
				if((1/g)<m) m=1/g;
				if((1/b)<m) m=1/b;

				a*=0.9f;
				(VPool+(*Vp))->dwColour=(VPool+(*Vp))->dwColour&0x00ffffff|((DWORD)(a*255.9)<<24);

				a=255;
				r*=m;
				g*=m;
				b*=m;

				col=((DWORD)a<<24)||((DWORD)r<<16)||((DWORD)r<<8)||((DWORD)b);

				(VPool+(*Vp++))->dwSpecular=col;
			}
		}

		}

////////////////////////////////////////////////////////////////////////////////////////////////////
	// LANDING LIGHTS
	if(PNumber==1332 || (gClass==7 && PresentSwitch==LANDING_LIGHT_SWITCH && (!NODE.SURFACE->dwFlags.b.Lite) && NODE.SURFACE->dwFlags.b.VColor && NODE.SURFACE->dwFlags.b.Alpha && NODE.SURFACE->dwFlags.b.Poly)){
		DWORD	nV=NODE.SURFACE->dwVCount;
		Int16	*Vp=VList;


		while(nV){
			float	d=__max(fabs((VPool+(*Vp))->vx-(VPool+(*Vp+1))->vx), fabs((VPool+(*Vp))->vx-(VPool+(*Vp+2))->vx));
			if(d>5.0f){
				for(int i=0; i<3; i++){
					DWORD	col=(VPool+(*Vp+i))->dwColour;
					float	a=((float)((col>>24)&0xff))/255.0f;
					a*=(PNumber==1332)?0.9:0.2f;//LANDING_LIGHT_ALPHA_CX;
					if(a>1.0f) a=1.0f;
					(VPool+(*Vp+i))->dwColour=(VPool+(*Vp+i))->dwColour&0x00ffffff|((DWORD)(a*255.9)<<24);

					col=(VPool+(*Vp+i))->dwSpecular;
					a=((float)((col>>24)&0xff))/255.0f;
					a*=(PNumber==1332)?0.9:0.2f;//LANDING_LIGHT_ALPHA_CX;
					if(a>1.0f) a=1.0f;
					(VPool+(*Vp+i))->dwSpecular=(VPool+(*Vp+i))->dwSpecular&0x00ffffff|((DWORD)(a*255.9)<<24);

				}
			}
			Vp+=3;
			nV-=3;
		}
	}
	

	////////////////////////////////// EMISSIVE FEATURES //////////////////////////////////////////////
	bool	MakeEmissive=false;

	// Runway Lights
	if(/*PresentSwitch==0 &&*/ gClass==2 && gType==46) MakeEmissive=true;
	// SmokeStack Flame
	if(gClass==2 /*&& gType==51*/ && NODE.SURFACE->dwFlags.b.BillBoard  && NODE.SURFACE->dwFlags.b.Alpha ) MakeEmissive=true;
	// Landing Lights DOTs
	if(gClass==7 && PresentSwitch==LANDING_LIGHT_SWITCH && NODE.SURFACE->dwFlags.b.Point && NODE.SURFACE->dwFlags.b.Lite) MakeEmissive=true;

	if(MakeEmissive){
		NODE.SURFACE->dwFlags.b.Lite=0;
		DWORD	nV=NODE.SURFACE->dwVCount;
		Int16	*Vp=VList;
		if(NODE.SURFACE->dwFlags.b.Texture){
			(VPool+(*Vp))->dwSpecular=0xffffffff;
			Vp++;
		} else {
			while(nV--){
				(VPool+(*Vp))->dwSpecular=(VPool+(*Vp))->dwColour;
				Vp++;
			}
		}
	}

	// LIGHTS
	if((gClass==7 || ModelType==TYPE_2D_PIT || ModelType==TYPE_3D_PIT ) && (PresentSwitch==LANDING_LIGHT_SWITCH || PresentSwitch==NAV_LIGHT_SWITCH) && AssignedLights<128 && NODE.SURFACE->dwFlags.b.Point){

		Int16	*Vp=VList;
		ModelLights[AssignedLights].Switch=PresentSwitch;
		ModelLights[AssignedLights].SwitchMask=0x01;

		if(PresentSwitch==NAV_LIGHT_SWITCH){
			if(NODE.SURFACE->dwFlags.b.Point){
				// assign the light position
				ModelLights[AssignedLights].Light.dvPosition=*((D3DVECTOR*)(VPool+(*Vp)));
			} else {
				ModelLights[AssignedLights].Light.dvPosition=*((D3DVECTOR*)(VPool+(*Vp)));
				if(ModelLights[AssignedLights].Light.dvPosition.x>(VPool+(*Vp+1))->vx)
					ModelLights[AssignedLights].Light.dvPosition=*((D3DVECTOR*)(VPool+(*Vp+1)));
				if(ModelLights[AssignedLights].Light.dvPosition.x>(VPool+(*Vp+2))->vx)
					ModelLights[AssignedLights].Light.dvPosition=*((D3DVECTOR*)(VPool+(*Vp+2)));

			}

			// check for near lights
			for(int a=0; a<AssignedLights; a++){
				float	xd=ModelLights[AssignedLights].Light.dvPosition.x-ModelLights[a].Light.dvPosition.x;
				float	yd=ModelLights[AssignedLights].Light.dvPosition.y-ModelLights[a].Light.dvPosition.y;
				float	zd=ModelLights[AssignedLights].Light.dvPosition.z-ModelLights[a].Light.dvPosition.z;
				
				float	d=sqrtf(xd*xd+yd*yd+zd*zd);

				// if a light at less than 1 feet abort adding light
				if(d<=1.0f)	goto Done;

			}
			

			// This light is illuminating only the model itself
			ModelLights[AssignedLights].Flags.OwnLight=true;

			ModelLights[AssignedLights].Light.dcvDiffuse.r=(float)(((VPool+(*Vp))->dwColour&0xff0000)>>16)/500.0f;
			ModelLights[AssignedLights].Light.dcvDiffuse.g=(float)(((VPool+(*Vp))->dwColour&0xff00)>>8)/500.0f;
			ModelLights[AssignedLights].Light.dcvDiffuse.b=(float)(((VPool+(*Vp))->dwColour&0xff))/500.0f;
			ModelLights[AssignedLights].Light.dcvSpecular.r=(float)(((VPool+(*Vp))->dwColour&0xff0000)>>16)/1000.0f;
			ModelLights[AssignedLights].Light.dcvSpecular.g=(float)(((VPool+(*Vp))->dwColour&0xff00)>>8)/1000.0f;
			ModelLights[AssignedLights].Light.dcvSpecular.b=(float)(((VPool+(*Vp))->dwColour&0xff))/1000.0f;
			ModelLights[AssignedLights].Light.dvRange=3.0f;
			ModelLights[AssignedLights].Light.dvAttenuation0=1.0f;
			ModelLights[AssignedLights].Light.dvAttenuation1=0.0f;
			ModelLights[AssignedLights].Light.dltType=D3DLIGHT_POINT;
				ModelLights[AssignedLights].Light.dcvAmbient.r=ModelLights[AssignedLights].Light.dcvDiffuse.r/3;
				ModelLights[AssignedLights].Light.dcvAmbient.g=ModelLights[AssignedLights].Light.dcvDiffuse.g/3;
				ModelLights[AssignedLights].Light.dcvAmbient.b=ModelLights[AssignedLights].Light.dcvDiffuse.b/3;
			
			// TAIL STROBO, a little stronger
			if(ModelLights[AssignedLights].Light.dcvDiffuse.r==ModelLights[AssignedLights].Light.dcvDiffuse.g && 
				ModelLights[AssignedLights].Light.dcvDiffuse.r==ModelLights[AssignedLights].Light.dcvDiffuse.b && 
				ModelLights[AssignedLights].Light.dvPosition.z<-5.0f ){
				ModelLights[AssignedLights].Light.dcvDiffuse.r*=1.5;
				ModelLights[AssignedLights].Light.dcvDiffuse.g*=1.5;
				ModelLights[AssignedLights].Light.dcvDiffuse.b*=1.5;
				ModelLights[AssignedLights].Light.dvAttenuation0=0.5f;
				ModelLights[AssignedLights].Light.dvRange=3.0f;
				ModelLights[AssignedLights].Light.dcvAmbient.r=0.15f;
				ModelLights[AssignedLights].Light.dcvAmbient.g=0.15f;
				ModelLights[AssignedLights].Light.dcvAmbient.b=0.15f;
			}

			
		}

		if(PresentSwitch==LANDING_LIGHT_SWITCH){
			if(NODE.SURFACE->dwFlags.b.Point){
				// assign the light position
				ModelLights[AssignedLights].Light.dvPosition=*((D3DVECTOR*)(VPool+(*Vp)));
			} else {
				ModelLights[AssignedLights].Light.dvPosition=*((D3DVECTOR*)(VPool+(*Vp)));
				if(ModelLights[AssignedLights].Light.dvPosition.x>(VPool+(*Vp+1))->vx)
					ModelLights[AssignedLights].Light.dvPosition=*((D3DVECTOR*)(VPool+(*Vp+1)));
				if(ModelLights[AssignedLights].Light.dvPosition.x>(VPool+(*Vp+2))->vx)
					ModelLights[AssignedLights].Light.dvPosition=*((D3DVECTOR*)(VPool+(*Vp+2)));

			}


			// check for near lights
			for(int a=0; a<AssignedLights; a++){
				float	xd=ModelLights[AssignedLights].Light.dvPosition.x-ModelLights[a].Light.dvPosition.x;
				float	yd=ModelLights[AssignedLights].Light.dvPosition.y-ModelLights[a].Light.dvPosition.y;
				float	zd=ModelLights[AssignedLights].Light.dvPosition.z-ModelLights[a].Light.dvPosition.z;
				
				float	d=sqrtf(xd*xd+yd*yd+zd*zd);

				// if a light at less than 1 feet abort adding light
				if(d<=1.0f)	goto Done;

			}

			// This light is illuminating not only the asme model
			ModelLights[AssignedLights].Flags.OwnLight=false;
			ModelLights[AssignedLights].Light.dcvDiffuse.r=1.0f;
			ModelLights[AssignedLights].Light.dcvDiffuse.g=1.0f;
			ModelLights[AssignedLights].Light.dcvDiffuse.b=1.0f;
			ModelLights[AssignedLights].Light.dcvSpecular.r=1.0f;
			ModelLights[AssignedLights].Light.dcvSpecular.g=1.0f;
			ModelLights[AssignedLights].Light.dcvSpecular.b=1.0f;
			ModelLights[AssignedLights].Light.dcvAmbient.r=0.0f;
			ModelLights[AssignedLights].Light.dcvAmbient.g=0.0f;
			ModelLights[AssignedLights].Light.dcvAmbient.b=0.0f;
			ModelLights[AssignedLights].Light.dvRange=1500.0f;
			ModelLights[AssignedLights].Light.dvAttenuation0=0.1f;
			ModelLights[AssignedLights].Light.dvAttenuation1=0.01f;
			ModelLights[AssignedLights].Light.dltType=D3DLIGHT_SPOT;
			ModelLights[AssignedLights].Light.dvTheta=0.1f;
			ModelLights[AssignedLights].Light.dvPhi=1.3f;
			ModelLights[AssignedLights].Light.dvFalloff=1.0f;
			ModelLights[AssignedLights].Light.dvDirection.z=0.23f;
			ModelLights[AssignedLights].Light.dvDirection.y=0.0f;
			ModelLights[AssignedLights].Light.dvDirection.x=1.0f;
		}

		AssignedLights++;

	}

Done:

	// Missile Flame
	if(PNumber==VIS_MFLAME_S || PNumber==VIS_MFLAME_L){
		ModelLights[AssignedLights].Flags.OwnLight=false;
		ModelLights[AssignedLights].Light.dcvDiffuse.r=1.0f;
		ModelLights[AssignedLights].Light.dcvDiffuse.g=1.0f;
		ModelLights[AssignedLights].Light.dcvDiffuse.b=0.7f;
		ModelLights[AssignedLights].Light.dcvSpecular.r=1.0f;
		ModelLights[AssignedLights].Light.dcvSpecular.g=1.0f;
		ModelLights[AssignedLights].Light.dcvSpecular.b=0.7f;
		ModelLights[AssignedLights].Light.dcvAmbient.r=0.0f;
		ModelLights[AssignedLights].Light.dcvAmbient.g=0.0f;
		ModelLights[AssignedLights].Light.dcvAmbient.b=0.0f;
		ModelLights[AssignedLights].Light.dvRange=30.0f;
		ModelLights[AssignedLights].Light.dvAttenuation0=0.5f;
		ModelLights[AssignedLights].Light.dvAttenuation1=0.1f;
		ModelLights[AssignedLights].Light.dltType=D3DLIGHT_POINT;
		ModelLights[AssignedLights].Light.dvTheta=0.1f;
		ModelLights[AssignedLights].Light.dvPhi=0.1f;
		ModelLights[AssignedLights].Light.dvFalloff=1.0f;
		ModelLights[AssignedLights].Light.dvDirection.z=0.0f;
		ModelLights[AssignedLights].Light.dvDirection.y=0.0f;
		ModelLights[AssignedLights].Light.dvDirection.x=-1.0f;
		ModelLights[AssignedLights].Light.dvPosition.x=0.0f;
		ModelLights[AssignedLights].Light.dvPosition.y=0.0f;
		ModelLights[AssignedLights].Light.dvPosition.z=0.0f;
		ModelLights[AssignedLights].Switch=-1;
		AssignedLights++;
	}

	// Bias for Translucent surfaces & CromaKey one
	if((/*NODE.SURFACE->dwFlags.b.Alpha || */NODE.SURFACE->dwFlags.b.ChromaKey)&&(!NODE.SURFACE->dwFlags.b.zBias)){
		NODE.SURFACE->dwFlags.b.zBias=1;
		NODE.SURFACE->dwzBias=1;
	}

	// PITs
	if(ModelType==TYPE_2D_PIT || ModelType==TYPE_3D_PIT){

		// Canopy Alpha
		if(PresentSwitch==3 || PresentSwitch==29){
			NODE.SURFACE->dwFlags.b.Lite=0;
			NODE.SURFACE->dwFlags.b.VColor=1;
			NODE.SURFACE->dwFlags.b.Gouraud=0;
			TransformAlpha(VPool, NODE.SURFACE->dwVCount, VList, 2.0, 0.0);
			TransformSpecularity(VPool, NODE.SURFACE->dwVCount, VList, 1.0f, 1.0f, 1.0f);
			TransformDiffuse(VPool, NODE.SURFACE->dwVCount, VList, 0.0);
		} else {
			// unlited surfaces
			if(NODE.SURFACE->dwFlags.b.Lite==0 && PresentSwitch==-1) {
				NODE.SURFACE->dwFlags.b.Lite=1;
				TransformSpecularity(VPool, NODE.SURFACE->dwVCount, VList, 0.15f, 0.15f, 0.15f);
			}
		}

		// ASSIGN THE TAXI LIGHT
		// This light is illuminating not only the asme model
		if(!AssignedTaxi){
			ModelLights[AssignedLights].Flags.OwnLight=false;
			ModelLights[AssignedLights].Light.dcvDiffuse.r=1.0f;
			ModelLights[AssignedLights].Light.dcvDiffuse.g=1.0f;
			ModelLights[AssignedLights].Light.dcvDiffuse.b=1.0f;
			ModelLights[AssignedLights].Light.dcvSpecular.r=1.0f;
			ModelLights[AssignedLights].Light.dcvSpecular.g=1.0f;
			ModelLights[AssignedLights].Light.dcvSpecular.b=1.0f;
			ModelLights[AssignedLights].Light.dcvAmbient.r=0.0f;
			ModelLights[AssignedLights].Light.dcvAmbient.g=0.0f;
			ModelLights[AssignedLights].Light.dcvAmbient.b=0.0f;
			ModelLights[AssignedLights].Light.dvRange=1500.0f;
			ModelLights[AssignedLights].Light.dvAttenuation0=0.1f;
			ModelLights[AssignedLights].Light.dvAttenuation1=0.01f;
			ModelLights[AssignedLights].Light.dltType=D3DLIGHT_SPOT;
			ModelLights[AssignedLights].Light.dvTheta=0.1f;
			ModelLights[AssignedLights].Light.dvPhi=1.3f;
			ModelLights[AssignedLights].Light.dvFalloff=1.0f;
			ModelLights[AssignedLights].Light.dvDirection.z=0.23f;
			ModelLights[AssignedLights].Light.dvDirection.y=0.0f;
			ModelLights[AssignedLights].Light.dvDirection.x=1.0f;
			ModelLights[AssignedLights].Light.dvPosition.x=-3.0f;
			ModelLights[AssignedLights].Light.dvPosition.y=0.0f;
			ModelLights[AssignedLights].Light.dvPosition.z=6.0f;
			ModelLights[AssignedLights].Flags.NotSelfLight=true;
			ModelLights[AssignedLights].Switch=LANDING_LIGHT_SWITCH ;
			ModelLights[AssignedLights].SwitchMask=0x01;
			AssignedLights++;
			AssignedTaxi=true;
		}
	}


	// Plane Shadows...
	if(gClass==7 && gPLevel==1){
		NODE.SURFACE->dwzBias=3;
		NODE.SURFACE->dwFlags.b.zBias=true;
	}
	
	NODE.SURFACE->SwitchNumber=0;
	NODE.SURFACE->SwitchMask=0;
	if(!NODE.SURFACE->dwFlags.b.Lite)
		NODE.SURFACE->DefaultSpecularity=0;
	NODE.SURFACE->dwFlags.b.SwEmissive=false;

}



void	AssignScripts(DxDbHeader* ModelHead)
{
	// if no script exit here
	if(!ModelHead->Scripts[0].Script) return;


	ScriptType *Script=&ModelHead->Scripts[0].Script;
	DWORD		*Argument=&ModelHead->Scripts[0].Arguments[0];

	
	switch((DWORD)*Script){
		
						//UH1 ***NEVER CALLED !!!! ***
		case	0:		*Script=SCRIPT_NONE;
						break;

						//AH64,
		case	1:		*Script=SCRIPT_HELY;
						Argument[2]=2;					// Dof
						Argument[1]=3;					// 4 dof
						((float*)Argument)[2]=540.0f;	// Degrees of rotation x seconds
						break;

						//Hokum,
		case	2:		*Script=SCRIPT_ROTATE;
						Argument[0]=2;				// Dof
						Argument[1]=3;				// just 1 dof
						((float*)Argument)[2]=360.0f;// Degrees of rotation x seconds
						break;

						//OneProp,
		case	3:		*Script=SCRIPT_ROTATE;
						Argument[0]=2;				// Dof
						Argument[1]=1;				// just 1 dof
						((float*)Argument)[2]=600.0f;// Degrees of rotation x seconds
						break;

						//C130,
		case	4:		*Script=SCRIPT_ROTATE;
						Argument[0]=2;				// Dof
						Argument[1]=4;				// just 1 dof
						((float*)Argument)[2]=200.0f;// Degrees of rotation x seconds
						break;

						//E3,
		case	5:		*Script=SCRIPT_ROTATE;
						Argument[0]=14;				// Dof
						Argument[1]=1;				// just 1 dof
						((float*)Argument)[2]=135.0f;// Degrees of rotation x seconds
						break;

						//VASIF,
		case	6:		*Script=SCRIPT_VASIF;
							break;

						//VASIN,
		case	7:		*Script=SCRIPT_VASIN;
							break;

						//Chaff,
		case	8:		*Script=SCRIPT_CHAFF;
							break;

						//Beacon,
		case	9:		*Script=SCRIPT_BEACON;
							break;

						//CollapseChute,
		case	10:		*Script=SCRIPT_CHUTEDIE;
							break;

						//LongBow,
		case	11:		*Script=SCRIPT_HELY;
						Argument[2]=2;					// Dof
						Argument[1]=4;					// 4 dof
						((float*)Argument)[2]=540.0f;	// Degrees of rotation x seconds
						break;

						//Cycle2,
		case	12:		*Script=SCRIPT_ANIMATE;
						Argument[0]=0;				// switch
						Argument[1]=100;			// Delay in mSec
						Argument[2]=2;				// Nr of Frames
						break;

						//Cycle4,
		case	13:		*Script=SCRIPT_ANIMATE;
						Argument[0]=0;				// switch
						Argument[1]=100;			// Delay in mSec
						Argument[2]=4;				// Nr of Frames
						break;

						//Cycle10,
		case	14:		*Script=SCRIPT_ANIMATE;
						Argument[0]=0;				// switch
						Argument[1]=100;			// Delay in mSec
						Argument[2]=10;				// Nr of Frames
						break;

						//TStrobe,
		case	15:		*Script=SCRIPT_ANIMATE;
						Argument[0]=7;				// switch
						Argument[1]=100;			// Delay in mSec
						Argument[2]=4;				// Nr of Frames
						break;

						//TU95,
		case	16:		*Script=SCRIPT_ROTATE;
						Argument[0]=2;					// Dof
						Argument[1]=8;					// 8 dof
						((float*)Argument)[2]=200.0f;	// Degrees of rotation x seconds
						break;

						//MeatBall,
		case	17:		*Script=SCRIPT_MEATBALL;
							break;

						//ComplexProp,
		case	18:		*Script=SCRIPT_ROTATE;
						Argument[0]=31;					// Dof
						Argument[1]=7;					// 7 dof
						((float*)Argument)[2]=200.0f;	// Degrees of rotation x seconds
						break;

		default:	*Script=SCRIPT_NONE;
						
	}

}



// *** This files assign Features to objects surfaces ****
DWORD	AssignSurfaceFeatures(BYTE*Model, DWORD CTN, DWORD MType)
{
	DWORD	DofLevel=0;
	DWORD	SwitchNumber[128];
	DWORD	DofNumber[128];
	DWORD	ActualSwitch=-1,ActualDof=-1;
	DWORD	*rt=(DWORD*)Model;
	D3DVERTEXEX	*VPool;

	NodeScannerType	NODE;

	memset(SwitchNumber, -1, sizeof(SwitchNumber));
	memset(DofNumber, -1, sizeof(DofNumber));
	
	AssignScripts((DxDbHeader*)Model);

	DWORD	dwNodesNr=((DxDbHeader*)rt)->dwNodesNr;		
	DWORD	pVPool=((DxDbHeader*)rt)->pVPool;
	DWORD	dwPoolSize=((DxDbHeader*)rt)->dwPoolSize;
	DWORD	dwNVertices=((DxDbHeader*)rt)->dwNVertices;
	PNumber=CTN;
	ModelType=MType;
	AssignedTaxi=false;
	
	// Skip Textures
	rt+=((DxDbHeader*)rt)->dwTexNr;
	*((DWORD*)&rt)+=sizeof(DxDbHeader);


	NODE.BYTE=(BYTE*)rt;

	VPool=(D3DVERTEXEX*)(Model+pVPool);

	AssignedLights=0;
	LastLightSwitch=0;
	memset(ModelLights, 0, sizeof(ModelLights));
	


	// thru all the model
	while(NODE.HEAD->Type!=DX_MODELEND){
				// Selects actions for each node
		switch(NODE.HEAD->Type){

			case	DX_ROOT:		break;

			case	DX_SURFACE:		AssignSurfaces(NODE, VPool, ActualSwitch, ActualDof);
									break;

									// Dofs management
			case	DX_DOF:			if( NODE.DOF->Type==SWITCH || NODE.DOF->Type==XSWITCH ){
										ActualSwitch=NODE.DOF->SwitchNumber;
									} else {
										ActualDof=NODE.DOF->dofNumber;
									}
									DofLevel++;
									SwitchNumber[DofLevel]=ActualSwitch;
									DofNumber[DofLevel]=ActualDof;
									break;

			case	DX_ENDDOF:		if(DofLevel){
										DofLevel--;
										ActualSwitch=LastLightSwitch=SwitchNumber[DofLevel];
										ActualDof=DofNumber[DofLevel];
									}
									break;

			case	DX_SWITCH:		
									break;

			case	DX_LIGHT:		break;


		}
		
		NODE.BYTE+=NODE.HEAD->dwNodeSize;
	}

	if(AssignedLights){
		// paged on 4 bytes
		DWORD	DataSize=sizeof(DXLightType)*AssignedLights;
		if(DataSize%4) DataSize+=4-(DataSize%4);

		// move vertices up in the model
		memcpy(((char*)VPool)+DataSize, VPool, ((DxDbHeader*)Model)->dwNVertices*VERTEX_STRIDE);
		memcpy(VPool, ModelLights, DataSize);

		((DxDbHeader*)Model)->pLightsPool=((DxDbHeader*)Model)->pVPool;
		((DxDbHeader*)Model)->dwLightsNr=AssignedLights;
		((DxDbHeader*)Model)->ModelSize+=DataSize;
		((DxDbHeader*)Model)->pVPool+=DataSize;
	}

	return ((DxDbHeader*)Model)->ModelSize;
}
