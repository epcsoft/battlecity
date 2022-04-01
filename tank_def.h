typedef  struct bulletshoot{
	int x;//��������
	int y;
	int direction;//����
	int speed;//�ٶ�
	int type;//0 �ҷ���1 �з�
}bulletshoot;
#define MAX_BULLET		100
typedef struct bullet_manager{
	int bulletcount;
	struct bulletshoot bulletshoots[MAX_BULLET];
}bullet_manager;
bullet_manager g_bullet_manager;

typedef  struct dr_tank{
	int x;//��������
	int y;
	int direction;//����
	int speed;//�ٶ�
	int movecount;
	int id;
	int level;
	int isred;
	int inittime;
	int crash_cool;//��ײ�ж���ȴ
}dr_tank;

#define MAX_DR_TANK 	5
typedef struct dr_tank_manager{
	int count;
	int tank_last;//ʣ��̹����
	struct dr_tank dr_tanks[MAX_DR_TANK];
}dr_tank_manager;
dr_tank_manager g_dr_tank_manager;

typedef struct explode{
	int x;//blk����
	int y;
	int f;//��������֡
	int type;//0С��ը��1��ը
}explode;
#define MAX_EXPLODE		100
typedef struct explode_manager{
	int explodecount;
	struct explode explodes[MAX_EXPLODE];
}explode_manager;
explode_manager g_explode_manager;
#define MAX_CARD	6
HDC mapimage;//�ؿ�����ͼ
int card_dx,card_dy;
int card_id;
int my_positionx;//��Ե�ͼpixel����
int my_positiony;
int my_direction;
int my_move_count;
int my_bullet_count;//�ѷ����ڵ�
int my_cardtimeout;//ʣ��ʱ��
int my_cardtype;//��������
int laochaoprotecttime;
int my_level;
int my_init;
int nowstage;
int my_life;
#define GAMEOVERCOUNT		100
int gameovercount;
#define MAX_SCENE 16	
typedef  enum mapdatadef{
	map_wood=0,
	map_rock,
	map_grass,
	map_water1,
	map_water2,
	map_ice,
	map_heart,
	map_dr_tank_ico,
	map_tank_ico,
	map_stage_flag,
	map_bx1,
	map_bx2,
	map_bz1,
	map_bz2,
	map_bz3,
	map_init
}mapdatadef;
#define INIT_TIME		25
#define DIRECT_UP		0
#define DIRECT_RIGHT	3
#define DIRECT_DOWN	2
#define DIRECT_LEFT		1

#define MAP_PIXEL		208
#define MAP_BLK	26	
#define LEVEL_RED		-1
#define TANK_SIZE		16
#define BULLET_SIZE	4
#define CARD_SIZE		16
#define PAI	3.1415926

#define VISUAL_KEY_UP			0
#define VISUAL_KEY_DOWN	1
#define VISUAL_KEY_LEFT		2
#define VISUAL_KEY_RIGHT	3
#define VISUAL_KEY_SELECT	4
#define VISUAL_KEY_START	5
#define VISUAL_KEY_A			6
#define VISUAL_KEY_B			7