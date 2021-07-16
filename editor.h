#include<stdio.h>
#define MENU_N 3				/*�˵�������*/
#define MENU_MAX 7				/*�����˵��������*/
#define WW 640					/*���ڿ���*/
#define WH 480					/*���ڸ߶�*/
#define BW 1					/*��ť�߿�Ŀ���*/
#define TH 20
#define MH 20
#define PULLMENU_W 106			/*�����˵��Ŀ���*/
#define MAX_DIALOG_H 300		/*�Ի���������*/
#define MAX_DIALOG_W 400		/*�Ի������߶�*/

#define MAX_VIEWLINE 23			/*�����ʾ�ı�������*/
#define MAX_VIEWCOL 68			/*�����ʾ�ı�������*/

#define CUR_ARROW 0
#define CUR_TEXT 1

#define MAX_PATH 256			/*��ļ�·��*/
#define MAX_STR 1024
#define MAX_DIR 256
#define MAX_FILE 256
#define MAX_F_NAME 32			/*��ļ���*/
#define TRUE 1
#define FALSE 0
#define NO 0
#define YES 1
#define NONE -1
/*����ɨ����*/
#define K_ESC 0x011b
#define K_UP 0x4800
#define K_DOWN 0x5000
#define K_LEFT 0x4b00
#define K_RIGHT 0x4d00
#define K_ENTER 0x1c0d
#define K_BACKSPACE 0x0e08
#define K_DEL 0x5300

#define ALT_F 0x2100
#define ALT_E 0x1200
#define ALT_V 0x2f00
#define ALT_S 0x1f00
#define ALT_T 0x1400

#define CTRL_A 0x1e01
#define CTRL_X 0x2d18
#define CTRL_C 0x2e03
#define CTRL_V 0x2f16
#define CTRL_Z 0x2c1a

#define CH_CR 0x0d
#define CH_LF 0x0a

struct button
{
      int x1;
      int y1;
      int x2;
      int y2;
      char s[10];
      char flag;
      int bw;
};
struct img_btn
{
    int x1;
    int y1;
    int x2;
    int y2;
    int img[32];
};
typedef struct textcell/*���浥���ַ��Ľڵ�*/
{
    char ch;
    struct textcell *next;
} text_cell;
typedef struct textline/*�����ַ������ļ�*/
{
    int len;
    int tag;
    text_cell *first;
    struct textline *next;
} text_line;
typedef struct edit_step/*���沽����Ϣ*/
{
	char type;/*type:'i'����,'d'ɾ��,'e'����*/
	int row;
	int col;
	char ch;
	struct edit_step *back;
}step;

typedef struct SELECT_TEXT/*���汻ѡ����ı���Ϣ*/
{
	int state;
	int start_row;
	int end_row;
	int start_col;
	int end_col;
} selected_text;

typedef struct CP/*������*/
{
	int state;
	text_line *head;
} clipboard;

/*draw_string.c ���ͼ�λ��ֺ���*/

void init_graphics();
void close_graphics();
void open_zk();/*���ֿ��ļ�*/
void draw_hz(int x0,int y0,char code[],int color);/*��ӡһ������*/
void draw_ch(int x,int y,unsigned char ch,int color);
void outascxy(int x,int y,unsigned char *s,int color);
void draw_string(int x,int y,char *s,int color);
void draw_text(int x,int y,char *s,int w,int color);
void close_zk();/*�ر��ֿ��ļ�*/

