#include "tray.h"

/** 解析图标路径 */
static char *_parse_icon_path(char *icon_path){
    // 检查图标路径
    char *icon_name = icon_path;
    if (icon_name == NULL)
    {
        icon_name = __DEFAULT_TRAY_ICON_NAME__;
        printf("[%s][Line: %d] 图标路径参数为空 (icon file path is null) ，切换为默认图标%s (default icon) \n", __FILE__, __LINE__, __DEFAULT_TRAY_ICON_NAME__);
    }
    else
    {
        // 判断提供的路径下，是否可以查到图标文件。如果没有查到，则继续回退至默认图标
        GFile *icon_file = g_file_new_for_path(icon_name);
        gboolean file_exists = g_file_query_exists(icon_file, NULL);
        if (!file_exists)
        {
            printf("[%s][Line: %d] 异常：无法加载图标文件：%s，切换为默认图标%s\n", __FILE__, __LINE__, icon_name, __DEFAULT_TRAY_ICON_NAME__);
            icon_name = __DEFAULT_TRAY_ICON_NAME__;
        }
        else
        {
            char *icon_file_abs_path = g_file_get_path(icon_file); // 绝对路径
            // printf("[tray_new] 解析图标绝对路径：%s\n", icon_file_abs_path);
            icon_name = icon_file_abs_path;
        }
    }
    return icon_name; 
}

struct tray *tray_new_by_config_json_string(char *config_json_string, void (*OnMenuItemActiveCallback)(void *data), int *argc, char ***argv){
    int create_tray_work_state = -1; // 当前创建托盘的工作状态 -1: 尚未成功 | 1 成功
    if(config_json_string == NULL || OnMenuItemActiveCallback == NULL){
        printf("[%s][Line: %d] 函数的参数为NULL\n", __FILE__, __LINE__);
        goto label_end_clear; 
    }
    struct tray * tray = (struct tray *)malloc(sizeof(struct tray));
    if(tray == NULL){
        printf("[%s][Line: %d] 创建结构体失败-struct tray new failed\n", __FILE__, __LINE__);
        goto label_end_clear; 
    }
    tray->runState = -1; // 运行状态为尚未初始化 
    gboolean check_init = gtk_init_check(argc, argv);
    if(check_init == FALSE){
        // 初始化GTK失败
        printf("[%s][Line: %d] 初始化GTK失败-gtk_init_check failead\n", __FILE__, __LINE__);
        goto label_end_clear; 
    }
    cJSON *json = cJSON_Parse(config_json_string);
    if(json == NULL){
        printf("[%s][Line: %d] cJSON_Parse failed\n", __FILE__, __LINE__);
        goto label_end_clear; 
    }
    // 解析托盘名称
    cJSON *name_json = cJSON_GetObjectItem(json, "name");
    if(name_json == NULL){
        printf("[%s][Line: %d] cJSON_GetObjectItem failed. Item: name\n", __FILE__, __LINE__);
        goto label_end_clear; 
    }
    if(cJSON_IsString(name_json)){
        char *tray_name =  cJSON_GetStringValue(name_json); 
        size_t tray_name_str_len = strlen(tray_name); 
        tray->trayName = (char *)malloc(tray_name_str_len + 1);
        strcpy(tray->trayName, tray_name); 
        tray->trayName[tray_name_str_len] = '\0'; 
    }else{
        printf("[%s][Line: %d] 该属性不是字符串 Item: name\n", __FILE__, __LINE__);
        goto label_end_clear; 
    }
    // 解析图标路径
    cJSON *icon_path_json = cJSON_GetObjectItem(json, "icon_path");
    if(icon_path_json == NULL){
        printf("[%s][Line: %d] cJSON_GetObjectItem failed. Item: icon_path_json\n", __FILE__, __LINE__);
        goto label_end_clear; 
    }
    if(cJSON_IsString(icon_path_json)){
        char *icon_path =  cJSON_GetStringValue(icon_path_json); 
        size_t icon_path_str_len = strlen(icon_path); 
        tray->iconOriginalName = (char *)malloc(icon_path_str_len + 1);
        strcpy(tray->iconOriginalName, icon_path); 
        tray->iconOriginalName[icon_path_str_len] = '\0';
    }else{
        printf("[%s][Line: %d] 该属性不是字符串 Item: icon_path\n", __FILE__, __LINE__);
        goto label_end_clear; 
    }
    tray->iconName = _parse_icon_path(tray->iconOriginalName); 

