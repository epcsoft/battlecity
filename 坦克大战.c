///epc frameworks////
#include ".\\E_engine\\E_define.h"
#include ".\\E_engine\\E_graph_extern.h"
#include ".\\E_engine\\E_graph.h"
#include ".\\E_engine\\E_input_extern.h"
#include ".\\E_engine\\E_input.h"
#include ".\\E_engine\\E_charge.h"
#include ".\\E_engine\\E_help.h"
#include ".\\E_engine\\E_debug.h"
#include "tank_def.h"
#define PCTEST	0
#define FUCKYIDONG	1

int isplaybgm=0;
int whichbgm=0;
int framskipcount=0;

void playsoundex(char *audio,int format,int size){
	if(isplaybgm==1){
		if(GetSoundState ()==AUDIO_IDLE )PlaySound(audio,format,size);
	}
}

void nowloading(HDC hdc){
	HDC bg;
	_ClearArea(hdc,1);
	if(scrchang==176){
		Loadresizepic(&bg,hdc,240,320,scrchang,scrkuan,"/gamemenu/loading.gif");
	}
	else {
		bg=CreateDC(scrchang,scrkuan);
		if(scrchang==220)LoadGif(bg,0,0,0,0,"/gamemenu/loading1.gif",0,0,0,0);
		else LoadGif(bg,0,0,0,0,"/gamemenu/loading.gif",0,0,0,0);
	}
	BitBlt2SCR(hdc,0,0,scrchang,scrkuan,bg,0,0,DRAW_SPRITE);
	//midtextout("请稍后",100,hdc);
	CloseHandle(bg);
	UpdateArea(hdc);

	if(PCTEST)Sleep(1000);
}


char scorefile[]="save.bin";
void loadscore(int *maxscore,int *mylevel,int *mylife){
	FILE *fp;
	//int maxscore;
	fp=fopen(scorefile,"rb");
	if(fp!=0){
		fread(maxscore,1,sizeof(maxscore),fp);
		fread(mylevel,1,sizeof(mylevel),fp);
		fread(mylife,1,sizeof(mylife),fp);
		fclose(fp);
	}
	else {
		*maxscore=0;
		*mylevel=0;
		*mylife=3;
	}
	return ;
}
void savescore(int maxscore,int mylevel,int mylife){
	FILE *fp;
	int stage,level,life;
	loadscore(&stage,&level,&life);
	fp=fopen(scorefile,"wb");
	if(stage>maxscore)fwrite(&stage,1,sizeof(stage),fp);
	else fwrite(&maxscore,1,sizeof(maxscore),fp);
	fwrite(&mylevel,1,sizeof(mylevel),fp);
	fwrite(&mylife,1,sizeof(mylife),fp);
	fclose(fp);
}
int mapdata[MAP_BLK][MAP_BLK];//记录场景ID
HDC pic_scene;
HDC pic_tanks[4];
HDC pic_cards;

void loadpics(){
	int r;
	char buf[64];
	pic_scene=CreateDC(64,56);
	pic_cards=CreateDC(16,96);
	LoadGif(pic_scene,0,0,0,0,"gamepic/pic_scene.gif",0,0,0,0);
	for(r=0;r<4;r++)
	{
		pic_tanks[r]=CreateDC(64,80);
		sprintf(buf,"gamepic/pic_tanks%d.gif",r);
		LoadGif(pic_tanks[r],0,0,0,0,buf,0,0,0,0);
	}
	LoadGif(pic_cards,0,0,0,0,"gamepic/pic_cards.gif",0,0,0,0);
	SetKeyColor(pic_scene,RGB(0,0,0));
	for(r=0;r<4;r++)SetKeyColor(pic_tanks[r],RGB(0,0,0));
	SetKeyColor(pic_cards,RGB(0,0,0));
	////旋转得到坦克四个方向
	//for(r=1;r<4;r++){
	//	SetBgColor(pic_tanks[r],0);
	//	ClearArea(pic_tanks[r]);
	//	for(rr=0;rr<5;rr++){
	//		for(rrr=0;rrr<4;rrr++){
	//			Rotate(pic_tanks[r],pic_tanks[0],
	//				rrr*TANK_SIZE,rr*TANK_SIZE,
	//				TANK_SIZE,TANK_SIZE,(double)r*(PAI/2.0f),1.0f,1.0f,
	//				rrr*TANK_SIZE+TANK_SIZE/2,rr*TANK_SIZE+TANK_SIZE/2,DRAW_SPRITE);
	//		}
	//	}
	//}
}
void unloadpics(){
	int r;
	CloseHandle(pic_scene);
	for(r=0;r<4;r++)CloseHandle(pic_tanks[r]);
	CloseHandle(pic_cards);
}

void getmap_py(int *px,int *py){
	*px=(scrchang-MAP_PIXEL)/4;
	*py=(scrkuan-MAP_PIXEL)/2;
}
void draw_scene_blk(HDC hdc,int dx,int dy,int sx,int sy,int size,int is2scr){//size: 0 8x8; 1 16x16; 2 32x32;x y 乘8是实际HDC坐标
	int p_size[3]={8,16,32};
	int map_px,map_py;
	getmap_py(&map_px,&map_py);
	if(is2scr==1)BitBlt2SCR(hdc,map_px+dx*8,map_py+dy*8,p_size[size],p_size[size],pic_scene,sx*8,sy*8,DRAW_SPRITE);
	else  BitBlt(hdc,dx*8,dy*8,p_size[size],p_size[size],pic_scene,sx*8,sy*8,DRAW_SPRITE);
}

void draw_scene_id(HDC hdc,int dx,int dy,int id,int is2scr){//通过ID绘制场景块，更加方便
	int scene_data[MAX_SCENE][3]={//X,Y SIZE
		0,0,0,
		1,0,0,
		2,0,0,
		3,0,0,
		4,0,0,
		5,0,0,//六种元素
		0,1,1,//heart
		2,1,0,//dr_tank
		2,2,0,
		3,1,1,
		6,0,1,
		6,2,1,
		4,3,1,
		4,5,1,
		0,3,2,
		6,4,1
	};
	draw_scene_blk(hdc,dx,dy,scene_data[id][0],scene_data[id][1],scene_data[id][2],is2scr);
}
void get_bullet_st_pos_by_tank(int *bulletx,int *bullety,int tank_x,int tank_y,int tank_direction){//通过坦克方向获取子弹初始坐标
	switch(tank_direction){
	case DIRECT_UP:
		*bulletx=tank_x+5;
		*bullety=tank_y;
		break;
	case DIRECT_DOWN:
		*bulletx=tank_x+6;
		*bullety=tank_y+TANK_SIZE-BULLET_SIZE;
		break;
	case DIRECT_LEFT:
		*bulletx=tank_x;
		*bullety=tank_y+7;
		break;
	case DIRECT_RIGHT:
		*bulletx=tank_x+TANK_SIZE-BULLET_SIZE;
		*bullety=tank_y+7;
		break;
	}
}


