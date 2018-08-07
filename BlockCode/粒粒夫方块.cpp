#include <graphics.h>
#include <easyx.h>
#include <conio.h>
#include <time.h>
#include <conio.h>
#pragma comment(lib,"Winmm.lib")
#define	WIDTH	10		// ��Ϸ�����
#define	HEIGHT	22		// ��Ϸ���߶�
#define	UNIT	20		// ÿ����Ϸ����λ��ʵ������

	IMAGE img1;
	IMAGE img2;
	IMAGE img3;
	int screenx=GetSystemMetrics(SM_CXSCREEN);
	int screeny=GetSystemMetrics(SM_CYSCREEN);

// �����������
enum CMD
{
	CMD_ROTATE,						// ������ת
	CMD_LEFT, CMD_RIGHT, CMD_DOWN,	// �������ҡ����ƶ�
	CMD_SINK,						// �������
	CMD_QUIT						// �˳���Ϸ
};

// ������Ʒ���ķ���
enum DRAW
{
	SHOW,	// ��ʾ����
	CLEAR,	// ��������
	FIX		// �̶�����
};

// �������ֶ���˹����
struct BLOCK
{
	WORD dir[4];	// ������ĸ���ת״̬
	COLORREF color;	// �������ɫ
}	g_Blocks[7] = {	{0x0F00, 0x4444, 0x0F00, 0x4444, RED},		// I
					{0x0660, 0x0660, 0x0660, 0x0660, BLUE},		// ��
					{0x4460, 0x02E0, 0x0622, 0x0740, MAGENTA},	// L
					{0x2260, 0x0E20, 0x0644, 0x0470, YELLOW},	// ��L
					{0x0C60, 0x2640, 0x0C60, 0x2640, CYAN},		// Z
					{0x0360, 0x4620, 0x0360, 0x4620, GREEN},	// ��Z
					{0x4E00, 0x4C40, 0x0E40, 0x4640, BROWN}};	// T

// ���嵱ǰ���顢��һ���������Ϣ
struct BLOCKINFO
{
	byte id;	// ���� ID
	char x, y;	// ��������Ϸ���е�����
	byte dir:2;	// ����
}	g_CurBlock, g_NextBlock;

// ������Ϸ��
BYTE g_World[WIDTH][HEIGHT] = {0};



/////////////////////////////////////////////
// ��������
/////////////////////////////////////////////

void Init();											// ��ʼ����Ϸ
void Quit();											// �˳���Ϸ
void NewGame();											// ��ʼ����Ϸ
void GameOver();										// ������Ϸ
CMD  GetCmd();											// ��ȡ��������
void DispatchCmd(CMD _cmd);								// �ַ���������
void NewBlock();										// �����µķ���
bool CheckBlock(BLOCKINFO _block);						// ���ָ�������Ƿ���Է���
void DrawUnit(int x, int y, COLORREF c, DRAW _draw);	// ����Ԫ����
void DrawBlock(BLOCKINFO _block, DRAW _draw = SHOW);	// ������
void OnRotate();										// ��ת����
void OnLeft();											// ���Ʒ���
void OnRight();											// ���Ʒ���
void OnDown();											// ���Ʒ���
void OnSink();											// ���׷���





