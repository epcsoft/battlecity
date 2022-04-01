#include "E_define.h"
#include "E_graph_extern.h"
HDC globeNum,Gnum,buttomimage;
void midtextoutEx(char txt[],int y,int num,  eDRAW mode,eFONT font,int bdColor,int fontColor,HDC _hdc){
	SetFont(_hdc,font);
	SetFgColor (_hdc, fontColor);
	SetBorderColor (_hdc,bdColor);
	y+=y_add;
	TextOut(_hdc,(LCD_CHANG-StringWidth (font, txt, num))/2,y,txt,num,mode);
}

void midtextout(char txt[],int y,HDC _hdc){
	midtextoutEx(txt,y,-1,DRAW_SPRITE,  FONT_MEDIUM,RGB(255,255,255),RGB(0,0,0),_hdc);
}
void textout(char txt[],int x,int y,HDC _hdc){
	y+=y_add;
	x+=x_add;
	SetFont(_hdc,FONT_MEDIUM);
	SetFgColor (_hdc, RGB(0,0,0));
	SetBorderColor (_hdc,RGB(0,0,0));
	//printf("%d,%d\n",di.width,StringWidth (font, txt, num));
	TextOut(_hdc,x,y,txt,-1,DRAW_SPRITE);
}

void drawtstxt(HDC hdc,char lefttxt[],char righttxt[]){
	SetFgColor(hdc,RGB(0,0,255));
	//ClearRect(hdc,0,LCD_KUAN-2-FontHeight(FONT_MEDIUM|FONT_BORDER),LCD_CHANG,LCD_KUAN);
	SetBorderColor(hdc,RGB(255,255,255));
	SetFont(hdc,FONT_MEDIUM|FONT_BORDER); 
	TextOut(hdc,0,LCD_KUAN-2-FontHeight(FONT_MEDIUM|FONT_BORDER),lefttxt,-1,DRAW_XOR);
	TextOut(hdc,LCD_CHANG-StringWidth(FONT_MEDIUM|FONT_BORDER,righttxt,-1),LCD_KUAN-2-FontHeight(FONT_MEDIUM|FONT_BORDER),righttxt,-1,DRAW_XOR);
	return;
}
void BitBlt2SCR(HDC hdc,int xd, int yd, int w, int h, HDC hdcs, int xs, int ys, eDRAW mode){
	//int cutshow=0;
	//int cutdown=0;
	//int cutleft=0;
	//int cutright=0;
	//if(xd<0){cutleft=xd*(-1);xd=0;}
	//if(xd+w>240){
	//	if(xd>240)return;
	//	cutright=240-xd;
	//	if(cutright<0)return;
	//	w=cutright;
	//}
	////if(xd+w>240)return;
	//if(yd<0){cutshow=yd*(-1);yd=0;}
	//if(yd+h>320){
	//	cutdown=yd+h-320;
	//	if(cutdown<0)return;
	//}
	//if(xd+w<=240)
	BitBlt(hdc,xd+x_add,yd+y_add,w,h,hdcs,xs,ys,mode);
}
void DrawLine2SCR(HDC hdc, int x0, int y0, int x1, int y1){
	if(scrmode==0||scrmode==1)DrawLine(hdc,x0,y0,x1,y1);
	else if(scrmode==2)DrawLine(hdc,x0,y0+40,x1,y1+40);
	else if(scrmode==3)DrawLine(hdc,x0+40,y0+80,x1+40,y1+80);
}
void _ClearArea (HDC hdc,int mode){
	char name[64];
	if(mode==1){
		sprintf(name,"bond%d.gif",scrmode);
		LoadGif(hdc,0,0,0,0,name,0,0,0,0);
	}
		//SetFgColor(hdc,RGB(0,0,0));
		//FillRect(hdc,x_add, y_add, x_add+scrchang,y_add+scrkuan);
}
void AlphaBlend2SCR  ( HDC  hdc,  
					  int  xd,  
					  int  yd,  
					  int  w,  
					  int  h,  
					  HDC  hdcs,  
					  int  xs,  
					  int  ys,  
					  int  mode   
					  )  
{
	if(scrmode==2)yd+=40;
	else if(scrmode==3){
		xd+=40;
		yd+=80;
	}
	AlphaBlend(hdc,xd,yd,w,h,hdcs,xs,ys,mode);
}
void BitBlt2SCR_Center(HDC hdc,int xd, int yd, int w, int h, HDC hdcs, int xs, int ys, eDRAW mode){
	BitBlt2SCR(hdc,xd-w/2,yd-h/2,w,h,hdcs,xs,ys,mode);
	//printf("\n%d,%d",xd-w/2,yd-h/2);
}
void Loadresizepic(HDC *mubiao,HDC tmp,int yuanshix,int yuanshiy,int mubiaox,int mubiaoy,char *filename){
	DCInfo dcinfo;
	if(GetDCInfo(*mubiao,&dcinfo)!=0)CloseHandle(*mubiao);
	if(GetDCInfo(*mubiao,&dcinfo)==0)
		*mubiao=CreateDC(yuanshix,yuanshiy);
	if(filename[strlen(filename)-1]=='f')
		LoadGif(*mubiao,0,0,0,0,filename,0,0,0,0);
	else if(filename[strlen(filename)-3]=='j')
		LoadJpg(*mubiao,0,0,0,0,filename,0,0,0,0,0);
	else if(filename[strlen(filename)-2]=='n')
		LoadPng(*mubiao,0,0,0,0,filename,0,0,0);
	Rotate(tmp,*mubiao,0,0,yuanshix,yuanshiy,0,(double)mubiaox/(double)yuanshix,(double)mubiaoy/(double)yuanshiy,mubiaox/2,mubiaoy/2, DRAW_COPY);
	CloseHandle(*mubiao);
	*mubiao=CreateDC(mubiaox,mubiaoy);
	BitBlt(*mubiao,0,0,mubiaox,mubiaoy,tmp,0,0,DRAW_COPY);
	//printf("%d",*mubiao);
}
void Rotate2SCR  ( HDC  hdc,  
				  HDC  hdcs,  
				  int  xs,  
				  int  ys,  
				  int  w,  
				  int  h,  
				  double  RotaryAngle,  
				  double  ZoomX,  
				  double  ZoomY,  
				  int  move_x,  
				  int  move_y,  
				  eDRAW  mode   
				  ) 
{
	move_y+=y_add;move_x+=x_add;
	//printf("\n %d,%d,%d,%d   %f, ");
	Rotate(hdc,hdcs,xs,ys,w,h,RotaryAngle,ZoomX,ZoomY,move_x,move_y,mode);;
}

