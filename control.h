#pragma once
#include<qt_windows.h>
#include<QWindow>
#include<QSettings>
#include<QDebug>
#include<QList>
#include<Tlhelp32.h>
#include<QTextCodec>
#include"tool.h"
#include"dm.tlh"
//#include<atlconv.h>

/*调试*/
//#define MYDEBUG

/*等待3分钟*/
#define SECONDFORWAITHOS 120
#define Q2W (TCHAR *)reinterpret_cast<const TCHAR *>

/*编码相关*/
//#ifdef UNICODE
//#define strcmp wcscmp
//#define toStdString toStdWString
//#else
//#define strcmp strcmp
//#define toStdString toStdString
//#endif // UNICODE


class Control :public QObject {

    Q_OBJECT
public:
    Control(WId, QSettings *, Tool *,QWidget *father);
    ~Control();

    enum Hotkey {
        CTRL_1 = 1,
        CTRL_2 = 2,
        CTRL_3 = 3,
        CTRL_4 = 4,
        CTRL_F7 = 5,
        CTRL_F1 = 6,
        CTRL_F2 = 7
    };

    /*大漠绑定窗口*/
    DWORD iniDM();
    DWORD destroyDM();

    /*检测窗口是否在右上角*/
    BOOL isWaiting();

    //接口 用于标记帐号index
    void raiseCurrentIndex();
    void dropCurrentIndex();
    int getCurrentIndex();

    //CTRL_1 接口
    void moveWindow();

    //CTRL_3 接口
    void quitHOS();

    //CTRL_4 接口
    void restartupHOS();

    //CTRL_F7 接口
    void inputAccount();

    //技能辅助线
    void paintCirle(int flag);
    //猎空荷枪实弹
    void heqiangshidan();
    void setManual(BOOL b);
    void setFastPattern(BOOL b);
    void luandouguaji();

    Tool *tool;



private:
    bool setHotkey();
    void relHotkey();
    void iniConnect();

    QWidget *father;
    BOOL waiting;
    //注册大漠插件
    Idmsoft *GetDmObject();
    //大漠实例指针
    Idmsoft *pDm;
    HWND hwnd;
    QSettings *settings;
    HANDLE hRestartupThread;
    HANDLE hHEQIANGSHIDAN;
    HANDLE hLUANDOUGUAJI;

    //手动按钮用
    BOOL manual;
    BOOL fastPattern;
    //BOOL painting;
    BOOL bHeqiangshidan;
    //int paintFlag;
    BOOL bLuandouguaji;

    //工具类，提供功能函数

    //配置用变量
    struct UserInfo {
        TCHAR *username;
        TCHAR *password;
    };

    //挂机用参数
    struct Params {
        int currentIndex;
        TCHAR *HOSPath;
        TCHAR *szExeFile;
        TCHAR *CookiesPath;
        RECT waitRect;

        /*游戏登陆取色*/
        int RGBPointerX1;
        int RGBPointerY1;
        int mRGB1;

        /*游戏准备取色*/
        int RGBPointerX2;
        int RGBPointerY2;
        int mRGB2;

        /*绘图参数*/
        //int smallPaintLeft;
        //int smallPaintTop;
        //int smallPaintRight;
        //int smallPaintBottom;
        //int largePaintLeft;
        //int largePaintTop;
        //int largePaintRight;
        //int largePaintBottom;

        /*游戏用*/
        RECT gameRect;
        POINT limitLeftTop;
        POINT limitRightBottom;
        int heqiangshidanquanping;

        int accountNum;

        /*帐号和密码*/
        QList<UserInfo> userInfos;

    } params;

    /*功能函数*/

    //检测线程状态
    void detectThreadHandle(HANDLE* hThread);
    
    //检测Cookies
    void detectCookies();


    //开新线程
    HANDLE newThread(PVOID pFunc);
    //移动窗口
    VOID moveToWaitOrGame(HWND, DWORD flags);

    //读取配置信息
    void readSettings();

    //进程相关功能函数
    BOOL getProcID(LPCTSTR, DWORD *);
    BOOL killProc(DWORD procID, UINT uExitCode);
    VOID killHOSProc();
    BOOL createProc(HANDLE *hProc, LPCTSTR szFileName);

    /*全局热键的线程函数，线程函数格式是固定的*/
    //CTRL_4 线程
    friend DWORD restartupHOSThreadProc(LPVOID);

    //CTRL_3 线程
    friend DWORD killHOSThreadProc(LPVOID);
    //CTRL_F7 线程
    friend DWORD inputAccountThreadProc(LPVOID);
    //绘图
    friend DWORD paintCircleThreadProc(LPVOID);
    //荷枪实弹线程
    friend DWORD heqiangshidanThreadProc(LPVOID);
    friend DWORD luandouguajiThreadProc(LPVOID);

private slots:
    void onRestartupThreadEnd(int flag);
signals:
    void needPlaySound(int);
    void restartupThreadEnd(int flag);

};