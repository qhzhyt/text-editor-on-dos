#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include<alloc.h>
#include "editor.h"
extern int start_x,start_y;                 /*外部变量光标的起始位置(cursor.c)*/
extern int cell_w,cell_h;                   /*外部变量单个字符所占位置的宽与高(cursor.c)*/
extern int msvisible,mousex,mousey,mousekey;/*外部变量鼠标的状态(mouse.c)*/
extern text_line *head;                     /*外部变量主数据链表的表头(file.c)*/
extern unsigned char item_active[MENU_N][MENU_MAX];
text_line *view_text[25];                   /*显示在屏幕上的文本，最多24行*/
text_line *viewtext_line[MAX_VIEWLINE];
int view_line,file_line;
int text_row=0,text_col=0;
int view_row=0,view_col=0;
int work_state=0;
int firstline=1;                            /*显示在屏幕上的第一行的编号*/
int text_end=FALSE;                         /*文件是否结束*/
text_line *cur_line;
text_line *first_textline;
text_cell *cur_cell;
int menu_altkey[]={ALT_F,ALT_E,ALT_V,ALT_S,ALT_T};/*菜单控制键*/
int first_open=1;    /*是否为第一次打开文件*/
int curfile_saved=1;/*当前操作的文件是否保存*/                       
step *curstep;/*记录步骤信息的表头*/
struct button btn_sup={620,43,636,59},btn_sdown={620,439,636,455};
selected_text *highlighted_text;
clipboard *clip_board;
extern int context_menu_state[MENU_MAX];

void insert_cell(text_cell *before,char ch);/*插入一个字符*/
text_cell *goto_textcol(text_line *line_head,int col);/*转到line_head行的第col个字符*/
void insert_line(int front,text_cell *before,char ch);/*插入一个新行*/
void del_cell(text_cell *before);/*删除一个字符*/
void del_line(text_line *before);/*删除一行*/
void comb_line(text_line *front,text_cell *before,text_line *next);/*连接两行*/
text_line *goto_viewline(text_line *head,int row);
void save_viewline();/*更新view_text*/
void print_viewtext();/*刷新屏幕字符*/
text_line *get_curtextline();
text_cell *goto_viewcell(text_line *viewline_head,int col);
void print_viewline(int line);/*打印一行字符*/

void create_viewlines();
void hlight_viewline(int line,int flag,int mode);/*高亮字符*/
void hlight_text();
void settag();
void hotkey_handle(int bios_key);/*快捷键处理*/

text_line *get_textline(int n);
text_line *goto_textline(text_line *head,int row);


