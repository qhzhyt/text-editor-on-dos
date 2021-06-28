#include <graphics.h>
#include<alloc.h>
#include<string.h>
#include"test.h"

extern int msvisible,mousex,mousey,mousekey;
void *imgp1,*imgp2;
void *imgp[20][30];
size_t areas;


int context_menu_state[MENU_MAX];
int img_close[16]=
{
    0x0000,/*0000000000000000*/
    0x0000,/*0000000000000000*/
    0x700e,/*0111000000001110*/
    0x381c,/*0011100000011100*/
    0x1c38,/*0001110000111000*/
    0x0e70,/*0000111001110000*/
    0x07e0,/*0000011111100000*/
    0x03c0,/*0000001111000000*/
    0x03c0,/*0000001111000000*/
    0x07e0,/*0000011111100000*/
    0x0e70,/*0000111001110000*/
    0x1c38,/*0001110000111000*/
    0x381c,/*0011100000011100*/
    0x700e,/*0111000000001110*/
    0x0000,/*0000000000000000*/
    0x0000,/*0000000000000000*/
};

void draw_bord(int x1,int y1,int x2,int y2,int c1,int c2,int bw);

void init_winbase()
{
	int i,j;
	for(i=0;i<20;i++)
		for(j=0;j<30;j++)
			imgp[i][j]=NULL;
}

void draw_workplace(int x1,int y1,int x2,int y2)
{
    int i,j;
    setfillstyle(1,15);
    bar(x1+BW,y1+BW,x2-BW,y2-BW);
    setcolor(0);
    for(j=0;j<=BW;j++)
        line(x1,j+y1,x2-j,y1+j);
    for(i=0;i<=BW;i++)
        line(x1+i,y1+BW,x1+i,y2-i);
    setcolor(15);
    for(j=0;j<=BW;j++)
        line(x2,j+y2-BW,x1+BW-j,y2+j-BW);
    for(i=0;i<=BW;i++)
        line(x2+i-BW,y2-BW,x2+i-BW,y1+BW-i);
}

void hlight_bar(int x1,int y1,int x2,int y2,int c)
{
    int i;
    setwritemode(1);
    setcolor(c);
    mouse_off();
    for(i=y1;i<=y2;i++)
    line(x1,i,x2,i);
    mouse_on();
    setwritemode(0);
}

void draw_scrollbar(int x1,int y1,int x2,int y2)
{
    int i,j;
    setfillstyle(1,15);
    bar(x1,y1,x2,y2);
    draw_btn(x1-1,y1,x2-1,y1+x2-x1,7,1);/*画向上的三角按钮*/
    for(i=0;i<5;i++)
        for(j=5-i;j<9-3+i;j++)
            putpixel(x1+1+j,y1+5+i,0);
    for(i=y1+x2-x1+1;i<y2-x2+x1-1;i+=2)
        for(j=x1;j<x2;j+=2)
            putpixel(j,i,7);
    for(i=y1+x2-x1+2;i<y2-x2+x1;i+=2)
        for(j=x1+1;j<x2-1;j+=2)
            putpixel(j,i,7);
    draw_btn(x1-1,y2-x2+x1,x2-1,y2,7,1);/*画向下的三角按钮*/
    for(i=0;i<5;i++)
        for(j=5-i;j<9-3+i;j++)
            putpixel(x1+1+j,y2-5-i,0);
}

