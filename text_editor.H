#include<stdio.h>
#define MENU_N 3				/*菜单栏项数*/
#define MENU_MAX 7				/*下拉菜单最多项数*/
#define WW 640					/*窗口宽度*/
#define WH 480					/*窗口高度*/
#define BW 1					/*按钮边框的宽度*/
#define TH 20
#define MH 20
#define PULLMENU_W 106			/*下拉菜单的宽度*/
#define MAX_DIALOG_H 300		/*对话框最大宽度*/
#define MAX_DIALOG_W 400		/*对话框最大高度*/

#define MAX_VIEWLINE 23			/*最多显示文本的行数*/
#define MAX_VIEWCOL 68			/*最多显示文本的列数*/

#define CUR_ARROW 0
#define CUR_TEXT 1

#define MAX_PATH 256			/*最长文件路径*/
#define MAX_STR 1024
#define MAX_DIR 256
#define MAX_FILE 256
#define MAX_F_NAME 32			/*最长文件名*/
#define TRUE 1
#define FALSE 0
#define NO 0
#define YES 1
#define NONE -1
/*键盘扫描码*/
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
typedef struct textcell/*保存单个字符的节点*/
{
    char ch;
    struct textcell *next;
} text_cell;
typedef struct textline/*保存字符的行文件*/
{
    int len;
    int tag;
    text_cell *first;
    struct textline *next;
} text_line;
typedef struct edit_step/*保存步骤信息*/
{
	char type;/*type:'i'插入,'d'删除,'e'换行*/
	int row;
	int col;
	char ch;
	struct edit_step *back;
}step;

typedef struct SELECT_TEXT/*保存被选择的文本信息*/
{
	int state;
	int start_row;
	int end_row;
	int start_col;
	int end_col;
} selected_text;

typedef struct CP/*剪贴板*/
{
	int state;
	text_line *head;
} clipboard;

/*draw_string.c 存放图形绘字函数*/

void init_graphics();
void close_graphics();
void open_zk();/*打开字库文件*/
void draw_hz(int x0,int y0,char code[],int color);/*打印一个汉字*/
void draw_ch(int x,int y,unsigned char ch,int color);
void outascxy(int x,int y,unsigned char *s,int color);
void draw_string(int x,int y,char *s,int color);
void draw_text(int x,int y,char *s,int w,int color);
void close_zk();/*关闭字库文件*/

/*menu.c */
void draw_window();
void draw_title_bar(int color,char *s);
int menu_process(int sta);
void exec_menu_item(int menu,int item);
void draw_btnbord(struct button *bt,int p,int bw);
void pullmenu_down(int n);
/*mouse.c 存放鼠标操作函数*/
void mouse_on(void); /* 显示鼠标器光标 */
void mouse_off(void);/* 关闭鼠标器光标 */
void mouse_toxy(unsigned int x, unsigned int y);   /*鼠标光标位置设置*/
void mouse_read();  /*读取鼠标状态*/
void view_arrow_surs();  /*显示箭头光标*/
void view_text_surs();   /*显示文本光标*/
int mouse_in(int x1,int y1,int x2,int y2);
int mouse_move(int x,int y);/*判断鼠标是否移动*/
int set_textarea(int x1,int y1,int x2,int y2);/*设置文本区域*/
void mouse_cursetting();/*鼠标光标自动切换*/
int mouse_on_btn(struct button *bt);
int double_click();/*判断鼠标是否双击*/


/*file.c 存放文件操作函数*/

void init_file();
void close_file();
char *read_line(FILE *fp,char *s);/*从文件中读取一行数据*/
char *link_str(int n,...);/*多个字符串连接函数，须保证首个字符串的长度充足*/
void print_log(char *text);/*向日志文件打印日志*/
char *open_dir(char *dir);
void get_childlist();
void draw_max_dialog(int i);
void save_text(char *filename);
void save_curfile();/*存储当前操作的文件*/
void free_list(text_line *head);
struct tm *get_curtime();
char *str_insert(char *s,int n,char ch);
char *str_del(char *s,int n);
/*cursor.c 光标移动相关函数*/
void cursor_init(int x,int y,int w,int h,int row,int col);
void cursor_blink();
void cur_moveto(int row,int col);
void cur_up();
void cur_down();
void cur_left();
void cur_right();
void cur_movetoxy(int x,int y,int *row,int *col);
void mdelay(int time);/*遇到键盘或鼠标就会中断的延时函数*/
/*work.c*/
void print_text(text_line *head);
void first_print();
void work_process();/*主要工作进程*/
void init_workstate();
void add_step(char type,int row ,int col,char ch);/*添加步骤用于撤消操作*/
void del_laststep();
void goto_backstep();/*撤消：返回上步操作*/
void clear_step();
void print_stateline(int lines ,int ln,int col);/*打印状态信息*/
void copy_text();
void paste_text(int line,int col);
void cut_text(text_line *cb_head);
void goto_line();
void select_all();
/*winbase.c 存放窗口绘制基本函数*/
void hlight_btn(struct button *t);/*高亮按钮*/
int prompt_dialog(int h,char *title,char *text,int bt_n,char *btn[]);/*绘制一个提示对话框,返回选择结果*/
void draw_scrollbar(int x1,int y1,int x2,int y2);/*绘制窗口滚动条*/
void draw_workplace(int x1,int y1,int x2,int y2);/*绘制一个工作区或输入框*/
void draw_win(int x1,int y1,int x2,int y2);/*画一个矩形区域*/
void draw_img(int x,int y,int *img,int c1,int c2);/*绘制img*/
void winp();/*按钮高亮状态切换*/
void draw_btn(int x1,int y1,int x2,int y2,int c,int bw);/*画一个按钮*/
void draw_btnbord(struct button *bt,int p,int bw);/*画按钮的边框，用不同边框模拟按钮的按下和弹起状态*/
char *input_dialog(int h,char *title,char *text,int bt_n,char *btn[]);/*弹出一个输入对话框,返回输入的内容*/
void draw_dialog(int x,int y,int w,int h,char *title,char *text,int bt_n,char *btn[]);
int pull_context_menu(int item_n,char *menu[],int x,int y);/*弹出右键菜单，并进行菜单选择，返回所选的羡慕编号*/
void save_bigimg(int x1,int x2,int y1,int y2);/*保存一块屏幕区域*/
void put_bigimg(int x1,int x2,int y1,int y2);/*将保存的区域还原*/
void init_winbase();
void hlight_bar(int x1,int y1,int x2,int y2,int c);/*高亮一个矩形框*/