void init_workstate()
{
	view_line=0;
        file_line=0;
	highlighted_text->state=0;
	clip_board->state=0;
	curstep=(step*)malloc(sizeof(step));/*创建步骤记录链表头结点*/
	if(!curstep)
	{
		print_log("内存不足无法存储步骤信息!\n");
		exit(0);
	}
	curstep->back=NULL;
	create_viewlines();
	save_viewline();
}
void first_print()
{
    int i;
	curfile_saved=1;
    firstline=1;
    view_line=0;
    file_line=0;
	clear_step();
    for(i=0;i<MAX_VIEWLINE;i++)
    {
		view_text[i]->first=NULL;
		view_text[i]->len=0;
		view_text[i]->tag=-1;
    } 
    save_viewline();
    print_viewtext();
	print_stateline(head->len,0,0);
    first_open=0;
}
void save_viewline()
{
    int i,l,tag;
    text_line *p,*t;
    p=goto_viewline(head,firstline);
    tag=p->tag;
    t=first_textline;
	text_end=0;
    for(i=0;i<MAX_VIEWLINE&&t;)
    {
        l=p->len;
        while(((l>0)||(!l&&!tag))&&i<MAX_VIEWLINE)
        {
            view_text[i]->first=p->first;
            view_text[i]->next=p->next;
            view_text[i]->tag=tag;
            viewtext_line[i]=t;
            if(l>MAX_VIEWCOL)
            {
                view_text[i]->len=MAX_VIEWCOL;
                p->first=goto_textcol(p,MAX_VIEWCOL+1);
                i++;
                tag++;
            }
            else
            {
                view_text[i]->len=l;
                i++;
                break;
            }
            l-=MAX_VIEWCOL;
      }
      t=t->next;
      p->first=p->next->first;
      p->len=p->next->len;
      p->next=p->next->next;
      tag=0;
    }
    view_text[i]->tag=-1;
    if(i<MAX_VIEWLINE||(view_text[i-1]->next==NULL&&view_text[i-1]->len<MAX_VIEWCOL))
        text_end=1;
    free(p);
	p=NULL;
}
void create_viewlines()
{
    int i;
    text_line *p;
	print_log("正在创建viewlines...\n");
    for(i=0;i<MAX_VIEWLINE;i++)
    {
        if(p=(text_line*)malloc(sizeof(text_line)))
		{
			view_text[i]=p;
			view_text[i]->first=NULL;
			view_text[i]->len=0;
			view_text[i]->tag=-1;
		} 
        else
        {
            print_log("内存不足无法创建viewlines!\n");
            exit(0);
        }
    }
	print_log("创建成功！\n");
}
void print_viewline(int line)
{
    int x,y=start_y+line*cell_h;
    text_cell *p;
    int j=0;
    setfillstyle(1,15);
    x=start_x+1;
    bar(x,y,x+cell_w*MAX_VIEWCOL,y+cell_h);
	if(view_text[line]->tag!=-1)
	{
		p=view_text[line]->first;
		while(j<MAX_VIEWCOL&&p)
		{
			draw_ch(x,y,p->ch,0);
			p=p->next;
			x+=cell_w;
			j++;
		}
	}
}
void print_viewtext()
{
    int x,y=start_y;
    int i,j=0;
    for(i=0;i<MAX_VIEWLINE;i++)
		print_viewline(i);

	
}
void work_process()
{
    int bios_key=1;
    text_line *p;
    text_cell *p2;
    char *btn_str[4]={"保存","放弃","取消"};
    int i;
    int row,col;
	char ch;
    while(1)
    {
        mouse_read();
        menu_process(-1);
        if(kbhit())
        {
            bios_key=bioskey(0);
            if(work_state==1)
            {
                i=bios_key&0x00ff;/* 截取扫描码中的ascii码 */
                if(i>31&&i<128)/* 判断是否为普通字符 */
                {
                    if(view_col)
                    {
                        insert_cell(cur_cell,i);
                        viewtext_line[view_row]->len++;
                    }
                    else
                    {
                        if(view_text[view_row]->tag)
                        {
                            goto_viewcell(view_text[view_row-1],MAX_VIEWCOL);
                            insert_cell(cur_cell,i);
                            viewtext_line[view_row-1]->len++;
                            goto_viewcell(view_text[view_row],1);
                        }
                        else
                        {
                            p2=(text_cell*)malloc(sizeof(text_cell));
                            p2->next=view_text[view_row]->first;
                            p2->ch=i;
                            viewtext_line[view_row]->first=p2;
                            viewtext_line[view_row]->len++;
                            view_text[view_row]->first=p2;
                        }
                    }
                    view_text[view_row]->len++;
                    view_col++;
					print_stateline(head->len,viewtext_line[view_row]->tag,view_text[view_row]->tag*MAX_VIEWCOL+view_col);
					add_step('i',viewtext_line[view_row]->tag,view_text[view_row]->tag*MAX_VIEWCOL+view_col,i);
                    if(view_text[view_row]->len>MAX_VIEWCOL)
                    {
                        save_viewline();
                        print_viewtext();
                    }
                    else
                        print_viewline(view_row);
                    if(view_col>MAX_VIEWCOL)
                    {
                        view_row++;
                        view_col=1;
                    }
                    goto_viewcell(view_text[view_row],view_col);
                    cur_moveto(view_row,view_col);
                }
                switch(bios_key)
                {
                    case K_ENTER:
						add_step('e',cur_line->tag,viewtext_line[view_row]->tag*MAX_VIEWCOL+view_col,0);
                        if(!view_row&&firstline==1&&!view_col)
                            insert_line(-1,NULL,0);
                        else
                        {
                            if(view_col)
                            insert_line(view_row,cur_cell,0);
                            else
                            {
                                if(!view_row)
                                {
                                    firstline--;
                                    save_viewline();
                                    view_row++;
                                }
                                view_col=view_text[view_row-1]->len;
                                goto_viewcell(view_text[view_row-1],view_col);
                                insert_line(view_row-1,cur_cell,0);
                            }
                        }
                        if(view_row==MAX_VIEWLINE-1)
                            firstline++;
                        else
                            view_row++;
                        save_viewline();
                        print_viewtext();
                        cur_moveto(view_row,0);
                        view_col=0;
                        get_curtextline();
                        goto_viewcell(view_text[view_row],view_col);
                        break;
                    case K_UP:
                        if(view_row)
                        {
                            view_row--;
                            if(view_col>view_text[view_row]->len)
                            {
                                view_col=view_text[view_row]->len;
                                cur_moveto(view_row,view_col);
                            }
                            else
                            cur_up();
                            get_curtextline();
                            goto_viewcell(view_text[view_row],view_col);
                        }       
                        else
                        {
                            if(firstline>1)
                            {
                                firstline--;
                                if(view_col>view_text[view_row]->len)
                                    view_col=view_text[view_row]->len;
                                save_viewline();
                                print_viewtext();
                                get_curtextline();
                                goto_viewcell(view_text[view_row],view_col);
                            }
                        }
                        break;
                    case K_DOWN:
                        if(view_row<MAX_VIEWLINE-1)
                        {
							if(view_text[view_row+1]->tag!=-1)
							{
								view_row++;
								if(view_col>view_text[view_row]->len)
								{
									view_col=view_text[view_row]->len;
									cur_moveto(view_row,view_col);
								}
								else
								cur_down();
								get_curtextline();
								goto_viewcell(view_text[view_row],view_col);
							}
                        }
                        else
                        {
							
                            if(!text_end)
                            {
                                firstline++;
                                save_viewline();
                                print_viewtext();
                                if(view_col>view_text[view_row]->len)
                                    view_col=view_text[view_row]->len;
                                get_curtextline();
                                goto_viewcell(view_text[view_row],view_col);
                            }
                        }
                        break;
                    case K_LEFT:
                        if(!view_col)
                        {
                            if(!view_row)
                            {
                                if(firstline>1)
                                {
                                    firstline--;
                                    save_viewline();
                                    print_viewtext();
                                    get_curtextline();
                                    view_col=view_text[view_row]->len;
                                    goto_viewcell(view_text[view_row],view_col);
                                    cur_moveto(view_row,view_col);
                                }
                            }
                            else
                            {
                                view_row--;
                                view_col=view_text[view_row]->len;
                                get_curtextline();
                                goto_viewcell(view_text[view_row],view_col);
                                cur_moveto(view_row,view_col);
                            }
                        }
                        else
                        {
                            cur_left();
                            view_col--;
                            goto_viewcell(view_text[view_row],view_col);
                        }
                        break;
                    case K_RIGHT:
                        if(view_col==view_text[view_row]->len)
                        {
							if(view_row<head->len-1)
							{
								
								if(view_row==MAX_VIEWLINE-1)
								{
									if(!text_end)
									{
										firstline++;
										save_viewline();
										print_viewtext();
										get_curtextline();
										view_col=0;
										goto_viewcell(view_text[view_row],view_col);
										cur_moveto(view_row,view_col);
									}
								}
								else
								{
									view_row++;
									view_col=0;
									get_curtextline();
									goto_viewcell(view_text[view_row],view_col);
									cur_moveto(view_row,view_col);
								}
							}
                        }
                        else
                        {
                            cur_right();
                            view_col++;
                            goto_viewcell(view_text[view_row],view_col);
                        }
                        break;
                    case K_DEL:/*直接将光标后移一格，进行backspace操作*/
                        if(view_row==MAX_VIEWLINE-1&&text_end&&view_col>=view_text[view_row]->len)
                            break;
                        if(view_col<view_text[view_row]->len)
                        {
                            view_col++;
                            goto_viewcell(view_text[view_row],view_col);
                        }       
                        else
                        {
                            if(view_row==MAX_VIEWLINE-1)
                            {
                                firstline++;
                                save_viewline();
                                view_col=0;
                                goto_viewcell(view_text[view_row],view_col);
                            }
                            else
                            {
                                view_row++;
                                view_col=0;
                                goto_viewcell(view_text[view_row],view_col);
                            }
                        }
                    case K_BACKSPACE:
						row=cur_line->tag;
						col=view_text[view_line]->tag*MAX_VIEWCOL+view_col;
						ch=cur_cell->ch;
                        if(!view_col)
                        {
                            if(!view_row)
                            {
                                if(firstline>1)
                                {
                                    firstline--;
                                    save_viewline();
                                    view_row++;
                                }
                            }
                            if(view_row)
                            {
                                view_row--;
                                view_col=view_text[view_row]->len;
                                if(view_text[view_row+1]->tag)
                                {
                                    
                                    get_curtextline();
                                    goto_viewcell(view_text[view_row],view_col);
                                    cur_moveto(view_row,view_col);
                                    save_viewline();
                                    print_viewtext();
                                }
                                else
                                {
									col=viewtext_line[view_row]->len;
                                    if(view_col)
                                    {
                                        goto_viewcell(view_text[view_row],view_col);
                                        comb_line(viewtext_line[view_row],cur_cell,viewtext_line[view_row+1]);
                                    }
                                    else/*光标在行首时，进行特殊操作*/
                                    {
                                        viewtext_line[view_row]->first=viewtext_line[view_row+1]->first;
                                        viewtext_line[view_row]->next=viewtext_line[view_row+1]->next;
                                        viewtext_line[view_row]->len=viewtext_line[view_row+1]->len;
                                    }
									settag();
									add_step('d',row,col,'\n');
                                    save_viewline();
                                    print_viewtext();
                                    get_curtextline();
                                    cur_moveto(view_row,view_col);
                                }
                            }   
                        }
                        else
                        {
                            if(view_col==1)
                            {
                                if(view_text[view_row]->tag)
                                    del_cell(goto_viewcell(view_text[view_row-1],MAX_VIEWCOL));
                                else
                                    cur_line->first=cur_line->first->next;		
                                cur_line->len--;
                                view_text[view_row]->first=view_text[view_row]->first->next;
                                view_text[view_row]->len--;
                                        
                                print_viewline(view_row);
                                view_col--;
                                goto_viewcell(view_text[view_row],0);     
                                cur_moveto(view_row,view_col);
                            }
                            else
                            {
                                view_col--;
                                cur_line->len--;
                                view_text[view_row]->len--;
                                goto_viewcell(view_text[view_row],view_col);
                                del_cell(cur_cell);
                                print_viewline(view_row);
								if(cur_line->len>68)
								{
									save_viewline();
									print_viewtext();
								}
                                cur_moveto(view_row,view_col);
                            }
							add_step('d',row,col,ch);
                        }
                        break;
				}
				print_stateline(head->len,cur_line->tag,view_text[view_row]->tag*MAX_VIEWCOL+view_col);
				cur_moveto(view_row,view_col);
            }
            for(i=0;i<MENU_N;i++)
                if(bios_key==menu_altkey[i])
                    menu_process(i);
			if(bios_key==K_ESC)
				if(!curfile_saved)
				{
                    i=prompt_dialog(125,"警告","文件未保存，继续退出将丢弃对文件的所有修改是否继续？",3,btn_str);
					if(i==1)
					{
						save_curfile();
						exit(0);
					}
					if(i==2)
						exit(0);
				}
			hotkey_handle(bios_key);		
		}
        if(work_state==1)
            cursor_blink();
		mouse_read();
        if(mousekey==1&&mouse_in(2,41,637,477))/*处理鼠标点击触发的光标移动事件*/
        {
			if(highlighted_text->state)
			{
				print_viewtext();
				highlighted_text->state=0;
			}
            if(mouse_on_btn(&btn_sup))
            {
                draw_btnbord(&btn_sup,1,1);
                delay(300);
                if(firstline>1)
                {
                    firstline--;
                    save_viewline();
                    print_viewtext();                    
                }
                draw_btnbord(&btn_sup,0,1);
            }
            if(mouse_on_btn(&btn_sdown))
            {
                draw_btnbord(&btn_sdown,1,1);
                delay(300);
                if(!text_end)
                {
                    firstline++;
                    save_viewline();
                    print_viewtext();                    
                }
                draw_btnbord(&btn_sdown,0,1);
            }
            cur_movetoxy(mousex,mousey,&view_row,&view_col);   
			if(view_row>head->len-1)
				view_row=head->len-1;
            if(view_col>view_text[view_row]->len)
            {
                view_col=view_text[view_row]->len;
                cur_moveto(view_row,view_col);
            }
			get_curtextline();
            goto_viewcell(view_text[view_row],view_col);
            work_state=1;
			print_stateline(head->len,cur_line->tag,view_text[view_row]->tag*MAX_VIEWCOL+view_col);
			cur_moveto(view_row,view_col);
        }
        if(mousekey==2&&mouse_in(2,41,637,477))/*如果在文本编辑区点击右键，则弹出右键菜单*/
        {
			int x=mousex,y=mousey;
			int select;
			char *menu[]={"撤消","剪贴","复制","粘贴","全选"};
			/*如果鼠标位置使弹出菜单无法显示完全，则调整菜单弹出位置*/
			if(mousex>WW-PULLMENU_W)
				x=mousex-PULLMENU_W;
			if(mousey>WH-8*20)
				y=mousey-8*20+3;
            select=pull_context_menu(5,menu,x,y);
            if(select!=NONE)
				exec_menu_item(1,select);
        }
        while(mousekey==1)/*处理鼠标左键长按事件*/
        {
            menu_process(-1);
            if(mouse_move(mousex,mousey))
                hlight_text();
            mouse_read();
        }
    }
}
void hlight_viewline(int line,int flag,int mode)
{
    switch(mode)/* 选取文字高亮方式 */
    {
        case 0:/* 高亮第line行 */
             hlight_bar(start_x+1,start_y+line*cell_h,start_x+MAX_VIEWCOL*cell_w-1,start_y+(line+1)*cell_h,14);
             break;
        case 1:/* 高亮第line行开始到第flag个字符，或整行字符 */
            if(flag<=view_text[line]->len)
                hlight_bar(start_x+1,start_y+line*cell_h,start_x+flag*cell_w-1,start_y+(line+1)*cell_h-1,14);
            else
                hlight_bar(start_x+1,start_y+line*cell_h,start_x+view_text[line]->len*cell_w-1,start_y+(line+1)*cell_h-1,14);
            break;
        case 2:/* 高亮第line行从第flag个字符到行尾间的字符 */
			if(flag<view_text[line]->len)
				hlight_bar(start_x+1+flag*cell_w,start_y+line*cell_h,start_x+view_text[line]->len*cell_w-1,start_y+(line+1)*cell_h-1,14);
			break;
		case 3:
			;   
    }
}

