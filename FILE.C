#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<dir.h>
#include<io.h>
#include<dos.h>
#include<time.h>
#include<graphics.h>
#include<string.h>
#include<alloc.h>
#include"test.h"

char cur_filename[MAX_F_NAME*2];/*当前文件名*/
char current_dir[MAX_PATH];/*当前目录名*/
char work_dir[MAX_PATH];/*程序所在目录路径*/
FILE *filename_cache,*dirname_cache,*cur_file;
char fn_cache_path[MAX_PATH],dn_cache_path[MAX_PATH];
char logfile_path[MAX_PATH];/*日志文件路径*/
char curdir_list[MAX_DIR][MAX_F_NAME];
char curfile_list[MAX_FILE][MAX_F_NAME];
int dir_n;
int file_n;

text_line *head;/*主链表头*/
int line_n=0;
extern int curfile_saved;
struct button btn_parentdir,btn_ok;
struct button btn_d_up,btn_d_down;
extern int msvisible,mousex,mousey,mousekey;
extern int img_close[16];

int img_parentdir[16]=
{
  0x0000,
  0x0080,
  0x01c0,
  0x03e0,
  0x07f0,
  0x1ffc,
  0x3ffe,
  0x03e0,
  0x03e0,
  0x03e0,
  0x03e0,
  0x03e0,
  0x03e0,
  0x03e0,
  0x0000,
};



struct button btn_d_close;

char *get_workdir();/*获得程序所在目录*/
char *get_parentdir(char *dir);
void print_itemlist(int m);
void max_dialog_process(int x1,int y1,int x2,int y2);
int is_rootdir(char *dir);/*判断目录名是否为根目录*/
int open_item(int x1,int y1,int x2,int y2,int item);
FILE *open_file(char *fn);/*打开文件*/
FILE *new_file(char *filename);/*新建文件*/
text_line *create_line(text_line *line_head);
text_line *create_text(text_line *head);


void init_file()
{
	int i=0,j;
	char num[6];
    get_workdir();
    strcpy(fn_cache_path,work_dir);
    strcpy(dn_cache_path,work_dir);
    strcpy(logfile_path,work_dir);
    strcat(fn_cache_path,"\\cache\\filename");
    strcat(dn_cache_path,"\\cache\\dirname");
    strcat(logfile_path,"\\test.log");
    fclose(fopen(logfile_path,"w"));/*创建日志文件*/
    getcwd(current_dir,MAX_PATH);
    strcat(current_dir,"\\");
	strcpy(cur_filename,"noname");
	while(cur_file=fopen(cur_filename,"r"))
	{
		i++;
		fclose(cur_file);
		cur_filename[6]='\0';
		itoa(i,num,5);
		strcat(cur_filename,num);
	}
	cur_file=fopen(cur_filename,"w");
	head=(text_line*)malloc(sizeof(text_line));
    if(!head)
    {
      closegraph();
      restorecrtmode();
      printf("Not Enough Memory !\n try to reduce the area used.\n");
      exit(1);
    } 
    head->next=NULL;
	create_text(head);
	fclose(cur_file);
	cur_file=NULL;
}

void close_file()
{
   chdir(work_dir);
   remove(fn_cache_path);
   remove(dn_cache_path);
}

