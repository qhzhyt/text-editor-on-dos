#include <dos.h>
#include <stdio.h> 
#include <stdlib.h>
#include <graphics.h> 
#include <time.h>
#include "test.h"

typedef struct
{
    int x1;
    int x2;
    int y1;
    int y2;
} area;

union REGS i_r,o_r;
struct SREGS s_s;

area text_area;
int cur_pattern=CUR_ARROW;
int msvisible,mousex,mousey,mousekey;
void set_cursor_pattern(int x, int y, unsigned int far *pattern);
unsigned int curs_arrow[32]= 
{
	/* 屏幕掩码*/
	0x3FFF,/* 0011111111111111 */
	0x1FFF,/* 0001111111111111 */
	0x0FFF,/* 0000111111111111 */
	0x07FF,/* 0000011111111111 */
	0x03FF,/* 0000001111111111 */
	0x01FF,/* 0000000111111111 */
	0x00FF,/* 0000000011111111 */
	0x007F,/* 0000000001111111 */
	0x003F,/* 0000000000111111 */
	0x001F,/* 0000000000011111 */
	0x01FF,/* 0000000111111111 */
	0x10FF,/* 0001000011111111 */
	0x30FF,/* 0011000011111111 */
	0xF87F,/* 1111100001111111 */
	0xF87F,/* 1111100001111111 */
	0xFC3F,/* 1111110000111111 */
	/* 光标屏蔽*/
	0x0000,/* 000000000000000 */
	0x4000,/* 010000000000000 */
	0x6000,/* 011000000000000 */
	0x7000,/* 011100000000000 */
	0x7800,/* 011110000000000 */
	0x7C00,/* 011111000000000 */
	0x7E00,/* 011111100000000 */
	0x7F00,/* 011111110000000 */
	0x7F80,/* 011111111000000 */
	0x7FC0,/* 011111111100000 */
	0x6C00,/* 011011000000000 */
	0x4600,/* 010001100000000 */
	0x0600,/* 000001100000000 */
	0x0300,/* 000000110000000 */
	0x0300,/* 000000110000000 */
	0x0180,/* 000000011000000 */

};
unsigned int curs_text[32]= 
{

	/* 光标屏蔽*/
	0x007f,/*0000000001111111*/
	0x007f,/*0000000001111111*/
	0x007f,/*0000000001111111*/
	0xe3ff,/*1110001111111111*/
	0xe3ff,/*1110001111111111*/
	0xe3ff,/*1110001111111111*/
	0xe3ff,/*1110001111111111*/
	0xe3ff,/*1110001111111111*/
	0xe3ff,/*1110001111111111*/
	0xe3ff,/*1110001111111111*/
	0xe3ff,/*1110001111111111*/
	0xe3ff,/*1110001111111111*/
	0xe3ff,/*1110001111111111*/
	0x007f,/*0000000001111111*/
	0x007f,/*0000000001111111*/
	0x007f,/*0000000001111111*/
	/*屏幕掩码*/
	0x0000,/*0000000000000000*/
	0x7f00,/*0111111100000000*/
	0x0800,/*0000100000000000*/
	0x0800,/*0000100000000000*/
	0x0800,/*0000100000000000*/
	0x0800,/*0000100000000000*/
	0x0800,/*0000100000000000*/
	0x0800,/*0000100000000000*/
	0x0800,/*0000100000000000*/
	0x0800,/*0000100000000000*/
	0x0800,/*0000100000000000*/
	0x0800,/*0000100000000000*/
	0x0800,/*0000100000000000*/
	0x0800,/*0000100000000000*/
	0x7f00,/*0111111100000000*/
	0x0000,/*0000000000000000*/
};

void mouse_on(void)
{
	i_r.x.ax=1;
	msvisible=TRUE;
	int86x(0x33,&i_r,&o_r,&s_s);
}

void mouse_off(void)
{
	i_r.x.ax=2;
	msvisible=FALSE;
	int86x(0x33,&i_r,&o_r,&s_s);
}

void mouse_toxy(unsigned int x, unsigned int y)   
{  
	i_r.x.ax = 4; 
	i_r.x.cx = x; 
	i_r.x.dx = y; 
	int86x(0x33, &i_r, &o_r, &s_s); 
	mousex = x; 
	mousey = y; 
}

void mouse_read() 
{
	i_r.x.ax=3;
	int86x(0x33,&i_r,&o_r,&s_s);
	mousex=o_r.x.cx;/* 鼠标器光标 x 方向坐标*/
	mousey=o_r.x.dx;/* 鼠标器光标 y 方向坐标*/
	mousekey=o_r.x.bx;/* 鼠标器按健状态 */
}

void set_cursor_pattern(int x, int y, unsigned int far *pattern)
{
	union REGS ireg;
	struct SREGS isreg;
	ireg.x.ax=9;
	ireg.x.bx=x;
	ireg.x.cx=y;
	ireg.x.dx=FP_OFF(pattern);
	isreg.es=FP_SEG(pattern);
	int86x(0x33, &ireg, &ireg, &isreg);
}

void view_arrow_surs()
{
    set_cursor_pattern(0,0,curs_arrow);
}

void view_text_surs()
{
    set_cursor_pattern(3,7,curs_text);
}

int mouse_in(int x1,int y1,int x2,int y2)
{
 if(mousex>=x1 && mousex<=x2 && mousey>=y1 && mousey<=y2) 
    return 1;
 return 0;
}



int mouse_move(int x,int y)
{
   mouse_read();
   if(x!=mousex||y!=mousey)
     return 1;
   return 0;
}

int set_textarea(int x1,int y1,int x2,int y2)
{
    text_area.x1=x1;
    text_area.x2=x2;
    text_area.y1=y1;
    text_area.y2=y2;
}

void mouse_cursetting()
{
    mouse_read();
    if(mouse_in(text_area.x1,text_area.y1,text_area.x2,text_area.y2))
    {
        if(cur_pattern!=CUR_TEXT)
        {
            mouse_off();
            view_text_surs();
            mouse_on();
            cur_pattern=CUR_TEXT;
        }
    }
    else
    {
        if(cur_pattern!=CUR_ARROW)
        {
            mouse_off();
            view_arrow_surs();
            mouse_on();
            cur_pattern=CUR_ARROW;
        }
    }
}

int mouse_on_btn(struct button *bt)
{
   return mouse_in(bt->x1,bt->y1,bt->x2,bt->y2);
}

int double_click()
{
    int t=1000;
    if(mousekey==1)
    {
        while(mousekey==1)
        {
            mouse_read();
        }
        while(t--)
        {
            mouse_read();
            if(mousekey==1)
                return 1;
            delay(1);
        }
    }
    return 0;
}