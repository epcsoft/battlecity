#include "E_define.h"
#include "E_input_extern.h"
#include "E_input.h"
#include "E_graph_extern.h"
#include "E_graph.h"
#define FEETEST	1

char appid[]="615220236000";
char chargeid[4][13]={
	"000000000000",
	"000000000000",
	"000000000000",
	"000000000000"
};
char chargecost[4]={4,2,2,2};
char filename[]="chargedata.bin";
//TestOnly!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#if (FEETEST ==1)
int SendFeeExt(int a,char b[],char c[],int d,char *f){
	printf("\nCharging!!!...\n%s\nappid:%s\nchargeid:%s\n",f,b,c);
	Sleep(1000);
	//if(rand()%10<5)
	return 1;
	//else return 0;
}
int GetFee(char * charge_id){
	printf("\nGetFee!! ID:%s",charge_id);
	return 2+rand()%4;
}
#endif
//testover!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void savecharge(int chargeflag){
	FILE *fp;
	fp=fopen(filename,"wb");
	fwrite(&chargeflag,1,sizeof(chargeflag),fp);
	fclose(fp);

}
int loadcharge(){
	FILE *fp;
	int chargeflag;
	fp=fopen(filename,"rb");
	if(fp==NULL)return 0;
	fread(&chargeflag,1,sizeof(chargeflag),fp);
	fclose(fp);
	return chargeflag;
}
void checkcharge(HDC hdc,int nowstage){
	FILE *fp;
	int keymode=1;
	eVKEY key1;
	int chargeflag;
	char tstxt[32];
	int r;
	int nowevent,key;
	chargeflag=loadcharge();
	if(!(chargeflag==0&&nowstage==0))return;
	
	sprintf(tstxt,"享受游戏全部功能？开启所有模式？永久开通吧，只需5元信息费，永久免费");
	if(SendFeeExt(0,appid,chargeid[chargeflag],0,tstxt)==1)
	{
		_ClearArea(hdc,1);
		midtextout("购买成功！",scrkuan/4,hdc);
		drawtstxt(hdc,"确定","");
		UpdateArea(hdc);
		chargeflag++;
		while(1){
			nowevent=getbichuevent();
			key=GetKey();
			if(key==VKEY_LSK||ckpen(nowevent)==PEN_LSK)break;
			Sleep(223);
		}
	}
	else {
		_ClearArea(hdc,1);
		midtextout("购买失败！",scrkuan/4,hdc);
		drawtstxt(hdc,"确定","");
		UpdateArea(hdc);
		while(1){
			nowevent=getbichuevent();
			key=GetKey();
			if(key==VKEY_LSK||ckpen(nowevent)==PEN_LSK)break;
			Sleep(223);
		}
		exit(0);
	}
	savecharge(chargeflag);

}