void FillRect2Scr(HDC hdc,int x1,int y1,int x2,int y2){
	FillRect(hdc,x1+x_add,y1+y_add,x2+x_add,y2+y_add);
}

void LoadGif2SCR(HDC hdc,int x,int y,int w,int h,char *filename,int cx,int cy,int a,int b){
	x+=x_add;
	y+=y_add;
	LoadGif(hdc,x,y,w,h,filename,cx,cy,a,b);
}
void refreshbuttom(HDC hdc){
	int pyy;
	if(scrmode<2)return;
	if(scrmode==2)pyy=40;
	else if(scrmode==3)pyy=80;
	BitBlt(hdc,0,LCD_KUAN-pyy,LCD_CHANG,pyy,buttomimage,0,0,DRAW_COPY);
}
void initgraphdata(){
	globeNum=CreateDC(60,9);
	LoadGif(globeNum,0,0,0,0,"/gamepic/interface_score1.gif",0,0,0,0);
	SetKeyColor(globeNum,RGB(0,0,255));	
	Gnum=CreateDC(150,15);
	LoadGif(Gnum,0,0,0,0,"/gamepic/maxLetter.gif",0,0,0,0);
	SetKeyColor(Gnum,RGB(255,0,0));
	if(scrmode==2){
		buttomimage=CreateDC(LCD_CHANG,40);
		LoadGif(buttomimage,0,0,LCD_CHANG,40,"bond.gif",40,440,0,0);
	}
	else if(scrmode==3){
		buttomimage=CreateDC(LCD_CHANG,80);
		LoadGif(buttomimage,0,0,LCD_CHANG,80,"bond.gif",0,400,0,0);
	}
}
void shownums(HDC hdc,int num,int x,int y){
	char txtbuf[16];
	int r;
	sprintf(txtbuf,"%9d",num);
	for(r=0;r<strlen(txtbuf);r++){
		if(txtbuf[r]>='0'&&txtbuf[r]<='9')
			BitBlt2SCR(hdc,x+r*7,y,6,9,globeNum,(txtbuf[r]-'0')*6,0,DRAW_SPRITE);
	}
	//BitBlt2SCR(hdc,x,y-10,100,11,*numhdc,0,0,DRAW_SPRITE);
}
void showGnums(HDC hdc,int num,int x,int y){
	char txtbuf[16];
	int r;
	sprintf(txtbuf,"%d",num);
	for(r=0;r<strlen(txtbuf);r++){
		if(txtbuf[r]>='0'&&txtbuf[r]<='9')
			BitBlt2SCR(hdc,x+r*15,y,15,15,Gnum,(txtbuf[r]-'0')*15,0,DRAW_SPRITE);
	}
	//BitBlt2SCR(hdc,x,y-10,100,11,*numhdc,0,0,DRAW_SPRITE);
}