void draw_tank(HDC hdc,int x,int y,int id,int level,int direction){//这个坐标用像素坐标，id是坦克类别，level是级别，其中敌人坦克LEVEL_RED值是变红的level
	int level_red[5]={0,1,3,1,1};
	int map_px,map_py;
	getmap_py(&map_px,&map_py);
	if(level==LEVEL_RED)level=level_red[id];
	BitBlt2SCR(hdc,map_px+x,map_py+y,TANK_SIZE,TANK_SIZE,pic_tanks[direction],level*TANK_SIZE,id*TANK_SIZE,DRAW_SPRITE);
}
void drawinit(HDC hdc,int x,int y){
	int map_px,map_py;
	getmap_py(&map_px,&map_py);
	draw_scene_id(hdc,x/8,y/8,map_init,1);
}
void draw_bullet(HDC hdc,int x,int y,int direction){
	int map_px,map_py;
	getmap_py(&map_px,&map_py);
	//printf("\n draw:%d,%d",x,y);
	BitBlt2SCR(hdc,map_px+x,map_py+y,BULLET_SIZE,BULLET_SIZE,pic_scene,48+direction*BULLET_SIZE,52,DRAW_SPRITE);
}

void draw_card(HDC hdc,int dx,int dy,int id){//x y 乘8是实际HDC坐标,id是card类型
	int map_px,map_py;
	getmap_py(&map_px,&map_py);
	BitBlt2SCR(hdc,map_px+dx*8,map_py+dy*8,CARD_SIZE,CARD_SIZE,pic_cards,0,id*CARD_SIZE,DRAW_SPRITE);
}

void drawbg(HDC hdc){
	int map_px,map_py;
	int r,rr,dr_tk_count=g_dr_tank_manager.tank_last;
	char txtbuf[16];
	getmap_py(&map_px,&map_py);
	SetFgColor(hdc,RGB(127,127,127));
	FillRect2Scr(hdc,0,0,scrchang,scrkuan);
	SetFgColor(hdc,RGB(0,0,0));
	FillRect2Scr(hdc,map_px,map_py,map_px+MAP_PIXEL-1,map_py+MAP_PIXEL-1);
	for(r=0;r<10;r++){
		if(dr_tk_count==0)break;
		for(rr=0;rr<2;rr++){
			if(dr_tk_count==0)break;
			draw_scene_id(hdc,MAP_BLK+1+rr,1+r,map_dr_tank_ico,1);
			dr_tk_count--;			
		}
	}
	draw_scene_id(hdc,MAP_BLK+1,MAP_BLK-5,map_stage_flag,1);
	sprintf(txtbuf,"%d",nowstage+1);textout(txtbuf,map_px+MAP_PIXEL+16,map_py+(MAP_BLK-3)*8,hdc);
	textout("IP",map_px+MAP_PIXEL+8,map_py+MAP_PIXEL/2,hdc);
	draw_scene_id(hdc,MAP_BLK+1,MAP_BLK/2+3,map_tank_ico,1);
	if(my_life>0)sprintf(txtbuf,"%d",my_life-1);
	else sprintf(txtbuf,"0");
	textout(txtbuf,map_px+MAP_PIXEL+16,map_py+(MAP_BLK/2+2)*8,hdc);
}

void newexplode(explode g_explode){
	if(g_explode_manager.explodecount<MAX_EXPLODE){
		g_explode_manager.explodes[g_explode_manager.explodecount]=g_explode;
		g_explode_manager.explodecount++;
	}
}
void moveexplode(int pos){
	int r;
	if(g_explode_manager.explodecount>0){
		for(r=pos;r<g_explode_manager.explodecount;r++){
			g_explode_manager.explodes[r]=g_explode_manager.explodes[r+1];
		}
		g_explode_manager.explodecount--;
	}
}
void draw_explode(HDC hdc){//绘制爆炸效果
	int r;
	for(r=0;r<g_explode_manager.explodecount;r++){
		switch(g_explode_manager.explodes[r].f/4){
		case 0:
			draw_scene_id(hdc,g_explode_manager.explodes[r].x,g_explode_manager.explodes[r].y,map_bz1,1);
			break;
		case  1:
			draw_scene_id(hdc,g_explode_manager.explodes[r].x,g_explode_manager.explodes[r].y,map_bz2,1);
			break;
		case 2:
			if(g_explode_manager.explodes[r].type==1)
				draw_scene_id(hdc,g_explode_manager.explodes[r].x-1,g_explode_manager.explodes[r].y-1,map_bz3,1);
			break;
		default:
			g_explode_manager.explodes[r].f=0;
			moveexplode(r);
			break;
		}
		g_explode_manager.explodes[r].f++;
	}
}



void newbullet(bulletshoot bullet){
	if(g_bullet_manager.bulletcount<MAX_BULLET){
		g_bullet_manager.bulletshoots[g_bullet_manager.bulletcount]=bullet;
		g_bullet_manager.bulletcount++;
	}
}
void movebullet(int pos){
	int r;
	if(g_bullet_manager.bulletcount>0){
		for(r=pos;r<g_bullet_manager.bulletcount;r++){
			g_bullet_manager.bulletshoots[r]=g_bullet_manager.bulletshoots[r+1];
		}
		g_bullet_manager.bulletcount--;
	}
}
void draw_bullets(HDC hdc){//绘制所有子弹
	int r;
	for(r=0;r<g_bullet_manager.bulletcount;r++){
		draw_bullet(hdc,g_bullet_manager.bulletshoots[r].x,g_bullet_manager.bulletshoots[r].y,g_bullet_manager.bulletshoots[r].direction);
	}
}
void do_bullets(){//子弹逻辑
	int r,fh;
	explode g_explode;
	my_bullet_count=0;
	for(r=0;r<g_bullet_manager.bulletcount;r++){
		if(g_bullet_manager.bulletshoots[r].type==0)my_bullet_count++;
		switch(g_bullet_manager.bulletshoots[r].direction){
		case DIRECT_UP:
			g_bullet_manager.bulletshoots[r].y-=g_bullet_manager.bulletshoots[r].speed;
			break;
		case DIRECT_DOWN:
			g_bullet_manager.bulletshoots[r].y+=g_bullet_manager.bulletshoots[r].speed;
			break;
		case DIRECT_LEFT:
			g_bullet_manager.bulletshoots[r].x-=g_bullet_manager.bulletshoots[r].speed;
			break;
		case DIRECT_RIGHT:
			g_bullet_manager.bulletshoots[r].x+=g_bullet_manager.bulletshoots[r].speed;
			break;
		}
		fh=bullet_move_judge(g_bullet_manager.bulletshoots[r].x,g_bullet_manager.bulletshoots[r].y,g_bullet_manager.bulletshoots[r].direction,g_bullet_manager.bulletshoots[r].type);
		if(fh==0)
		{
			g_explode.f=0;
			g_explode.type=0;
			g_explode.x=g_bullet_manager.bulletshoots[r].x/8;
			g_explode.y=g_bullet_manager.bulletshoots[r].y/8;
			newexplode(g_explode);
			movebullet(r);
		}
		else if(fh==2){
			gameovercount=GAMEOVERCOUNT;
		}
	}
}

