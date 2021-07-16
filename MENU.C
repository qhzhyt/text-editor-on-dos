#include <conio.h>
#include <dos.h>
#include <graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include "editor.h"

extern int msvisible,mousex,mousey,mousekey;
extern int curfile_saved;
unsigned char item_active[MENU_N][MENU_MAX]=
{
    {1,1,1,1,1},
    {0,0,0,0,1,0},
    {0,0}
}; /* 菜单项是否激活的标志*/
struct button main_menu[MENU_N];
int state=0;

char *(menu_bar[])={"文件(F)","编辑(E)","搜索(S)"};
char *menu_item[][7]=
{
    {"新建","打开","保存","另存为","退出"},
    {"撤消","剪贴","复制","粘贴","全选"},
    {"搜索","替换"},
};
int mitem_n[]={5,5,2};
int selected_item;

void *p;
size_t area;

void draw_menu_bar();
int mouse_on_btn(struct button *t);
void draw_btnbord(struct button *bt,int p,int bw);
void btn_push(struct button *bt);
void btn_up(struct button *bt);
void pullmenu_up(int n);
void handle_pullmenu(int m);
void create_mainmenu();
void draw_menu_bar();
void draw_state_bar();

void exec_menu_item(int menu,int item);

void draw_window()
{
    setfillstyle(1,7);
    bar(0,0,WW,WH);
    draw_title_bar(15,"文本编辑器");
    draw_menu_bar();
    draw_workplace(2,41,637,459);
    draw_scrollbar(621,43,635,455);
    draw_state_bar();
    print_stateline(0,0,0);
    set_textarea(2,41,620,457);
    cursor_init(5,44,9,18,48,69);
}
int menu_process(int sta)
{
    int i;
    if(sta!=-1)
    {
        pullmenu_down(sta);
        handle_pullmenu(sta);
    }
    else
    {
        mouse_cursetting();
        if(mousekey==1)
        {
            for(i=0;i<MENU_N;i++)
            {
                if(mouse_on_btn(&(main_menu[i])))
                {
                    mouse_cursetting();
                    pullmenu_down(i);
                    handle_pullmenu(i);
                }
            }
        }
        else
        {
            for(i=0;i<MENU_N;i++)	
                if(mouse_on_btn(&(main_menu[i]))) 
                    hlight_btn(&(main_menu[i]));
        }
    }
    return 1;
}

void exec_menu_item(int menu,int item)
{  
   switch (menu)
   {
      case 0:
        switch (item)
        {
			case 0:
				draw_max_dialog(0);
				break;
            case 1:
                draw_max_dialog(1);
                break;
            case 2:
				save_curfile();
                break;
            case 3:
                draw_max_dialog(3);
                break;
            case 4:
                if(!curfile_saved)
				{
                    char *btn_str[4]={"保存","放弃","取消"};
                    int i;
                    i=prompt_dialog(125,"警告","文件未保存，继续退出将丢弃对文件的所有修改是否继续？",3,btn_str);
					if(i==1)
					{
						save_curfile();
						exit(0);
					}
					if(i==2)
						exit(0);
				}
                close_graphics();
                exit(0);
                break;
        }
		break;
	case 1:
		switch(item)
		{
			case 0:
				goto_backstep();
                break;
                
            case 2:
                copy_text();
                break;
            case 3:
                hotkey_handle(CTRL_V);
                break;
            case 1:
                hotkey_handle(CTRL_X);
                break;
                
            case 4:
                select_all();
                break;
		}
    }
}

void pullmenu_down(int m)
{
    struct button *bt=&main_menu[m];
    char **item=menu_item[m];
    int i;
    int n=mitem_n[m];
    set_textarea(-1,0,0,0);
    mouse_cursetting();
    btn_push(bt);
	save_bigimg(bt->x1+1,bt->x1+PULLMENU_W,bt->y2+2,bt->y2+n*20+10);
    mouse_off();
    draw_win(bt->x1+1,bt->y2+2,bt->x1+PULLMENU_W,bt->y2+n*20+10);
    for(i=0;i<n;i++)
    {
        if(item_active[m][i])
        {
            setcolor(15);
            draw_string(bt->x1+15,bt->y2+9+i*20,item[i],15);
            setcolor(0);
            draw_string(bt->x1+14,bt->y2+8+i*20,item[i],0);
        }
        else
            draw_string(bt->x1+15,bt->y2+9+i*20,item[i],15);
    }
    mouse_on();
    while(mousekey==1)
        mouse_read(); 
}