// ������
void main()
{
	Init();

	CMD c;
	while(true)
	{
		c = GetCmd();
		DispatchCmd(c);

		// ���˳�ʱ����ʾ�Ի�����ѯ�û��Ƿ��˳�
		if (c == CMD_QUIT) 
		{
			HWND wnd = GetHWnd();
			if (MessageBox(wnd, _T("��Ҫ�˳���Ϸ��"), _T("����"), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
							if (MessageBox(wnd, _T("���ٿ��ǿ��ǣ�����\n����Ҫִ���˳�ô��"), _T("���һ����ʾ"), MB_OKCANCEL | MB_ICONWARNING) == IDOK)
				Quit();
		}
	}
}

void Welcome()
{	
	initgraph(screenx, screeny);
	putimage(0, 0,1920, 1080,&img2,0,0);

	//ȫ������
	HWND hwnd = GetHWnd();
	SetWindowLong(hwnd , GWL_STYLE , GetWindowLong(hwnd, GWL_STYLE) - WS_CAPTION);
	SetWindowPos(hwnd , HWND_TOP , 0 , 0 , screenx , screeny , SWP_SHOWWINDOW);

	int c=255;
	int flag=1;
	LOGFONT f;
	gettextstyle(&f);                     
	f.lfHeight = 30;                      
	_tcscpy(f.lfFaceName, _T("����"));    
	f.lfQuality = PROOF_QUALITY;    
	settextstyle(&f);                     
	setbkmode(TRANSPARENT);
	while(!kbhit())
	{

		setcolor(RGB(c, c, c));
		outtextxy(screenx/2-100, screeny-50, " ���������ʼ��Ϸ");
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
// ��ʼ����Ϸ
void Init()
{
	initgraph(275, 275);
		// ������
	mciSendString(_T("open res\\��������.mp3 alias mymusic"), NULL, 0, NULL);
    mciSendString(_T("open res\\��Ч.mp3 alias yx"), NULL, 0, NULL);
		
	outtextxy(0,0,"ʲô����Ϸ");
	Sleep(1500);
	outtextxy(0,25,"��������");
	Sleep(1500);
	outtextxy(0,50,"�����Լ�ȥ�ٶȰ�");
	Sleep(1500);
	outtextxy(0,75,"������Դ��ô��");
	Sleep(1500);
	outtextxy(0,100,"���ﻹ��������������");
	Sleep(3000);
	outtextxy(0,125,"�ðɣ������ˣ���ʼ��");
	Sleep(1500);
	outtextxy(0,150,"��Ȼ����Ϸ");
	Sleep(1500);
	outtextxy(0,175,"��ô��û��");
	Sleep(1000);
	outtextxy(0,200,"BGM�أ�");
	Sleep(1000);
		// ��������
	mciSendString(_T("play mymusic repeat"), NULL, 0, NULL);	
		outtextxy(0,225,"��ô��");
	Sleep(2000);
		outtextxy(0,250,"��ȫ���أ�");
	Sleep(2000);
	
	loadimage(&img1, _T("res\\start.jpg"));
	loadimage(&img2, _T("res\\background1.jpg"));
	loadimage(&img3, _T("res\\background2.jpg"));
	


	Welcome();
	putimage(0, 0,1920, 1080,&img3,0,0);

	setorigin(screenx/2-320 , screeny/2-240);


	srand((unsigned)time(NULL));
	setbkmode(TRANSPARENT);			// ����ͼ�����ı���ɫΪ͸��
	
	//������



	putimage(0, 0,640, 480,&img1,0,0);

	
	// ��������ԭ��
	setorigin(220+screenx/2-320, 20+screeny/2-240);

;

	// ��ʼ����Ϸ

	NewGame();
}


// �˳���Ϸ
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
	gettextstyle(&f);                     // ��ȡ��ǰ��������
	f.lfHeight = 72;                      // ��������߶�
	_tcscpy(f.lfFaceName, _T("Snap ITC"));    // ��������Ϊ�����塱(�߰汾 VC �Ƽ�ʹ�� _tcscpy_s ����)
	f.lfQuality = PROOF_QUALITY;    // �������Ч��Ϊ�����  
	settextstyle(&f);                     // ����������ʽ
	
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


// ��ʼ����Ϸ
void NewGame()
{
	// �����Ϸ��

		for(int x = 0; x < 10; x++)
			for(int y = 0; y < HEIGHT; y++)
				
			{
				g_World[x][y] = 0;
				putimage(x * UNIT, (HEIGHT - y - 1) * UNIT,UNIT, HEIGHT,&img1,x * UNIT+220, (HEIGHT - y - 1) * UNIT+20);
			}
	// ������һ������
	g_NextBlock.id = rand() % 7;
	g_NextBlock.dir = rand() % 4;
	g_NextBlock.x = WIDTH + 1;
	g_NextBlock.y = HEIGHT - 1;

	// ��ȡ�·���
	NewBlock();
}


// ������Ϸ
void GameOver()
{
	HWND wnd = GetHWnd();
	if (MessageBox(wnd, _T("��Ϸ������\n�ǲ��ǲ�����\n��������һ����"), _T("���˰���"), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		NewGame();

	}
	else
		if(MessageBox(wnd, _T("ʲô��������\n��ȷ��û���\nȷ���˳�ô��"), _T("����������"), MB_YESNO | MB_ICONWARNING) == IDYES)
			Quit();
		else
		NewGame();
}


// ��ȡ��������
DWORD m_oldtime;
CMD GetCmd()
{
	// ��ȡ����ֵ
	while(true)
	{
		// �����ʱ���Զ�����һ��
		DWORD newtime = GetTickCount();
		if (newtime - m_oldtime >= 500)
		{
			m_oldtime = newtime;
			return CMD_DOWN;
		}

		// ����а��������ذ�����Ӧ�Ĺ���
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

		// ��ʱ (���� CPU ռ����)
		Sleep(20);
	}
}


// �ַ���������
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


// �����µķ���
void NewBlock()
{
	g_CurBlock.id = g_NextBlock.id,		g_NextBlock.id = rand() % 7;
	g_CurBlock.dir = g_NextBlock.dir,	g_NextBlock.dir = rand() % 4;
	g_CurBlock.x = (WIDTH - 4) / 2;
	g_CurBlock.y = HEIGHT + 2;

	// �����·���ֱ���оֲ���ʾ
	WORD c = g_Blocks[g_CurBlock.id].dir[g_CurBlock.dir];
	while((c & 0xF) == 0)
	{
		g_CurBlock.y--;
		c >>= 4;
	}

	// �����·���
	DrawBlock(g_CurBlock);

	// ������һ������

	putimage((WIDTH + 1) * UNIT, 0, (WIDTH + 5) * UNIT - 1, 4 * UNIT - 1,&img1,(WIDTH + 1) * UNIT+220, 20);
	DrawBlock(g_NextBlock);

	// ���ü�ʱ���������ж��Զ�����
	m_oldtime = GetTickCount();
}


// ����Ԫ����
void DrawUnit(int x, int y, COLORREF c, DRAW _draw)
{
	// ���㵥Ԫ�����Ӧ����Ļ����
	int left = x * UNIT;
	int top = (HEIGHT - y - 1) * UNIT;
	int right = (x + 1) * UNIT - 1;
	int bottom = (HEIGHT - y) * UNIT - 1;

	// ����Ԫ����
	switch(_draw)
	{
		case SHOW:
			// ����ͨ����
			setlinecolor(0x006060);
			roundrect(left + 1, top + 1, right - 1, bottom - 1, 5, 5);
			setlinecolor(0x003030);
			roundrect(left, top, right, bottom, 8, 8);
			setfillcolor(c);
			setlinecolor(LIGHTGRAY);
			fillrectangle(left + 2, top + 2, right - 2, bottom - 2);
			break;

		case FIX:
			// ���̶��ķ���
			setfillcolor(RGB(GetRValue(c) * 2 / 3, GetGValue(c) * 2 / 3, GetBValue(c) * 2 / 3));
			setlinecolor(DARKGRAY);
			fillrectangle(left + 1, top + 1, right - 1, bottom - 1);
			break;

		case CLEAR:
			// ��������

	        putimage(x * UNIT, (HEIGHT - y - 1) * UNIT,UNIT, HEIGHT,&img1,x * UNIT+220, (HEIGHT - y - 1) * UNIT+20);
	 		break;
	}
}


// ������
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


// ���ָ�������Ƿ���Է���
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


// ��ת����
void OnRotate()
{
	// ��ȡ������ת�� x ƫ����
	int dx;
	BLOCKINFO tmp = g_CurBlock;
	tmp.dir++;					if (CheckBlock(tmp))	{	dx = 0;		goto rotate;	}
	tmp.x = g_CurBlock.x - 1;	if (CheckBlock(tmp))	{	dx = -1;	goto rotate;	}
	tmp.x = g_CurBlock.x + 1;	if (CheckBlock(tmp))	{	dx = 1;		goto rotate;	}
	tmp.x = g_CurBlock.x - 2;	if (CheckBlock(tmp))	{	dx = -2;	goto rotate;	}
	tmp.x = g_CurBlock.x + 2;	if (CheckBlock(tmp))	{	dx = 2;		goto rotate;	}
	return;

rotate:
	// ��ת
	DrawBlock(g_CurBlock, CLEAR);
	g_CurBlock.dir++;
	g_CurBlock.x += dx;
	DrawBlock(g_CurBlock);	
}


// ���Ʒ���
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


// ���Ʒ���
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


// ���Ʒ���
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
		OnSink();	// ��������ʱ��ִ�С����׷��顱����


}


// ���׷���
void OnSink()
{
	int i, x, y;

	// �������Ʒ���
	DrawBlock(g_CurBlock, CLEAR);
	BLOCKINFO tmp = g_CurBlock;
	tmp.y--;
	while (CheckBlock(tmp))
	{
		g_CurBlock.y--;
		tmp.y--;
	}
	DrawBlock(g_CurBlock, FIX);

	// �̶���������Ϸ��
	WORD b = g_Blocks[g_CurBlock.id].dir[g_CurBlock.dir];
	for(i = 0; i < 16; i++, b <<= 1)
		if (b & 0x8000)
		{
			if (g_CurBlock.y - i / 4 >= HEIGHT)
			{	// �������Ĺ̶�λ�ó����߶ȣ�������Ϸ
				GameOver();
				return;
			}
			else
				g_World[g_CurBlock.x + i % 4][g_CurBlock.y - i / 4] = 1;
		}

	// ����Ƿ���Ҫ�����У������
	BYTE remove = 0;	// �� 4 λ������Ƿ����漰�� 4 ���Ƿ���������Ϊ
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

	if (remove)	// ���������������
	{
		// ��ʱ 300 ����
		Sleep(300);

		// �����ղű�ǵ���2
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

	// �����·���

	NewBlock();
}