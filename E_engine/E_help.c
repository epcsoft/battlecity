#include "E_define.h"
#include "E_graph_extern.h"
#include "E_graph.h"
#include "E_input_extern.h"
#include "E_input.h"

int process_str(char strdata[],char text[],int in_every)//text outputtxt
{
	unsigned char txt[3];
	long ta;
	int row_all;
	int in,row;
	//long load;
	row_all=0;
	in=0;
	row=0;
	for(ta=0;ta<16384;ta++)
	{	
		memset(txt,0,3);
		if(strdata[ta]==0)break;
		txt[0]=strdata[ta];//读一个字节
		if(txt[0]==0x0d||txt[0]==0x0a)//如果是回车键
		{
			row_all++;//换行
			if(row_all==326)
			{
				row_all--;
				break;
			}
			in=0;
			ta++;
			continue;
		}
		if(txt[0]>127)//如果是全角符
		{
			//printf("全角：%s\n",txt);Sleep(1000);
			memcpy(txt,strdata+ta,2);//读两个字节
			if(in>=in_every-1)
			{
				row_all++;
				in=0;
			}
			if(row_all==326)
			{
				row_all--;
				break;
			}
			memcpy(text+row_all*49+in,txt,2);
			in=in+2;
			ta++;
		}
		else//如果是半角符
		{
			//printf("半角：%s",txt);Sleep(1000);
			if(in>=in_every)
			{
				row_all++;
				in=0;
			}
			if(row_all==326)
			{
				row_all--;
				break;
			}
			memcpy(text+row_all*49+in,txt,1);
			in++;
		}
	}
	row_all++;
	return row_all;
}
int showbar2(int maxsum,int nowsum,int x,int y,HDC hdc,HDC *bar2){
	if(maxsum==0)maxsum=1;
	BitBlt(hdc,x,y,12,125,*bar2,0,0,DRAW_SPRITE);
	BitBlt(hdc,x,y+80*nowsum/maxsum,12,12,*bar2,0,137,DRAW_SPRITE);
	//printf("%d\n",bichusta.event);
	if(isinrect(bichusta.x,bichusta.y,x,y,LCD_CHANG,y+113)){
		return (bichusta.y-y+6)*maxsum/113;
	}
	return -1;
}
int showxiangxi(HDC hdc,char name[],char filename[]){
	FILE *fp;
	char rowtxt[49];
	int rowall,rowevery;
	HDC bg;
	HDC	bar2;
	int r;
	int nowshow=0;
	int needrefresh=1;
	int line;
	long len;
	char key;eVKEY key1;
	int fh=0;
	int barsel=-1;
	char info[4096];
	char text[4096];
	//int pyx=0,pyy=0;
	int titley;
	int nowevent;
	if(scrmode==0)titley=8;
	else titley=15;
	_ClearArea(hdc,1);
	bar2=CreateDC(12,228);
	LoadGif(bar2,0,0,0,0,"/help/bar2.gif",0,0,0,0);
	SetKeyColor(bar2,RGB(0,0,0));
	memset(info,0,sizeof(info));
	fp=fopen(filename,"rb");
	fseek(fp,0,SEEK_END);
	len=ftell(fp);
	rewind(fp);
	fread(info,1,len,fp);
	fclose(fp);
	_ClearArea(hdc,0);
	if(scrmode==0){
		if(scrchang==176){
			rowevery=7;
			Loadresizepic(&bg,hdc,240,320,176,220,"/help/bg.gif");
		}
		else
		{
			rowevery=6;
			bg=CreateDC(scrchang,scrkuan);
			LoadGif(bg,0,0,0,0,"/help/bg1.gif",0,0,0,0);
		}
	}
	else 
	{
		rowevery=10;
		bg=CreateDC(scrchang,scrkuan);
		LoadGif(bg,0,0,0,0,"/help/bg.gif",0,0,0,0);
	}
	SetBgColor(hdc,RGB(255,255,255));
	UpdateArea(hdc);
	SetFont(hdc,FONT_MEDIUM|FONT_BORDER);
	SetFgColor (hdc, RGB(0,0,0));
	SetBorderColor (hdc,RGB(255,255,255));
	line=((scrchang-8)/StringWidth ( FONT_MEDIUM|FONT_BORDER,"我",-1))*2; 
	memset(text,0,sizeof(text));
	rowall=process_str(info,text,line);
	//printf("%s\n\n",text);

	while(1){
		if(needrefresh==1){
			needrefresh=0;
			//SetFgColor(hdc,RGB(255,255,255));
			//FillRect(hdc,0,24,218,154);
			SetFgColor(hdc,RGB(0,0,0));
			BitBlt2SCR(hdc,0,0,scrchang,scrkuan,bg,0,0,DRAW_COPY);
			midtextoutEx(name,titley,-1,DRAW_SPRITE,FONT_LARGE|FONT_BORDER,RGB(0,0,0),RGB(255,255,255),hdc);
			if(nowshow<0)nowshow=0;
			for(r=0;r<rowevery;r++){
				memcpy(rowtxt,text+(r+nowshow)*49,49);
				textout(rowtxt,5,scrkuan/6+r*22,hdc);
				//printf("%s\n",rowtxt);
			}

		}
		//barsel=showbar2(rowall-rowevery+1,nowshow,scrchang-15,scrkuan/6,hdc,&bar2);
		//if(barsel!=-1){needrefresh=1;nowshow=barsel;}
		UpdateArea(hdc);
		//Sleep(10);
		nowevent=getbichuevent();
		if(barsel!=-1)continue;
		key=GetGameAction(CheckKey(0));
		key1=GetKey();
		if(_ckpen(nowevent)==PEN_DOWN)key1=VKEY_DOWN;
		else if(_ckpen(nowevent)==PEN_UP)key1=VKEY_UP;
		else if(_ckpen(nowevent)==PEN_LSK)key1=VKEY_LSK;
		else if(_ckpen(nowevent)==PEN_RSK)key1=VKEY_RSK;
		if(ckkey(key,key1,0)==KEY_DOWN){
			if(nowshow<(rowall-rowevery+1)){nowshow++;needrefresh=1;}
			else {nowshow=0;needrefresh=1;}
		}
		else if(ckkey(key,key1,0)==KEY_UP){
			if(nowshow>0){nowshow--;needrefresh=1;}
			else {nowshow=rowall-rowevery+1;needrefresh=1;}
		}
		if(ckkey(key,key1,0)==KEY_RSK){break;}
		if(ckkey(key,key1,0)==KEY_LSK){break;}

	}
	CloseHandle(bg);
	CloseHandle(bar2);
	return fh;
}