void new_dr_tank(dr_tank g_dr_tank){
	if(g_dr_tank_manager.count<MAX_DR_TANK){
		g_dr_tank_manager.dr_tanks[g_dr_tank_manager.count]=g_dr_tank;
		g_dr_tank_manager.count++;
	}
}
void move_dr_tank(int pos){
	int r;
	if(g_dr_tank_manager.count>0){
		for(r=pos;r<g_dr_tank_manager.count;r++){
			g_dr_tank_manager.dr_tanks[r]=g_dr_tank_manager.dr_tanks[r+1];
		}
		g_dr_tank_manager.count--;
	}
}
void draw_dr_tank(HDC hdc){//绘制敌坦克
	int r;
	int level;
	for(r=0;r<g_dr_tank_manager.count;r++){
		if(g_dr_tank_manager.dr_tanks[r].isred==1)
			level=LEVEL_RED;
		else level=g_dr_tank_manager.dr_tanks[r].level;
		if(g_dr_tank_manager.dr_tanks[r].inittime>0)
			drawinit(hdc,g_dr_tank_manager.dr_tanks[r].x,g_dr_tank_manager.dr_tanks[r].y);
			else draw_tank(hdc,g_dr_tank_manager.dr_tanks[r].x,g_dr_tank_manager.dr_tanks[r].y,
			g_dr_tank_manager.dr_tanks[r].id,level,g_dr_tank_manager.dr_tanks[r].direction);
	}
}
void do_dr_tank(){//logic
	int r,rr;
	int tmp;
	bulletshoot g_bullet;
	dr_tank g_tank;
	int hasredtank=0;
	int move_distance=0;
	for(r=0;r<g_dr_tank_manager.count;r++){
		if(g_dr_tank_manager.dr_tanks[r].inittime>0)
			{
				g_dr_tank_manager.dr_tanks[r].inittime--;
				continue;
			}
		if(rand()%100<2&&my_cardtype!=0){
			g_bullet.direction=g_dr_tank_manager.dr_tanks[r].direction;
			g_bullet.speed=4;
			g_bullet.type=1;
			get_bullet_st_pos_by_tank(&g_bullet.x,&g_bullet.y,
				g_dr_tank_manager.dr_tanks[r].x,g_dr_tank_manager.dr_tanks[r].y,g_dr_tank_manager.dr_tanks[r].direction);
			newbullet(g_bullet);
		}
		if(g_dr_tank_manager.dr_tanks[r].movecount>0&&my_cardtype!=0){
			if(g_dr_tank_manager.dr_tanks[r].direction==DIRECT_UP)g_dr_tank_manager.dr_tanks[r].y-=g_dr_tank_manager.dr_tanks[r].speed;
			if(g_dr_tank_manager.dr_tanks[r].direction==DIRECT_DOWN)g_dr_tank_manager.dr_tanks[r].y+=g_dr_tank_manager.dr_tanks[r].speed;
			if(g_dr_tank_manager.dr_tanks[r].direction==DIRECT_LEFT)g_dr_tank_manager.dr_tanks[r].x-=g_dr_tank_manager.dr_tanks[r].speed;
			if(g_dr_tank_manager.dr_tanks[r].direction==DIRECT_RIGHT)g_dr_tank_manager.dr_tanks[r].x+=g_dr_tank_manager.dr_tanks[r].speed;
			g_dr_tank_manager.dr_tanks[r].movecount-=2;
		}
		else if(my_cardtype!=0){
			switch (g_dr_tank_manager.dr_tanks[r].direction){
				case DIRECT_UP:
					move_distance=tank_move_judge(g_dr_tank_manager.dr_tanks[r].x,g_dr_tank_manager.dr_tanks[r].y-8,r);
				break;
				case DIRECT_DOWN:
					move_distance=tank_move_judge(g_dr_tank_manager.dr_tanks[r].x,g_dr_tank_manager.dr_tanks[r].y+8,r);
				break;
				case DIRECT_LEFT:
					move_distance=tank_move_judge(g_dr_tank_manager.dr_tanks[r].x-8,g_dr_tank_manager.dr_tanks[r].y,r);
				break;
				case DIRECT_RIGHT:
					move_distance=tank_move_judge(g_dr_tank_manager.dr_tanks[r].x+8,g_dr_tank_manager.dr_tanks[r].y,r);
				break;
			}
			g_dr_tank_manager.dr_tanks[r].movecount=8*move_distance;
			if(move_distance==0){
				do 
				{
					tmp=rand()%4;
				} while (tmp==g_dr_tank_manager.dr_tanks[r].direction);
				g_dr_tank_manager.dr_tanks[r].direction=tmp;
			}
		}
		if(g_dr_tank_manager.dr_tanks[r].crash_cool>0)g_dr_tank_manager.dr_tanks[r].crash_cool--;
		if(g_dr_tank_manager.dr_tanks[r].isred==1)hasredtank=1;
	}
	if(g_dr_tank_manager.count==MAX_DR_TANK-1||g_dr_tank_manager.tank_last==0)return;
	r=rand()%3;
	for(rr=0;rr<g_dr_tank_manager.count;rr++){//防止坦克重叠
		if(abs(g_dr_tank_manager.dr_tanks[rr].x+TANK_SIZE/2-(r*(MAP_BLK/2-1)*8+TANK_SIZE/2)<TANK_SIZE)&&
			g_dr_tank_manager.dr_tanks[rr].y<=16)return ;
	}
	g_tank.direction=DIRECT_DOWN;
	g_tank.id=1+rand()%4;
	if(g_tank.id==2)g_tank.level=2;
	else g_tank.level=0;
	g_tank.speed=2;
	g_tank.movecount=0;
	if(hasredtank==0&&rand()%100<30)g_tank.isred=1;
	else g_tank.isred=0;
	g_tank.x=r*(MAP_BLK/2-1)*8;
	g_tank.crash_cool=0;
	g_tank.inittime=INIT_TIME;
	//printf("\n new x:%d,id:%d level:%d isred:%d",g_tank.x,g_tank.id,g_tank.level,g_tank.isred);
	g_tank.y=0;
	g_dr_tank_manager.tank_last--;
	new_dr_tank(g_tank);
}
void initmanagers(){
	g_bullet_manager.bulletcount=0;
	g_explode_manager.explodecount=0;
	g_dr_tank_manager.count=0;
	g_dr_tank_manager.tank_last=20;
}
#define MAX_STAGE	35
char stagefile[]="stagedata.bin";
void initstage(){
	int _mapdata=-1;
	int r,rr;
	FILE *fp=fopen(stagefile,"wb");
	for(r=0;r<MAX_STAGE;r++){
		for(rr=0;rr<MAP_BLK*MAP_BLK;rr++){
			fwrite(&_mapdata,1,sizeof(int),fp);
		}
	}
	fclose(fp);
}

