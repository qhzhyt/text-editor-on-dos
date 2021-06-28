#include<stdio.h>
#include<graphics.h>
#include<string.h>
#include"test.h"
FILE *hzk_p,*asc_p;
void open_zk()/*打开字库文件*/
{
  if(!(hzk_p=fopen("font\\hzk16","rb")))/*汉字16×16字库*/
  {
   printf("HZK16 file not exist! Enter to system\n");
   getch();
   closegraph();
   exit(1);
  }
  if(!(asc_p=fopen("font\\asc16","rb")))/*英文16×8字库*/
  { 
   printf("ASC16 file not exist! Enter to system\n");
   getch();
   closegraph();
   exit(1);
  }
}
void get_hz(char incode[],char bytes[])/*从字库获取点阵信息*/
{
    unsigned char qh,wh;
    unsigned long offset;
    qh=incode[0]-0xa0;/*得到区号*/
    wh=incode[1]-0xa0;/*得到位号*/
    offset=(94*(qh-1)+(wh-1))*32L; /*得到偏移位置*/
    fseek(hzk_p,offset,SEEK_SET); /*移文件指针到要读取的汉字字模处*/
    fread(bytes,32,1,hzk_p);/*读取 32 个汉字的汉字字模*/
}
void draw_hz(int x0,int y0,char code[],int color)
{
    unsigned char mask[]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};/*屏蔽字模每行各位的数组*/
    int i,j,x,y,pos;
    char mat[32];
    get_hz(code,mat);
    y=y0;
    for (i=0;i<16;++i)
    {
        x=x0;
        pos=2*i;
        for(j=0;j<16;++j)
        {
        /*屏蔽出汉字字模的一个位,即一个点是 1 则显示,否则不显示该点*/
            if ((mask[j%8]&mat[pos+j/8])!=NULL)
            putpixel(x,y,color);
            ++x;
        }
        ++y;
    }
}
void draw_ch(int x,int y,unsigned char ch,int color) 
{
    char buf[16];
    unsigned char i,j,e;
    fseek(asc_p,ch*16l,0);
    fread(buf,16,1,asc_p);
    for(i=0;i<16;i++)
    {
        e=buf[i];
        for(j=0;j<8;j++)
        {
            if(e&0x80)
            putpixel(j+x,i+y,color);
            e<<=1;
        }
    }
}
void outascxy(int x,int y,unsigned char *s,int color)
{
    char buf[16];
    int m;
    for(m=0;s[m];m++)
    {
        draw_ch(x,y,s[m],color);
        x+=8;
    }
}
void draw_string(int x,int y,char *s,int color)
{
    int len=strlen(s),i=0;
    mouse_off();
    settextjustify(0,2);
    settextstyle(1,0,1);
    while(i<len)
    {
        if(s[i]<0)/*判断是s[i]是否为ascii字符*/
        {
            draw_hz(x,y,&s[i],color);
            i+=2;
            x+=17;
        }
        else
        {
            draw_ch(x,y+2,s[i],color);
            i+=1;
            x+=9;
        }
    }
    mouse_on();
}
void draw_text(int x,int y,char *s,int w,int color)
{
    int len=strlen(s),i=0;
    int x0=x;
    mouse_off();
    settextjustify(0,2);
    settextstyle(1,0,1);
    while(i<len)
    {
        if(s[i]<0)/*判断是s[i]是否为ascii字符*/
        {
            draw_hz(x,y,&s[i],color);
            i+=2;
            x+=17;
        }
        else
        {
            draw_ch(x,y+2,s[i],color);
            i+=1;
            x+=9;
        }
        if(!(i%w)||!(i%(w-1)))
        {
            x=x0;
            y+=18;
        } 
    }
    mouse_on();
}
void close_zk()
{
    fclose(hzk_p);
    fclose(asc_p);
}
