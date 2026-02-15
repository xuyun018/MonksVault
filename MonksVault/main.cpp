// main.c
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shellapi.h>
#include "resource.h"

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define IDC_APP_LIST        100
#define IDC_ICON_STATIC     101
#define IDC_DESC_STATIC     102

HINSTANCE g_hInst;
LANGID g_CurrentLangID = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);

// 目标语言的原生 LANGID
#define LANGID_ENGLISH          MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)
#define LANGID_CHINESE_SIM      MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED)
#define LANGID_CHINESE_TRAD     MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL)
#define LANGID_JAPANESE         MAKELANGID(LANG_JAPANESE, SUBLANG_JAPANESE_JAPAN)
#define LANGID_KOREAN           MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN)

// 软件信息结构
typedef struct {
	UINT idName;
	UINT idDesc;
	UINT idIcon;
	UINT idBin;
} APP_INFO;

APP_INFO g_Apps[] = {
	{ IDS_APP_EDR_NAME, IDS_APP_EDR_DESC, IDB_EDR_ICON, IDR_BIN_EDR },
	{ IDS_APP_FAV_NAME, IDS_APP_FAV_DESC, IDB_FAV_ICON, IDR_BIN_FAV }
};
int g_nAppCount = sizeof(g_Apps) / sizeof(g_Apps[0]);

int g_nSelectedApp = 0;  // 当前选中项

static HMENU g_hHelpMenu = NULL;      // 帮助菜单句柄
static int   g_nHelpMenuIndex = -1;    // 帮助菜单在主菜单中的索引

						 // 函数声明
BOOL LoadStringEx(UINT uID, LPWSTR lpBuffer, int cchBufferMax, LANGID langId);
void RefreshUI(HWND hWnd);
void SetLanguage(HWND hWnd, LANGID newLangId);
void UpdateLeftPanel(HWND hWnd, int index);
void UpdateListColumns(HWND hList);
void UpdateListItems(HWND hList);
void SaveCurrentItem(HWND hWnd);
void InstallCurrentItem(HWND hWnd);
void UpdateIconImage(HWND hWnd, int index, int width, int height);

// 加载字符串（指定语言）
BOOL LoadStringEx(UINT uID, LPWSTR lpBuffer, int cchBufferMax, LANGID langId)
{
	LANGID oldLang = GetThreadUILanguage();
	SetThreadUILanguage(langId);
	BOOL result = LoadStringW(g_hInst, uID, lpBuffer, cchBufferMax) > 0;
	SetThreadUILanguage(oldLang);
	return result;
}

