#include "E_define.h"
#include "E_input.h"
#include "E_input_extern.h"
#include "E_graph_extern.h"
#include "E_graph.h"
int LCD_CHANG;
int LCD_KUAN;
int x_add,y_add;
int scrchang,scrkuan;
int scrmode;

VmEvent bichusta;
int bichuevents[MAX_BICHUEVENTS];
int bichuevent=0;

int getbichuevent(){
	int firstevent,r;
	if(bichuevent==0)return -1;
	firstevent=bichuevents[0];
	for(r=0;r<bichuevent;r++){
		bichuevents[r]=bichuevents[r+1];
	}
	bichuevent--;
	return firstevent;
}
void addbichuevent(int newevent){
	bichuevents[bichuevent++]=newevent;
	if(bichuevent==MAX_BICHUEVENTS)getbichuevent();
}
void clearqueen(){
	bichusta.x=-1;
	while(GetKey()!=0){}
	bichuevent=0;
}
int bichugetter(void *a){
	while(1){
		GetEvent(&bichusta);
		addbichuevent(bichusta.event);
	}
	return 0;
}
char isinrect(int x,int y,int x1,int y1,int x2,int y2){
	if(x>=x1&&x<=x2&&y>=y1&&y<=y2)return 1;
	return 0;
}
char _isinrect(int x,int y,int x1,int y1,int x2,int y2){
	x-=x_add;
	y-=y_add;
	if(x>=x1&&x<=x2&&y>=y1&&y<=y2)return 1;
	return 0;
}
char ckpeninrect(int x,int y,int x1,int y1,int x2,int y2,int nowevent){
	x-=x_add;y-=y_add;
	if(nowevent==VEVENT_PENDOWN&&isinrect(x,y,x1,y1,x2,y2))return 1;
	return 0;
}
char _ckpeninrect(int x,int y,int x1,int y1,int x2,int y2,int nowevent){
	if(isinrect(x,y,x1*LCD_CHANG/320,y1*LCD_KUAN/480,x2*LCD_CHANG/320,y2*LCD_KUAN/480))return 1;
	return 0;
}
int _ckpen(int nowevent){
	if(ckpeninrect(bichusta.x,bichusta.y,0,300,40,320,nowevent)==1){return PEN_LSK;}
	if(ckpeninrect(bichusta.x,bichusta.y,80,300,105,320,nowevent)==1){return PEN_UP;}
	if(ckpeninrect(bichusta.x,bichusta.y,140,300,170,320,nowevent)==1){return PEN_DOWN;}
	if(ckpeninrect(bichusta.x,bichusta.y,200,300,240,320,nowevent)==1){return PEN_RSK;}
	return 0;
}
int ckpen(int nowevent){
	if(isinrect(bichusta.x,bichusta.y,0,LCD_KUAN-20,36,LCD_KUAN)==1&&nowevent==VEVENT_PENDOWN){return PEN_LSK;}
	if(isinrect(bichusta.x,bichusta.y,LCD_CHANG-36,LCD_KUAN-20,LCD_CHANG,LCD_KUAN)==1&&nowevent==VEVENT_PENDOWN){return PEN_RSK;}
	return 0;
}
char ckkey(	char key,eVKEY key1,int once){
	if(once==1){
		while(1){
			if(bichusta.event==VEVENT_KEYUP||bichusta.event==VEVENT_PENUP)break;
		}
	}
	if(key1==VKEY_RSK)return KEY_RSK;
	if(key1==VKEY_LSK)return KEY_LSK;
	if(key1==VKEY_OK)return KEY_OK;
	if(key1==VKEY_UP)return KEY_UP;
	else if(key1==VKEY_DOWN)return KEY_DOWN;
	else if(key1==VKEY_LEFT)return KEY_LEFT;
	else if(key1==VKEY_RIGHT)return KEY_RIGHT;
	return key1;
}
int anykeyortouch(int mode){
	int nowevent;
	if(mode==1)clearqueen();
	while(1){
		nowevent=getbichuevent();
		if(nowevent==VEVENT_KEYUP||nowevent==VEVENT_PENUP)break;
	}
	if(mode==1)clearqueen();
	return 0;
}
int msgshow(HDC hdc,char *msg){//1 是 0否
	int fh;
	eVKEY key1;
	int nowevent;
	_ClearArea(hdc,1);
	if(scrmode>0)
		midtextoutEx(msg,scrkuan/2-10,-1,DRAW_SPRITE,  FONT_MEDIUM,RGB(255,255,255),RGB(0,0,0),hdc);
	else midtextoutEx(msg,scrkuan/2-10,-1,DRAW_SPRITE,  FONT_SMALL,RGB(255,255,255),RGB(0,0,0),hdc);
//	midtextout(msg,,hdc);
	drawtstxt(hdc,"是","否");
	UpdateArea(hdc);	
	while(1){
		nowevent=getbichuevent();
		key1=GetKey();
		if(ckpen(nowevent)==PEN_LSK)key1=VKEY_LSK;
		if(ckpen(nowevent)==PEN_RSK)key1=VKEY_RSK;
		if(ckkey(0,key1,0)==KEY_LSK){
			fh=1;break;
		}
		if(ckkey(0,key1,0)==KEY_RSK){
			fh=0;break;
		}	
		Sleep(50);
	}
	_ClearArea(hdc,1);
	return fh;
}

int msgshowEx(HDC hdc,char *msg,int msgy,int iscls,int fontcolor){//1 是 0否
	int fh;
	eVKEY key1;
	int nowevent;
	if(iscls==1){
		SetBgColor(hdc,RGB(223,223,233));
		ClearArea(hdc);
	}
	midtextoutEx(msg,msgy,-1,DRAW_SPRITE,  FONT_MEDIUM,RGB(255,255,255),fontcolor,hdc);
	//	midtextout(msg,,hdc);
	drawtstxt(hdc,"是","否");
	UpdateArea(hdc);	
	while(1){
		nowevent=getbichuevent();
		key1=GetKey();
		if(ckpen(nowevent)==PEN_LSK)key1=VKEY_LSK;
		if(ckpen(nowevent)==PEN_RSK)key1=VKEY_RSK;
		if(ckkey(0,key1,0)==KEY_LSK){
			fh=1;break;
		}
		if(ckkey(0,key1,0)==KEY_RSK){
			fh=0;break;
		}	
		Sleep(50);
	}
	_ClearArea(hdc,1);
	return fh;
}