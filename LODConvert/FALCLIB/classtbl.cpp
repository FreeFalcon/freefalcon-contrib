// Use objects in directory ObjectSet0708 with this file.
/*
 * Machine Generated Class Table Constants loader file.
 * NOTE: This file is read only. DO NOT ATTEMPT TO MODIFY IT BY HAND.
 * Generated on 11-August-1999 at 16:28:04
 * Generated from file Access Class Table
 */

#include <windows.h>
#include <stdio.h>
#include "F4vu.h"
#include "ClassTbl.h"
#include "F4Find.h"
#include "entity.h"
 

// Which langauge should we use?
int gLangIDNum = 1;

/*
* Class Table Constant Init Function
*/

void InitClassTableAndData(char *name,char *objset)
{
FILE* filePtr;
char  fileName[MAX_PATH];

if (stricmp(objset,"objects") != 0)
  {
  //Check for correct object data version
  ShiAssert( stricmp("ObjectSet0708",objset) == 0);
  }

sprintf(fileName, "%s\\%s.ini", FalconObjectDataDir, name);

gLangIDNum = GetPrivateProfileInt("Lang", "Id", 0, fileName);

filePtr = OpenCampFile(name,"ct","rb");
if (filePtr)
    {
     fread (&NumEntities, sizeof (short), 1, filePtr);
     Falcon4ClassTable = new Falcon4EntityClassType[NumEntities];
     fread (Falcon4ClassTable, sizeof (Falcon4EntityClassType), NumEntities,filePtr);
     fclose(filePtr);
	 // VP_changes Oct 25. The next segment is used to create the list of all existent objects
	 /*
	 FILE* deb = fopen("c:\\traceA10\\classtbl.txt", "a");
	 int i;
	 for ( i=0; i < NumEntities; i++)
	 {
		 fprintf(deb, "%d Domain=%u Class=%u Type=%u STYPE=%u SPTYPE=%u OWNER=%u VisType=%d %d %d %d %d %d %d\n", 
			 i,
			 Falcon4ClassTable[i].vuClassData.classInfo_[0], 
			 Falcon4ClassTable[i].vuClassData.classInfo_[1], 
			 Falcon4ClassTable[i].vuClassData.classInfo_[2],
			 Falcon4ClassTable[i].vuClassData.classInfo_[3], 
			 Falcon4ClassTable[i].vuClassData.classInfo_[4],
			 Falcon4ClassTable[i].vuClassData.classInfo_[5],
			 Falcon4ClassTable[i].visType[0],
			 Falcon4ClassTable[i].visType[1],
			 Falcon4ClassTable[i].visType[2],
			 Falcon4ClassTable[i].visType[3],
			 Falcon4ClassTable[i].visType[4],
			 Falcon4ClassTable[i].visType[5],
			 Falcon4ClassTable[i].visType[6]
			 );   
	 }	
	 fclose(deb);   
	*/ 
    }
else
    {
    Falcon4ClassTable = NULL;
    }
}
