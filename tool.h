#pragma once
#include<qt_windows.h>
#include<QString>
#include <QtMultimedia/QMediaPlayer>
#include<QSound>
#include<iostream>
class Tool {

public:

    Tool();

    //公用功能函数
    BOOL QString2Wchar(QString buf, TCHAR **);
    BOOL QString2Char(QString buf, CHAR **);
    DWORD strlenUtf8(CONST CHAR *);
    BOOL writeCookies(LPCTSTR cookiesPath, int index);

    //播放音频
    void playSound(LPCSTR);

    //
    void setFocusHOS(BOOL flag);
    //
    BOOL compareRGB(COLORREF color, DWORD rgb);

    //检测鼠标
    VOID detectedCursor(CONST POINT *limitLeftTop, CONST POINT *limitRightBottom, CONST RECT *gameRect);

    //解除鼠标限制
    VOID relieveCursor();

    //发送虚拟按键
    VOID sendStringToWnd(HWND hwnd, LPCTSTR lpStr);
    VOID sendTabToWnd(HWND hwnd);
    VOID sendEnterToWnd(HWND hwnd);
    VOID sendF2ToWnd(HWND hwnd);
private:


    //限制鼠标 线程句柄
    HANDLE hThreadLimitCursor;

    struct LimitParams{
        POINT leftTop;
        POINT rightBottom;
    }limitParams;

    //限制鼠标
    VOID limitCursor(CONST POINT *limitLeftTop, CONST POINT *limitRrightBottom);

    //限制鼠标线程
    friend DWORD limitCursorThread(LPVOID);

};

