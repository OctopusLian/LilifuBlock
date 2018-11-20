#include <iostream>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>

using namespace std;

//交换a和b
void swap(int &a, int &b){
	int t=a;
	a = b;
	b = t;
}
int getrand(int min, int max)
{
	return(min+rand()%(max-min+1));
}

WINDOW *create_newwin(int height,int width,int starty,int startx);
void destory_win(WINDOW *local_win);

int game_win_height=30;	
int game_win_width=45;

int hint_win_height=10;
int hint_win_width=20;
WINDOW * game_win, *hint_win ,*score_win;
int key;

//定义了一个Piece类
class Piece
{
public:
	int score;  //得分
	int shape; 	//当前方块的形状
	int next_shape;

	int head_x;  //当前方块首个box的位置，标记位置
	int head_y;

	int size_h;  //当前方块的size
	int size_w;

	int next_size_h;  //下一个方块的size
	int next_size_w;

	int box_shape[4][4];
	int next_box_shape[4][4];

	int box_map[30][45];  //用来标记游戏框内的

	bool game_over;

public:
	void initial();  //初始化函数
	void set_shape(int &cshape, int box_shape[][4],int &size_w, int & size_h);  //设置方块形状

	void score_next();  //显示下一个方块的形状以及分数
	void judge();  //判断是否层满
	void move();  //移动函数，通过方向键控制
	void rotate();  //旋转函数
	bool isaggin();  //判断下一次行动是否会越界或者重合
	bool exsqr(int row);  //判断当前行是否为空	
};

int main()
{

	initscr();
	//raw();
	cbreak();
	noecho();
	curs_set(0);
	keypad(stdscr,TRUE);

	refresh();
	
	game_win = create_newwin(game_win_height, game_win_width, 0,0);
	wborder(game_win, '*', '*', '*', '*', '*', '*', '*', '*');
	wrefresh(game_win);

	hint_win = create_newwin(hint_win_height, hint_win_width, 0, game_win_width+10);
	mvprintw(0, game_win_width+10+2,"%s","Next");
	refresh();

	score_win = create_newwin(hint_win_height, hint_win_width, 20, game_win_width+10);
	mvprintw(20, game_win_width+10+2,"%s","Score");
	refresh();



	Piece* pp = new Piece;
	pp->initial();


	while(1)
	{
		pp->move();
		if(pp->game_over)
			break;
	}

	destory_win(game_win);
	destory_win(hint_win);
	destory_win(score_win);
	delete pp;
	system("clear");

	int row,col;
	getmaxyx(stdscr,row,col);
	mvprintw(row/2,col/2 ,"%s","GAMER OVER ! \n ");
	mvprintw(row/2+2,col/2-2 ,"%s","Wait 5s to return tthe erminal ! \n ");
	refresh();

	sleep(5);
	endwin();
	return 0;
}

WINDOW *create_newwin(int height, int width, int starty, int startx)
{
	WINDOW *local_win;
	local_win = newwin(height, width, starty, startx);
	box(local_win,0,0);
	wrefresh(local_win);
	return local_win;
}

void destory_win(WINDOW *local_win)
{
	wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wrefresh(local_win);
	delwin(local_win);
}


void Piece::initial()
{
	score=0;
	game_over=false;
	for(int i =0;i<game_win_height;i++)
		for(int j=0;j<game_win_width;j++){
			if(i==0 || i==game_win_height-1 || j==0 || j==game_win_width-1){
				box_map[i][j]=1;
			}
			else
				box_map[i][j]=0;
		}

	srand((unsigned)time(0));
	shape=getrand(0,6);
	set_shape(shape,box_shape,size_w,size_h);

	next_shape=getrand(0,6);
	set_shape(next_shape,next_box_shape,next_size_w,next_size_h);

	for(int i =0;i<4;i++)
		for(int j=0;j<4;j++)
			if(next_box_shape[i][j]==1){
				mvwaddch(hint_win,(hint_win_height-size_h)/2+i,(hint_win_width-size_w)/2+j,'#');
				wrefresh(hint_win);
			}
				

	mvwprintw(score_win, hint_win_height/2,hint_win_width/2-2,"%d",score);
	wrefresh(score_win);
}