void pullmenu_up(int n)
{
    struct button *bt=&main_menu[n];
    set_textarea(2,41,637,477);
    put_bigimg(bt->x1+1,0,bt->y2+2,0);
    btn_up(bt);
}

void handle_pullmenu(int m)
{
    int i=0;
    int x=0,y=0;
    struct button *bt=&main_menu[m];
    int n=mitem_n[m];
    int key;
    int hlight_item=NONE;
    setcolor(0);
    selected_item=NONE;
    while(1)
    {
        mouse_read();
        if(mousekey==1)
            break;
        if(kbhit())
        {
            if(hlight_item!=NONE)
            hlight_bar(bt->x1+3,bt->y2+6+hlight_item*20,bt->x1+97,bt->y2+24+hlight_item*20,14);
            key=bioskey(0);
            switch(key)
            {
                case K_ENTER:
                    if(hlight_item!=NONE)
                    {
                        pullmenu_up(m);
                        exec_menu_item(m,hlight_item);
                        return;
                    }
                    break;
                case K_ESC:
                    pullmenu_up(m);
                    return;
                    break;
                case K_UP:
                    if(hlight_item==NONE||hlight_item==0)
                        hlight_item=n-1;
                    else
                        hlight_item--;
                    break;
                case K_DOWN:
                    if(hlight_item==n-1)
                        hlight_item=0;
                    else
                        hlight_item++;
                    break;
                case K_LEFT:
                    pullmenu_up(m);
                    if(m==0)
                        m=MENU_N-1;
                    else
                        m--;
                    pullmenu_down(m);
                    bt=&main_menu[m];
                    n=mitem_n[m];
                    hlight_item=0;
                    break;
                case K_RIGHT:
                    pullmenu_up(m);
                    if(m==MENU_N-1)
                        m=0;
                    else
                        m++;
                    pullmenu_down(m);
                    bt=&main_menu[m];
                    n=mitem_n[m];
                    hlight_item=0;
                    break;
            }
            hlight_bar(bt->x1+3,bt->y2+6+hlight_item*20,bt->x1+97,bt->y2+24+hlight_item*20,14);
        }
        if(mouse_move(x,y))
        {
            x=mousex;
            y=mousey;
            for(i=0;i<n;i++)
            {
                mouse_read();
                if(mouse_in(bt->x1+4,bt->y2+6+i*20,bt->x1+97,bt->y2+22+i*20))
                {
                    if(hlight_item!=NONE)
                        hlight_bar(bt->x1+3,bt->y2+6+hlight_item*20,bt->x1+97,bt->y2+24+hlight_item*20,14);
                    hlight_bar(bt->x1+3,bt->y2+6+i*20,bt->x1+97,bt->y2+24+i*20,14);
                    hlight_item=i;
                    while(mouse_in(bt->x1+4,bt->y2+6+i*20,bt->x1+97,bt->y2+22+i*20))
                    {
                        x=mousex;
                        y=mousey;
                        mouse_read();
                        if(mousekey==1)
                        {
                            if(item_active[m][i]==YES)
                            {
                                selected_item=i;
                                break;
                            }
                            else
                            selected_item=NONE;
                        }
                        if(kbhit()) break;
                    }
                }
            }
        
        }
        if(selected_item!=NONE)
        {
            pullmenu_up(m);
            exec_menu_item(m,selected_item);
            return;
        }
    }
    pullmenu_up(m);
}
void draw_title_bar(int color,char *s)
{
    setfillstyle(1,9);
    bar(0,0,WW,TH);
    draw_string(2,2,s,color);
}
void create_mainmenu()
{
    int i,j;
    for(i=0;i<MENU_N;i++)
    {
        main_menu[i].x1=72*i+2;
        main_menu[i].y1=21;
        main_menu[i].x2=72*(i+1)+1;
        main_menu[i].y2=40;
    }
}
void draw_menu_bar()
{   
    int i;
    create_mainmenu();
    for(i=0;i<MENU_N;i++)
        draw_string(main_menu[i].x1,main_menu[i].y1,menu_bar[i],0);
}
void draw_state_bar()
{
	setcolor(0);
	line(3,461,637,461);
	line(3,461,3,477);
	setcolor(15);
	line(3,477,637,477);
	line(637,461,637,477);
}
void btn_push(struct button *bt)
{
    winp(bt,1);
    while(mousekey==1)
    {
        mouse_read();
        if(kbhit()) break;
    }
}
void btn_up(struct button *bt)
{
    setcolor(7);
    mouse_off();
    rectangle(bt->x1,bt->y1,bt->x2,bt->y2);
    mouse_on();
    while(mousekey==1)
    {
        mouse_read();
        if(kbhit()) break;
    }
}