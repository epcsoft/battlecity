void midtextoutEx(char txt[],int y,int num,  eDRAW mode,eFONT font,int bdColor,int fontColor,HDC _hdc);
void midtextout(char txt[],int y,HDC _hdc);
void textout(char txt[],int x,int y,HDC _hdc);
void drawtstxt(HDC hdc,char lefttxt[],char righttxt[]);
void BitBlt2SCR(HDC hdc,int xd, int yd, int w, int h, HDC hdcs, int xs, int ys, eDRAW mode);
void DrawLine2SCR(HDC hdc, int x0, int y0, int x1, int y1);
void _ClearArea (HDC hdc,int mode);
void BitBlt2SCR_Center(HDC hdc,int xd, int yd, int w, int h, HDC hdcs, int xs, int ys, eDRAW mode);
void Loadresizepic(HDC *mubiao,HDC tmp,int yuanshix,int yuanshiy,int mubiaox,int mubiaoy,char *filename);
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
				  ) ;
void FillRect2Scr(HDC hdc,int x1,int y1,int x2,int y2);
void LoadGif2SCR(HDC hdc,int x,int y,int w,int h,char *filename,int cx,int cy,int a,int b);
void refreshbuttom(HDC hdc);
void shownums(HDC hdc,int num,int x,int y);
void showGnums(HDC hdc,int num,int x,int y);
void initgraphdata();
void AlphaBlend2SCR  ( HDC  hdc,  
					  int  xd,  
					  int  yd,  
					  int  w,  
					  int  h,  
					  HDC  hdcs,  
					  int  xs,  
					  int  ys,  
					  int  mode   
					  )  ;