void draw_win(int sx,int sy,int ex,int ey)
{
    mouse_off();
    setfillstyle(1,7);
    bar(sx,sy,ex,ey);
    setcolor(15);
    line(sx,sy,sx,ey);
    line(sx,sy,ex,sy);
    setcolor(0);
    line(ex,sy,ex,ey);
    line(sx,ey,ex,ey);
    mouse_on();
}
void winp(struct button *t,int state)
{
    mouse_off();
    if(state==1)/*用亮暗两种边框线进行按钮高亮与非高亮状态的转换*/
    {
        setcolor(0);/*两条暗边框线*/
        line(t->x1,t->y1,t->x1,t->y2);
        line(t->x1,t->y1,t->x2,t->y1);
        setcolor(15);/*两条亮边框线*/
        line(t->x2,t->y1,t->x2,t->y2);
        line(t->x1,t->y2,t->x2,t->y2);
    }
    else
    {
        setcolor(15);/*两条亮边框线*/
        line(t->x1,t->y1,t->x1,t->y2);
        line(t->x1,t->y1,t->x2,t->y1);
        setcolor(0);/*两条暗边框线*/
        line(t->x2,t->y1,t->x2,t->y2);
        line(t->x1,t->y2,t->x2,t->y2);
  }
  mouse_on();
}
void hlight_btn(struct button *t)
{
    winp(t,0);
    while(mouse_on_btn(t))/*如果鼠标在按钮上一直高亮按钮*/
    {
        mouse_read();
        if(mousekey!=0) break;
        if(kbhit()) break;
    }
    setcolor(7);
    mouse_off();
    rectangle(t->x1,t->y1,t->x2,t->y2);
    mouse_on();
}


void draw_btn(int x1,int y1,int x2,int y2,int c,int bw)
{
    setfillstyle(1,c);
    bar(x1+bw,y1+bw,x2-bw,y2-bw);
    bar(x1,y1,x2,y2);
    draw_bord(x1,y1,x2,y2,15,0,bw);
}

void draw_bord(int x1,int y1,int x2,int y2,int c1,int c2,int bw)
{
    int i,j;
    mouse_off();
    setcolor(c1);
    for(j=0;j<=bw;j++)
        line(x1,j+y1,x2-j,y1+j);
    for(i=0;i<=bw;i++)
        line(x1+i,y1+bw,x1+i,y2-i);
    setcolor(c2);
    for(j=0;j<=bw;j++)
        line(x2,j+y2-bw,x1+bw-j,y2+j-bw);
    for(i=0;i<=bw;i++)
        line(x2+i-bw,y2-bw,x2+i-bw,y1+bw-i);
    mouse_on();
}

void draw_btnbord(struct button *bt,int p,int bw)
{
    if(p==1)
        draw_bord(bt->x1,bt->y1,bt->x2-2,bt->y2-2,0,15,bw);/*画未按下按钮的边框线*/
    if(p==0)
        draw_bord(bt->x1,bt->y1,bt->x2-2,bt->y2-2,15,0,bw);/*画按钮被按下的边框线*/
}

void draw_img(int x,int y,int *img,int c1,int c2)
{
    int i,j,t;
    mouse_off();
    for(i=0;i<16;i++)
    {
        t=img[i];
        for(j=0;j<16;j++)
        {
            if(t%2)
                putpixel(16-j+x,y+i,c1);
            else
                if(c2>-1)/*c2=-1时不画img的背景色*/
                    putpixel(16-j+x,y+i,c2);
            t>>=1;
        }
    }
    mouse_on();
}