char *open_dir(char *dir)
{
    char t[2*MAX_PATH]="cd ";
    if(strcmp(dir,".."))
      link_str(3,current_dir,dir,"\\");
    else
      get_parentdir(current_dir);
    system(link_str(4,t,current_dir," >> ",logfile_path));
    return current_dir;
}
void get_childlist()
{
    int i=0;
    char s[MAX_F_NAME];
    char t[2*MAX_PATH]="dir ";
    system(link_str(3,t,"/ad/b > ",dn_cache_path));
    t[4]='\0';
    system(link_str(3,t,"/b > ",fn_cache_path));

    dirname_cache=fopen(dn_cache_path,"r");
    filename_cache=fopen(fn_cache_path,"r");

    while(read_line(dirname_cache,curdir_list[i++]));
    dir_n=i-1;
    i=0;
    while(read_line(filename_cache,curfile_list[i++]));
    file_n=i-dir_n-1;

    fclose(dirname_cache);
    fclose(filename_cache);
}
char *get_workdir()
{
    getcwd(work_dir,MAX_PATH);
    return work_dir;
}
char *get_parentdir(char *dir)
{
    int l=strlen(dir)-1;
    while(l--)
    {
        if(dir[l]=='\\')
        {
            dir[l+1]='\0';
            break;
        }
    }
    return dir;
}
char *link_str(int n,...)
{
   char *s;
   va_list val;
   va_start(val,n);
   s=va_arg(val,char *);
   while(--n)
      s=strcat(s,va_arg(val,char *));
   va_end(val);
   return s;
}
char *read_line(FILE *fp,char *s)
{
    char ch;
    int i=0;
    while((ch=fgetc(fp))!='\n')
    {
	if(ch==EOF)
	  return NULL;
        s[i]=ch;
        i++;
    }
    if(i==0)
        return NULL;
    s[i]='\0';
    return s;
}
void draw_max_dialog(int i)
{
    int x1=(WW-MAX_DIALOG_W)/2;
    int x2=WW-x1;
    int y1=100;
    int y2=MAX_DIALOG_H+100;
    mouse_off();
    save_bigimg(x1,x2,y1,y2);
    draw_win(x1,y1,x2,y2);
    setfillstyle(1,9);
    bar(x1+1,y1,x2,y1+20);
    draw_img(x2-20,y1+2,img_close,15,4);

    draw_string(x1+10,y1+25,"位置",0);
    draw_string(x1+45,y1+22,":",0);
    setfillstyle(1,15);
    bar(x1+60,y1+24,x1+300,y1+42);

    draw_btn(x1+320,y1+24,x1+338,y1+42,7,0);
    if(is_rootdir(current_dir))/*如果当前目录为根目录绘制上层目录按钮为不可用状态*/
        draw_img(x1+320,y1+25,img_parentdir,15,-1);
    else
        draw_img(x1+320,y1+25,img_parentdir,2,-1);

    draw_string(x1+62,y1+25,current_dir,8);
    draw_workplace(x1+6,y1+45,x2-6,y2-50);

    draw_scrollbar(x2-20,y1+47,x2-7,y2-51);

    print_itemlist(0);

    draw_string(x1+10,y2-46,"文件名",0);
    draw_string(x1+65,y2-49,":",0);
    draw_workplace(x1+80,y2-47,x1+300,y2-27);
    draw_btn(x1+325,y2-46,x1+365,y2-26,7,0);
    switch(i)
    {
        case 0:
            draw_string(WW/2-20,y1+2,"新建",0);
			draw_string(x1+327,y2-44,"确定",0);
			btn_ok.flag='n';
            break;
        case 1:
            draw_string(WW/2-20,y1+2,"打开",0);
            draw_string(x1+327,y2-44,"打开",0);
            btn_ok.flag='o';
            break;
        case 3:
            draw_string(WW/2-20,y1+2,"另存为",0);
            draw_string(x1+327,y2-44,"保存",0);
            btn_ok.flag='s';
    }
    mouse_on();
    set_textarea(x1+80,y2-47,x1+300,y2-27);
    
    if(btn_parentdir.x1!=x1+320)/*设置按钮位置*/
    {
        btn_parentdir.x1=x1+320;
        btn_parentdir.x2=x1+340;
        btn_parentdir.y1=y1+24;
        btn_parentdir.y2=y1+44;
        
        btn_ok.x1=x1+325;
        btn_ok.x2=x1+367;
        btn_ok.y1=y2-46;
        btn_ok.y2=y2-24;
        
        btn_d_up.x1=x2-21;
        btn_d_up.x2=x2-6;
        btn_d_up.y1=y1+47;
        btn_d_up.y2=y1+62;
        
        btn_d_down.x1=x2-21;
        btn_d_down.x2=x2-6;
        btn_d_down.y1=y2-64;
        btn_d_down.y2=y2-49;
        
    }
    
    max_dialog_process(x1,y1,x2,y2);
}