void savestage(int stage){
	FILE *fp=fopen(stagefile,"rb+");
	int r,rr;
	if(fp==NULL){
		initstage();
		fp=fopen(stagefile,"rb+");
	}
	fseek(fp,4*MAP_BLK*MAP_BLK*stage,SEEK_CUR);
	for(r=0;r<MAP_BLK;r++){
		for(rr=0;rr<MAP_BLK;rr++){
			fwrite(&mapdata[r][rr],1,sizeof(int),fp);
		}
	}
	fclose(fp);
}
void loadstage(int stage){
	FILE *fp=fopen(stagefile,"rb");
	int r,rr;
	if(fp==NULL){
		initstage();
		fp=fopen(stagefile,"rb");
	}
	fseek(fp,4*MAP_BLK*MAP_BLK*stage,SEEK_CUR);
	for(r=0;r<MAP_BLK;r++){
		for(rr=0;rr<MAP_BLK;rr++){
			fread(&mapdata[r][rr],1,sizeof(int),fp);
		}
	}
	fclose(fp);
}
void automap(int stage){
char filename[64];
int r,rr,rrr;
HDC tmpimage=CreateDC(MAP_PIXEL,MAP_PIXEL);
sprintf(filename,"%d.gif",stage);
//printf("\n%s",filename);
LoadGif(tmpimage,0,0,0,0,filename,16,16,0,0);
memset(mapdata,-1,sizeof(mapdata));
for(r=0;r<MAP_BLK;r++){
	for(rr=0;rr<MAP_BLK;rr++){
		for(rrr=0;rrr<6;rrr++){
			if(GetPixel(tmpimage,rr*8,r*8)==GetPixel(pic_scene,rrr*8,0)
				&&GetPixel(tmpimage,rr*8+1,r*8)==GetPixel(pic_scene,rrr*8+1,0)){
				mapdata[r][rr]=rrr;
				break;
			}
		}
	}
}
CloseHandle(tmpimage);
laochaochange(0);
}
void mapedit(HDC hdc,int stage){
	int map_px,map_py;
	int map_blk_selx=0,map_blk_sely=0;
	int map_blk_sel_id=map_wood;
	eVKEY key;

	int r,rr;
	getmap_py(&map_px,&map_py);
	loadpics();
	_ClearArea(hdc,1);
	//初始化场景
	loadstage(stage);
	laochaochange(0);
	while(1){
		nowstage=stage;
		drawbg(hdc);
		SetFgColor(hdc,RGB(255,255,255));

		if(clock()%30>15)
			if(map_blk_sel_id==-1)FillRect2Scr(hdc,map_px+map_blk_selx*8,map_py+map_blk_sely*8,
				map_px+map_blk_selx*8+8,map_py+map_blk_sely*8+8);
			else draw_scene_id(hdc,map_blk_selx,map_blk_sely,map_blk_sel_id,1);
		//DrawRect(hdc,map_px+map_blk_selx*8,map_py+map_blk_sely*8,
		//	map_px+map_blk_selx*8+8,map_py+map_blk_sely*8+8);
		for(r=0;r<MAP_BLK;r++){
			for(rr=0;rr<MAP_BLK;rr++){
				if(mapdata[r][rr]>=0)draw_scene_id(hdc,rr,r,mapdata[r][rr],1);
			}
		}
		UpdateArea(hdc);
		key=GetKey();
		if(ckkey(0,key,0)==KEY_LEFT){
			map_blk_selx--;
			if(map_blk_selx<0)map_blk_selx=MAP_BLK-1;
		}
		if(ckkey(0,key,0)==KEY_RIGHT){
			map_blk_selx++;
			if(map_blk_selx==MAP_BLK)map_blk_selx=0;
		}
		if(ckkey(0,key,0)==KEY_UP){
			map_blk_sely--;
			if(map_blk_sely<0)map_blk_sely=MAP_BLK-1;
		}
		if(ckkey(0,key,0)==KEY_DOWN){
			map_blk_sely++;
			if(map_blk_sely==MAP_BLK)map_blk_sely=0;
		}
		if(ckkey(0,key,0)==VKEY_STAR&&map_blk_sel_id>=0){
			map_blk_sel_id--;
			//printf("\nmin %d",map_blk_sel_id);
		}
		if(ckkey(0,key,0)==VKEY_POUND&&map_blk_sel_id<MAX_SCENE-1){
			map_blk_sel_id++;
			//printf("\nadd %d",map_blk_sel_id);
		}
		if(ckkey(0,key,0)==KEY_LSK){
			mapdata[map_blk_sely][map_blk_selx]=map_blk_sel_id;
			//printf("\nadd %d,%d  %d",map_blk_selx,map_blk_sely,map_blk_sel_id);
		}
		if(ckkey(0,key,0)==KEY_RSK)savestage(stage);
		if(ckkey(0,key,0)==VKEY_0){
			memset(mapdata,-1,sizeof(mapdata));
			laochaochange(0);
		}
		if(ckkey(0,key,0)==VKEY_7){//场景自动识别
			automap(stage);
		}
		if(ckkey(0,key,0)==VKEY_5){
			stage++;
			loadstage(stage);
			automap(stage);
			savestage(stage);
		}
		if(ckkey(0,key,0)==VKEY_4){
			stage--;
			loadstage(stage);
		}
		if(ckkey(0,key,0)==VKEY_6){
			stage++;
			loadstage(stage);
		}
		Sleep(50);
	}
	unloadpics();
}

