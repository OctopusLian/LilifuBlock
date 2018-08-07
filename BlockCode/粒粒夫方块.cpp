#include <graphics.h>
#include <easyx.h>
#include <conio.h>
#include <time.h>
#include <conio.h>
#pragma comment(lib,"Winmm.lib")
#define	WIDTH	10		// 游戏区宽度
#define	HEIGHT	22		// 游戏区高度
#define	UNIT	20		// 每个游戏区单位的实际像素

	IMAGE img1;
	IMAGE img2;
	IMAGE img3;
	int screenx=GetSystemMetrics(SM_CXSCREEN);
	int screeny=GetSystemMetrics(SM_CYSCREEN);

// 定义操作类型
enum CMD
{
	CMD_ROTATE,						// 方块旋转
	CMD_LEFT, CMD_RIGHT, CMD_DOWN,	// 方块左、右、下移动
	CMD_SINK,						// 方块沉底
	CMD_QUIT						// 退出游戏
};

// 定义绘制方块的方法
enum DRAW
{
	SHOW,	// 显示方块
	CLEAR,	// 擦除方块
	FIX		// 固定方块
};

// 定义七种俄罗斯方块
struct BLOCK
{
	WORD dir[4];	// 方块的四个旋转状态
	COLORREF color;	// 方块的颜色
}	g_Blocks[7] = {	{0x0F00, 0x4444, 0x0F00, 0x4444, RED},		// I
					{0x0660, 0x0660, 0x0660, 0x0660, BLUE},		// 口
					{0x4460, 0x02E0, 0x0622, 0x0740, MAGENTA},	// L
					{0x2260, 0x0E20, 0x0644, 0x0470, YELLOW},	// 反L
					{0x0C60, 0x2640, 0x0C60, 0x2640, CYAN},		// Z
					{0x0360, 0x4620, 0x0360, 0x4620, GREEN},	// 反Z
					{0x4E00, 0x4C40, 0x0E40, 0x4640, BROWN}};	// T

// 定义当前方块、下一个方块的信息
struct BLOCKINFO
{
	byte id;	// 方块 ID
	char x, y;	// 方块在游戏区中的坐标
	byte dir:2;	// 方向
}	g_CurBlock, g_NextBlock;

// 定义游戏区
BYTE g_World[WIDTH][HEIGHT] = {0};



/////////////////////////////////////////////
// 函数声明
/////////////////////////////////////////////

void Init();											// 初始化游戏
void Quit();											// 退出游戏
void NewGame();											// 开始新游戏
void GameOver();										// 结束游戏
CMD  GetCmd();											// 获取控制命令
void DispatchCmd(CMD _cmd);								// 分发控制命令
void NewBlock();										// 生成新的方块
bool CheckBlock(BLOCKINFO _block);						// 检测指定方块是否可以放下
void DrawUnit(int x, int y, COLORREF c, DRAW _draw);	// 画单元方块
void DrawBlock(BLOCKINFO _block, DRAW _draw = SHOW);	// 画方块
void OnRotate();										// 旋转方块
void OnLeft();											// 左移方块
void OnRight();											// 右移方块
void OnDown();											// 下移方块
void OnSink();											// 沉底方块





