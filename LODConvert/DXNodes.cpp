#include "stdafx.h"
#include "Polylib.h"
#include "DXNodes.h"

extern	DWORD	NodeCount;

DXNode::DXNode(NodeType T)
{
	NodeSize=sizeof(this);
	Type=T;
	Next=Prev=NULL;
	NodeNumber=NodeCount;
}


DXVertex::DXVertex(NodeType T)
{
	memset(this,0,sizeof(DXVertex));
	NodeSize=sizeof(DXVertex);
	Type=T;
	Texture=-1;
	NodeNumber=NodeCount;
}

DXTriangle::DXTriangle(NodeType T)
{
	memset(this,0,sizeof(DXTriangle));
	NodeSize=sizeof(DXTriangle);
	Type=T;
	Texture=-1;
	NodeNumber=NodeCount;
}

DXLine::DXLine(NodeType T)
{
	memset(this,0,sizeof(DXLine));
	NodeSize=sizeof(DXLine);
	Type=T;
	Texture=-1;
	NodeNumber=NodeCount;
}


DXDof::DXDof(NodeType T)
{
	memset(this, 0, sizeof(DXDof));
	NodeSize=sizeof(DXDof);
	Type=T;
	NodeNumber=NodeCount;
}


DXDofData::DXDofData(DofType T)
{
	Type=T;
	dofNumber=-1;
}

DXVertex::~DXVertex()
{
}

DXLine::~DXLine()
{
}

DXTriangle::~DXTriangle()
{
}



DXDofData::~DXDofData()
{
}

DXDof::~DXDof()
{
}


DXSlot::DXSlot(NodeType T)
{
	Type=T;
	SlotNr=-1;
	NodeSize=sizeof(DXSlot);
	NodeNumber=NodeCount;
}


DXSlot::~DXSlot()
{
}
