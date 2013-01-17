
#include "stdafx.h"
#include "Polylib.h"
#include "dxvbmanager.h"
#include "DXNodes.h"

DWORD	NodeCount;

DXNode::DXNode(NodeType T)
{
	NodeSize=sizeof(this);
	Type=T;
	Next=Prev=NULL;
	NodeNumber=NodeCount;
	SelectID=0;
	SelectV=0;
	TreeItem=NULL;
}


DXVertex::DXVertex(NodeType T)
{
	memset(this,0,sizeof(DXVertex));
	NodeSize=sizeof(DXVertex);
	Type=T;
	NodeNumber=-1;
}

DXTriangle::DXTriangle(NodeType T)
{
	memset(this,0,sizeof(DXTriangle));
	NodeSize=sizeof(DXTriangle);
	Type=T;
	NodeNumber=-1;
}

DXLine::DXLine(NodeType T)
{
	memset(this,0,sizeof(DXLine));
	NodeSize=sizeof(DXLine);
	Type=T;
	NodeNumber=-1;
}


DXDof::DXDof(NodeType T)
{
	memset(this, 0, sizeof(DXDof));
	NodeSize=sizeof(DXDof);
	Type=T;
	NodeNumber=-1;
}

DXEndDof::DXEndDof(NodeType T)
{
	memset(this, 0, sizeof(DXEndDof));
	NodeSize=sizeof(DXEndDof);
	Type=T;
	NodeNumber=-1;
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




DXDof::~DXDof()
{
}

DXEndDof::~DXEndDof()
{
}

DXSlot::DXSlot(NodeType T)
{
	Type=T;
	Slot.SlotNr=-1;
	NodeSize=sizeof(DXSlot);
	NodeNumber=-1;
}


DXSlot::~DXSlot()
{
}