void updateimage(){
	int r,rr;
	SetBgColor(mapimage,RGB(0,0,0));
	ClearArea(mapimage);
	for(r=0;r<MAP_BLK;r++){
		for(rr=0;rr<MAP_BLK;rr++){
			if(mapdata[r][rr]>=0)draw_scene_id(mapimage,rr,r,mapdata[r][rr],0);
		}
	}
	SetKeyColor(mapimage,0);
}
void InitStageImage(HDC hdc,int stage){
	loadstage(stage);
	mapimage=CreateDC(MAP_PIXEL,MAP_PIXEL);
	updateimage();
	
}
void DeInitstage(){
	CloseHandle(mapimage);
	unloadpics();
}
int ckmapid_cancross(int mapid){//判断mapid是否可通过
	//printf("\n mapid:%d",mapid);
	if(mapid==map_rock||mapid==map_heart||mapid==map_water1||mapid==map_water2||mapid==map_wood)return 0;
	if(mapid==map_ice)return 2;
	return 1;
}
int tank_move_judge(int tank_x,int tank_y,int id){//判断某个区块坦克是否能通过
int mapid;
int spd;
int r;
if(tank_x<0||tank_x>MAP_PIXEL-TANK_SIZE||tank_y<0||tank_y>MAP_PIXEL-TANK_SIZE)return 0;
	if((spd=ckmapid_cancross(mapdata[(tank_y)/8][(tank_x)/8]))==0)return 0;
	else if((spd=ckmapid_cancross(mapdata[(tank_y)/8][(tank_x+8)/8]))==0)return 0;
	else if((spd=ckmapid_cancross(mapdata[(tank_y+8)/8][(tank_x)/8]))==0)return 0;
	else if((spd=ckmapid_cancross(mapdata[(tank_y+8)/8][(tank_x+8)/8]))==0)return 0;
	//坦克碰撞判断
	for(r=0;r<g_dr_tank_manager.count;r++){//与敌方坦克碰撞判断
		 if(id!=r){
				if(abs(g_dr_tank_manager.dr_tanks[r].x+8-(tank_x+8))<16&&
					abs(g_dr_tank_manager.dr_tanks[r].y+8-(tank_y+8))<16&&
					g_dr_tank_manager.dr_tanks[r].crash_cool==0
					){
						g_dr_tank_manager.dr_tanks[r].crash_cool=TANK_SIZE/g_dr_tank_manager.dr_tanks[r].speed;
						return 0;
				}
		}
	}
	if(abs(my_positionx+8-(tank_x+8))<16&&//与玩家坦克碰撞
		abs(my_positiony+8-(tank_y+8))<16&&id!=-1
		){
			return 0;
	}
	return spd;
}
int ckmapid_cancross_bullet(int mapid,int type){//判断mapid是否可通过
	
	if(mapid==map_wood){
		//printf("\n mapid:%d",mapid);
		return -1;
	}
	if(mapid==map_rock)
	{
		if(type==0&&my_level==3)return -1;
		return 2;
	}
	if(mapid==map_heart)return 0;
	return 1;
}
void earsemapdata(int bullet_x,int bullet_y){
	mapdata[bullet_y/8][bullet_x/8]=-1;
	//updateimage();
}
void erasemapimage(int bullet_x,int bullet_y,int direction){
	SetFgColor(mapimage,0);
	bullet_x=bullet_x/8*8;
	bullet_y=bullet_y/8*8;//坐标矫正
	//printf("\nerase");
	switch(direction){
		case DIRECT_UP:
			if(GetPixel(mapimage,bullet_x,bullet_y+4)!=0||
				GetPixel(mapimage,bullet_x+8,bullet_y+4)!=0)
				FillRect(mapimage,bullet_x,bullet_y+4,bullet_x+15,bullet_y+7);
			else {
				FillRect(mapimage,bullet_x,bullet_y,bullet_x+15,bullet_y+3);
				earsemapdata(bullet_x,bullet_y);
				earsemapdata(bullet_x+8,bullet_y);
			}
		break;
		case DIRECT_DOWN:
			if(GetPixel(mapimage,bullet_x,bullet_y)!=0||
				GetPixel(mapimage,bullet_x+8,bullet_y)!=0)
				FillRect(mapimage,bullet_x,bullet_y,bullet_x+15,bullet_y+3);
			else {
				FillRect(mapimage,bullet_x,bullet_y+4,bullet_x+15,bullet_y+7);
				earsemapdata(bullet_x,bullet_y);
				earsemapdata(bullet_x+8,bullet_y);
			}
		break;
		case DIRECT_LEFT:
			if(GetPixel(mapimage,bullet_x+4,bullet_y)!=0||
				GetPixel(mapimage,bullet_x+4,bullet_y+8)!=0)
				FillRect(mapimage,bullet_x+4,bullet_y,bullet_x+7,bullet_y+15);
			else {
				FillRect(mapimage,bullet_x,bullet_y,bullet_x+3,bullet_y+15);
				earsemapdata(bullet_x,bullet_y);
				earsemapdata(bullet_x,bullet_y+8);
			}
			break;
		case DIRECT_RIGHT:
			if(GetPixel(mapimage,bullet_x,bullet_y)!=0||
				GetPixel(mapimage,bullet_x,bullet_y+8)!=0)
				FillRect(mapimage,bullet_x,bullet_y,bullet_x+3,bullet_y+15);
			else {
				FillRect(mapimage,bullet_x+4,bullet_y,bullet_x+7,bullet_y+15);
				earsemapdata(bullet_x,bullet_y);
				earsemapdata(bullet_x,bullet_y+8);
			}
			break;
		}
}
int bullet_move_judge(int bullet_x,int bullet_y,int direction,int type){//判断某个区块子弹是否能通过，并返回是否要破坏
	int mapid;
	int fh1=0,fh2=0;
	int r;
	explode g_explode;
	if(bullet_x<0|bullet_x>MAP_PIXEL-BULLET_SIZE||bullet_y<0||bullet_y>MAP_PIXEL-BULLET_SIZE)return 0;
	if(direction==DIRECT_UP||direction==DIRECT_DOWN){
		fh1=ckmapid_cancross_bullet(mapdata[(bullet_y)/8][(bullet_x)/8],type);
		fh2=ckmapid_cancross_bullet(mapdata[(bullet_y)/8][(bullet_x+8)/8],type);
	}
	if(direction==DIRECT_LEFT||direction==DIRECT_RIGHT){
		fh1=ckmapid_cancross_bullet(mapdata[(bullet_y)/8][(bullet_x)/8],type);
		fh2=ckmapid_cancross_bullet(mapdata[(bullet_y+8)/8][(bullet_x)/8],type);
	}
//	if(type==0)printf("\n ck %d,%d",fh1,fh2);
	if(fh1==2||fh2==2)return 0;
	if(fh1==-1||fh2==-1)
	{
		erasemapimage(bullet_x,bullet_y,direction);//清除地图镜像数据
		if(fh1==-1||fh2==-1)return 0;		
	}

	for(r=0;r<g_bullet_manager.bulletcount;r++){
		if(type!=g_bullet_manager.bulletshoots[r].type){
			if(abs(g_bullet_manager.bulletshoots[r].x-bullet_x)<8&&abs(g_bullet_manager.bulletshoots[r].y-bullet_y)<8){
					movebullet(r);
					return 0;
			}
		}
		if(type==1&&my_cardtype!=4){//打爆我
			if(abs(my_positionx+TANK_SIZE/2-(bullet_x+BULLET_SIZE/2))<15&&
				abs(my_positiony+TANK_SIZE/2-(bullet_y+BULLET_SIZE/2))<15){
					if(my_level==3)my_level--;
					else {
						printf("fuckme");
						playsoundex("sound/bzme.mp3",MEDIA_MP3,0);
						my_level=0;
						g_explode.f=0;
						g_explode.type=1;
						g_explode.x=my_positionx/8;
						g_explode.y=my_positiony/8;
						newexplode(g_explode);
						my_life--;
						if(my_life>0){
							my_init=INIT_TIME;
						}
						else {
							my_positionx=-1;
							my_positiony=-1;
						}
					}
					return 0;
			}
		}
	}
	if(type==0){
		for(r=0;r<g_dr_tank_manager.count;r++){
			if(g_dr_tank_manager.dr_tanks[r].inittime!=0)continue;
			if(abs(g_dr_tank_manager.dr_tanks[r].x+TANK_SIZE/2-(bullet_x+BULLET_SIZE/2))<15&&
				abs(g_dr_tank_manager.dr_tanks[r].y+TANK_SIZE/2-(bullet_y+BULLET_SIZE/2))<15){
				if(g_dr_tank_manager.dr_tanks[r].isred==1)
					{
						g_dr_tank_manager.dr_tanks[r].isred=0;
						card_dx=rand()%(MAP_BLK-2);
						card_dy=rand()%(MAP_BLK-2);
						card_id=rand()%MAX_CARD;
						playsoundex("sound/card.mp3",MEDIA_MP3,0);
					}
				if(g_dr_tank_manager.dr_tanks[r].level>0){
					g_dr_tank_manager.dr_tanks[r].level--;
					return 0;
				}
				else {
					g_explode.f=0;
					g_explode.type=1;
					g_explode.x=g_dr_tank_manager.dr_tanks[r].x/8;
					g_explode.y=g_dr_tank_manager.dr_tanks[r].y/8;
					move_dr_tank(r);
					newexplode(g_explode);
					playsoundex("sound/bzdr.mp3",MEDIA_MP3,0);
					return 0;
				}
			}
		}
	}

	if(fh1==0||fh2==0){
		playsoundex("sound/bzme.mp3",MEDIA_MP3,0);
		if(mapdata[MAP_BLK-2][MAP_BLK/2-1]!=-1){
			g_explode.f=0;
			g_explode.type=1;
			g_explode.x=MAP_BLK/2-1;
			g_explode.y=MAP_BLK-2;
			newexplode(g_explode);
			mapdata[MAP_BLK-2][MAP_BLK/2-1]=-1;
			FillRect(mapimage,(MAP_BLK/2-1)*8,(MAP_BLK-2)*8,(MAP_BLK/2-1)*8+16,(MAP_BLK-2)*8+16);
		}
		return 2;
	}
	if(fh1!=1||fh2!=1)return 0;
	return 1;
}

