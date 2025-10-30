#ifndef __TRAY_HEADER_20251025234600__
#define __TRAY_HEADER_20251025234600__
#include<stdlib.h>
#include<stdio.h>

#include"./string_list/string_list.h"
#include"./cJSON/cJSON.h"

#ifdef __linux__
#include<libayatana-appindicator/app-indicator.h>
#endif

#define __DEFAULT_TRAY_ICON_NAME__ "computer"

/** 菜单应用对象 */
struct tray {
    char *trayName; 
    char *iconName; // 图标文件路径（已转换的绝对路径 或 图标名称）
    char *iconOriginalName; // 图标的原始数据 
    int systemTag; // 运行的操作系统类型，-1 陌生的操作系统 | 1 linux | 2 windows 32位 | 3 windows 64位 | 4 其他类unix
    int runState; // 托盘应用的运行状态： -1 尚未初始化|0 已初始化但是还没有启动|1 已经启动

    #ifdef __linux__
    AppIndicator *appIndicator; 
    GtkWidget *menu;
    #endif 
    
    void (*onMenuItemActiveCallback)(void *data);

};

/** 根据配置字符串数据，创建托盘应用对象 */
struct tray *tray_new_by_config_json_string(char *config_json_string, void (*OnMenuItemActiveCallback)(void *data), int *argc, char ***argv);

/** 递归地解析菜单JSON数据，创建GtkMenu菜单对象（LINUX GTK环境） */
static int _tray_gtk_menu_new_by_config_json(struct tray *tray, cJSON *menu_item_list_json, GtkWidget *menu_widget, void (*on_menu_item_active_call)(GtkWidget *widget, void *user_data)); 
/** GtkMenu 菜单项激活的回调函数（LINUX GTK环境） */
static void _tray_menu_item_active_callback(GtkWidget *widget, void *user_data);

/** 托盘应用启动，这会导致进程进入主循环，也就是阻塞了 */
void tray_start_main_loop(struct tray *tray);

/** 托盘应用退出 */
void tray_quit(struct tray *tray);

/** 打印托盘应用信息（用于调试） */
void tray_print_info(struct tray*tray); 

/** 获取当前系统的类型 -1 陌生的操作系统 | 1 linux | 2 windows 32位 | 3 windows 64位 | 4 其他类unix */
static int _tray_get_system_tag();

#endif