/*menu.c */
void draw_window();
void draw_title_bar(int color,char *s);
int menu_process(int sta);
void exec_menu_item(int menu,int item);
void draw_btnbord(struct button *bt,int p,int bw);
void pullmenu_down(int n);
/*mouse.c �������������*/
void mouse_on(void); /* ��ʾ�������� */
void mouse_off(void);/* �ر��������� */
void mouse_toxy(unsigned int x, unsigned int y);   /*�����λ������*/
void mouse_read();  /*��ȡ���״̬*/
void view_arrow_surs();  /*��ʾ��ͷ���*/
void view_text_surs();   /*��ʾ�ı����*/
int mouse_in(int x1,int y1,int x2,int y2);
int mouse_move(int x,int y);/*�ж�����Ƿ��ƶ�*/
int set_textarea(int x1,int y1,int x2,int y2);/*�����ı�����*/
void mouse_cursetting();/*������Զ��л�*/
int mouse_on_btn(struct button *bt);
int double_click();/*�ж�����Ƿ�˫��*/


/*file.c ����ļ���������*/

void init_file();
void close_file();
char *read_line(FILE *fp,char *s);/*���ļ��ж�ȡһ������*/
char *link_str(int n,...);/*����ַ������Ӻ������뱣֤�׸��ַ����ĳ��ȳ���*/
void print_log(char *text);/*����־�ļ���ӡ��־*/
char *open_dir(char *dir);
void get_childlist();
void draw_max_dialog(int i);
void save_text(char *filename);
void save_curfile();/*�洢��ǰ�������ļ�*/
void free_list(text_line *head);
struct tm *get_curtime();
char *str_insert(char *s,int n,char ch);
char *str_del(char *s,int n);
/*cursor.c ����ƶ���غ���*/
void cursor_init(int x,int y,int w,int h,int row,int col);
void cursor_blink();
void cur_moveto(int row,int col);
void cur_up();
void cur_down();
void cur_left();
void cur_right();
void cur_movetoxy(int x,int y,int *row,int *col);
void mdelay(int time);/*�������̻����ͻ��жϵ���ʱ����*/
/*work.c*/
void print_text(text_line *head);
void first_print();
void work_process();/*��Ҫ��������*/
void init_workstate();
void add_step(char type,int row ,int col,char ch);/*���Ӳ������ڳ�������*/
void del_laststep();
void goto_backstep();/*�����������ϲ�����*/
void clear_step();
void print_stateline(int lines ,int ln,int col);/*��ӡ״̬��Ϣ*/
void copy_text();
void paste_text(int line,int col);
void cut_text(text_line *cb_head);
void goto_line();
void select_all();
/*winbase.c ��Ŵ��ڻ��ƻ�������*/
void hlight_btn(struct button *t);/*������ť*/
int prompt_dialog(int h,char *title,char *text,int bt_n,char *btn[]);/*����һ����ʾ�Ի���,����ѡ����*/
void draw_scrollbar(int x1,int y1,int x2,int y2);/*���ƴ��ڹ�����*/
void draw_workplace(int x1,int y1,int x2,int y2);/*����һ���������������*/
void draw_win(int x1,int y1,int x2,int y2);/*��һ����������*/
void draw_img(int x,int y,int *img,int c1,int c2);/*����img*/
void winp();/*��ť����״̬�л�*/
void draw_btn(int x1,int y1,int x2,int y2,int c,int bw);/*��һ����ť*/
void draw_btnbord(struct button *bt,int p,int bw);/*����ť�ı߿��ò�ͬ�߿�ģ�ⰴť�İ��º͵���״̬*/
char *input_dialog(int h,char *title,char *text,int bt_n,char *btn[]);/*����һ������Ի���,�������������*/
void draw_dialog(int x,int y,int w,int h,char *title,char *text,int bt_n,char *btn[]);
int pull_context_menu(int item_n,char *menu[],int x,int y);/*�����Ҽ��˵��������в˵�ѡ�񣬷�����ѡ����Ľ���*/
void save_bigimg(int x1,int x2,int y1,int y2);/*����һ����Ļ����*/
void put_bigimg(int x1,int x2,int y1,int y2);/*�����������ԭ*/
void init_winbase();
void hlight_bar(int x1,int y1,int x2,int y2,int c);/*����һ�����ο�*/
/*void alloc_space();*/