// resource.h
#ifndef RESOURCE_H
#define RESOURCE_H

#define IDI_MAIN_ICON			100

// 位图资源
#define IDB_EDR_ICON            101
#define IDB_FAV_ICON            102

// 二进制资源（每个软件一个）
#define IDR_BIN_EDR             201
#define IDR_BIN_FAV             202

// 通用界面字符串 (1000-1999)
#define IDS_APP_TITLE           1001
#define IDS_MENU_LANGUAGE       1002
#define IDS_LIST_NAME           1003
#define IDS_LIST_DESC           1004
#define IDS_BTN_SAVEAS          1005      // “另存为”按钮
#define IDS_BTN_INSTALL         1006      // “安装”按钮

// 语言菜单项名称 (2000-2999)
#define IDS_LANG_ENGLISH        2001
#define IDS_LANG_CHINESE_SIM    2002
#define IDS_LANG_CHINESE_TRAD   2003
#define IDS_LANG_JAPANESE       2004
#define IDS_LANG_KOREAN         2005

// 软件名称 (3000-3099)
#define IDS_APP_EDR_NAME        3001
#define IDS_APP_FAV_NAME        3002

// 软件详细描述 (3100-3199)
#define IDS_APP_EDR_DESC        3101
#define IDS_APP_FAV_DESC        3102

// 菜单命令 ID (10000+)
#define IDM_LANG_ENGLISH        10001
#define IDM_LANG_CHINESE_SIM    10002
#define IDM_LANG_CHINESE_TRAD   10003
#define IDM_LANG_JAPANESE       10004
#define IDM_LANG_KOREAN         10005

// 按钮命令 ID
#define IDC_BTN_SAVEAS          20001
#define IDC_BTN_INSTALL         20002

#define IDS_FILTER_EXECUTABLE_DESC  4001   // 可执行文件描述，如 "Executable Files"
#define IDS_FILTER_ALL_DESC         4002   // 所有文件描述，如 "All Files"

#endif