    // 解析菜单
    cJSON *menu_item_list_json = cJSON_GetObjectItem(json, "menu_item_list");
    if(menu_item_list_json == NULL){
        printf("[%s][Line: %d] cJSON_GetObjectItem failed. Item: menu_item_list_json\n", __FILE__, __LINE__);
        goto label_end_clear; 
    }
    GtkWidget *menu = gtk_menu_new(); 
    if(menu == NULL){
        printf("[%s][Line: %d] GtkMenu 对象创建失败-gtk_menu_new failed\n", __FILE__, __LINE__);
        goto label_end_clear; 
    }
    int menu_new_state = _tray_gtk_menu_new_by_config_json(tray, menu_item_list_json, menu, _tray_menu_item_active_callback);
    if(menu_new_state != 0){
        // 创建菜单失败
        gtk_widget_destroy(menu); 
        goto label_end_clear; 
    }
    tray->menu = menu; 
    // 解析JSON完成，销毁json对象
    cJSON_Delete(json); 
    // 创建托盘指示器
    tray->appIndicator = app_indicator_new(tray->trayName, tray->iconName, APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
    if (tray->appIndicator == NULL)
    {
        printf("[%s][Line: %d] 创建托盘提示器对象失败 (app_indicator_new)\n", __FILE__, __LINE__); 
        goto label_end_clear; 
    } 
    app_indicator_set_status(tray->appIndicator, APP_INDICATOR_STATUS_ACTIVE);
    app_indicator_set_title(tray->appIndicator, NULL);  // 托盘标题首个字符会被改动，不便提供 

    tray->systemTag = _tray_get_system_tag();  // 获取当前系统类型
    tray->onMenuItemActiveCallback = OnMenuItemActiveCallback; 
    gtk_widget_show_all(tray->menu); 
    app_indicator_set_menu(tray->appIndicator, GTK_MENU(tray->menu)); 
    tray->runState = 0; // 运行状态为已初始化

    create_tray_work_state = 1; // 到这里就成功了，因此调整工作状态为成功
    // 清理内存然后返回
label_end_clear:
    if(create_tray_work_state == 1){
        // 成功，直接返回对象
        return tray;
    }else {
        // 失败，清理缓存，然后返回NULL
        if(tray){
            if(tray->trayName) { free(tray->trayName); }
            if(tray->iconName) { g_free(tray->iconName); }
            if(tray->iconOriginalName) { free(tray->iconOriginalName); }
            if(tray->menu){ gtk_widget_destroy(tray->menu); } 
            if(tray->appIndicator){ g_object_unref(tray->appIndicator); }
            free(tray); 
        }
        if(json){ cJSON_Delete(json); }
        return NULL; 
    } 
}

// 菜单项Active事件的回调的数据
struct tray_menu_item_active_call_data{
    int menu_item_id; 
    int menu_item_type;
    struct tray *tray_ptr; 
}; 
/** 菜单项激活的回调函数（LINUX GTK环境） */
static void _tray_menu_item_active_callback(GtkWidget *widget, void *user_data){
    // printf("按钮被点击，数据指针：%p\n", user_data);  
    struct tray_menu_item_active_call_data *data = (struct tray_menu_item_active_call_data *)user_data; 
    // printf("按钮名称：%s\n", data->btnName);  
    data->tray_ptr->onMenuItemActiveCallback(&data->menu_item_id);  
}

/** 递归地解析菜单JSON数据，创建GtkMenu菜单对象 */
static int _tray_gtk_menu_new_by_config_json(struct tray *tray, cJSON *menu_item_list_json, GtkWidget *menu_widget, void (*on_menu_item_active_call)(GtkWidget *widget, void *user_data)){ 
    if(cJSON_IsArray(menu_item_list_json)){
        // 遍历列表，创建菜单项
        int arr_len = cJSON_GetArraySize(menu_item_list_json);
        for(int i=0;i<arr_len;i++){
            cJSON *menu_item_json = cJSON_GetArrayItem(menu_item_list_json, i);
            cJSON *id_json = cJSON_GetObjectItem(menu_item_json, "id");
            cJSON *label_json = cJSON_GetObjectItem(menu_item_json, "label");
            cJSON *type_json = cJSON_GetObjectItem(menu_item_json, "type");
            if(id_json != NULL && label_json != NULL && type_json != NULL){
                // 解析数据
                int id = (int)cJSON_GetNumberValue(id_json);
                char *label = cJSON_GetStringValue(label_json); 
                int type = (int)cJSON_GetNumberValue(type_json); 
                GtkWidget *menu_item = NULL; 

                switch (type)
                {
                case 1:
                    /* 普通按钮 */
                    menu_item = gtk_menu_item_new_with_label(label);
                    break;
                case 2: 
                    /* 带复选框的按钮 */
                    menu_item = gtk_check_menu_item_new_with_label(label);
                    break;
                case -1:
                    /* 子菜单按钮 */
                    menu_item = gtk_menu_item_new_with_label(label);
                    GtkWidget *submenu = gtk_menu_new(); 
                    cJSON *arr = cJSON_GetObjectItem(menu_item_json, "children"); 
                    _tray_gtk_menu_new_by_config_json(tray, arr, submenu, on_menu_item_active_call);
                    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), submenu); 
                    break;
                default:
                    break;
                }
                // 将创建能的菜单项放入菜单
                gtk_menu_shell_append(GTK_MENU_SHELL(menu_widget), menu_item);  
                // 菜单项绑定事件
                struct tray_menu_item_active_call_data *data = (struct tray_menu_item_active_call_data *)malloc(sizeof(struct tray_menu_item_active_call_data)); 
                data->menu_item_id = id;
                data->menu_item_type = type; 
                data->tray_ptr = tray; 
                g_signal_connect(menu_item, "activate", G_CALLBACK(on_menu_item_active_call), data); //绑定事件
            }else{
                printf("[%s][Line: %d] 解析JSON异常：查询菜单项失败\n", __FILE__, __LINE__);
                return -1;
            }
        }
    }else {
        printf("[%s][Line: %d] 解析JSON异常：不是列表\n", __FILE__, __LINE__);
    }
    return 0;
}