// 更新图标（根据控件尺寸重新加载缩放后的位图）
void UpdateIconImage(HWND hWnd, int index, int width, int height)
{
	HWND hIconStatic = GetDlgItem(hWnd, IDC_ICON_STATIC);
	if (!hIconStatic) return;

	// 删除旧位图
	HBITMAP hOldBmp = (HBITMAP)SendMessage(hIconStatic, STM_GETIMAGE, IMAGE_BITMAP, 0);
	if (hOldBmp)
		DeleteObject(hOldBmp);

	// 加载缩放后的位图
	HBITMAP hBmp = (HBITMAP)LoadImage(g_hInst, MAKEINTRESOURCE(g_Apps[index].idIcon),
		IMAGE_BITMAP, width, height, LR_DEFAULTCOLOR);
	if (hBmp)
	{
		SendMessage(hIconStatic, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
	}
}

// 更新左侧面板（描述文本，图标已在WM_SIZE中处理）
void UpdateLeftPanel(HWND hWnd, int index)
{
	// 更新描述文本
	HWND hDescStatic = GetDlgItem(hWnd, IDC_DESC_STATIC);
	if (hDescStatic)
	{
		WCHAR szDesc[1024];
		LoadStringEx(g_Apps[index].idDesc, szDesc, 1024, g_CurrentLangID);
		SetWindowTextW(hDescStatic, szDesc);
	}

	// 强制重绘图标（尺寸可能已变，但图标已在WM_SIZE中处理）
	// 这里我们重新触发一次图标更新（但尺寸需要已知，由WM_SIZE负责）
	// 为简单，我们可以在WM_SIZE中统一更新图标
}

// 更新列表列标题
void UpdateListColumns(HWND hList)
{
	LVCOLUMN col = { 0 };
	col.mask = LVCF_TEXT;
	WCHAR szText[64];

	LoadStringEx(IDS_LIST_NAME, szText, 64, g_CurrentLangID);
	col.pszText = szText;
	ListView_SetColumn(hList, 0, &col);

	LoadStringEx(IDS_LIST_DESC, szText, 64, g_CurrentLangID);
	col.pszText = szText;
	ListView_SetColumn(hList, 1, &col);
}

// 更新列表内容
void UpdateListItems(HWND hList)
{
	ListView_DeleteAllItems(hList);

	LVITEMW item = { 0 };
	item.mask = LVIF_TEXT;
	WCHAR szName[128], szDesc[256];

	for (int i = 0; i < g_nAppCount; i++)
	{
		item.iItem = i;
		item.pszText = L"";
		ListView_InsertItem(hList, &item);

		LoadStringEx(g_Apps[i].idName, szName, 128, g_CurrentLangID);
		ListView_SetItemText(hList, i, 0, szName);

		LoadStringEx(g_Apps[i].idDesc, szDesc, 256, g_CurrentLangID);
		ListView_SetItemText(hList, i, 1, szDesc);
	}

	ListView_SetItemState(hList, 0, LVIS_SELECTED, LVIS_SELECTED);
}

// 刷新整个界面（语言切换后调用）
void RefreshUI(HWND hWnd)
{
	HWND hList = GetDlgItem(hWnd, IDC_APP_LIST);
	HWND hBtnSave = GetDlgItem(hWnd, IDC_BTN_SAVEAS);
	HWND hBtnInstall = GetDlgItem(hWnd, IDC_BTN_INSTALL);

	UpdateListColumns(hList);
	UpdateListItems(hList);
	UpdateLeftPanel(hWnd, g_nSelectedApp);

	// 更新按钮文本
	WCHAR szBuf[64];
	LoadStringEx(IDS_BTN_SAVEAS, szBuf, 64, g_CurrentLangID);
	SetWindowTextW(hBtnSave, szBuf);
	LoadStringEx(IDS_BTN_INSTALL, szBuf, 64, g_CurrentLangID);
	SetWindowTextW(hBtnInstall, szBuf);

	// 更新窗口标题
	LoadStringEx(IDS_APP_TITLE, szBuf, 128, g_CurrentLangID);
	SetWindowTextW(hWnd, szBuf);

	// 强制重新布局
	RECT rc;
	GetClientRect(hWnd, &rc);
	SendMessage(hWnd, WM_SIZE, 0, MAKELPARAM(rc.right, rc.bottom));
}

// 设置新语言
void SetLanguage(HWND hWnd, LANGID newLangId)
{
	if (newLangId == g_CurrentLangID)
		return;
	g_CurrentLangID = newLangId;

	if (g_hHelpMenu != NULL && g_nHelpMenuIndex != -1)
	{
		WCHAR szBuf[64];
		HMENU hMenu = GetMenu(hWnd);

		// 更新帮助主菜单标题
		LoadStringEx(IDS_HELP_MENU, szBuf, 64, g_CurrentLangID);
		MENUITEMINFOW mii = { sizeof(mii) };
		mii.fMask = MIIM_STRING;
		mii.dwTypeData = szBuf;
		SetMenuItemInfoW(hMenu, g_nHelpMenuIndex, TRUE, &mii);

		// 更新“关于”子项
		LoadStringEx(IDS_HELP_ABOUT, szBuf, 64, g_CurrentLangID);
		mii.dwTypeData = szBuf;
		SetMenuItemInfoW(g_hHelpMenu, IDM_HELP_ABOUT, FALSE, &mii);

		DrawMenuBar(hWnd);
	}

	RefreshUI(hWnd);
}

// 从资源提取二进制数据并保存到文件
BOOL SaveResourceToFile(UINT uResId, LPCWSTR lpszFilePath)
{
	HRSRC hRes = FindResourceW(g_hInst, MAKEINTRESOURCEW(uResId), RT_RCDATA);
	if (!hRes) return FALSE;

	DWORD dwSize = SizeofResource(g_hInst, hRes);
	if (dwSize == 0) return FALSE;

	HGLOBAL hData = LoadResource(g_hInst, hRes);
	if (!hData) return FALSE;

	LPVOID pData = LockResource(hData);
	if (!pData) return FALSE;

	HANDLE hFile = CreateFileW(lpszFilePath, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD dwWritten;
	BOOL bSuccess = WriteFile(hFile, pData, dwSize, &dwWritten, NULL);
	CloseHandle(hFile);

	return bSuccess && (dwWritten == dwSize);
}

// 获取临时目录并生成临时文件名
BOOL GetTempFilePath(LPWSTR lpszPath, DWORD cchPath, LPCWSTR lpszPrefix, LPCWSTR lpszExt)
{
	WCHAR szTempPath[MAX_PATH];
	if (!GetTempPathW(MAX_PATH, szTempPath))
		return FALSE;

	WCHAR szTempName[MAX_PATH];
	if (!GetTempFileNameW(szTempPath, lpszPrefix, 0, szTempName))
		return FALSE;

	lstrcpyW(lpszPath, szTempName);
	WCHAR *pExt = wcsrchr(lpszPath, L'.');
	if (pExt)
		lstrcpyW(pExt, lpszExt);
	else
		lstrcatW(lpszPath, lpszExt);

	return TRUE;
}

// “另存为”功能
void SaveCurrentItem(HWND hWnd)
{
	int index = g_nSelectedApp;
	if (index < 0 || index >= g_nAppCount)
	{
		MessageBoxW(hWnd, L"No item selected.", L"Error", MB_ICONWARNING);
		return;
	}

	// 获取软件名称用于默认文件名
	WCHAR szName[128];
	LoadStringEx(g_Apps[index].idName, szName, 128, g_CurrentLangID);

	// 加载过滤器描述字符串
	WCHAR szExecDesc[64], szAllDesc[64];
	LoadStringEx(IDS_FILTER_EXECUTABLE_DESC, szExecDesc, 64, g_CurrentLangID);
	LoadStringEx(IDS_FILTER_ALL_DESC, szAllDesc, 64, g_CurrentLangID);

	// 构建过滤器：描述1\0模式1\0描述2\0模式2\0\0
	WCHAR szFilterBuffer[256] = { 0 };
	WCHAR* p = szFilterBuffer;

	// 复制可执行文件描述
	wcscpy(p, szExecDesc);
	p += wcslen(szExecDesc) + 1;

	// 复制模式 *.exe
	wcscpy(p, L"*.exe");
	p += wcslen(L"*.exe") + 1;

	// 复制所有文件描述
	wcscpy(p, szAllDesc);
	p += wcslen(szAllDesc) + 1;

	// 复制模式 *.*
	wcscpy(p, L"*.*");
	p += wcslen(L"*.*") + 1;

	// 最后加一个额外的空字符
	*p = L'\0';

	OPENFILENAMEW ofn = { 0 };
	WCHAR szFile[MAX_PATH] = L"";
	wcscpy_s(szFile, MAX_PATH, szName);
	wcscat_s(szFile, MAX_PATH, L".exe");  // 默认扩展名改为 .exe

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = szFilterBuffer;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = L"exe";              // 默认扩展名 exe
	ofn.Flags = OFN_OVERWRITEPROMPT;

	if (GetSaveFileNameW(&ofn))
	{
		if (SaveResourceToFile(g_Apps[index].idBin, szFile))
			MessageBoxW(hWnd, L"File saved successfully.", L"Success", MB_OK);
		else
			MessageBoxW(hWnd, L"Failed to save file.", L"Error", MB_ICONERROR);
	}
}

// “安装”功能
void InstallCurrentItem(HWND hWnd)
{
	int index = g_nSelectedApp;
	if (index < 0 || index >= g_nAppCount)
	{
		MessageBoxW(hWnd, L"No item selected.", L"Error", MB_ICONWARNING);
		return;
	}

	WCHAR szTempFile[MAX_PATH];
	if (!GetTempFilePath(szTempFile, MAX_PATH, L"INST", L".exe"))
	{
		MessageBoxW(hWnd, L"Failed to create temp file path.", L"Error", MB_ICONERROR);
		return;
	}

	if (!SaveResourceToFile(g_Apps[index].idBin, szTempFile))
	{
		MessageBoxW(hWnd, L"Failed to extract binary to temp file.", L"Error", MB_ICONERROR);
		return;
	}

	HINSTANCE hRet = ShellExecuteW(hWnd, L"open", szTempFile, NULL, NULL, SW_SHOW);
	if ((INT_PTR)hRet <= 32)
	{
		MessageBoxW(hWnd, L"Failed to execute the file.", L"Error", MB_ICONERROR);
		DeleteFileW(szTempFile);
	}
}

// 关于对话框过程
INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hTab, hInfoStatic, hQQ1, hQQ2, hQQ3, hLinkGit, hLinkBili;
	static HWND hWechatImg, hAlipayImg, hPaypalText;
	static int nCurPage = 0;

	switch (message)
	{
	case WM_INITDIALOG:
	{
		// 获取控件句柄
		hTab = GetDlgItem(hDlg, IDC_TAB);
		hInfoStatic = GetDlgItem(hDlg, IDC_STATIC_INFO);
		hQQ1 = GetDlgItem(hDlg, IDC_EDIT_QQ1);
		hQQ2 = GetDlgItem(hDlg, IDC_EDIT_QQ2);
		hQQ3 = GetDlgItem(hDlg, IDC_EDIT_QQ3);
		hLinkGit = GetDlgItem(hDlg, IDC_LINK_GITHUB);
		hLinkBili = GetDlgItem(hDlg, IDC_LINK_BILIBILI);
		hWechatImg = GetDlgItem(hDlg, IDC_STATIC_WECHAT_IMG);
		hAlipayImg = GetDlgItem(hDlg, IDC_STATIC_ALIPAY_IMG);
		hPaypalText = GetDlgItem(hDlg, IDC_STATIC_PAYPAL_TEXT);

		WCHAR szBuf[256];

		// 设置对话框标题
		LoadStringEx(IDS_ABOUT_TITLE, szBuf, 256, g_CurrentLangID);
		SetWindowTextW(hDlg, szBuf);

		// 添加Tab页
		TCITEMW tie = { 0 };
		tie.mask = TCIF_TEXT;

		LoadStringEx(IDS_ABOUT_TAB_INFO, szBuf, 256, g_CurrentLangID);
		tie.pszText = szBuf;
		TabCtrl_InsertItem(hTab, 0, &tie);

		LoadStringEx(IDS_ABOUT_TAB_WECHAT, szBuf, 256, g_CurrentLangID);
		tie.pszText = szBuf;
		TabCtrl_InsertItem(hTab, 1, &tie);

		LoadStringEx(IDS_ABOUT_TAB_ALIPAY, szBuf, 256, g_CurrentLangID);
		tie.pszText = szBuf;
		TabCtrl_InsertItem(hTab, 2, &tie);

		LoadStringEx(IDS_ABOUT_TAB_PAYPAL, szBuf, 256, g_CurrentLangID);
		tie.pszText = szBuf;
		TabCtrl_InsertItem(hTab, 3, &tie);

		// 设置信息页内容
		// 版本
		LoadStringEx(IDS_ABOUT_VERSION, szBuf, 256, g_CurrentLangID);
		SetWindowTextW(hInfoStatic, szBuf);

		// QQ群
		WCHAR szQQPrefix[64];
		LoadStringEx(IDS_ABOUT_QQ, szQQPrefix, 64, g_CurrentLangID);
		if (lstrlenW(szQQPrefix) == 0) wcscpy_s(szQQPrefix, L"QQ");

		WCHAR szQQ[128];
		wsprintfW(szQQ, L"%s: 131182440", szQQPrefix);
		SetWindowTextW(hQQ1, szQQ);
		wsprintfW(szQQ, L"%s: 5669743", szQQPrefix);
		SetWindowTextW(hQQ2, szQQ);
		wsprintfW(szQQ, L"%s: 777846820", szQQPrefix);
		SetWindowTextW(hQQ3, szQQ);

		// 链接
		LoadStringEx(IDS_ABOUT_GITHUB, szBuf, 256, g_CurrentLangID);
		SetWindowTextW(hLinkGit, szBuf);
		LoadStringEx(IDS_ABOUT_BILIBILI, szBuf, 256, g_CurrentLangID);
		SetWindowTextW(hLinkBili, szBuf);

		// 加载微信支付图片
		HBITMAP hBmpWechat = (HBITMAP)LoadImageW(g_hInst, MAKEINTRESOURCEW(IDB_WECHAT_PAY),
			IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
		if (hBmpWechat)
			SendMessageW(hWechatImg, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmpWechat);

		// 加载支付宝图片
		HBITMAP hBmpAlipay = (HBITMAP)LoadImageW(g_hInst, MAKEINTRESOURCEW(IDB_ALIPAY),
			IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
		if (hBmpAlipay)
			SendMessageW(hAlipayImg, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmpAlipay);

		// 设置PayPal账户文本
		LoadStringEx(IDS_PAYPAL_ACCOUNT, szBuf, 256, g_CurrentLangID);
		SetWindowTextW(hPaypalText, szBuf);

		// 默认显示第一页（信息页），隐藏支付页面控件
		ShowWindow(hInfoStatic, SW_SHOW);
		ShowWindow(hQQ1, SW_SHOW);
		ShowWindow(hQQ2, SW_SHOW);
		ShowWindow(hQQ3, SW_SHOW);
		ShowWindow(hLinkGit, SW_SHOW);
		ShowWindow(hLinkBili, SW_SHOW);
		ShowWindow(hWechatImg, SW_HIDE);
		ShowWindow(hAlipayImg, SW_HIDE);
		ShowWindow(hPaypalText, SW_HIDE);

		return (INT_PTR)TRUE;
	}

	case WM_NOTIFY:
	{
		if (((LPNMHDR)lParam)->idFrom == IDC_TAB && ((LPNMHDR)lParam)->code == TCN_SELCHANGE)
		{
			int sel = TabCtrl_GetCurSel(hTab);
			// 隐藏所有页面控件
			ShowWindow(hInfoStatic, SW_HIDE);
			ShowWindow(hQQ1, SW_HIDE);
			ShowWindow(hQQ2, SW_HIDE);
			ShowWindow(hQQ3, SW_HIDE);
			ShowWindow(hLinkGit, SW_HIDE);
			ShowWindow(hLinkBili, SW_HIDE);
			ShowWindow(hWechatImg, SW_HIDE);
			ShowWindow(hAlipayImg, SW_HIDE);
			ShowWindow(hPaypalText, SW_HIDE);

			// 显示选中页
			switch (sel)
			{
			case 0: // Info
				ShowWindow(hInfoStatic, SW_SHOW);
				ShowWindow(hQQ1, SW_SHOW);
				ShowWindow(hQQ2, SW_SHOW);
				ShowWindow(hQQ3, SW_SHOW);
				ShowWindow(hLinkGit, SW_SHOW);
				ShowWindow(hLinkBili, SW_SHOW);
				break;
			case 1: // WeChat
				ShowWindow(hWechatImg, SW_SHOW);
				break;
			case 2: // Alipay
				ShowWindow(hAlipayImg, SW_SHOW);
				break;
			case 3: // PayPal
				ShowWindow(hPaypalText, SW_SHOW);
				break;
			}
			return TRUE;
		}
		break;
	}

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		else if (HIWORD(wParam) == STN_CLICKED)
		{
			// 处理链接点击
			if ((HWND)lParam == hLinkGit)
			{
				ShellExecuteW(hDlg, L"open", L"https://github.com/xuyun018/", NULL, NULL, SW_SHOW);
			}
			else if ((HWND)lParam == hLinkBili)
			{
				ShellExecuteW(hDlg, L"open", L"https://space.bilibili.com/473687944", NULL, NULL, SW_SHOW);
			}
		}
		break;

	case WM_DESTROY:
		// 删除图片资源（可选）
	{
		HBITMAP hBmp = (HBITMAP)SendMessageW(hWechatImg, STM_GETIMAGE, IMAGE_BITMAP, 0);
		if (hBmp) DeleteObject(hBmp);
		hBmp = (HBITMAP)SendMessageW(hAlipayImg, STM_GETIMAGE, IMAGE_BITMAP, 0);
		if (hBmp) DeleteObject(hBmp);
	}
	break;
	}
	return (INT_PTR)FALSE;
}

// 窗口过程
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
	{
		HWND hEdit;
		HWND hButton;

		// 创建左侧图标静态控件
		CreateWindowW(L"STATIC", NULL,
			WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_CENTERIMAGE,
			0, 0, 0, 0,  // 尺寸将在 WM_SIZE 中调整
			hWnd, (HMENU)IDC_ICON_STATIC, g_hInst, NULL);

		// 创建左侧描述编辑框
		hEdit = CreateWindowW(L"EDIT", NULL,
			WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY |
			WS_VSCROLL | ES_AUTOVSCROLL | WS_BORDER,
			0, 0, 0, 0,
			hWnd, (HMENU)IDC_DESC_STATIC, g_hInst, NULL);
		SendMessage(hEdit, WM_SETFONT, (WPARAM)(HFONT)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE, 0));

		// 创建右侧列表
		HWND hList = CreateWindowW(WC_LISTVIEW, NULL,
			WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
			0, 0, 0, 0,
			hWnd, (HMENU)IDC_APP_LIST, g_hInst, NULL);
		ListView_SetExtendedListViewStyle(hList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

		// 初始化列表列（插入时设置宽度，但文本稍后更新）
		LVCOLUMN col = { 0 };
		col.mask = LVCF_TEXT | LVCF_WIDTH;
		col.cx = 120; col.pszText = L""; ListView_InsertColumn(hList, 0, &col);
		col.cx = 260; col.pszText = L""; ListView_InsertColumn(hList, 1, &col);

		// 创建两个按钮
		hButton = CreateWindowW(L"BUTTON", NULL,
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			0, 0, 0, 0,
			hWnd, (HMENU)IDC_BTN_SAVEAS, g_hInst, NULL);
		SendMessage(hButton, WM_SETFONT, (WPARAM)(HFONT)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE, 0));

		hButton = CreateWindowW(L"BUTTON", NULL,
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			0, 0, 0, 0,
			hWnd, (HMENU)IDC_BTN_INSTALL, g_hInst, NULL);
		SendMessage(hButton, WM_SETFONT, (WPARAM)(HFONT)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE, 0));

		// 创建菜单
		HMENU hMenu = CreateMenu();
		HMENU hLangMenu = CreatePopupMenu();
		HMENU hHelpMenu = CreatePopupMenu();
		WCHAR szBuf[64];

		LoadStringEx(IDS_LANG_ENGLISH, szBuf, 64, LANGID_ENGLISH);
		AppendMenuW(hLangMenu, MF_STRING, IDM_LANG_ENGLISH, szBuf);
		LoadStringEx(IDS_LANG_CHINESE_SIM, szBuf, 64, LANGID_CHINESE_SIM);
		AppendMenuW(hLangMenu, MF_STRING, IDM_LANG_CHINESE_SIM, szBuf);
		LoadStringEx(IDS_LANG_CHINESE_TRAD, szBuf, 64, LANGID_CHINESE_TRAD);
		AppendMenuW(hLangMenu, MF_STRING, IDM_LANG_CHINESE_TRAD, szBuf);
		LoadStringEx(IDS_LANG_JAPANESE, szBuf, 64, LANGID_JAPANESE);
		AppendMenuW(hLangMenu, MF_STRING, IDM_LANG_JAPANESE, szBuf);
		LoadStringEx(IDS_LANG_KOREAN, szBuf, 64, LANGID_KOREAN);
		AppendMenuW(hLangMenu, MF_STRING, IDM_LANG_KOREAN, szBuf);

		LoadStringEx(IDS_MENU_LANGUAGE, szBuf, 64, g_CurrentLangID);
		AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hLangMenu, szBuf);

		LoadStringEx(IDS_HELP_ABOUT, szBuf, 64, g_CurrentLangID);
		AppendMenuW(hHelpMenu, MF_STRING, IDM_HELP_ABOUT, szBuf);

		g_hHelpMenu = hHelpMenu;
		// 获取当前主菜单项数量（新菜单将加在末尾）
		g_nHelpMenuIndex = GetMenuItemCount(hMenu);

		// 加载帮助主菜单标题并添加到主菜单
		LoadStringEx(IDS_HELP_MENU, szBuf, 64, g_CurrentLangID);
		AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, szBuf);

		SetMenu(hWnd, hMenu);

		// 初始化界面
		RefreshUI(hWnd);
		break;
	}

	case WM_SIZE:
	{
		int width = LOWORD(lParam);
		int height = HIWORD(lParam);

		// 计算左侧宽度（1/3）和右侧宽度（2/3）
		int leftWidth = width / 3;
		int rightWidth = width - leftWidth - 20; // 减去边距10+10

												 // 左侧内部布局
		int iconSize = leftWidth - 20;  // 预览图方形，边距10
		int iconX = 10;
		int iconY = 10;
		int iconW = iconSize;
		int iconH = iconSize;

		int descX = 10;
		int descY = iconY + iconH + 10;
		int descW = leftWidth - 20;
		int descH = height - descY - 10;

		// 右侧内部布局
		int listX = leftWidth + 10;
		int listY = 10;
		int listW = rightWidth;
		int listH = height - 70; // 为按钮留出空间

		int btnY = listY + listH + 10;
		int btnW = 120;
		int btnH = 30;
		int btnSaveX = listX;
		int btnInstallX = listX + btnW + 10;

		// 使用 DeferWindowPos 批量更新
		HDWP hdwp = BeginDeferWindowPos(5);  // 5个控件

		hdwp = DeferWindowPos(hdwp, GetDlgItem(hWnd, IDC_ICON_STATIC), NULL,
			iconX, iconY, iconW, iconH,
			SWP_NOZORDER);
		hdwp = DeferWindowPos(hdwp, GetDlgItem(hWnd, IDC_DESC_STATIC), NULL,
			descX, descY, descW, descH,
			SWP_NOZORDER);
		hdwp = DeferWindowPos(hdwp, GetDlgItem(hWnd, IDC_APP_LIST), NULL,
			listX, listY, listW, listH,
			SWP_NOZORDER);
		hdwp = DeferWindowPos(hdwp, GetDlgItem(hWnd, IDC_BTN_SAVEAS), NULL,
			btnSaveX, btnY, btnW, btnH,
			SWP_NOZORDER);
		hdwp = DeferWindowPos(hdwp, GetDlgItem(hWnd, IDC_BTN_INSTALL), NULL,
			btnInstallX, btnY, btnW, btnH,
			SWP_NOZORDER);

		EndDeferWindowPos(hdwp);

		// 更新预览图（根据新尺寸重新加载缩放后的位图）
		UpdateIconImage(hWnd, g_nSelectedApp, iconW, iconH);
		break;
	}

	case WM_COMMAND:
	{
		if (lParam != 0) // 来自控件
		{
			if (HIWORD(wParam) == BN_CLICKED)
			{
				switch (LOWORD(wParam))
				{
				case IDC_BTN_SAVEAS:
					SaveCurrentItem(hWnd);
					break;
				case IDC_BTN_INSTALL:
					InstallCurrentItem(hWnd);
					break;
				}
			}
		}
		else // 来自菜单
		{
			switch (LOWORD(wParam))
			{
			case IDM_LANG_ENGLISH:
				SetLanguage(hWnd, LANGID_ENGLISH);
				break;
			case IDM_LANG_CHINESE_SIM:
				SetLanguage(hWnd, LANGID_CHINESE_SIM);
				break;
			case IDM_LANG_CHINESE_TRAD:
				SetLanguage(hWnd, LANGID_CHINESE_TRAD);
				break;
			case IDM_LANG_JAPANESE:
				SetLanguage(hWnd, LANGID_JAPANESE);
				break;
			case IDM_LANG_KOREAN:
				SetLanguage(hWnd, LANGID_KOREAN);
				break;
			case IDM_HELP_ABOUT:
				DialogBoxW(g_hInst, MAKEINTRESOURCEW(IDD_ABOUT), hWnd, AboutDlgProc);
				break;
			default:
				break;
			}
		}
		break;
	}

	case WM_NOTIFY:
	{
		LPNMHDR pnmh = (LPNMHDR)lParam;
		if (pnmh->idFrom == IDC_APP_LIST && pnmh->code == LVN_ITEMCHANGED)
		{
			LPNMLISTVIEW pnmlv = (LPNMLISTVIEW)lParam;
			if ((pnmlv->uChanged & LVIF_STATE) &&
				((pnmlv->uNewState & LVIS_SELECTED) != (pnmlv->uOldState & LVIS_SELECTED)) &&
				(pnmlv->uNewState & LVIS_SELECTED))
			{
				g_nSelectedApp = pnmlv->iItem;
				UpdateLeftPanel(hWnd, g_nSelectedApp);
				// 更新预览图（图标可能不变，但为保持一致，可重新加载）
				RECT rc;
				GetClientRect(GetDlgItem(hWnd, IDC_ICON_STATIC), &rc);
				UpdateIconImage(hWnd, g_nSelectedApp, rc.right - rc.left, rc.bottom - rc.top);
			}
		}
		break;
	}

	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLOREDIT:
	{
		HDC hdcStatic = (HDC)wParam;
		SetTextColor(hdcStatic, RGB(0, 0, 0));
		SetBkMode(hdcStatic, OPAQUE);
		return (LRESULT)GetStockObject(WHITE_BRUSH);
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProcW(hWnd, msg, wParam, lParam);
	}
	return 0;
}

// 入口函数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	g_hInst = hInstance;

	INITCOMMONCONTROLSEX icc = { sizeof(INITCOMMONCONTROLSEX), ICC_LISTVIEW_CLASSES };
	InitCommonControlsEx(&icc);

	WNDCLASSEXW wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = L"MonksVaultClass";
	// 加载大图标（用于 Alt+Tab 和标题栏）
	wc.hIcon = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_MAIN_ICON));
	// 加载小图标（用于窗口左上角和任务栏）
	wc.hIconSm = (HICON)LoadImageW(hInstance, MAKEINTRESOURCEW(IDI_MAIN_ICON),
		IMAGE_ICON, GetSystemMetrics(SM_CXSMICON),
		GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);

	if (!RegisterClassExW(&wc))
		return 0;

	WCHAR szTitle[128];
	LoadStringEx(IDS_APP_TITLE, szTitle, 128, g_CurrentLangID);

	HWND hWnd = CreateWindowExW(0, wc.lpszClassName, szTitle,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		900, 600, NULL, NULL, hInstance, NULL);

	if (!hWnd) return 0;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}