void max_dialog_process(int x1,int y1,int x2,int y2)
{
  int state=0;
  int bios_key;
  int x=130,y=146;
  int selected_item=-1,item_x,item_y;
  int first_line=0;
  int i;
  char s[16]={'\0'};
  int sn=0,cur_n=0;
  char t;
  char *btn_s[]={"继续","取消"};
  cursor_init(x1+83,y2-44,9,18,1,16);
  while(1)
  {
    mouse_cursetting();
    if(kbhit())
    {
      bios_key=bioskey(0);
      sn=strlen(s);
      if(state==2&&sn<15)
      {
          i=bios_key&0x00ff;
          if(i>31&&i<128)
          {
              str_insert(s,cur_n,i);
              cur_n++;
              sn++;
              setfillstyle(1,15);
              bar(x1+84,y2-45,x1+84+9*sn,y2-30);
              draw_string(x1+84,y2-46,s,0);
              cur_right();
          }
      }
      switch(bios_key)
      {
        case K_ENTER:
            if(state==1)
            {
                if(selected_item!=NONE)
                {
                    if(open_item(x1,y1,x2,y2,selected_item))
                        return;
                    else
                    {
                        selected_item=-1;
                        first_line=0;
                        state=0;
                    }
                }
			}
			if(state==2)
			{
				FILE *ft;
				int t;
				
			   draw_btnbord(&btn_ok,1,0);
			   delay(30);
			   draw_btnbord(&btn_ok,0,0); 
			   switch(btn_ok.flag)
			   {
					case 'n':
						put_bigimg(x1,x2,y1,y2);
						if(ft=new_file(s))
						{
							text_line *p=(text_line*)malloc(sizeof(text_line));
							fclose(ft);
							cur_file=open_file(s);
							free_list(head);
							create_text(head);
							fclose(cur_file);
							cur_file=NULL;
							set_textarea(2,41,620,477);
							cursor_init(4,44,9,18,48,69);
							first_print();
							return;
						}
						return;
							break;
				   
					case 'o':
					   if(selected_item!=NONE)
					   if(open_item(x1,y1,x2,y2,selected_item))
							return;
						else
						{
							selected_item=-1;
							first_line=0;
							state=0;
						}
					break;
					case 's':
						put_bigimg(x1,x2,y1,y2);
						if(ft=fopen(s,"r"))
						{
						  if(prompt_dialog(100,"警告","该文件已存在，继续操作将会覆盖原文件，是否继续？",2,btn_s)==1)
								save_text(s);
						}
						else
							save_text(s);
						
						set_textarea(2,41,620,477);
						cursor_init(4,44,9,18,48,69);
						return;
			   }
			}				
            break;
        case K_ESC:
            put_bigimg(x1,x2,y1,y2);
            return;
            break;
        case K_BACKSPACE:
            if(cur_n>0)
            {
              str_del(s,cur_n-1);
              cur_n--;
              cur_left();
              setfillstyle(1,15);
              bar(x1+84,y2-45,x1+84+9*sn,y2-30);
              draw_string(x1+84,y2-46,s,0);
              sn--;
            }
            break;
        case K_DEL:
            if(cur_n<sn)
            {
               str_del(s,cur_n);
               setfillstyle(1,15);
               bar(x1+84,y2-45,x1+84+9*sn,y2-30);
               draw_string(x1+84,y2-46,s,0);
            }
            break;
        case K_UP:
            if(state)
            {
                if(state==1&&(selected_item/3))
                {
                    if(selected_item!=NONE)
                        hlight_bar(item_x,item_y+2,item_x+116,item_y+20,14);
                    if(selected_item/3==first_line)
                    {
                        first_line--;
                        print_itemlist(first_line);
                    }
                    else
                        item_y-=20;
                    hlight_bar(item_x,item_y+2,item_x+116,item_y+20,14);
                    selected_item-=3;
                }
           }
            break;
        case K_DOWN:
            if(state)
            {
                if(state==1&&selected_item+3<file_n+dir_n)
                {
                    if(selected_item!=NONE)
                        hlight_bar(item_x,item_y+2,item_x+116,item_y+20,14);
                    if(selected_item/3==first_line+9)
                    {
                        first_line++;
                        print_itemlist(first_line);
                    }
                    else
                      item_y+=20;
                    hlight_bar(item_x,item_y+2,item_x+116,item_y+20,14);
                    selected_item+=3;
                }
            }
            break;
        case K_LEFT:
            if(state)
            {
                if(state==1&&selected_item)
                {
                    if(selected_item!=NONE)
                        hlight_bar(item_x,item_y+2,item_x+116,item_y+20,14);
                    if(selected_item%3==0)
                    {
                        item_x+=250;
                        if(selected_item/3==first_line)
                        {
                          first_line--;
                          print_itemlist(first_line);
                        }
                        else
                        item_y-=20;
                    }
                    else
                    item_x-=125;
                    hlight_bar(item_x,item_y+2,item_x+116,item_y+20,14);
                    selected_item--;
                }
                if(state==2&&cur_n>0)
                {
                    cur_left();
                    cur_n--;
                }
            }
            break;
        case K_RIGHT:
            if(state)
            {
                if(state==1&&selected_item<file_n+dir_n-1)
                {
                    if(selected_item!=NONE)
                        hlight_bar(item_x,item_y+2,item_x+116,item_y+20,14);
                    if(selected_item%3==2)
                    {
                        item_x-=250;
                        if(selected_item/3==first_line+9)
                        {
                          first_line++;
                          print_itemlist(first_line);
                          delay(30);
                        }
                        else
                          item_y+=20;
                    }
                    else
                    item_x+=125;
                    hlight_bar(item_x,item_y+2,item_x+116,item_y+20,14);
                    selected_item++;
                }
            }
            if(state==2&&cur_n<sn)
                {
                    cur_right();
                    cur_n++;
                }
            break;
      }
      }
   x=130;
   y=146;
   if(state==2)
   {
      cursor_blink();
   }
   mouse_read();
   if(mousekey==1)
   {
    if(mouse_in(x2-20,y1+2,x2-4,y1+18))/**/
    {
        hlight_bar(x2-20,y1+2,x2-4,y1+18,7);
        put_bigimg(x1,x2,y1,y2);
        return;
    }
    if(mouse_on_btn(&btn_parentdir)&&!is_rootdir(current_dir))
    {
        draw_btnbord(&btn_parentdir,1,0);
        delay(30);
        draw_btnbord(&btn_parentdir,0,0);
        open_dir("..");/*返回上层目录*/
        get_childlist();
        setfillstyle(1,15);
        bar(x1+60,y1+24,x1+300,y1+42);
        draw_string(x1+62,y1+25,current_dir,8);
        first_line=0;
        state=0;
        print_itemlist(0);
        selected_item=-1;
        if(is_rootdir(current_dir))
            draw_img(x1+320,y1+25,img_parentdir,15,-1);
    }
    if(mouse_on_btn(&btn_d_up)&&first_line)
    {
        if(selected_item!=NONE)
        {
            hlight_bar(item_x,item_y+2,item_x+116,item_y+20,14);
            if(selected_item/3==first_line+9)
              selected_item-=3;
            else
              item_y+=20;
        }
        draw_btnbord(&btn_d_up,1,1);
        first_line--;
        print_itemlist(first_line);
        delay(30);
        draw_btnbord(&btn_d_up,0,1);
        if(selected_item!=NONE)
           hlight_bar(item_x,item_y+2,item_x+116,item_y+20,14);
    }
    if(mouse_on_btn(&btn_ok))
    {
		FILE *ft;
       draw_btnbord(&btn_ok,1,0);
       delay(30);
       draw_btnbord(&btn_ok,0,0); 
       switch(btn_ok.flag)
       {
		   case 'n':
				put_bigimg(x1,x2,y1,y2);
				if(ft=new_file(s))
				{
					text_line *p=(text_line*)malloc(sizeof(text_line));
					fclose(ft);
					cur_file=open_file(s);
					free_list(head);
					create_text(head);
					fclose(cur_file);
					cur_file=NULL;
					set_textarea(2,41,620,477);
					cursor_init(4,44,9,18,48,69);
					first_print();
					return;
				}
				return;
					break;
		   
           case 'o':
               if(selected_item!=NONE)
               if(open_item(x1,y1,x2,y2,selected_item))
                    return;
                else
                {
                    selected_item=-1;
                    first_line=0;
                    state=0;
                }
            break;
			case 's':
				put_bigimg(x1,x2,y1,y2);
				if(ft=fopen(s,"r"))
				{
					if(prompt_dialog(100,"警告","该文件已存在，继续操作将会覆盖原文件，是否继续？",2,btn_s)==1)
						save_text(s);
				}
				else
					save_text(s);
				set_textarea(2,41,620,477);
				cursor_init(4,44,9,18,48,69);	
				return;
       }
    }
    if(mouse_on_btn(&btn_d_down)&&((file_n+dir_n)/3>first_line+9))
    {
        if(selected_item!=NONE)
        {
            hlight_bar(item_x,item_y+2,item_x+116,item_y+20,14);
            if(selected_item/3==first_line)
            selected_item+=3;
            else
              item_y-=20;
        }
        draw_btnbord(&btn_d_down,1,1);
        first_line++;
        print_itemlist(first_line);
        delay(30);
        draw_btnbord(&btn_d_down,0,1);
        if(selected_item!=NONE)
            hlight_bar(item_x,item_y+2,item_x+116,item_y+20,14);
    }
    if(mouse_in(x1+80,y2-47,x1+300,y2-27))
    {
        if(state!=2)
        {
            state=2; 
        }
            if(mousex>x1+84+9*sn)
            {
                cur_moveto(0,sn);
                cur_n=sn;
            }
            else
            {
                cur_movetoxy(mousex,mousey,&i,&cur_n);
            }
    }
    for(i=first_line*3;i<first_line*3+3*10&&i<file_n+dir_n;i++)
    {
        if(mouse_in(x,y,x+120,y+19))
        {
            if(i!=selected_item)
            {
              hlight_bar(x,y+2,x+116,y+20,14);
              if(selected_item!=NONE)
              hlight_bar(item_x,item_y+2,item_x+116,item_y+20,14);
              item_x=x;
              item_y=y;
              selected_item=i;
              state=1;
            }
            if(double_click())
            {
                if(open_item(x1,y1,x2,y2,selected_item))
                    return;
                else
                {
                    selected_item=-1;
                    first_line=0;
                    state=0;
                }
            }
            strcpy(s,curfile_list[i]);
            setfillstyle(1,15);
            bar(x1+84,y2-45,x1+84+9*16,y2-30);
            draw_string(x1+84,y2-46,s,0);
            sn=strlen(s);
        }
        x+=125;
        if(i%3==2)
        {
            x=130;
            y+=20;
        }
    }
   }
   }
}
FILE *new_file(char *filename)
{
	FILE *fp=fopen(filename,"r");
	char *bt_str[]={"确定"};
	char s[50];
	
	if(fp)
	{
		sprintf(s,"文件%s已存在，请重新输入文件名！",filename);
		prompt_dialog(100,"新建文件失败！",s,1,bt_str);
		return NULL;
	}
	fp=fopen(filename,"w+");
	if(!fp)
	{
		sprintf(s,"内存不足，无法创建%s，请清理内存后重试！",filename);
		prompt_dialog(100,"新建文件失败！",s,1,bt_str);
		return NULL;
	}
	return fp;
}
int open_item(int x1,int y1,int x2,int y2,int item)
{
    if(item<dir_n)
    {
        open_dir(curdir_list[item]);
        get_childlist();
        setfillstyle(1,15);
        bar(x1+60,y1+24,x1+300,y1+42);
        draw_string(x1+62,y1+25,current_dir,8);
        print_itemlist(0);
        draw_img(x1+320,y1+25,img_parentdir,2,-1);
        print_itemlist(0);
        return 0;
    }
    else
    {
		if(btn_ok.flag=='o')
		{
			put_bigimg(x1,x2,y1,y2);
			if(head->next)/* 如果链表曾被创建，则释放链表 */
			{
				free_list(head);
			}	
			cur_file=open_file(curfile_list[item]);
			print_log("正在创建链表。。。\n");
			create_text(head);
			print_log("创建链表成功！");
			fclose(cur_file);
			set_textarea(2,41,620,477);
			cursor_init(4,44,9,18,48,69);
			first_print();
			return 1;
		}
        return 2;
    }
    
}
void print_itemlist(int m)
{
    int x=130,y=146;
    int n=0;
    int i=m*3;
    mouse_off();
    setfillstyle(1,15);
    bar(x,y+2,x+366,y+200);
    for(;i<m*3+3*10&&i<file_n+dir_n;i++,n++)
    {
        if(i<dir_n)
            draw_string(x,y+2,curfile_list[i],2);
        else
          draw_string(x,y+2,curfile_list[i],12);
        x+=125;
        if(n%3==2)
        {
            x=130;
            y+=20;
        }
    }
      mouse_on();
}
FILE *open_file(char *fn)
{
    FILE *fp;
    if((fp=fopen(fn,"r"))==NULL)
    {
        print_log("打开文件失败！\n");
        exit(1);
    }
	draw_title_bar(0,fn);
	strcpy(cur_filename,fn);
    return fp;
}
int is_rootdir(char *dir)
{
    if(strlen(dir)<4&&strlen(dir)>1)
        return 1;
    return 0;
}
struct tm *get_curtime()
{
	struct tm *t_m;
	char cur_time[30];
	time_t timep;
	time (&timep);
    t_m=localtime(&timep);
	return t_m;
}
void print_log(char *text)/*向日志文件打印系统日志*/
{
    struct tm *t_m=get_curtime();/*获取系统当前时间*/
    FILE *fp=fopen(logfile_path,"a");
    fprintf(fp,"[%d-%d-%d %d:%d:%d]",t_m->tm_year+1900,t_m->tm_mon+1,t_m->tm_mday,t_m->tm_hour,t_m->tm_min,t_m->tm_sec);
    fprintf(fp,"%s",text);
    fclose(fp);
}
char *str_insert(char *s,int n,char ch)
{
    int i;
    int l=strlen(s);
    for(i=l;i>n-1;i--)
        s[i+1]=s[i];
    s[n]=ch;
    return s;
}
char *str_del(char *s,int n)
{
    int i;
    int l=strlen(s);
    for(i=n;i<l;i++)
        s[i]=s[i+1];
    return s;
}
int item_isexist(char *name)
{
    int i;
    for(i=dir_n;i<dir_n+file_n;i++)
        if(!strcmp(name,curfile_list[i]))
            return 1;
    for(i=0;i<dir_n;i++)
        if(!strcmp(name,curfile_list[i]))
            return 2;
    return 0;
}
text_line *create_line(text_line *line_head)
{
    text_cell *p1,*p2;
    int n=0;
    char ch;
    p1=(text_cell*)malloc(sizeof(text_cell));
    if((ch=fgetc(cur_file))==EOF)
        return NULL;
    if(ch=='\n')
    {
        line_head->first=NULL;
        line_head->len=n;
        return line_head;
    }
    p1->ch=ch;
    line_head->first=p1;
    n++;
    while((ch=fgetc(cur_file))!=EOF)
    {
        if(ch=='\n')
        {
            p1->next=NULL;
            line_head->len=n;
            return line_head;
        }
        p2=(text_cell*)malloc(sizeof(text_cell));
        if(!(p2))
        {
          closegraph();
          restorecrtmode();
          printf("Not Enough Memory !\n try to reduce the area used.\n");
          getch();
          exit(1);
        } 
        p2->ch=ch;
        p1->next=p2;
        n++;
        p1=p2;
    }
    p1->next=NULL;
    line_head->len=n;
    return line_head;
}