void tray_start_main_loop(struct tray *tray){
    tray->runState = 1; // 运行状态为已启动
    // printf("[tray_start_main_loop] 托盘应用已启动\n"); 
    gtk_main();
    // printf("[tray_start_main_loop] 托盘应用已结束主循环\n"); 
    g_object_unref(tray->appIndicator);
}

void tray_quit(struct tray *tray){
    if(tray == NULL){
        printf("[tray_quit] 传入的托盘应用对象为空，请检查\n"); 
        return; 
    }
    gtk_main_quit(); 
}

void tray_print_info(struct tray*tray){
    printf("=================== 打印托盘信息 ====================\n");
    printf("托盘指针: %p \n托盘名称：%s \n图标：%s \nAppIndicator指针：%p \nGTK菜单指针：%p \n", tray, tray->trayName, tray->iconName, tray->appIndicator, tray->menu); 
    if(tray->systemTag == 1){
        printf("当前操作系统：%s\n", "linux");
    }else{
        printf("当前操作系统：%s\n", "未知"); 
    }
    if(tray->runState == -1){
        printf("托盘尚未初始化\n");
    }else if(tray->runState == 0){
        printf("托盘已初始化但是还没有启动\n");
    }else if(tray->runState == 1){
        printf("托盘已经启动\n");
    }else {
        printf("托盘处于未知状态，请检查\n");
    }
    printf("=====================================================\n");
}

static int _tray_get_system_tag(){
    #ifdef __linux__
        return 1;
    #elif _WIN32
        #ifdef _WIN64 
            return 2;
        #else 
            return 3; 
        #endif
    #elif __unix__
        return 4; 
    #else
        return -1;
    #endif
}