void Piece::set_shape(int &cshape, int shape[][4],int &size_w,int &size_h)
{
	//首先将用来表示的4x4数组初始化为0
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			shape[i][j]=0;
	//设置7种初始形状并设置它们的size
	switch(cshape)
	{
		case 0:	
			size_h=1;
			size_w=4;	
			shape[0][0]=1;
			shape[0][1]=1;
			shape[0][2]=1;
			shape[0][3]=1;
			break;
		case 1:
			size_h=2;
			size_w=3;
			shape[0][0]=1;
			shape[1][0]=1;
			shape[1][1]=1;
			shape[1][2]=1;
			break;
		case 2:
			size_h=2;
			size_w=3;	
			shape[0][2]=1;
			shape[1][0]=1;
			shape[1][1]=1;
			shape[1][2]=1;
			break;
		case 3:
			size_h=2;
			size_w=3;
			shape[0][1]=1;
			shape[0][2]=1;
			shape[1][0]=1;
			shape[1][1]=1;
			break;

		case 4:
			size_h=2;
			size_w=3;
			shape[0][0]=1;
			shape[0][1]=1;
			shape[1][1]=1;
			shape[1][2]=1;
			break;

		case 5:	
			size_h=2;
			size_w=2;
			shape[0][0]=1;
			shape[0][1]=1;
			shape[1][0]=1;
			shape[1][1]=1;
			break;

		case 6:	
			size_h=2;
			size_w=3;
			shape[0][1]=1;
			shape[1][0]=1;
			shape[1][1]=1;
			shape[1][2]=1;
			break;
	}

	//设置完形状以后初始化方块的起始位置
	head_x=game_win_width/2;
	head_y=1;

	//如果刚初始化就重合了，游戏结束
	if(isaggin())    /* GAME OVER ! */
		game_over=true;

}

void Piece::rotate()
{
	int temp[4][4]={0};  //临时变量
	int temp_piece[4][4]={0};  //备份用的数组
	int i,j,tmp_size_h,tmp_size_w;

	tmp_size_w=size_w;
	tmp_size_h=size_h;

	for(int i=0; i<4;i++)
		for(int j=0;j<4;j++)
			temp_piece[i][j]=box_shape[i][j];  //备份一下当前的方块，如果旋转失败则返回到当前的形状


	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			temp[j][i]=box_shape[i][j];  //斜对角线对称
	i=size_h;
	size_h=size_w;
	size_w=i;
	for(i=0;i<size_h;i++)
		for(j=0;j<size_w;j++)
			box_shape[i][size_w-1-j]=temp[i][j];  //左右对称

	/*如果旋转以后重合，则返回到备份的数组形状*/
	if(isaggin()){
		for(int i=0; i<4;i++)
			for(int j=0;j<4;j++)
				box_shape[i][j]=temp_piece[i][j];
		size_w=tmp_size_w;  //记得size也要变回原来的size
		size_h=tmp_size_h;
	}
	/*如果旋转成功，那么在屏幕上进行显示*/
	else{
		for(int i=0; i<4;i++)
			for(int j=0;j<4;j++){
				if(temp_piece[i][j]==1){
					mvwaddch(game_win,head_y+i,head_x+j,' ');  //移动到game_win窗口的某个坐标处打印字符
					wrefresh(game_win);
				}
			}
		for(int i=0; i<size_h;i++)
			for(int j=0;j<size_w;j++){
				if(this->box_shape[i][j]==1){
					mvwaddch(game_win,head_y+i,head_x+j,'#');
					wrefresh(game_win);
				}
		}

	}
}