int ckkeypad(int lastevent){
	int padcur[8][2]={
		43,354,  43,436, 7,390,  84,390,
		135,422  ,188,422,
		255,388, 255,433
	};
	int keysize[2]={
		45,45
	};
	int r;
		for(r=0;r<8;r++){
			if(_ckpeninrect(bichusta.x,bichusta.y,padcur[r][0],padcur[r][1],
				padcur[r][0]+keysize[0],padcur[r][1]+keysize[1],lastevent)==1)return r;
		}
		return -1;
}
void laochaochange(int mode){
int r;
for(r=0;r<4;r++){
	mapdata[MAP_BLK-3][MAP_BLK/2-2+r]=mode;
	draw_scene_id(mapimage,MAP_BLK/2-2+r,MAP_BLK-3,mapdata[MAP_BLK-3][MAP_BLK/2-2+r],0);
}
for(r=0;r<2;r++){
	mapdata[MAP_BLK-r-1][MAP_BLK/2-2]=mode;
	draw_scene_id(mapimage,MAP_BLK/2-2,MAP_BLK-r-1,mapdata[MAP_BLK-r-1][MAP_BLK/2-2],0);
	mapdata[MAP_BLK-r-1][MAP_BLK/2+1]=mode;
	draw_scene_id(mapimage,MAP_BLK/2+1,MAP_BLK-r-1,mapdata[MAP_BLK-r-1][MAP_BLK/2+1],0);
}
mapdata[MAP_BLK-2][MAP_BLK/2-1]=map_heart;
}
void docard(HDC hdc){//道具
	int r;
	explode g_explode;
	int map_px,map_py;
	if(laochaoprotecttime>0){
		if(laochaoprotecttime==1){
			laochaochange(0);
		}
		laochaoprotecttime--;
	}
	if(my_cardtimeout>0){
		if(my_cardtype==4){
			getmap_py(&map_px,&map_py);
			BitBlt2SCR(hdc,map_px+my_positionx,map_py+my_positiony,TANK_SIZE,TANK_SIZE,pic_scene,
				48,16*(my_cardtimeout%2),DRAW_SPRITE);
		}
		my_cardtimeout--;
	}
	else my_cardtype=-1;
	if(abs(my_positionx-card_dx*8)<16&&abs(my_positiony-card_dy*8)<16&&card_id!=-1){
		if(card_id==0){
			my_cardtimeout=300;
		}
		if(card_id==1){
			playsoundex("sound/bzdr.mp3",MEDIA_MP3,0);
			for(r=0;r<g_dr_tank_manager.count;r++){
				g_explode.f=0;
				g_explode.type=1;
				g_explode.x=g_dr_tank_manager.dr_tanks[r].x/8;
				g_explode.y=g_dr_tank_manager.dr_tanks[r].y/8;
				newexplode(g_explode);
			}
			g_dr_tank_manager.count=0;
		}
		if(card_id==2){
			if(my_level<3)my_level++;
		}
		if(card_id==3){
			laochaoprotecttime=600;
			laochaochange(1);
		}
		if(card_id==4){
			my_cardtimeout=400;
		}
		if(card_id==5)
			my_life++;
		my_cardtype=card_id;
		card_id=-1;
	}
}
int startani(HDC hdc){
	int r;
	char txtbuf[64];
	eVKEY key1;
	int visualkey;
	int nowevent;
	int maxstage,selstage=0;
	int selquit=0;
	SetFgColor(hdc,RGB(127,127,127));
	for(r=0;r<=scrkuan/2;r+=8){
		FillRect2Scr(hdc,0,0,scrchang,r);
		FillRect2Scr(hdc,0,scrkuan-r,scrchang,scrkuan);
		UpdateArea(hdc);
		Sleep(10);
	}
	loadscore(&maxstage,&r,&r);
	selstage=maxstage;
	clearqueen();
	while(1){
		SetFgColor(hdc,RGB(127,127,127));
		FillRect2Scr(hdc,0,0,scrchang,scrkuan);
		sprintf(txtbuf,"关卡 %d",selstage+1);
		midtextout(txtbuf,scrkuan/2-16,hdc);
		if(selquit==0){
			if(selstage>maxstage)midtextout("该关卡未解锁!",scrkuan-20,hdc);
			else midtextout("左右键选关,开始键开始游戏",scrkuan-20,hdc);
		}
		UpdateArea(hdc);
		if(selquit!=0)break;
		nowevent=getbichuevent();
		key1=GetKey();
		visualkey=ckkeypad(nowevent);
		if(ckkey(0,key1,0)==KEY_LEFT||(visualkey==VISUAL_KEY_LEFT&&nowevent==VEVENT_PENUP))selstage--;
		if(ckkey(0,key1,0)==KEY_RIGHT||(visualkey==VISUAL_KEY_RIGHT&&nowevent==VEVENT_PENUP))selstage++;
		if(ckkey(0,key1,0)==KEY_OK||(visualkey==VISUAL_KEY_START&&nowevent==VEVENT_PENUP)){
			if(selstage<=maxstage)selquit=1;
		}
		if(selstage<0)selstage=MAX_STAGE-1;
		if(selstage>MAX_STAGE-1)selstage=0;
		Sleep(10);
	}
	playsoundex("sound/start.mp3",MEDIA_MP3,0);
	Sleep(2230);
	for(r=scrkuan/2;r>=0;r-=8){
		SetFgColor(hdc,0);
		FillRect2Scr(hdc,0,0,scrchang,scrkuan);
		SetFgColor(hdc,RGB(127,127,127));
		FillRect2Scr(hdc,0,0,scrchang,r);
		FillRect2Scr(hdc,0,scrkuan-r,scrchang,scrkuan);
		UpdateArea(hdc);
		Sleep(10);
	}
	return selstage;
}
int gamemain(HDC hdc){
#define GAME_WAITSTART	1
#define GAME_PLAYING	2
#define GAME_PAUSE		3
	int map_px,map_py;
	///////////////////////////////////事件参数//////////////////////////////////////
	int needload=1;
	int needloadbg=1;
	int gamewincount;
	int gamestatic=GAME_WAITSTART;
	///////////////////////////////////图层元素参数//////////////////////////////////

	///////////////////////////////////游戏数据//////////////////////////////////////
	unsigned long clockst,clocken;
	char txtbuf[128];
	int r,rr,rrr;
	bulletshoot bullet;
	int menuquit=0;
	///////////////////////////////////输入参数//////////////////////////////////////
	eVKEY key1;
	int visualkeypush;
	int nowevent,lastevent;
	getmap_py(&map_px,&map_py);
	loadpics();
	nowloading(hdc);
	loadscore(&nowstage,&my_level,&my_life);
	nowstage=startani(hdc);
	clearqueen();
	bichusta.x=-1;
	while(1){
		//printf("%d %d %d \n",rabity,jumpjsd,needadd);
		clockst=clock();
		///////////////////////////////////载入数据//////////////////////////////////////
		if(needload==1){

			needload=0;
		}
		///////////////////////////////////处理事件//////////////////////////////////////
		if(gamestatic==GAME_WAITSTART){
			InitStageImage(hdc,nowstage);
			initmanagers();
			card_id=-1;
			my_cardtimeout=0;
			gameovercount=-1;
			gamewincount=-1;
			my_init=INIT_TIME;
			gamestatic=GAME_PLAYING;
		}
		if(gamestatic==GAME_PLAYING){
			if(my_init>0){
				my_direction=DIRECT_UP;
				my_move_count=0;
				my_positionx=8*8;//相对地图pixel坐标
				my_positiony=MAP_PIXEL-16;		
				//my_level=0;
				my_cardtimeout=100;
				my_cardtype=4;
				my_init--;
			}
			drawbg(hdc);
			BitBlt2SCR(hdc,map_px,map_py,MAP_PIXEL,MAP_PIXEL,mapimage,0,0,DRAW_SPRITE);
			if(my_init>0)drawinit(hdc,my_positionx,my_positiony);
				else if(my_life>0)draw_tank(hdc,my_positionx,my_positiony,0,my_level,my_direction);
			do_bullets();
			do_dr_tank();
			draw_bullets(hdc);
			draw_dr_tank(hdc);
			for(r=0;r<MAP_BLK;r++){
				for(rr=0;rr<MAP_BLK;rr++){
					if(mapdata[r][rr]==map_grass)draw_scene_id(hdc,rr,r,mapdata[r][rr],1);
					else if(mapdata[r][rr]==map_water1&&clock()%15>7)draw_scene_id(hdc,rr,r,map_water2,1);
				}
			}
			draw_explode(hdc);
			docard(hdc);
			if(my_life==0){
				if(gameovercount==-1)gameovercount=GAMEOVERCOUNT;
			}
			if(g_dr_tank_manager.tank_last==0&&g_dr_tank_manager.count==0&&gamewincount==-1)gamewincount=GAMEOVERCOUNT;
			if(gameovercount>0)gameovercount--;
			else if(gameovercount==0)break;
			if(gamewincount>0)gamewincount--;
			else if(gamewincount==0)break;
			if(card_id!=-1){
				if((clock()/100)%2==0)draw_card(hdc,card_dx,card_dy,card_id);
			}
			if(my_move_count>0){
				if(my_direction==DIRECT_UP)my_positiony-=2;
				if(my_direction==DIRECT_DOWN)my_positiony+=2;
				if(my_direction==DIRECT_LEFT)my_positionx-=2;
				if(my_direction==DIRECT_RIGHT)my_positionx+=2;
				my_move_count-=2;
			}
		}


		
		UpdateArea(hdc);	
		///////////////////////////////////输入控制//////////////////////////////////////
		nowevent=getbichuevent();
		if(nowevent!=-1){
			if(nowevent==VEVENT_PENMOVE&&lastevent==VEVENT_PENUP)lastevent=VEVENT_PENUP;
			else lastevent=nowevent;
		}
		if(gameovercount==-1){
				key1=GetKey();

				visualkeypush=ckkeypad(lastevent);
		
				if(ckkey(0,key1,0)==KEY_OK||((visualkeypush==VISUAL_KEY_A||visualkeypush==VISUAL_KEY_B)&&nowevent==VEVENT_PENUP)){
					if(((my_level>1&&my_bullet_count<2)||(my_level<=1&&my_bullet_count==0))&&my_init==0){
						get_bullet_st_pos_by_tank(&bullet.x,&bullet.y,my_positionx,my_positiony,my_direction);
						bullet.type=0;
						bullet.speed=4;
						bullet.direction=my_direction;
						playsoundex("sound/shoot.mp3",MEDIA_MP3,0);
						//printf("\n new %d,%d",bullet.x,bullet.y);
						newbullet(bullet);
					}
				}
				if(ckkey(0,key1,0)==KEY_RSK||visualkeypush==VISUAL_KEY_SELECT){
					if(mainmenu(hdc,1)==3){
						gameovercount=0;
						menuquit=1;
						break;
					}
				}
				if(lastevent==VEVENT_PENUP){
					bichusta.x=-1;
				}
				if((lastevent!=VEVENT_PENUP||visualkeypush==-1)&&my_move_count==0&&my_init==0){
					if(CheckKey(VKEY_UP)||visualkeypush==VISUAL_KEY_UP)
					{
						my_direction=DIRECT_UP;
						my_move_count=8*tank_move_judge(my_positionx,my_positiony-8,-1);
					}
					else if(CheckKey(VKEY_DOWN)||visualkeypush==VISUAL_KEY_DOWN)
					{
						my_direction=DIRECT_DOWN;
						my_move_count=8*tank_move_judge(my_positionx,my_positiony+8,-1);
					}
					else if(CheckKey(VKEY_LEFT)||visualkeypush==VISUAL_KEY_LEFT)
					{
						my_direction=DIRECT_LEFT;
						my_move_count=8*tank_move_judge(my_positionx-8,my_positiony,-1);
					}
					else if(CheckKey(VKEY_RIGHT)||visualkeypush==VISUAL_KEY_RIGHT)
					{
						my_direction=DIRECT_RIGHT;
						my_move_count=8*tank_move_judge(my_positionx+8,my_positiony,-1);
					}
			
				}
		}
		clocken=clock();
		if(clocken-clockst<20)Sleep(20-(clocken-clockst));
	}
	DeInitstage();
	if(gamewincount!=-1&&gameovercount==-1){
		if(nowstage<MAX_STAGE-1)nowstage++;
		else nowstage=0;
		savescore(nowstage,my_level,my_life);
		return 1;
	}
	else {
		if(menuquit==1&&my_life>0)return 0;
		my_level=0;
		my_life=3;
		savescore(nowstage,my_level,my_life);
		//remove(scorefile);
		return 0;
	}
}