void select_all()
{
    int i;
   for(i=0;i<MAX_VIEWLINE;i++)
   {
       if(view_text[i]->tag==-1)
        break;
       hlight_viewline(i,0,2);
   }
    
    highlighted_text->state=1;
}
void hotkey_handle(int bios_key)
{
	switch(bios_key)
	{
		case CTRL_A:
            select_all();
            break;
        case CTRL_C:
            copy_text();
            break;
		case CTRL_V:
            paste_text(viewtext_line[view_row]->tag,view_text[view_row]->tag*MAX_VIEWCOL+view_col);
            save_viewline();
            highlighted_text->state=0;
            print_viewtext();
            break;
		case CTRL_X:
			cut_text(clip_board->head);
			save_viewline();
			highlighted_text->state=0;
			print_viewtext();
			break;
		case CTRL_Z:
			if(curstep->back)
				goto_backstep();
	}
}
void copy_text()
{
	int i,j,k,l,m;
	text_line *line_sta,*pl,*tl;
	text_cell *col_sta,*pc,*tc;
	i=highlighted_text->start_row;
	j=highlighted_text->start_col;
	k=highlighted_text->end_row;
	l=highlighted_text->end_col;
	line_sta=get_textline(i);
	if(line_sta==NULL)
	{
		return;
	}
	col_sta=goto_textcol(line_sta,j);
	pl=(text_line *)malloc(sizeof(text_line));
	if(!pl)
	{
		print_log("无法复制文本，内存不足！\n");
		exit(0);
	}
	clip_board->head=pl;
	clip_board->state=1;
	pl->len=line_sta->len-j;
	if(i!=k)
		j=0;
	while(i<k)
	{	
		pc=(text_cell *)malloc(sizeof(text_cell));
		if(!pc)
		{
			print_log("无法复制文本，内存不足！、n");
			exit(0);
		}		
		pc->ch=col_sta->ch;
		pl->first=pc;
		
		col_sta=col_sta->next;
		while(col_sta)
		{
			tc=(text_cell *)malloc(sizeof(text_cell));
			if(!tc)
			{
				print_log("无法复制文本，内存不足！、n");
				exit(0);
			}			
			tc->ch=col_sta->ch;
			pc->next=tc;
			pc=tc;
			col_sta=col_sta->next;
		}
		pc->next=NULL;
		line_sta=line_sta->next;
		col_sta=line_sta->first;
		tl=(text_line *)malloc(sizeof(text_line));
		tl->len=line_sta->len;
		if(!tl)
		{
			print_log("无法复制文本，内存不足！、n");
			exit(0);
		}		
		pl->next=tl;
		pl=tl;
		i++;
	}
	pc=(text_cell *)malloc(sizeof(text_cell));
	if(!pc)
	{
		print_log("无法复制文本，内存不足！\n");
		exit(0);
	}	
	pc->ch=col_sta->ch;
	pl->first=pc;
	pl->len=l-j;
	col_sta=col_sta->next;
	while(j<l)
	{
		tc=(text_cell *)malloc(sizeof(text_cell));
		tc->ch=col_sta->ch;
		pc->next=tc;
		pc=tc;
		col_sta=col_sta->next;
		j++;
	}
	pc->next=NULL;
	pl->next=NULL;
}
void paste_text(int line,int col)
{
	text_line *line_sta,*pl,*tl,*temp;
	text_cell *col_sta,*pc,*tc,*tempc;
	int t;
	line_sta=clip_board->head;
	pl=get_textline(line);
	temp=pl->next;
	t=pl->len-col;
	pl->len=col+line_sta->len;
	col_sta=line_sta->first;
	pc=goto_textcol(line_sta,col);
	tempc=pc->next;
	while(col_sta)
	{			
		tc=(text_cell *)malloc(sizeof(text_cell));
		tc->ch=col_sta->ch;
		pc->next=tc;
		pc=tc;
		col_sta=col_sta->next;
	}	
	pc->next;
	line_sta=line_sta->next;
	while(line_sta)
	{
		tl=(text_line *)malloc(sizeof(text_line));
		if(!tl)
		{
			print_log("无法复制文本，内存不足！、n");
			exit(0);
		}
		col_sta=line_sta->first;
		tl->len=line_sta->len;
		pl->next=tl;
		pl=tl;
		pc=(text_cell *)malloc(sizeof(text_cell));
		
		pc->ch=col_sta->ch;
		pl->first=pc;
		col_sta=col_sta->next;	
		while(col_sta)
		{
			tc=(text_cell *)malloc(sizeof(text_cell));
			tc->ch=col_sta->ch;
			pc->next=tc;
			pc=tc;
			col_sta=col_sta->next;			
		}
		pc->next=NULL;
		line_sta=line_sta->next;
	}
	pc->next=tempc;
	pl->next=temp;
	pl->len+=t;
	settag();
}
void cut_text(text_line *cb_head)
{
	int i=highlighted_text->start_row,j=highlighted_text->start_col;
	int t1=highlighted_text->end_col-j;
	text_line *line_sta=get_textline(i),*pl,*tl;
	text_cell *col_sta=goto_textcol(line_sta,j),*pc,*tc;
	pl=(text_line *)malloc(sizeof(text_line));
	tl=line_sta;
	tc=col_sta;
	pl->first=col_sta;
	cb_head=pl;
	if(i==highlighted_text->end_row)
	{
		while(j<highlighted_text->end_col)
		{
			col_sta=col_sta->next;
			j++;
		}
		tl->len-=t1;
		tc->next=col_sta->next;
		col_sta->next=NULL;
		pl->len=t1;
		pl->next=NULL;
		return;
	}
	pl->next=line_sta->next;
	tl->len=j;
	pl->len=line_sta->len-j;
	while(i<highlighted_text->end_row)
	{
		line_sta=line_sta->next;
		i++;
	}
	col_sta=line_sta->first;
	tl->next=line_sta->next;
	tl->len+=line_sta->len-highlighted_text->end_col;
	line_sta->len=highlighted_text->end_col;
	j=0;
	while(j<highlighted_text->end_col-1)
	{
		col_sta=col_sta->next;
		j++;
	}
	tc->next=col_sta->next;	
	col_sta->next=NULL;
	line_sta->next=NULL;
	context_menu_state[3]=1;
    item_active[1][3]=1;
	settag();
}
void hlight_text()/*模拟文字的选中操作*/
{
    int sta_x=mousex,sta_y=mousey;
    int sta_row,sta_col,i=1,j=1;
    int t_row,t_col;
    cur_movetoxy(sta_x,sta_y,&sta_row,&sta_col);
    if(sta_col>view_text[sta_row]->len)
		sta_col=view_text[sta_row]->len;
    while(mousekey==1)
    {
        if(mouse_move(mousex,mousey))
        {
            mouse_read();
            if(mousex>start_x+(sta_col+i)*cell_w&&(sta_col+i)<=view_text[sta_row]->len)
            {
                hlight_bar(start_x+(sta_col+i-1)*cell_w,start_y+(sta_row+j-1)*cell_h,start_x+(sta_col+i)*cell_w-1,start_y+(sta_row+j)*cell_h-1,14);
                    i++;
            }
            mouse_read();
            if(mousex<start_x+(sta_col+i-2)*cell_w&&(sta_col+i)>=0)
            {
                hlight_bar(start_x+(sta_col+i-1)*cell_w,start_y+(sta_row+j-1)*cell_h,start_x+(sta_col+i)*cell_w-1,start_y+(sta_row+j)*cell_h-1,14);
                i--;
            }
            if(mousey>start_y+(sta_row+j)*cell_h&&(sta_row+j)<=MAX_VIEWLINE&&view_text[sta_row+j]->tag!=-1)
            {
                hlight_viewline(sta_row+j-1,sta_col+i-1,2);
                mouse_read();
                i=(mousex-start_x)/cell_w-sta_col;
                hlight_viewline(sta_row+j,sta_col+i-1,1);
                j++;
            }
            if(mousey<start_y+(sta_row+j-1)*cell_h&&(sta_row+j)>=0)
			{
                hlight_viewline(sta_row+j-1,sta_col+i-1,1);
                mouse_read();
                i=(mousex-start_x)/cell_w-sta_col;                   
                hlight_viewline(sta_row+j-2,sta_col+i-1,2);
                j--;
            }
        }
        mouse_read();
    }
	
	i--,j--;
	if(!(!i&&!j))
	{
		if(j>0)
		{
			highlighted_text->start_row=viewtext_line[sta_row]->tag;
			highlighted_text->end_row=highlighted_text->start_row+j;
		}
		else
		{
			highlighted_text->end_row=viewtext_line[sta_row]->tag;
			highlighted_text->start_row=highlighted_text->end_row+j;
		}
		
		if(i>0)
		{
			
			highlighted_text->start_col=(view_text[sta_row]->tag)*MAX_VIEWCOL+sta_col;
			highlighted_text->end_col=(highlighted_text->start_col+i);
		}
		else
		{
			highlighted_text->end_col=view_text[sta_row]->tag*MAX_VIEWCOL+sta_col;
			highlighted_text->start_col=highlighted_text->end_col+i;
		}
		highlighted_text->state=1;
		context_menu_state[1]=1;
		context_menu_state[2]=1;
		context_menu_state[5]=1;
        item_active[1][1]=1;
        item_active[1][2]=1;
        item_active[1][5]=1;
		
	}
}
text_line *get_textline(int n)
{
	text_line *p=head;
	while(n--)
	{
		p=p->next;
		if(!p)
			return NULL;
	}
	return p;
}
text_line *get_curtextline()
{
    cur_line=viewtext_line[view_row];
    return cur_line;
}
text_cell *goto_viewcell(text_line *viewline_head,int col)
{
    text_cell *col_p;
    if(!col)
        cur_cell=NULL;
    else
    {
      col_p=viewline_head->first;
      while(--col&&col_p)
        col_p=col_p->next;
      cur_cell=col_p;
    }
    return cur_cell;
}
text_cell *goto_textcell(text_line *head ,int row,int col)
{
   return goto_textcol(goto_textline(head,row),col);
}
text_line *goto_viewline(text_line *head,int row)
{
    int line=0;
    text_line *row_p,*p;
    row_p=head->next;
    while(line+(row_p->len-1)/MAX_VIEWCOL+1<row&&row_p)
    {
        line+=((row_p->len-1)/MAX_VIEWCOL+1);
        row_p=row_p->next;
    }
    first_textline=row_p;
    p=(text_line*)malloc(sizeof(text_line));
    p->first=goto_textcol(row_p,MAX_VIEWCOL*(row-line-1)+1);
    p->len=row_p->len-MAX_VIEWCOL*(row-line-1);
    p->tag=row-line-1;
    p->next=row_p->next;
    return p;
}
text_line *goto_textline(text_line *head,int row)
{
    text_line *row_p;
    row_p=head->next;
    while(--row&&row_p)
        row_p=row_p->next;
    cur_line=row_p;
    return row_p;
}
text_cell *goto_textcol(text_line *line_head,int col)
{
    text_cell *col_p;
    col_p=line_head->first;
    while(--col&&col_p)
        col_p=col_p->next;
    cur_cell=col_p;
    return col_p;
}
void insert_cell(text_cell *before,char ch)
{
    text_cell *p;
    p=(text_cell*)malloc(sizeof(text_cell));
    if(!p)
    {
        print_log("插入字符失败:内存不足!\n");
        exit(0);
    }
    p->ch=ch;
    p->next=before->next;
    before->next=p;
}
void comb_line(text_line *front,text_cell *before,text_line *next)
{
    before->next=next->first;
    front->len=front->len+next->len;
    front->next=next->next;
}
void insert_line(int front,text_cell *before,char ch)
{
    text_line *p,*t;
	int col;
	t=(text_line*)malloc(sizeof(text_line));
	if(!t)
    {
        print_log("无法创建新行：内存不足!\n");
        exit(0);
    }
	if(front==-1)
	{
        t->len=0;
        t->first=NULL;
        t->next=head->next;
        head->next=t;
	}
    else
    {
        p=viewtext_line[front];
        col=view_text[front]->tag*MAX_VIEWCOL+p->len;
        t->len=p->len-col;
        p->len=col;
        t->first=before->next;
        before->next=NULL;
        t->next=p->next;
        p->next=t;
    }
	settag();
}
void settag()
{
	text_line *p=head->next;
	int i=1;
	while(p)
	{
		p->tag=i;
		p=p->next;
		i++;
	}
	head->len=i-1;
}
void del_cell(text_cell *before)
{
    text_cell *t=before->next;
    before->next=before->next->next;
    free(t);
}
void del_line(text_line *before)
{
    text_line *t=before->next;
    before->next=before->next->next;
    free(t);
	settag();
}
void add_step(char type,int row ,int col,char ch)
{
	step *p=(step*)malloc(sizeof(step));
	if(!p)
	{
		print_log("内存不足无法存储步骤信息!\n");
		exit(0);
	}
	curfile_saved=0;
	curstep->type=type;
	curstep->row=row;
	curstep->col=col;
	curstep->ch=ch;
	p->back=curstep;
	curstep=p;
	if(curstep->back)
		context_menu_state[0]=1;
}
void del_laststep()
{
	step *t;
	t=curstep->back;
	free(curstep);
	curstep=t;
}
void goto_backstep()
{
	step *sp=curstep->back;
	text_line *tp,*tt;
	text_cell *p,*tc;
	if(!sp)
	{
		print_log("无法撤消操作，操作记录为空！\n");
		return;
	}
	switch(sp->type)/* 操作类型判定 */
	{
		case 'i':							/* 插入操作，删除插入的数据 */
			tp=get_textline(sp->row);
			p=goto_textcol(tp,sp->col-1);
			if(!p)
			{
				p=tp->first;
				tp->first=p->next;
				free(p);
			}
			else
			{
				tc=p->next;
				p->next=tc->next;
				free(tc);
			}
			tp->len--;
			break;
		case 'd':							/* 删除操作，删除的字符若为普通字符则恢复原字符，若为'\n'则在原位置插入新行*/
			if(sp->col&&sp->ch!='\n')
			{
				tp=get_textline(sp->row);
				p=goto_textcol(tp,sp->col-1);
				tc=(text_cell*)malloc(sizeof(text_cell));
				if(!tc)
				{
					print_log("撤消操作失败：内存不足！\n");
					exit(0);
				}
				tp->len++;
				tc->ch=sp->ch;
				tc->next=p->next;
				p->next=tc;	
			}
			if(sp->ch=='\n')
			{
				tt=(text_line*)malloc(sizeof(text_line));
				if(!tt)
				{
					print_log("撤消操作失败：内存不足！\n");
					exit(0);	
				}
				tp=get_textline(sp->row);
				p=goto_textcol(tp,sp->col);
				tt->first=p->next;
				p->next=NULL;
				tt->next=tp->next;
				tt->len=tp->len-sp->col;
				tp->next=tt;
				tp->len=sp->col;
				settag();
			}
			break;
		case 'e':
			tp=get_textline(sp->row);
			tt=tp->next;
			if(sp->col)
			{
				p=goto_textcol(tp,sp->col);
				p->next=tt->first;
				tp->next=tt->next;
				tp->len=tp->len+tt->len;
				free(tt);
			}
			else
			{
				tp->first=tt->first;
				tp->len=tt->len;
				tp->next=tt->next;
				free(tt);
			}
			settag();
			break;
	}
	del_laststep();
	save_viewline();
	print_viewtext();
	work_state=0;
	if(!curstep->back)
		context_menu_state[0]=0;
}
void clear_step()
{
	while(curstep->back)
		del_laststep();
	context_menu_state[0]=0;
}
void print_stateline(int lines ,int ln,int col)
{
	char state_bar[50];
	sprintf(state_bar,"lines:%-4d   ln:%d  col:%d",lines,ln,col);
	setfillstyle(1,7);
	bar(5,462,636,476);
	draw_string(5,460,state_bar,0);
}
