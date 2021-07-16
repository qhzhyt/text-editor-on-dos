#include <stdio.h>
#include <graphics.h>
#include "editor.h"

main()
{
	init_file();
	init_workstate();
	init_graphics();
	init_winbase();
	draw_window();
	get_childlist();
	mouse_on();
	work_process();
	getch();
	close_graphics();
}

void init_graphics()
{
    int driver=DETECT;
    int mode=0;
    print_log("正在初始化图形环境\n");
    registerbgidriver(EGAVGA_driver);
    initgraph(&driver,&mode,"");
    open_zk();
    print_log("初始化图形环境成功\n");
}

void close_graphics()
{
    mouse_off();
    closegraph();
    close_file();
    close_zk();
}