void Piece::move(){
	 fd_set set;
	 FD_ZERO(&set);
	 FD_SET(0, &set);
	      
	 struct timeval timeout;
	 timeout.tv_sec = 0;
	 timeout.tv_usec= 500000;
	      
  	if (select(1, &set, NULL, NULL, &timeout) == 0){
		head_y++;
		if(isaggin()){
			head_y--;
			for(int i=0;i<size_h;i++)
				for(int j=0;j<size_w;j++)
					if(box_shape[i][j]==1)
						box_map[head_y+i][head_x+j]=1;
			score_next();
		}
		else{
			for(int i=size_h-1; i>=0;i--)
				for(int j=0;j<size_w;j++){
					if(this->box_shape[i][j]==1){
						mvwaddch(game_win,head_y-1+i,head_x+j,' ');
						mvwaddch(game_win,head_y+i,head_x+j,'#');

					}
				}
			wrefresh(game_win);
		}

  	}
	      
	if (FD_ISSET(0, &set)) {
	        while ((key = getch()) == -1) ;

		if(key==KEY_LEFT){
			head_x--;
			if(isaggin())
				head_x++; //undo
			else{
				for(int i=0; i<size_h;i++)
					for(int j=0;j<size_w;j++){
						if(this->box_shape[i][j]==1){
							mvwaddch(game_win,head_y+i,head_x+j+1,' ');
							mvwaddch(game_win,head_y+i,head_x+j,'#');

						}
					}
				wrefresh(game_win);
			}
		}

		if(key==KEY_RIGHT){
			head_x++;
			if(isaggin())
				head_x--;
			else{
				for(int i=0; i<size_h;i++)
					for(int j=size_w-1;j>=0;j--){
						if(this->box_shape[i][j]==1){
							mvwaddch(game_win,head_y+i,head_x+j-1,' ');
							mvwaddch(game_win,head_y+i,head_x+j,'#');

						}
					}
				wrefresh(game_win);
			}
		}

		if(key==KEY_DOWN){
			head_y++;
			if(isaggin()){
				head_y--;
				for(int i=0;i<size_h;i++)
					for(int j=0;j<size_w;j++)
						if(box_shape[i][j]==1)
							box_map[head_y+i][head_x+j]=1;

				score_next();
				
			}
			else{
				for(int i=size_h-1; i>=0;i--)
					for(int j=0;j<size_w;j++){
						if(this->box_shape[i][j]==1){
							mvwaddch(game_win,head_y-1+i,head_x+j,' ');
							mvwaddch(game_win,head_y+i,head_x+j,'#');

						}
					}
				wrefresh(game_win);
			}
		}

		if(key==KEY_UP)
			rotate();

		if(head_x+size_w+1>game_win_width)
			head_x=game_win_width-size_w-1;
		if(head_x<1)
			head_x=1;
	}
}

bool Piece::isaggin(){
	for(int i=0;i<size_h;i++)
		for(int j=0;j<size_w;j++){
			if(box_shape[i][j]==1){
				if(head_y+i > game_win_height-2)  //下面出界
					return true;
				if(head_x+j > game_win_width-2 || head_x+i-1<0)  //左右出界
					return true;
				if(box_map[head_y+i][head_x+j]==1)  //与已占用的box重合
					return true ;
			}
		}
	return false;
}

bool Piece::exsqr(int row){
	for(int j=1;j<game_win_width-1;j++)
		if(box_map[row][j]==1)
			return true;
	return false;
}

void Piece::judge(){
	int i,j;
	int line=0;  //记录层满的行数
	bool full;
	for(i=1;i<game_win_height-1;i++){  //除去边界
		full=true;
		for(j=1;j<game_win_width-1;j++){
			if(box_map[i][j]==0)  //存在未被占用的box
				full=false;   //说明本层未满
		}
		if(full){
			line++;
			score+=50;
			for(j=1;j<game_win_width-1;j++)
				box_map[i][j]=0;
		}
	}
	/*上面判断完后 看line的值,如果非 0 说明有层已满需要进行消除*/
	if(line!=0){
	for(i=game_win_height-2;i>=2;i--){
		int s=i;
		if(exsqr(i)==0){
			while(s>1 && exsqr(--s)==0);
			for(j=1;j<game_win_width-1;j++){
				box_map[i][j]=box_map[s][j];
				box_map[s][j]=0;
			}
		}
	}

	/*清空和移动标记完成以后就要屏幕刷新了，重新打印game_win*/
	for(int i=1;i<game_win_height-1;i++)
			for(int j=1;j<game_win_width-1;j++){
				if(box_map[i][j]==1){
					mvwaddch(game_win,i,j,'#');
					wrefresh(game_win);
				}
				else{
					mvwaddch(game_win,i,j,' ');
					wrefresh(game_win);
				}
			}
	}
}			

void Piece::score_next(){
	score+=10;
	judge();

	mvwprintw(score_win, hint_win_height/2,hint_win_width/2-2,"%d",score);
	wrefresh(score_win);


	set_shape(next_shape,box_shape,size_w,size_h);

	this->next_shape=getrand(0,6);
	set_shape(next_shape,next_box_shape,next_size_w,next_size_h);


	for(int i =1;i<hint_win_height-1;i++)
		for(int j=1;j<hint_win_width-1;j++){
			mvwaddch(hint_win, i, j,' ');
			wrefresh(hint_win);
		}
	for(int i =0;i<4;i++)
		for(int j=0;j<4;j++)
			if(next_box_shape[i][j]==1){
				mvwaddch(hint_win,(hint_win_height-size_h)/2+i,(hint_win_width-size_w)/2+j,'#');
				wrefresh(hint_win);
		}
}