// 主函数
void main()
{
	Init();

	CMD c;
	while(true)
	{
		c = GetCmd();
		DispatchCmd(c);

		// 按退出时，显示对话框咨询用户是否退出
		if (c == CMD_QUIT) 
		{
			HWND wnd = GetHWnd();
			if (MessageBox(wnd, _T("您要退出游戏吗？"), _T("提醒"), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
							if (MessageBox(wnd, _T("请再考虑考虑！！！\n您仍要执意退出么？"), _T("最后一次提示"), MB_OKCANCEL | MB_ICONWARNING) == IDOK)
				Quit();
		}
	}
}

void Welcome()
{	
	initgraph(screenx, screeny);
	putimage(0, 0,1920, 1080,&img2,0,0);

	//全屏窗口
	HWND hwnd = GetHWnd();
	SetWindowLong(hwnd , GWL_STYLE , GetWindowLong(hwnd, GWL_STYLE) - WS_CAPTION);
	SetWindowPos(hwnd , HWND_TOP , 0 , 0 , screenx , screeny , SWP_SHOWWINDOW);

	int c=255;
	int flag=1;
	LOGFONT f;
	gettextstyle(&f);                     
	f.lfHeight = 30;                      
	_tcscpy(f.lfFaceName, _T("宋体"));    
	f.lfQuality = PROOF_QUALITY;    
	settextstyle(&f);                     
	setbkmode(TRANSPARENT);
	while(!kbhit())
	{

		setcolor(RGB(c, c, c));
		outtextxy(screenx/2-100, screeny-50, " 按任意键开始游戏");
		if(c<=0)
			flag=0;
		
		if(flag)
			c -= 5;
		
		else
		{
			c += 5;
			if(c>=255)
				flag=1;
		}
		Sleep(20);
	}
		getch();

	
}
// 初始化游戏
void Init()
{
	initgraph(275, 275);
		// 打开音乐
	mciSendString(_T("open res\\背景音乐.mp3 alias mymusic"), NULL, 0, NULL);
    mciSendString(_T("open res\\音效.mp3 alias yx"), NULL, 0, NULL);
		
	outtextxy(0,0,"什么鬼游戏");
	Sleep(1500);
	outtextxy(0,25,"不想做了");
	Sleep(1500);
	outtextxy(0,50,"所以自己去百度吧");
	Sleep(1500);
	outtextxy(0,75,"网上资源那么多");
	Sleep(1500);
	outtextxy(0,100,"干嘛还找我们做！！！");
	Sleep(3000);
	outtextxy(0,125,"好吧，不玩了，开始吧");
	Sleep(1500);
	outtextxy(0,150,"既然是游戏");
	Sleep(1500);
	outtextxy(0,175,"怎么能没有");
	Sleep(1000);
	outtextxy(0,200,"BGM呢？");
	Sleep(1000);
		// 播放音乐
	mciSendString(_T("play mymusic repeat"), NULL, 0, NULL);	
		outtextxy(0,225,"怎么能");
	Sleep(2000);
		outtextxy(0,250,"不全屏呢！");
	Sleep(2000);
	
	loadimage(&img1, _T("res\\start.jpg"));
	loadimage(&img2, _T("res\\background1.jpg"));
	loadimage(&img3, _T("res\\background2.jpg"));
	


	Welcome();
	putimage(0, 0,1920, 1080,&img3,0,0);

	setorigin(screenx/2-320 , screeny/2-240);


	srand((unsigned)time(NULL));
	setbkmode(TRANSPARENT);			// 设置图案填充的背景色为透明
	
	//主界面



	putimage(0, 0,640, 480,&img1,0,0);

	
	// 设置坐标原点
	setorigin(220+screenx/2-320, 20+screeny/2-240);

;

	// 开始新游戏

	NewGame();
}


// 退出游戏
void Quit()
{
	int i;
	setlinecolor(0x000000);
	setorigin(0, 0);
	for(i=0;i<screeny;i++)
	{
	line(0,i,screenx,i);
	Sleep(3);
	}

	LOGFONT f;
	gettextstyle(&f);                     // 获取当前字体设置
	f.lfHeight = 72;                      // 设置字体高度
	_tcscpy(f.lfFaceName, _T("Snap ITC"));    // 设置字体为“黑体”(高版本 VC 推荐使用 _tcscpy_s 函数)
	f.lfQuality = PROOF_QUALITY;    // 设置输出效果为抗锯齿  
	settextstyle(&f);                     // 设置字体样式
	
	for(i=0;i<255;i++)
	{
		Sleep(20);
		settextcolor(RGB(i,i,i));
		outtextxy(screenx/2-320, screeny/2-100, _T("TO BE CONTINUE"));
	}
	Sleep(1000);

	closegraph();
	exit(0);
}


// 开始新游戏
void NewGame()
{
	// 清空游戏区

		for(int x = 0; x < 10; x++)
			for(int y = 0; y < HEIGHT; y++)
				
			{
				g_World[x][y] = 0;
				putimage(x * UNIT, (HEIGHT - y - 1) * UNIT,UNIT, HEIGHT,&img1,x * UNIT+220, (HEIGHT - y - 1) * UNIT+20);
			}
	// 生成下一个方块
	g_NextBlock.id = rand() % 7;
	g_NextBlock.dir = rand() % 4;
	g_NextBlock.x = WIDTH + 1;
	g_NextBlock.y = HEIGHT - 1;

	// 获取新方块
	NewBlock();
}


// 结束游戏
void GameOver()
{
	HWND wnd = GetHWnd();
	if (MessageBox(wnd, _T("游戏结束。\n是不是不服？\n想重新来一局吗？"), _T("死了吧你"), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		NewGame();

	}
	else
		if(MessageBox(wnd, _T("什么！！！。\n您确定没点错？\n确定退出么？"), _T("朋友你点错了"), MB_YESNO | MB_ICONWARNING) == IDYES)
			Quit();
		else
		NewGame();
}


// 获取控制命令
DWORD m_oldtime;
CMD GetCmd()
{
	// 获取控制值
	while(true)
	{
		// 如果超时，自动下落一格
		DWORD newtime = GetTickCount();
		if (newtime - m_oldtime >= 500)
		{
			m_oldtime = newtime;
			return CMD_DOWN;
		}

		// 如果有按键，返回按键对应的功能
		if (kbhit())
		{
			switch(getch())
			{
				case 'w':
				case 'W':	return CMD_ROTATE;
				case 'a':
				case 'A':	return CMD_LEFT;
				case 'd':
				case 'D':	return CMD_RIGHT;
				case 's':
				case 'S':	return CMD_DOWN;
				case 27:	return CMD_QUIT;
				case ' ':	return CMD_SINK;
				case 0:
				case 0xE0:
					switch(getch())
					{
						case 72:	return CMD_ROTATE;
						case 75:	return CMD_LEFT;
						case 77:	return CMD_RIGHT;
						case 80:	return CMD_DOWN;
					}
			}
		}

		// 延时 (降低 CPU 占用率)
		Sleep(20);
	}
}


// 分发控制命令
void DispatchCmd(CMD _cmd)
{
	switch(_cmd)
	{
		case CMD_ROTATE:	OnRotate();		break;
		case CMD_LEFT:		OnLeft();		break;
		case CMD_RIGHT:		OnRight();		break;
		case CMD_DOWN:		OnDown();		break;
		case CMD_SINK:		OnSink();		break;
		case CMD_QUIT:		break;
	}
}


// 生成新的方块
void NewBlock()
{
	g_CurBlock.id = g_NextBlock.id,		g_NextBlock.id = rand() % 7;
	g_CurBlock.dir = g_NextBlock.dir,	g_NextBlock.dir = rand() % 4;
	g_CurBlock.x = (WIDTH - 4) / 2;
	g_CurBlock.y = HEIGHT + 2;

	// 下移新方块直到有局部显示
	WORD c = g_Blocks[g_CurBlock.id].dir[g_CurBlock.dir];
	while((c & 0xF) == 0)
	{
		g_CurBlock.y--;
		c >>= 4;
	}

	// 绘制新方块
	DrawBlock(g_CurBlock);

	// 绘制下一个方块

	putimage((WIDTH + 1) * UNIT, 0, (WIDTH + 5) * UNIT - 1, 4 * UNIT - 1,&img1,(WIDTH + 1) * UNIT+220, 20);
	DrawBlock(g_NextBlock);

	// 设置计时器，用于判断自动下落
	m_oldtime = GetTickCount();
}


// 画单元方块
void DrawUnit(int x, int y, COLORREF c, DRAW _draw)
{
	// 计算单元方块对应的屏幕坐标
	int left = x * UNIT;
	int top = (HEIGHT - y - 1) * UNIT;
	int right = (x + 1) * UNIT - 1;
	int bottom = (HEIGHT - y) * UNIT - 1;

	// 画单元方块
	switch(_draw)
	{
		case SHOW:
			// 画普通方块
			setlinecolor(0x006060);
			roundrect(left + 1, top + 1, right - 1, bottom - 1, 5, 5);
			setlinecolor(0x003030);
			roundrect(left, top, right, bottom, 8, 8);
			setfillcolor(c);
			setlinecolor(LIGHTGRAY);
			fillrectangle(left + 2, top + 2, right - 2, bottom - 2);
			break;

		case FIX:
			// 画固定的方块
			setfillcolor(RGB(GetRValue(c) * 2 / 3, GetGValue(c) * 2 / 3, GetBValue(c) * 2 / 3));
			setlinecolor(DARKGRAY);
			fillrectangle(left + 1, top + 1, right - 1, bottom - 1);
			break;

		case CLEAR:
			// 擦除方块

	        putimage(x * UNIT, (HEIGHT - y - 1) * UNIT,UNIT, HEIGHT,&img1,x * UNIT+220, (HEIGHT - y - 1) * UNIT+20);
	 		break;
	}
}


// 画方块
void DrawBlock(BLOCKINFO _block, DRAW _draw)
{
	WORD b = g_Blocks[_block.id].dir[_block.dir];
	int x, y;

	for(int i = 0; i < 16; i++, b <<= 1)
		if (b & 0x8000)
		{
			x = _block.x + i % 4;
			y = _block.y - i / 4;
			if (y < HEIGHT)
				DrawUnit(x, y, RGB(x*y*i,255-i*5,i*15), _draw);
		}
}


// 检测指定方块是否可以放下
bool CheckBlock(BLOCKINFO _block)
{
	WORD b = g_Blocks[_block.id].dir[_block.dir];
	int x, y;

	for(int i = 0; i < 16; i++, b <<= 1)
		if (b & 0x8000)
		{
			x = _block.x + i % 4;
			y = _block.y - i / 4;
			if ((x < 0) || (x >= WIDTH) || (y < 0))
				return false;

			if ((y < HEIGHT) && (g_World[x][y]))
				return false;
		}

	return true;
}


// 旋转方块
void OnRotate()
{
	// 获取可以旋转的 x 偏移量
	int dx;
	BLOCKINFO tmp = g_CurBlock;
	tmp.dir++;					if (CheckBlock(tmp))	{	dx = 0;		goto rotate;	}
	tmp.x = g_CurBlock.x - 1;	if (CheckBlock(tmp))	{	dx = -1;	goto rotate;	}
	tmp.x = g_CurBlock.x + 1;	if (CheckBlock(tmp))	{	dx = 1;		goto rotate;	}
	tmp.x = g_CurBlock.x - 2;	if (CheckBlock(tmp))	{	dx = -2;	goto rotate;	}
	tmp.x = g_CurBlock.x + 2;	if (CheckBlock(tmp))	{	dx = 2;		goto rotate;	}
	return;

rotate:
	// 旋转
	DrawBlock(g_CurBlock, CLEAR);
	g_CurBlock.dir++;
	g_CurBlock.x += dx;
	DrawBlock(g_CurBlock);	
}


// 左移方块
void OnLeft()
{
	BLOCKINFO tmp = g_CurBlock;
	tmp.x--;
	if (CheckBlock(tmp))
	{
		DrawBlock(g_CurBlock, CLEAR);
		g_CurBlock.x--;
		DrawBlock(g_CurBlock);
	}
	
}


// 右移方块
void OnRight()
{
	BLOCKINFO tmp = g_CurBlock;
	tmp.x++;
	if (CheckBlock(tmp))
	{
		DrawBlock(g_CurBlock, CLEAR);
		g_CurBlock.x++;
		DrawBlock(g_CurBlock);
	}
	
}


// 下移方块
void OnDown()
{
	BLOCKINFO tmp = g_CurBlock;
	tmp.y--;
	if (CheckBlock(tmp))
	{
		DrawBlock(g_CurBlock, CLEAR);
		g_CurBlock.y--;
		DrawBlock(g_CurBlock);
	}
	else
		OnSink();	// 不可下移时，执行“沉底方块”操作


}


// 沉底方块
void OnSink()
{
	int i, x, y;

	// 连续下移方块
	DrawBlock(g_CurBlock, CLEAR);
	BLOCKINFO tmp = g_CurBlock;
	tmp.y--;
	while (CheckBlock(tmp))
	{
		g_CurBlock.y--;
		tmp.y--;
	}
	DrawBlock(g_CurBlock, FIX);

	// 固定方块在游戏区
	WORD b = g_Blocks[g_CurBlock.id].dir[g_CurBlock.dir];
	for(i = 0; i < 16; i++, b <<= 1)
		if (b & 0x8000)
		{
			if (g_CurBlock.y - i / 4 >= HEIGHT)
			{	// 如果方块的固定位置超出高度，结束游戏
				GameOver();
				return;
			}
			else
				g_World[g_CurBlock.x + i % 4][g_CurBlock.y - i / 4] = 1;
		}

	// 检查是否需要消掉行，并标记
	BYTE remove = 0;	// 低 4 位用来标记方块涉及的 4 行是否有消除行为
	for(y = g_CurBlock.y; y >= max(g_CurBlock.y - 3, 0); y--)
	{
		i = 0;
		for(x = 0; x < WIDTH; x++)
			if (g_World[x][y] == 1)
				i++;

		if (i == WIDTH)
		{
			remove |= (1 << (g_CurBlock.y - y));
			setfillcolor(LIGHTGREEN);
			setlinecolor(LIGHTGREEN);
			setfillstyle(BS_HATCHED, HS_DIAGCROSS);
			fillrectangle(0, (HEIGHT - y - 1) * UNIT + UNIT / 2 - 5, WIDTH * UNIT - 1, (HEIGHT - y - 1) * UNIT + UNIT / 2 + 5);
			setfillstyle(BS_SOLID);
		}
	}

	if (remove)	// 如果产生整行消除
	{
		// 延时 300 毫秒
		Sleep(300);

		// 擦掉刚才标记的行2
		IMAGE img;
		for(i = 0; i < 4; i++, remove >>= 1)
		{
			if (remove & 1)
			{
				for(y = g_CurBlock.y - i + 1; y < HEIGHT; y++)
					for(x = 0; x < WIDTH; x++)
					{
						g_World[x][y - 1] = g_World[x][y];
						g_World[x][y] = 0;
					}


				getimage(&img, 0, 0, WIDTH * UNIT, (HEIGHT - (g_CurBlock.y - i + 1)) * UNIT);
				putimage(0, UNIT, &img);
			}
		}
		for(x = 0; x < 10; x++)
			for(y = g_CurBlock.y - i + 1; y < HEIGHT; y++)
				if(g_World[x][y] == 0)
				putimage(x * UNIT, (HEIGHT - y - 1) * UNIT,UNIT, HEIGHT,&img1,x * UNIT+220, (HEIGHT - y - 1) * UNIT+20);
			
		mciSendString(_T("play yx repeat "), NULL, 0, NULL);
		Sleep(300);
		mciSendString(_T("stop yx"), NULL, 0, NULL);
	}

	// 产生新方块

	NewBlock();
}