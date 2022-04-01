#include "E_define.h"
#include "E_graph_extern.h"
#include "E_graph.h"
#include "E_input_extern.h"
#include "E_input.h"
DeviceCaps   dev1 ;
int _TotalMemory;
void memtest(HDC hdc){
	char txtbuf[128];
	SetFgColor(hdc,RGB(255,255,255));FillRect(hdc,0, 300, 240,320);sprintf(txtbuf,"%d",FreeMemory());midtextout(txtbuf,300,hdc);
}
void showerr(HDC hdc,int code){
	char txtbuf[128];
	int keymode=1;
	SetBgColor (hdc,0xFFFFFF);
	ClearArea (hdc);
	SetFgColor (hdc,0x0);
	SetBorderColor (hdc,0xFF0000);
	if(code==0){
		TextOut(hdc,0,0,"�Բ���",-1,DRAW_COPY);
		TextOut(hdc,0,20,"����Ϸ��֧��",-1,DRAW_COPY);
		TextOut(hdc,0,40,"�����ֻ���Ļ�ֱ���",-1,DRAW_COPY);
		TextOut(hdc,0,60,"��˶Ժ������أ�",-1,DRAW_COPY);
		TextOut(hdc,0,100,"��������������˳���",-1,DRAW_COPY);
		GetDeviceCaps  (&dev1); 
		sprintf(txtbuf,"���ķֱ���:%dx%d",dev1.ScreenWidth,dev1.ScreenHeight);TextOut(hdc,0,80,txtbuf,-1,DRAW_COPY);
	}
	else {
		TextOut(hdc,0,0,"�Բ���",-1,DRAW_COPY);
		TextOut(hdc,0,20,"��Ϸ����,�����Ǵ������",-1,DRAW_COPY);
		TextOut(hdc,0,40,"�뷴��������!",-1,DRAW_COPY);
		sprintf(txtbuf,"FreeMem:%d/%d",FreeMemory(),_TotalMemory);TextOut(hdc,0,60,txtbuf,-1,DRAW_COPY);
		TextOut(hdc,0,100,"��������������˳���",-1,DRAW_COPY);
	}
	UpdateArea(hdc);
	anykeyortouch(keymode);
	exit(0);
}