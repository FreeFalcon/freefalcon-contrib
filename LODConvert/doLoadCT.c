//====================================================================================
//====================================================================================
int	doLoadCT()
{
				int						c,d;
				HANDLE				fp;
				char					nctr[6];

	sprintf(szMsg, "%sFalcon4.ct", KoreaObjFolder); 
	fp = fopen(szMsg,"rb");
	ctNumber = dogetshort(fp);
	if (ct)
	{
		free(ct);
		ct = NULL;
	}
	ct = (CT*)malloc(ctNumber*sizeof(CT));
	memset(ct, 0, (ctNumber*sizeof(CT)));
	for(c=0;c<ctNumber;c++)
	{
		fseek(fp,c*81+10,SEEK_SET);
		ct[c].domain = dogetchar(fp);
		ct[c].cclass = dogetchar(fp);
		ct[c].type = dogetchar(fp);
		ct[c].subtype = dogetchar(fp);
		ct[c].specific = dogetchar(fp);
		ct[c].mode = dogetchar(fp);
		fseek(fp,10,SEEK_CUR);
		BubbleDist = dogetfloat(fp);
		fseek(fp,32,SEEK_CUR);
		for(d=0;d<8;d++)
			ct[c].parent[d] = dogetshort(fp);
//		fseek(fp,2,SEEK_CUR);
		ct[c].ftype = dogetchar(fp);
		ct[c].foffset = dogetlong(fp);
			
	}
	fclose(fp);
	sprintf(szMsg, "%sFalcon4.fcd", KoreaObjFolder); // Load Features
	fp = fopen(szMsg,"rb");
	for(c=0;c<ctNumber;c++)
	{
		if(ct[c].ftype == 1)
		{
			fseek(fp,ct[c].foffset*60+10,SEEK_SET);
			fread(ct[c].name,20,1,fp);
		}
	}
	fclose(fp);
	sprintf(szMsg, "%sFalcon4.ocd", KoreaObjFolder); // Load Objects 
	fp = fopen(szMsg,"rb");
	for(c=0;c<ctNumber;c++)
	{
		if(ct[c].ftype == 3)
		{
			fseek(fp,ct[c].foffset*54+4,SEEK_SET);
			fread(ct[c].name,20,1,fp);
		}
	}
	fclose(fp);
	sprintf(szMsg, "%sFalcon4.ucd", KoreaObjFolder); // Load Units
	fp = fopen(szMsg,"rb");
	for(c=0;c<ctNumber;c++)
	{
		if(ct[c].ftype == 4)
		{
			fseek(fp,(ct[c].foffset)*336+232,SEEK_SET);
			fread(ct[c].name,22,1,fp);
		}
	}
	fclose(fp);
	sprintf(szMsg, "%sFalcon4.vcd", KoreaObjFolder); // Load Vehicles
	fp = fopen(szMsg,"rb");
	for(c=0;c<ctNumber;c++)
	{
		if(ct[c].ftype == 5)
		{
			if (bLP)
				fseek(fp,ct[c].foffset*176+16,SEEK_SET);  //LP
			else
				fseek(fp,ct[c].foffset*160+10,SEEK_SET);	//FF
			fread(ct[c].name,15,1,fp);
			fread(nctr,5,1,fp);
		}
	}
	fclose(fp);
	sprintf(szMsg, "%sFalcon4.wcd", KoreaObjFolder); // Load Weapons
	fp = fopen(szMsg,"rb");
	for(c=0;c<ctNumber;c++)
	{
		if(ct[c].ftype == 6)
		{
			fseek(fp,ct[c].foffset*60+14,SEEK_SET);
			fread(ct[c].name,16,1,fp);
		}
	}
	fclose(fp);
	return(0);
} // end doLoadCT

