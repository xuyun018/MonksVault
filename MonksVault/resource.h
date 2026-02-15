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
// 帮助菜单字符串 ID
#define IDS_HELP_MENU			1003
#define IDS_HELP_ABOUT			1004
#define IDS_LIST_NAME           1005
#define IDS_LIST_DESC           1006
#define IDS_BTN_SAVEAS          1007      // “另存为”按钮
#define IDS_BTN_INSTALL         1008      // “安装”按钮

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
// 帮助菜单命令 ID
#define IDM_HELP_ABOUT			10006

// 按钮命令 ID
#define IDC_BTN_SAVEAS          20001
#define IDC_BTN_INSTALL         20002

#define IDS_FILTER_EXECUTABLE_DESC  4001   // 可执行文件描述，如 "Executable Files"
#define IDS_FILTER_ALL_DESC         4002   // 所有文件描述，如 "All Files"

// 关于对话框控件ID
#define IDD_ABOUT               200
#define IDC_TAB                 201
#define IDC_STATIC_INFO         202
#define IDC_EDIT_QQ1            203
#define IDC_EDIT_QQ2            204
#define IDC_EDIT_QQ3            205
#define IDC_LINK_GITHUB         206
#define IDC_LINK_BILIBILI       207
#define IDC_STATIC_WECHAT_IMG   208
#define IDC_STATIC_ALIPAY_IMG   209
#define IDC_STATIC_PAYPAL_TEXT  210

// 位图资源
#define IDB_WECHAT_PAY          301
#define IDB_ALIPAY              302

// 关于对话框字符串ID
#define IDS_ABOUT_TITLE         500
#define IDS_ABOUT_TAB_INFO      501
#define IDS_ABOUT_TAB_WECHAT    502
#define IDS_ABOUT_TAB_ALIPAY    503
#define IDS_ABOUT_TAB_PAYPAL    504
#define IDS_ABOUT_VERSION       505
#define IDS_ABOUT_QQ            506
#define IDS_ABOUT_GITHUB        507
#define IDS_ABOUT_BILIBILI      508
#define IDS_PAYPAL_ACCOUNT      509

#endif
