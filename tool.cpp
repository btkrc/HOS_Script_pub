#include "tool.h"
/*QString转WCHAR*/

Tool::Tool() {
    hThreadLimitCursor = 0;
}

BOOL Tool::QString2Wchar(QString buf, TCHAR **szStr) {
    TCHAR *pTemp = (TCHAR *)reinterpret_cast<const TCHAR *>(buf.utf16());
    DWORD length = lstrlen(pTemp);
    *szStr = new TCHAR[length + 2];
    memset(*szStr, 0, length + 2);

    wcscpy_s(*szStr, length + 2, pTemp);
    return TRUE;
}

/*QString转CHAR*/
BOOL Tool::QString2Char(QString buf, CHAR **szStr) {
    std::string str = buf.toLocal8Bit();
    const CHAR *pTemp = str.c_str();
    DWORD size = strlenUtf8(pTemp);
    *szStr = new CHAR[size];
    strcpy_s(*szStr, size, pTemp);
    return TRUE;
}

/*计算UTF-8编码实际占用字节*/
DWORD Tool::strlenUtf8(CONST CHAR *szStr) {
    UCHAR ch;
    CONST CHAR *p = szStr;
    DWORD size = 0;
    while (*p) {
        ch = *p;
        if (ch > 0 && ch <= 0x7F) {
            size += 1;
            p += 1;
        }
        else if (ch >= 0x80 && ch <= 0x7FF) {
            size += 2;
            p += 2;
        }
        else if (ch >= 0x800 && ch <= 0xFFFF) {
            size += 3;
            p += 3;
        }
        else if (ch >= 0x10000 && ch <= 0x10FFFF) {
            size += 4;
            p += 4;
        }
    }

    return size + 1;
}

/*写入Cookies*/
BOOL Tool::writeCookies(LPCTSTR cookiesPath, int index) {
    LPCTSTR lpWriteCookiesPath;
    HANDLE hFileCookies, hWriteCookies;
    LPVOID fileBuf;
    DWORD dwFileSize;

    if (index == 0) {
        lpWriteCookiesPath = TEXT(".\\index0");
    }
    else if (index == 12) {
        lpWriteCookiesPath = TEXT(".\\index12");
    }
    else if (index == 22) {
        lpWriteCookiesPath = TEXT(".\\index22");
    }
    else {
        return FALSE;
    }

    
    hWriteCookies = CreateFile(
        lpWriteCookiesPath,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0
    );
    if (!hWriteCookies) {
        OutputDebugString(TEXT("CreateFile Error\n"));
        return FALSE;
    }

    /*获得待写入Cookies的大小*/
    dwFileSize = GetFileSize(hWriteCookies, NULL);
    fileBuf = malloc(dwFileSize);
    memset(fileBuf, 0, dwFileSize);

    /*读取待写入Cookies*/
    if (!ReadFile(
        hWriteCookies,
        fileBuf,
        dwFileSize,
        NULL,
        NULL
    )) {
        free(fileBuf);
        return FALSE;
    }
    CloseHandle(hWriteCookies);

    /*打开Cookies文件准备写入*/
    hFileCookies = CreateFile(
        cookiesPath,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        0
    );
    if (!hFileCookies) {
        OutputDebugString(TEXT("CreateFile Error\n"));
        free(fileBuf);
        return FALSE;
    }

    /*从开始位置写入*/
    SetFilePointer(hFileCookies, 0, 0, FILE_BEGIN);

    if (!WriteFile(
        hFileCookies,
        fileBuf,
        dwFileSize,
        NULL,
        NULL
    )) {
        CloseHandle(hFileCookies);
        free(fileBuf);
        return FALSE;
    }

    CloseHandle(hFileCookies);
    free(fileBuf);
    return TRUE;
}

/*播放音频*/

void Tool::playSound(LPCSTR lpFileName) {
    QSound::play(lpFileName);
}
BOOL Tool::compareRGB(COLORREF color, DWORD rgb) {
    BYTE *pb = (BYTE *)&color;
    DWORD transColor = (*(pb) << 16) + ((*(pb + 1)) << 8) + (*(pb + 2));

    if (transColor == rgb) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}
