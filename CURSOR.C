#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <time.h>
#include "editor.h"

extern int msvisible,mousex,mousey,mousekey;
int start_x,start_y;/*光标开始处的位置坐标*/
int cell_w,cell_h;/*光标单次移动的左右距离和上下距离*/
int cur_x,cur_y;
int cur_row,cur_col;
int row_a,col_a;/*光标可移动的最大行数和列数*/

void cursor_blink()
{
    int t=500;
    setcolor(0);
    mouse_off();
    line(cur_x,cur_y+1,cur_x,cur_y+14);/* 画出光标 */
    mouse_on();
    mdelay(t);							/* 延时一段时间，模拟光标闪烁的效果 */
    setcolor(15);
    mouse_off();
    line(cur_x,cur_y+1,cur_x,cur_y+14);/* 消除光标 */
    mouse_on();
    mdelay(t);
}

void mdelay(int time)
{
    while(time--)
    {
        mouse_read();
        if(kbhit()||mousekey)
            break;
        delay(10);
    }
}

void cursor_init(int x,int y,int w,int h,int row,int col)
{
    start_x=x;
    start_y=y;
    cell_w=w;
    cell_h=h;
    cur_x=x;
    cur_y=y;
    row_a=row;
    col_a=col;
    cur_row=0;
    cur_col=0;
}

void cur_moveto(int row,int col)
{
    if(row<row_a&&col<col_a)
    {
        cur_row=row;
        cur_col=col;
        cur_x=start_x+cell_w*col;
        cur_y=start_y+cell_h*row;
    }
}

void cur_up()
{
    cur_y-=cell_h;
    cur_row--;
}

void cur_down()
{
    cur_y+=cell_h;
    cur_row++;
}

void cur_left()
{
    cur_x-=cell_w;
    cur_col--;
}

void cur_right()
{
      cur_x+=cell_w;
      cur_col++;
}

void cur_movetoxy(int x,int y,int *row,int *col)
{
    cur_row=(y-start_y)/cell_h;
    cur_col=(x-start_x)/cell_w;
    cur_x=start_x+cell_w*cur_col;
    cur_y=start_y+cell_h*cur_row;
    *row=cur_row;
    *col=cur_col;
}