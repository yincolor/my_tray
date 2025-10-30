
#include "./tray/include/tray.h"

/** 读取字符串，从文件中 */
char *_str_read_from_file(const char *file_path){
    if(file_path == NULL){
        printf("[_str_read_from_file] 提供的文件路径为空\n");
        return NULL;
    }
    FILE *f = fopen(file_path, "r");
    if(f == NULL){
        printf("[_str_read_from_file] 无法打开文件%s\n", file_path);
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET); 
    char *str = (char *)malloc(file_size+1);
    fread(str, file_size, 1, f);
    fclose(f);
    f = NULL; 
    str[file_size] = '\0';
    return str; 
}
/** 删除字符串 char* */
void _str_delete(char *str){
    free(str); 
    str = NULL; 
}

struct tray *tray; 
void on_my_menu_button_clicked_call(char *menu_button_name){
    printf("我的菜单按钮被点击了%s\n", menu_button_name);
    if(strcmp(menu_button_name, "退出") == 0){
        tray_quit(tray); 
    }else if(strcmp(menu_button_name, "打开网站") == 0){
        pid_t pid = fork();
        if (pid == 0)
        {
            execlp("xdg-open", "xdg-open", "../index.html", NULL);
            perror("xdg-open failed");
            exit(EXIT_FAILURE);
        }
    }
}

void on_my_menu_item_active_call(void *menu_item_id){
    int id = *((int *)menu_item_id); 
    printf("我的菜单按钮被点击了%d\n", id);
    if(id == 0){
        tray_quit(tray); 
    }else if(id == 1){
        pid_t pid = fork();
        if (pid == 0)
        {
            execlp("xdg-open", "xdg-open", "../index.html", NULL);
            perror("xdg-open failed");
            exit(EXIT_FAILURE);
        }
    }else {

    }
}

int main(int argc, char *argv[])
{
    char *tray_json_str = _str_read_from_file("../tray_info.json");

    tray = tray_new_by_config_json_string(tray_json_str, on_my_menu_item_active_call, &argc, &argv);  
    
    tray_print_info(tray);
    printf("[main] 托盘应用启动\n"); 
    tray_start_main_loop(tray); 

    _str_delete(tray_json_str); 
    return 0;
}
