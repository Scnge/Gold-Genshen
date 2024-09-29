#include<iostream>
#include<stdio.h>
#include<graphics.h>
#include<math.h>
#include<time.h> 

#include "tools.h"
using namespace std;

#define win_width 1080
#define win_height 640
#define max_mine 10
#define PI 3.1415926535
#define max_angle 90

IMAGE img[11];
IMAGE imgbk;
IMAGE imgsole;
IMAGE imgbk2;
IMAGE imgbk3;
IMAGE imgbk4;

int type;

enum Image
{
	i_sole = 0,
	i_small = 1,
	i_big = 2,
	i_mola = 3
};

enum mine_type
{
	Big,
	Small,
	Mola,
	Left,
	Right,

	m_nomal,
	m_long,
	m_short,
};

struct Sole//人物信息
{
	int x, y;
	int width = 120;
	int height = 120;
	int score = 0;
}sole;

struct Mine//石头信息
{
	int x, y;
	int size;
	bool flag;
	int type;
	int gold;
}mine[max_mine];

struct Hook //钩子信息
{
	int x, y;
	int endx, endy;
	int len;
	int angle;
	int dir;
	int vx, vy;
	int state;
	int index;//记录抓到哪个
}hook;

void gameinit()
{
	initgraph(win_width, win_height, SHOWCONSOLE);
	loadimage(&img[0], "res/0.png", sole.width, sole.height);
	loadimage(&imgbk, "res/bk.jpg");
	loadimage(&img[1], "res/1.png");
	loadimage(&img[2], "res/2.png");
	loadimage(&img[3], "res/3.png");
	loadimage(&img[4], "res/4.png", sole.width, sole.height);
	sole.x = win_width / 2 - sole.width / 2;
	sole.y = 0;

	srand(GetTickCount());//初始化原石信息
	for (int i = 0; i < max_mine; i++)
	{
		mine[i].flag = true;
		mine[i].gold = rand() % 500 + 1;
		mine[i].size = 60;
		mine[i].type = rand() % 3;
		mine[i].x = rand() % (win_width - 60);
		mine[i].y = rand() % (win_height - 180) + 150;
	}

	hook.x = sole.x + 44;//初始化钩子
	hook.y = sole.y + 100;
	hook.len = 50;
	hook.endx = hook.x;
	hook.endy = hook.y + hook.len;
	hook.angle = 0;
	hook.dir = Right;
	hook.vx = 0;
	hook.vy = 0;
	hook.state = m_nomal;
	hook.index = -1;
}

void gamedraw()
{
	putimage(0, 120, &imgbk);//绘制背景

	setfillcolor(RGB(35, 52, 82));//绘制角色背景颜色
	solidrectangle(0, 0, 1080, 120);
	putimagePNG(480, 0, &img[0]);

	for (int i = 0; i < max_mine; i++)
	{
		if (mine[i].flag == true)
		{
			switch (mine[i].type)
			{
			case Small:
				putimagePNG(mine[i].x, mine[i].y, &img[i_small]);
				break;
			case Big:
				putimagePNG(mine[i].x, mine[i].y, &img[i_big]);
				break;
			case Mola:
				putimagePNG(mine[i].x, mine[i].y, &img[i_mola]);
				break;
			}
		}
	}

	if (type == 1)
	{
		putimagePNG(sole.x, sole.y, &img[0]);
	}
	else if (type == 2)
	{
		putimagePNG(sole.x, sole.y, &img[4]);
	}

	setlinecolor(BROWN);//绘制钩子
	setlinestyle(PS_SOLID, 5);
	line(hook.x, hook.y, hook.endx, hook.endy);
}


void hookrock()
{
	if (hook.state == m_nomal)
	{
		if (hook.dir == Right)
		{
			hook.angle++;
		}
		else
		{
			hook.angle--;
		}
		if (hook.angle > max_angle)
		{
			hook.dir = Left;
		}
		else if (hook.angle < -max_angle)
		{
			hook.dir = Right;
		}
		hook.endx = hook.x + sin(PI / 180 * hook.angle) * hook.len;
		hook.endy = hook.y + cos(PI / 180 * hook.angle) * hook.len;
	}
}

int distance(Hook hook)
{
	int dis = sqrt((float)(hook.x - hook.endx) * (hook.x - hook.endx) + (hook.y - hook.endy) * (hook.y - hook.endy));
	return dis <= hook.len;
}

void control(int speed)
{
	if (GetAsyncKeyState(VK_SPACE) && (hook.state == m_nomal))
	{
		hook.vx = sin(PI / 180 * hook.angle) * speed;
		hook.vy = cos(PI / 180 * hook.angle) * speed;
		hook.state = m_long;
	}

	if (hook.endx<0 || hook.endx>win_width || hook.endy > win_height)
	{
		hook.state = m_short;
	}
	if (hook.state == m_long)
	{
		hook.endx += hook.vx;
		hook.endy += hook.vy;
	}
	if (hook.state == m_short)
	{
		hook.endx -= hook.vx;
		hook.endy -= hook.vy;
		if (distance(hook))
		{
			hook.state = m_nomal;
		}
	}
}

void grap()
{
	for (int i = 0; i < max_mine; i++)//判断抓到哪个
	{
		if (mine[i].flag == true && hook.endx > mine[i].x && hook.endx < mine[i].x + mine[i].size && hook.endy>mine[i].y && hook.endy < mine[i].y + mine[i].size)
		{
			hook.index = i;
			break;
		}
	}

	if (hook.index != -1)//抓到
	{
		hook.state = m_short;
		mine[hook.index].x = hook.endx;
		mine[hook.index].y = hook.endy;
		if (distance(hook))
		{
			mine[hook.index].flag = false;
			hook.index = -1;
			hook.state = m_nomal;
		}
	}
}

void start1()
{
	loadimage(&imgbk2, "res/bk2.jpg");

	int flag = 0;

	while (1)
	{
		BeginBatchDraw();
		putimage(0, 0, &imgbk2);

		ExMessage msg;
		if (peekmessage(&msg))
		{
			if (msg.message == WM_LBUTTONDOWN &&
				msg.x > 469 && msg.x < 621 &&
				msg.y > 225 && msg.y < 476)
			{
				flag = 1;
			}
			else if (msg.message == WM_LBUTTONUP && flag)
			{
				return;
			}
		}
		EndBatchDraw();
	}
}

void start2()
{
	loadimage(&imgbk3, "res/bk3.jpg");

	int flag = 0;

	while (1)
	{
		BeginBatchDraw();
		putimage(0, 0, &imgbk3);

		ExMessage msg;
		if (peekmessage(&msg))
		{
			if (msg.message == WM_LBUTTONDOWN)
			{
				flag = 1;
			}
			else if (msg.message == WM_LBUTTONUP && flag)
			{
				return;
			}
		}
		EndBatchDraw();
	}
}

int main()
{
	gameinit();
	DWORD t1, t2;
	t1 = t2 = GetTickCount();
	BeginBatchDraw();

	start1();
	start2();

	while (1)
	{
		if (t2 - t1 > 15)
		{
			hookrock();
			t1 = t2;
		}
		t2 = GetTickCount();
		gamedraw();
		FlushBatchDraw();
		control(5);
		grap();
	}

	EndBatchDraw();

	return 0;
}