void draw_dialog(int x,int y,int w,int h,char *title,char *text,int bt_n,char *btn[])
{
    int x1=x,y1=y,x2=x+w,y2=y+h;
    int line_len=w/9;
    int line=(strlen(text)-1)/line_len+1;
    char tag[30];
    strcpy(tag,title);
    tag[8]='\0';
    mouse_off();
    draw_win(x1,y1,x2,y2);
    setfillstyle(1,9);
    bar(x1+1,y1,x2,y1+20);
    draw_img(x2-20,y1+2,img_close,15,4);
    draw_text(x1+6,y1+22,text,line_len,0);
    if(!strcmp(tag,"输入框："))/*判断是否需要输入框*/
    {
        draw_string(x1+2,y1+2,&title[8],0);
        draw_workplace(x1+9,y1+20+19*line,x2-9,y1+40+19*line);
        set_textarea(x1+9,y1+20+19*line,x2-9,y1+40+19*line);
        cursor_init(x1+11,y1+22+19*line,9,18,1,16);
    }
    else
        draw_string(x1+2,y1+2,title,0);
    switch(bt_n)/*根据bt_n的值画相应数目的按钮*/
    {
        case 3:
            draw_btn(x1+146,y2-26,x1+206,y2-6,7,0);
            draw_string((2*x1+352)/2-strlen(btn[2])*4,y2-24,btn[2],0);
        case 2:
            draw_btn(x1+236,y2-26,x1+296,y2-6,7,0);
            draw_string((2*x1+532)/2-strlen(btn[1])*4,y2-24,btn[1],0);
        case 1:
            draw_btn(x1+326,y2-26,x1+386,y2-6,7,0);
            draw_string((2*x1+712)/2-strlen(btn[2])*4,y2-24,btn[0],0);
    }
    mouse_on();
}
int prompt_dialog(int h,char *title,char *text,int bt_n,char *btn[])
{
    int x1=(WW-MAX_DIALOG_W)/2;
    int x2=WW-x1;
    int y1=100;
    int y2=y1+h;
    int select=NONE;
    set_textarea(-1,-1,-1,-1);
	save_bigimg(x1,x2,y1,y2);
    draw_dialog(x1,y1,MAX_DIALOG_W,h,title,text,bt_n,btn);
    while(1)
    {
        mouse_cursetting(); 
        mouse_read();
        if(mousekey==1)
        {
            if(mouse_in(x2-20,y1+2,x2-4,y1+18))
            {
                select=0;
                draw_bord(x2-20,y1+2,x2-4,y1+18,0,15,0);
                delay(100);
                draw_bord(x2-20,y1+2,x2-4,y1+18,15,0,0);
            }
            if(mouse_in(x1+326,y2-26,x1+386,y2-6))
            {
                select=1;
                draw_bord(x1+326,y2-26,x1+386,y2-6,0,15,0);
                delay(100);
                draw_bord(x1+326,y2-26,x1+386,y2-6,15,0,0);
            }            
            if(mouse_in(x1+236,y2-26,x1+296,y2-6))
            {
                select=2;
                draw_bord(x1+236,y2-26,x1+296,y2-6,0,15,0);
                delay(100);
                draw_bord(x1+236,y2-26,x1+296,y2-6,15,0,0);
            }     
            if(mouse_in(x1+146,y2-26,x1+206,y2-6))
            {
                select=3;
                draw_bord(x1+146,y2-26,x1+206,y2-6,0,15,0);
                delay(100);
                draw_bord(x1+146,y2-26,x1+206,y2-6,15,0,0);
            }  
        }
        if(kbhit()&&bioskey(0)==K_ESC)
            select=0;
        if(select!=NONE)
        {
            put_bigimg(x1,x2,y1,y2);
            set_textarea(2,41,620,477);
            return select;
        }
    }
}
char *input_dialog(int h,char *title,char *text,int bt_n,char *btn[])
{
    char tip[30]="输入框：";
    int x1=(WW-MAX_DIALOG_W)/2;
    int x2=WW-x1;
    int y1=100;
    int y2=y1+h;
    int select=NONE;       /*记录所选项目*/
    int bios_key;          /*存储键入的扫描码*/
    int input_state=0;     /*记录输入状态*/
    char input[30]={'\0'}; /*存储要输入的字符串*/
    int cur_n=0;           /*记录光标位置*/
    int i;
    int line_len=MAX_DIALOG_W/9;
    int line=(strlen(text)-1)/line_len+1;
    strcat(tip,title);
    set_textarea(-1,-1,-1,-1);
    mouse_cursetting();
	save_bigimg(x1,x2,y1,y2);
    draw_dialog(x1,y1,MAX_DIALOG_W,h,title,text,bt_n,btn);
    while(1)
    {
        if(input_state)
            cursor_blink();
        mouse_cursetting(); 
        mouse_read();
        if(mousekey==1)
        {
            if(mouse_in(x2-20,y1+2,x2-4,y1+18))/*如果鼠标点击了关闭按钮*/
            {
                select=0;
                draw_bord(x2-20,y1+2,x2-4,y1+18,0,15,0);
                delay(100);
                draw_bord(x2-20,y1+2,x2-4,y1+18,15,0,0);
            }
            if(mouse_in(x1+326,y2-26,x1+386,y2-6))
            {
                select=1;
                draw_bord(x1+326,y2-26,x1+386,y2-6,0,15,0);
                delay(100);
                draw_bord(x1+326,y2-26,x1+386,y2-6,15,0,0);
            }            
            if(mouse_in(x1+236,y2-26,x1+296,y2-6))
            {
                select=2;
                draw_bord(x1+236,y2-26,x1+296,y2-6,0,15,0);
                delay(100);
                draw_bord(x1+236,y2-26,x1+296,y2-6,15,0,0);
            }     
            if(mouse_in(x1+146,y2-26,x1+206,y2-6))
            {
                select=3;
                draw_bord(x1+146,y2-26,x1+206,y2-6,0,15,0);
                delay(100);
                draw_bord(x1+146,y2-26,x1+206,y2-6,15,0,0);
            }  
            if(mouse_in(x1+9,y1+20+19*line,x2-9,y1+40+19*line))
            {
                if(!input_state)
                    input_state=1;
                if(mousex>x1+12+9*strlen(input))
                {
                    cur_n=strlen(input);
                    cur_moveto(0,cur_n);
                }
                else
                  cur_movetoxy(mousex,mousey,&i,&cur_n);
            }
        }
        if(kbhit())
        {
            bios_key=bioskey(0);   /*读取被按下的按键并存储*/
            i=bios_key&0x00ff;     /*截取所获得按键的ascii码值*/
            if(i>31&&i<128&&strlen(input)<29)
            {
                str_insert(input,cur_n,i);
                cur_n++;
                setfillstyle(1,15);
                bar(x1+12,y1+22+19*line,x2-12,y1+38+19*line);
                draw_string(x1+12,y1+22+19*line,input,0);
                cur_right();
            }
            switch(bios_key)
            {
                case K_ESC:
                    select=0;
                    break;
                case K_BACKSPACE:
                    if(input_state&&cur_n>0)
                    {
                        str_del(input,cur_n-1);
                        cur_n--;
                        cur_left();
                        setfillstyle(1,15);
                        bar(x1+12,y1+22+19*line,x2-12,y1+38+19*line);
                        draw_string(x1+12,y1+22+19*line,input,0);
                    }
                    break;
               case K_DEL:
					if(input_state&&cur_n<strlen(input))                   
					{
                        str_del(input,cur_n);
                        setfillstyle(1,15);
                        bar(x1+12,y1+22+19*line,x2-12,y1+38+19*line);
                        draw_string(x1+12,y1+22+19*line,input,0);
					}
                   break;
                case K_LEFT:
                    if(input_state&&cur_n>0)
                    {
                        cur_left();
                        cur_n--;
                    }
                    break;
                case K_RIGHT:
                    if(input_state&&cur_n<strlen(input))
                    {
                        cur_right();
                        cur_n++;
                    }
            }
        }
        if(select!=NONE)
        {
			put_bigimg(x1,x2,y1,y2);
            set_textarea(2,41,620,477);
            if(select==1)
                return input;
			return NULL;
        }
    }
}
void save_bigimg(int x1,int x2,int y1,int y2)
{
	/*由于一次申请的内存太多容易失败故分多次申请*/
	int m=(y2-y1)/20+1,n=(x2-x1)/20+1;
	int i,j;
	areas=imagesize(0,0,20,20);
	mouse_off();
	for(i=0;i<m;i++)
		for(j=0;j<n;j++)
		{
			imgp[i][j]=malloc(areas);
			if(!imgp[i][j])
			{
				closegraph();
				restorecrtmode();
				print_log("无法记录屏幕信息：内存不足！\n");
				exit(1);
			} 
			getimage(x1+j*20,y1+i*20,x1+(j+1)*20,y1+(i+1)*20,imgp[i][j]);
			
		}
	mouse_on();
}
void put_bigimg(int x1,int x2,int y1,int y2)
{
	int i,j;
	mouse_off();
	for(i=0;imgp[i][0];i++)
		for(j=0;imgp[i][j];j++)
		{
			putimage(x1+j*20,y1+i*20,imgp[i][j],COPY_PUT);
			free(imgp[i][j]);
			imgp[i][j]=NULL;
		}
	mouse_on();
}
int pull_context_menu(int item_n,char *menu[],int x,int y)
{
    size_t are;
	int i,bios_key,selected_item=NONE,hlight_item=NONE;
	int x0,y0;
	/* are=imagesize(x,y,x+PULLMENU_W,y+item_n*20+10);
    imgp1=malloc(are); */
    set_textarea(-1,0,0,0);
    mouse_cursetting();
	save_bigimg(x,x+PULLMENU_W,y,y+item_n*20+10);
    mouse_off();
    /* getimage(x,y,x+PULLMENU_W,y+item_n*20+10,imgp1); */
    draw_win(x,y,x+PULLMENU_W,y+item_n*20+10);
    for(i=0;i<item_n;i++)
    {
        if(context_menu_state[i])
        {
            setcolor(15);
            draw_string(x+3,y+6+i*20,menu[i],15);
            setcolor(0);
            draw_string(x+4,y+7+i*20,menu[i],0);
        }
        else
            draw_string(x+3,y+6+i*20,menu[i],15);
    }
	mouse_on();
	while(1)
	{
		while(mousekey==2)/*防止鼠标未及时松开时产生误操作*/
			mouse_read();
		if(kbhit())
		{
			if(hlight_item!=NONE)
				hlight_bar(x+3,y+6+hlight_item*20,x+97,y+24+hlight_item*20,14);
			bios_key=bioskey(0);
			switch(bios_key)
			{
				case K_UP:
                    if(hlight_item==NONE||hlight_item==0)
                        hlight_item=item_n-1;
                    else
                        hlight_item--;
                    break;
                case K_DOWN:
                    if(hlight_item==item_n-1)
                        hlight_item=0;
                    else
                        hlight_item++;
                    break;
				case K_ENTER:
					if(hlight_item==NONE)
						selected_item=-2;
					else
						selected_item=hlight_item;
					break;
				default:
					selected_item=-2;
			}
			hlight_bar(x+3,y+6+hlight_item*20,x+97,y+24+hlight_item*20,14);
		}
		mouse_read();
		if(mousekey&&(mousekey!=1||(mousekey==1&&!mouse_in(x+4,y+6+i*20,x+97,y+22+i*20))))
			selected_item=-2;
		if(mouse_move(x0,y0))
        {
            x0=mousex;
            y0=mousey;
            for(i=0;i<item_n;i++)
            {
                mouse_read();
                if(mouse_in(x+4,y+6+i*20,x+97,y+22+i*20))
                {
                    if(hlight_item!=NONE)
                        hlight_bar(x+3,y+6+hlight_item*20,x+97,y+24+hlight_item*20,14);
                    hlight_bar(x+3,y+6+i*20,x+97,y+24+i*20,14);
                    hlight_item=i;
                    while(mouse_in(x+4,y+6+i*20,x+97,y+22+i*20))
                    {
                        x0=mousex;
                        y0=mousey;
                        mouse_read();
                        if(mousekey==1)
                        {
                            if(context_menu_state[i]==YES)
                            {
                                selected_item=i;
                                break;
                            }
                        }
                        if(kbhit()) 
							break;
                    }
                }
            }
        }	
		if(selected_item!=NONE)
		{
			put_bigimg(x,x+PULLMENU_W,y,y+item_n*20+10);
			if(selected_item==-2)
				return  NONE;
			else
				return selected_item;
		}
	}
}