text_line *create_text(text_line *head)
{
    text_line *p1,*p2;
    int n=0;
    if(feof(cur_file))
	{
		p1=(text_line*)malloc(sizeof(text_line));
		p1->len=0;
		p1->first=NULL;
		p1->next=NULL;
		p1->tag=1;
		head->next=p1;
		head->len=1;
		return head;
	}
    p1=(text_line*)malloc(sizeof(text_line));
    create_line(p1);
    head->next=p1;
    while(!feof(cur_file))
    {
        p2=(text_line*)malloc(sizeof(text_line));
        if(create_line(p2)==NULL)
		{
			p1->tag=n+1;
			p1->next=NULL;
			head->len=n+1;
			return head;
		}
		n++;
		p1->tag=n;
        p1->next=p2;
        p1=p2;
    }
	p1->tag=n+1;
    p1->next=NULL;
	head->len=n+1;
    return head;
}

void save_text(char *filename)
{
    FILE *fp=fopen(filename,"w");
    text_line *pl;
    text_cell *pc;
    pl=head->next;
    while(pl)
    {
        pc=pl->first;
        while(pc)
        {
            fputc(pc->ch,fp);
            pc=pc->next;
        }
		fputc('\n',fp);
        pl=pl->next;
    }
    fclose(fp);
    curfile_saved=1;
}
void save_curfile()
{
	save_text(cur_filename);
	
	
	
}
void free_list(text_line *head)
{
    text_line *p1=head->next,*p2;
    text_cell *p3,*p4;
    while(p1)
    {
        p3=p1->first;
        while(p3)
        {
            p4=p3;
            p3=p3->next;
            free(p4);
        }
        p2=p1;
        p1=p1->next;
        free(p2);
    }
    head->next=NULL;
}
