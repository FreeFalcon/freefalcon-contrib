//**************************************************************************************
//* DX Nodes structures                                                                *
//* 2005 [R]ed                                                                         *
//* This nodes are just used as intermediate transformation from BSP to DX format      *
//**************************************************************************************

#pragma once

#include "DxDefines.h"
#include "DXVbmanager.h"



class	DXNode 
{
	public:
		DXNode(NodeType T=ROOT);
		~DXNode() {};

	public:
		DWORD			NodeNumber, NodeSize;
		NodeType		Type;
		DWORD			SelectID,SelectV;
		D3DCOLORVALUE	FrameColour;
		DXNode			*Next,*Prev;
		HTREEITEM		TreeItem;

};


class	DXVertex: public DXNode
{
	public:
		DXVertex(NodeType T=DOT);
		~DXVertex();

	public:
		D3DVECTOR		FaceNormal;
		DxSurfaceType	Surface;
		D3DVERTEXEX		Vertex;					// Vertex Data
};


class	DXLine: public DXNode
{
	public:
		DXLine(NodeType T=LINE);
		~DXLine();

	public:
		D3DVECTOR		FaceNormal;
		DxSurfaceType	Surface;
		D3DVERTEXEX	Vertex[2];					// Vertex Data
};



class	DXTriangle: public DXNode
{
	public:
		DXTriangle(NodeType T=TRIANGLE);
		~DXTriangle();

	public:
		D3DVECTOR		FaceNormal;
		DxSurfaceType	Surface;
		D3DVERTEXEX		Vertex[3];					// Vertex Data
};




class	DXDof: public DXNode
{
	public:
		DXDof(NodeType T=DOF);
		~DXDof();

	public:
		DxDofType	dof;
};


class	DXSlot: public DXNode
{
	public:
		DXSlot(NodeType T=SLOT);
		~DXSlot();

	public:
		DxSlotType	Slot;
};


class	DXEndDof: public DXNode
{
	public:
		DXEndDof(NodeType T=CLOSEDOF);
		~DXEndDof();

	public:
		DxDofEndType	Sign;
		
};