int mainmenu(HDC hdc,int mode){
	int r;
	int nowsl=0;
	eVKEY key;
	int visualkeypush;
	int nowevent;
	HDC menuimage=CreateDC(scrchang,scrkuan);
	if(mode==0)LoadGif(menuimage,0,0,0,0,"gamemenu/gamemenu.gif",0,0,0,0);
	else LoadGif(menuimage,0,0,0,0,"gamemenu/gamemenu2.gif",0,0,0,0);
	clearqueen();
	if(mode==0){
		SetFgColor(hdc,0);
		FillRect2Scr(hdc,0,0,scrchang,scrkuan);
		for(r=scrkuan;r>=0;r-=4){
			BitBlt2SCR(hdc,0,r,scrchang,scrkuan-r,menuimage,0,0,DRAW_COPY);
			UpdateArea(hdc);
			Sleep(25);
		}
		loadpics();
	}
	while(1){
		BitBlt2SCR(hdc,0,0,scrchang,scrkuan,menuimage,0,0,DRAW_COPY);
		draw_tank(hdc,80,126+22*nowsl,0,0,DIRECT_RIGHT);
		UpdateArea(hdc);
		key=GetKey();
		visualkeypush=ckkeypad(nowevent);
		nowevent=getbichuevent();
		if(ckkey(0,key,0)==KEY_UP||(visualkeypush==VISUAL_KEY_UP&&nowevent==VEVENT_PENUP))nowsl--;
		if(ckkey(0,key,0)==KEY_DOWN||((visualkeypush==VISUAL_KEY_DOWN||visualkeypush==VISUAL_KEY_SELECT)&&nowevent==VEVENT_PENUP))
			nowsl++;
		if(ckkey(0,key,0)==KEY_OK||(visualkeypush==VISUAL_KEY_START&&nowevent==VEVENT_PENUP)){
			if(nowsl==0)break;
			else if(nowsl==1){
				clearqueen();
				LoadGif2SCR(hdc,0,0,0,0,"gamemenu/gamehelp.gif",0,0,0,0);
				UpdateArea(hdc);
				Sleep(223);
				anykeyortouch(1);
			}
			else if(nowsl==2){
				isplaybgm=msgshowEx(hdc,"是否开启音效?",LCD_KUAN/3,1,RGB(0,0,0));
			}
			else if(nowsl==3){
				if(mode==0){
					if(msgshowEx(hdc,"确定退出游戏?",LCD_KUAN/3,1,0)==1)exit(0);
					else _ClearArea(hdc,1);
				}
				else {
					if(msgshowEx(hdc,"确定返回主菜单?",LCD_KUAN/3,1,0)==1)break;
					else _ClearArea(hdc,1);
				}
			}
		}
		if(nowsl<0)nowsl=3;
		if(nowsl>3)nowsl=0;
		Sleep(50);
	}
	if(mode==0)unloadpics();
	else {
		clearqueen();
		Sleep(223);
	}
	CloseHandle(menuimage);

	return nowsl;
}
int main(int argc,char **argv)
{
	char txtbuf[128];
	int keymode;
	int nowsl=0,rrr;
	DeviceCaps   dev ;
	HDC hdc=GetScreenDC();
	THREAD_ATTR xiancheng;
	HANDLE bichuhandle;
	globeNum=CreateDC(60,9);
	LoadGif(globeNum,0,0,0,0,"/gamepic/interface_score1.gif",0,0,0,0);
	SetKeyColor(globeNum,RGB(0,0,255));	
	xiancheng.createflag=0;
	xiancheng.priority=8;
	xiancheng.object=0;
	xiancheng.stacksize=256;
	bichuhandle=CreateThread(bichugetter,NULL,&xiancheng);
	GetDeviceCaps  ( &dev   ); 
	scrmode=-1;
	LCD_CHANG=dev.ScreenWidth;
	LCD_KUAN=dev.ScreenHeight;
	//if(dev.ScreenHeight==220&&dev.ScreenWidth==176)scrmode=0;
	if(dev.ScreenHeight==320&&dev.ScreenWidth==240)scrmode=1;
	if(dev.ScreenHeight==400&&dev.ScreenWidth==240)scrmode=2;
	if(dev.ScreenHeight==480&&dev.ScreenWidth==320)scrmode=3;


	scrchang=256;
	scrkuan=240;
	x_add=(LCD_CHANG-scrchang)/2;
	if(scrmode==3)y_add=40;
	else if(scrmode==2)y_add=20;
	else y_add=0;
	//initgraphdata();
	//gamemain(hdc);
	//mapedit(hdc,23);

	//startani(hdc);
	isplaybgm=msgshowEx(hdc,"是否开启音效?",LCD_KUAN/3,1,RGB(0,0,0));
		while(1){
			mainmenu(hdc,0);
			_ClearArea(hdc,1);
			while(1){
					if(gamemain(hdc)==0){
						playsoundex("sound/gameover.mp3",MEDIA_MP3,0);
						_ClearArea(hdc,1);
						LoadGif2SCR(hdc,0,0,0,0,"gamemenu/gameover.gif",0,0,0,0);
						UpdateArea(hdc);
						Sleep(2230);
						break;
					}
				}
			}


	
	exit(0);
	return 0;
}