/*切换焦点窗口*/
void Tool::setFocusHOS(BOOL flag) {
    HWND hwnd = FindWindow(TEXT("Heroes of the Storm"), TEXT("《风暴英雄》"));

    if (hwnd) {
        if (flag) {
            SetForegroundWindow(hwnd);
        }
        //else {
        //    SetForegroundWindow(NULL);
        //}
    }
}

/*解除鼠标限制*/
VOID Tool::relieveCursor() {
    if (hThreadLimitCursor) {
        TerminateThread(hThreadLimitCursor, 0);
        hThreadLimitCursor = 0;
    }
}

/*检测鼠标位置，以判断是否开启限制*/
VOID Tool::detectedCursor(CONST POINT *limitLeftTop, CONST POINT *limitRightBottom, CONST RECT *gameRect) {
    RECT rectNow;
    HWND hwnd = FindWindow(TEXT("Heroes of the Storm"), TEXT("《风暴英雄》"));
    if (!hwnd) {
        MessageBox(NULL, TEXT("detectedCursor error\n"), NULL, MB_OK);
        return;
    }
    GetWindowRect(hwnd, &rectNow);
    /*如果当前处于游戏状态，则限制鼠标*/
    if (rectNow.left == gameRect->left
        && rectNow.right == gameRect->right
        && rectNow.top == gameRect->top
        && rectNow.bottom == gameRect->bottom) {
        if (hThreadLimitCursor == 0) {
            limitCursor(limitLeftTop, limitRightBottom);
        }
    }
    else {
        if (hThreadLimitCursor) {
            TerminateThread(hThreadLimitCursor, 0);
        }
        hThreadLimitCursor = 0;
    }
}

/*限制鼠标*/
VOID Tool::limitCursor(CONST POINT *limitLeftTop, CONST POINT *limitRightBottom) {
    limitParams.leftTop = *limitLeftTop;
    limitParams.rightBottom = *limitRightBottom;
    hThreadLimitCursor = CreateThread(
        NULL,
        0,
        (LPTHREAD_START_ROUTINE)limitCursorThread,
        (LPVOID)this,
        0,
        NULL
    );
    if (!hThreadLimitCursor) {
        MessageBox(NULL, TEXT("limitCursor Thread Create Error\n"), NULL, MB_OK);
    }
}

/*限制鼠标线程函数*/
DWORD limitCursorThread(LPVOID lpParams) {
    Tool *pTool = (Tool *)lpParams;
    POINT ptNow;
    while (TRUE) {
        GetCursorPos(&ptNow);
        /*超出左边时*/
        if (ptNow.x < pTool->limitParams.leftTop.x) {
            SetCursorPos(pTool->limitParams.leftTop.x, ptNow.y);
        }
        /*超出右边时*/
        else if (ptNow.x > pTool->limitParams.rightBottom.x) {
            SetCursorPos(pTool->limitParams.rightBottom.x, ptNow.y);
        }
        else if (ptNow.y < pTool->limitParams.leftTop.y) {
            SetCursorPos(ptNow.x, pTool->limitParams.leftTop.y);
        }
        else if (ptNow.y > pTool->limitParams.rightBottom.y) {
            SetCursorPos(ptNow.x, pTool->limitParams.rightBottom.y);
        }
    }
}

/*发送字符串*/
VOID Tool::sendStringToWnd(HWND hwnd, LPCTSTR lpStr) {
    LPCTSTR p = lpStr;
    while (*p) {
        SendMessage(hwnd, WM_CHAR, *p, 1);
        p++;
    }
}

VOID Tool::sendTabToWnd(HWND hwnd) {
    SendMessage(hwnd, WM_SYSKEYDOWN, VK_TAB, 0);
    SendMessage(hwnd, WM_SYSKEYUP, VK_TAB, 0);
}

VOID Tool::sendEnterToWnd(HWND hwnd) {
    SendMessage(hwnd, WM_SYSKEYDOWN, VK_RETURN, 0);
    SendMessage(hwnd, WM_SYSKEYUP, VK_RETURN, 0);
}

VOID Tool::sendF2ToWnd(HWND hwnd) {
    SendMessage(hwnd, WM_SYSKEYDOWN, VK_F2, 0);
    SendMessage(hwnd, WM_SYSKEYUP, VK_F2, 0);
}
