#include "control.h"


Control::Control(WId wid, QSettings *settings, Tool *tool, QWidget *father) {
    this->hwnd = (HWND)wid;
    this->hRestartupThread = 0;
    this->hHEQIANGSHIDAN = 0;
    this->settings = settings;
    this->tool = tool;
    this->father = father;
    this->manual = FALSE;
    this->fastPattern = FALSE;
    this->bHeqiangshidan = FALSE;
    this->iniConnect();
    this->setHotkey();
    this->readSettings();
    this->pDm = GetDmObject();
    this->bLuandouguaji = FALSE;
    this->waiting = FALSE;
}

Control::~Control() {
    this->relHotkey();
    /*将当前index写入配置文件*/

    settings->beginGroup("DevOption");
    settings->setValue(QString("CurrentIndex"), this->params.currentIndex);
    settings->endGroup();

    /*释放资源*/
    delete this->params.szExeFile;
    delete this->params.HOSPath;

    for (int i = 0; i < params.userInfos.length(); i++) {
        UserInfo userInfo = params.userInfos.at(i);
        delete userInfo.username;
        delete userInfo.password;
    }
}

DWORD Control::iniDM() {
    HWND hwnd = FindWindow(TEXT("Heroes of the Storm"), TEXT("《风暴英雄》"));
    if (!hwnd) {
        MessageBox(0, TEXT("未找到风暴英雄游戏窗口"), 0, 0);
        return 1;
    }
    this->pDm->BindWindow((long)hwnd, "gdi", "windows", "windows", 0);
    return 0;
}

DWORD Control::destroyDM() {
    this->pDm->UnBindWindow();
    return 0;
}


void Control::raiseCurrentIndex() {
    params.currentIndex++;
    if (params.currentIndex >= params.accountNum) {
        params.currentIndex = 0;
    }
}
void Control::dropCurrentIndex() {
    params.currentIndex--;
    if (params.currentIndex < 0) {
        params.currentIndex = params.accountNum - 1;
    }
}

int Control::getCurrentIndex() {
    return this->params.currentIndex;
}

void Control::iniConnect() {
    QObject::connect(
        this,
        SIGNAL(restartupThreadEnd(int)),
        this,
        SLOT(onRestartupThreadEnd(int))
    );
}

//注册大漠插件
Idmsoft *Control::GetDmObject() {
    Idmsoft *m_dm = NULL;
    bool m_bInit = false;
    //直接加载dll创建对象，避免进行注册文件
    typedef HRESULT(_stdcall *pfnGCO)(REFCLSID, REFIID, void **);
    pfnGCO fnGCO = NULL;
    HINSTANCE hdllInst = LoadLibrary(TEXT("dm.dll"));
    fnGCO = (pfnGCO)GetProcAddress(hdllInst, "DllGetClassObject");
    if (fnGCO != 0)
    {
        IClassFactory *pcf = NULL;
        HRESULT hr = (fnGCO)(__uuidof(dmsoft), IID_IClassFactory, (void **)&pcf);
        if (SUCCEEDED(hr) && (pcf != NULL))
        {
            hr = pcf->CreateInstance(NULL, __uuidof(Idmsoft), (void **)&m_dm);
            if ((SUCCEEDED(hr) && (m_dm != NULL)) == FALSE)
                return NULL;

        }
        pcf->Release();
        m_bInit = true;
    }
    else
        m_bInit = false;

    //dm->Reg(L"注册码", L"dass");//收费版本需要注册后才能使用
/*6.1538版本修改内存可直接调用*/
    DWORD pid = GetCurrentProcessId();
    int handle = (int)GetModuleHandle(TEXT("dm.dll"));
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    int value = 1;
    bool is = WriteProcessMemory(hProcess, LPVOID(handle + 1078240), &value, 1, NULL);

    return m_dm;

}

/*技能辅助线*************************************

**********************************************/

//void Control::paintCirle(int flag) {
//    this->paintFlag = flag;
//    HANDLE hThread = newThread((LPVOID)paintCircleThreadProc);
//}

void Control::heqiangshidan() {
    /*关闭参与线程*/
    if (!bHeqiangshidan) {
        bHeqiangshidan = TRUE;
        this->iniDM();
        hHEQIANGSHIDAN = newThread((LPVOID)heqiangshidanThreadProc);
        this->tool->playSound(":/HOS_Script/Resources/hqsd1.wav");
    }
    else {
        bHeqiangshidan = FALSE;
        this->destroyDM();
        this->tool->playSound(":/HOS_Script/Resources/hqsd2.wav");
        detectThreadHandle(&hHEQIANGSHIDAN);

    }
}
/*乱斗挂机送人头 接口函数*/
void Control::luandouguaji() {
    if (!this->bLuandouguaji) {
        this->bLuandouguaji = TRUE;
        this->hLUANDOUGUAJI = newThread((LPVOID)luandouguajiThreadProc);
        this->tool->playSound(":/HOS_Script/Resources/ah.wav");
    }
    else {
        this->bLuandouguaji = FALSE;
        this->tool->playSound(":/HOS_Script/Resources/mua.wav");
        detectThreadHandle(&hLUANDOUGUAJI);
    }
}
//BOOL Control::isPainting() {
//    return this->painting;
//}
//void Control::setPainting(BOOL flag) {
//    this->painting = flag;
//    if (flag == FALSE) {
//        HWND hwnd = FindWindow(TEXT("Heroes of the Storm"), TEXT("《风暴英雄》"));
//        if (hwnd) {
//            //SendMessage(hwnd, WM_KEYDOWN, VK_SPACE, 0);
//            keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);
//        }
//    }
//}

void Control::setManual(BOOL b) {
    this->manual = b;
}

void Control::setFastPattern(BOOL b) {
    this->fastPattern = b;
}


//
//DWORD paintCircleThreadProc(LPVOID lpParams) {
//    Control *pControl = (Control *)lpParams;
//    HWND hwnd = FindWindow(TEXT("Heroes of the Storm"), TEXT("《风暴英雄》"));
//    if (hwnd) {
//        //SendMessage(hwnd, WM_KEYDOWN, VK_SPACE, 0);
//        keybd_event(VK_SPACE, 0, 0, 0);
//        int left, top, right, bottom;
//        if (pControl->paintFlag == 1) {
//            left = pControl->params.smallPaintLeft;
//            top = pControl->params.smallPaintTop;
//            right = pControl->params.smallPaintRight;
//            bottom = pControl->params.smallPaintBottom;
//
//        }
//        else {
//            left = pControl->params.largePaintLeft;
//            top = pControl->params.largePaintTop;
//            right = pControl->params.largePaintRight;
//            bottom = pControl->params.largePaintBottom;
//
//        }
//        pControl->setPainting(TRUE);
//        while (pControl->isPainting()) {
//            PAINTSTRUCT ps;
//            //HDC  hdc = BeginPaint(hwnd, &ps);
//            HDC hdc = GetDC(hwnd);
//            HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
//            /*透明画刷，绘制空心圆*/
//            HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
//            SelectObject(hdc, hPen);
//            SelectObject(hdc, hBrush);
//            Ellipse(hdc, left, top, right, bottom);
//
//            DeleteObject(hPen);
//            DeleteObject(hBrush);
//            ReleaseDC(hwnd, hdc);
//
//            //EndPaint(hwnd, &ps);
//        }
//    }
//    return 0;
//}

/*技能辅助线end*************************************

**********************************************/

bool Control::setHotkey() {
    /*注册CTRL + 1*/
    BOOL ret = false;
    if (!RegisterHotKey(
        hwnd,
        this->CTRL_1,
        MOD_CONTROL,
        0x31
    )) {
        MessageBox(hwnd, TEXT("RegisterHotKey Error:CTRL_1\n"), TEXT("ERROR!"), MB_OK | MB_ICONEXCLAMATION);
        return false;
    }
    /*注册CTRL + 2*/
    if (!RegisterHotKey(
        hwnd,
        this->CTRL_2,
        MOD_CONTROL,
        0x32
    )) {
        MessageBox(hwnd, TEXT("RegisterHotKey Error:CTRL_2\n"), TEXT("ERROR!"), MB_OK | MB_ICONEXCLAMATION);
        return false;
    }
    /*注册CTRL + 3*/
    if (!RegisterHotKey(
        hwnd,
        this->CTRL_3,
        MOD_CONTROL,
        0x33
    )) {
        MessageBox(hwnd, TEXT("RegisterHotKey Error:CTRL_3\n"), TEXT("ERROR!"), MB_OK | MB_ICONEXCLAMATION);
        return false;
    }
    /*注册CTRL + 4*/
    if (!RegisterHotKey(
        hwnd,
        this->CTRL_4,
        MOD_CONTROL,
        0x34
    )) {
        MessageBox(hwnd, TEXT("RegisterHotKey Error:CTRL_4\n"), TEXT("ERROR!"), MB_OK | MB_ICONEXCLAMATION);
        return false;
    }
    /*注册CTRL + F7*/
    if (!RegisterHotKey(
        hwnd,
        this->CTRL_F7,
        MOD_CONTROL,
        VK_F7
    )) {
        MessageBox(hwnd, TEXT("RegisterHotKey Error:CTRL_F7\n"), TEXT("ERROR!"), MB_OK | MB_ICONEXCLAMATION);
        return false;
    }
    /*注册CTRL + F1*/
    if (!RegisterHotKey(
        hwnd,
        this->CTRL_F1,
        MOD_CONTROL,
        VK_F1
    )) {
        MessageBox(hwnd, TEXT("RegisterHotKey Error:CTRL_F7\n"), TEXT("ERROR!"), MB_OK | MB_ICONEXCLAMATION);
        return false;
    }
    /*注册CTRL + F2*/
    if (!RegisterHotKey(
        hwnd,
        this->CTRL_F2,
        MOD_CONTROL,
        VK_F2
    )) {
        MessageBox(hwnd, TEXT("RegisterHotKey Error:CTRL_F7\n"), TEXT("ERROR!"), MB_OK | MB_ICONEXCLAMATION);
        return false;
    }
    return true;
}

void Control::relHotkey() {
    if (!UnregisterHotKey(hwnd, this->CTRL_1)) {
        MessageBox(hwnd, TEXT("UnregisterHotKey:CTRL_1 Error\n"), NULL, MB_OK | MB_ICONEXCLAMATION);
    }
    if (!UnregisterHotKey(hwnd, this->CTRL_2)) {
        MessageBox(hwnd, TEXT("UnregisterHotKey:CTRL_2 Error\n"), NULL, MB_OK | MB_ICONEXCLAMATION);
    }
    if (!UnregisterHotKey(hwnd, this->CTRL_3)) {
        MessageBox(hwnd, TEXT("UnregisterHotKey:CTRL_3 Error\n"), NULL, MB_OK | MB_ICONEXCLAMATION);
    }
    if (!UnregisterHotKey(hwnd, this->CTRL_4)) {
        MessageBox(hwnd, TEXT("UnregisterHotKey:CTRL_4 Error\n"), NULL, MB_OK | MB_ICONEXCLAMATION);
    }
    if (!UnregisterHotKey(hwnd, this->CTRL_F7)) {
        MessageBox(hwnd, TEXT("UnregisterHotKey:CTRL_F7 Error\n"), NULL, MB_OK | MB_ICONEXCLAMATION);
    }
    if (!UnregisterHotKey(hwnd, this->CTRL_F1)) {
        MessageBox(hwnd, TEXT("UnregisterHotKey:CTRL_F1 Error\n"), NULL, MB_OK | MB_ICONEXCLAMATION);
    }
    if (!UnregisterHotKey(hwnd, this->CTRL_F2)) {
        MessageBox(hwnd, TEXT("UnregisterHotKey:CTRL_F2 Error\n"), NULL, MB_OK | MB_ICONEXCLAMATION);
    }
}


void Control::readSettings() {
    settings->setIniCodec(QTextCodec::codecForName("utf-8"));
    settings->beginGroup("DevOption");

    tool->QString2Wchar(settings->value("HOSPath").toString(), &params.HOSPath);

    tool->QString2Wchar(
        settings->value("szExeFile").toString(),
        &params.szExeFile
    );
    tool->QString2Wchar(
        settings->value("CookiesPath").toString(),
        &params.CookiesPath
    );

    params.currentIndex = settings->value("CurrentIndex", 0).toInt();
    settings->endGroup();

    /*绘图用参数*/
    settings->beginGroup("paintSettings");
    params.smallPaintLeft = settings->value("smallPaintLeft", 0).toInt();
    params.smallPaintTop = settings->value("smallPaintTop", 0).toInt();
    params.smallPaintRight = settings->value("smallPaintRight", 0).toInt();
    params.smallPaintBottom = settings->value("smallPaintBottom", 0).toInt();
    params.largePaintLeft = settings->value("largePaintLeft", 0).toInt();
    params.largePaintTop = settings->value("largePaintTop", 0).toInt();
    params.largePaintRight = settings->value("largePaintRight", 0).toInt();
    params.largePaintBottom = settings->value("largePaintBottom", 0).toInt();
    settings->endGroup();

    /*挂机用*/
    settings->beginGroup("waitSettings");
    params.waitRect.left = settings->value("waitLeft").toInt();
    params.waitRect.top = settings->value("waitTop").toInt();
    params.waitRect.right = settings->value("waitRight").toInt();
    params.waitRect.bottom = settings->value("waitBottom").toInt();
    /*游戏登录取色*/
    params.RGBPointerX1 = settings->value("RGBPointerX1").toInt();
    params.RGBPointerY1 = settings->value("RGBPointerY1").toInt();
    params.mRGB1 = settings->value("RGB1").toInt();
    /*游戏准备取色*/
    params.RGBPointerX2 = settings->value("RGBPointerX2").toInt();
    params.RGBPointerY2 = settings->value("RGBPointerY2").toInt();
    params.mRGB2 = settings->value("RGB2").toInt();
    settings->endGroup();

    /*游戏用*/
    settings->beginGroup("gameSettings");
    params.gameRect.left = settings->value("gameLeft").toInt();
    params.gameRect.top = settings->value("gameTop").toInt();
    params.gameRect.right = settings->value("gameRight").toInt();
    params.gameRect.bottom = settings->value("gameBottom").toInt();
    params.limitLeftTop.x = settings->value("limitLeft").toInt();
    params.limitRightBottom.x = settings->value("limitRight").toInt();
    params.limitLeftTop.y = settings->value("limitTop").toInt();
    params.limitRightBottom.y = settings->value("limitBottom").toInt();
    params.heqiangshidanquanping = settings->value("heqiangshidanquanping").toInt();
    settings->endGroup();


    /*帐号信息读取*/
    settings->beginReadArray("logins");
    params.accountNum = settings->value("size").toInt();

    for (int i = 0; i < params.accountNum; i++) {
        settings->setArrayIndex(i);
        UserInfo info;
        /*QString转TCHAR* */
        tool->QString2Wchar(settings->value("username").toString(), &info.username);
        tool->QString2Wchar(settings->value("password").toString(), &info.password);
        params.userInfos.append(info);
    }

    settings->endArray();
}


/*进程相关功能函数*******************************************************
/
/*获取指定名字进程的id，用于打开进程*/
BOOL Control::getProcID(LPCTSTR lpProcName, DWORD *pdwProcID) {
    HANDLE hSnapshot;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapshot == INVALID_HANDLE_VALUE) {
        MessageBox(hwnd, TEXT("CreateToolhelp32Snapshot Error\n"), NULL, MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    if (Process32First(hSnapshot, &entry) == TRUE) {
        do {
            if (wcscmp(entry.szExeFile, lpProcName) == 0) {
                CloseHandle(hSnapshot);
                *pdwProcID = entry.th32ProcessID;
                return TRUE;
            }
        } while (Process32Next(hSnapshot, &entry) == TRUE);
    }
    return FALSE;
}

/*功能函数，创建子进程*/
BOOL Control::createProc(HANDLE *hProc, LPCTSTR szFileName) {
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);


    //strcpy_s(str, size, szFileName);
    if (CreateProcess(
        szFileName,
        NULL,
        NULL,
        NULL,
        FALSE,
        CREATE_UNICODE_ENVIRONMENT,
        NULL,
        NULL,
        &si,
        &pi
    )) {
        *hProc = pi.hProcess;
        CloseHandle(pi.hThread);
        //delete str;
        return TRUE;
    }
    //delete str;
    return FALSE;
}


/*根据进程杀死进程*/
BOOL Control::killProc(DWORD procID, UINT uExitCode) {
    HANDLE hProc = OpenProcess(
        PROCESS_ALL_ACCESS,
        FALSE,
        procID
    );
    DWORD ret = GetLastError();
    if (!hProc) {
        MessageBox(hwnd, TEXT("OpenProcess Error\n"), NULL, MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    if (TerminateProcess(hProc, uExitCode)) {
        return TRUE;
    }

    return FALSE;
}
BOOL Control::isWaiting() {
    return waiting;
}
/*关游戏进程*/
VOID Control::killHOSProc() {
    DWORD HOSProcID;
    if (this->getProcID(this->params.szExeFile, &HOSProcID)) {
        BOOL ret = this->killProc(HOSProcID, 0);
        if (!ret) {
            MessageBox(this->hwnd, TEXT("killProc Error\n"), NULL, MB_OK | MB_ICONEXCLAMATION);
        }
    }
}

/****************************************************************************************/

/*功能函数*/

void Control::detectThreadHandle(HANDLE *hThread) {
    if (*hThread) {
        TerminateThread(*hThread, 0);
        CloseHandle(*hThread);
        *hThread = 0;
    }
}

void Control::detectCookies() {
    tool->writeCookies(params.CookiesPath, params.currentIndex);
}

/*创建线程*/
HANDLE Control::newThread(PVOID pFunc) {
    /*把当前对象地址传进参数，友元不属于类成员*/
    HANDLE hThread = CreateThread(
        NULL,
        0,
        (LPTHREAD_START_ROUTINE)pFunc,
        this,
        0,
        NULL
    );

    if (!hThread) {
        MessageBox(hwnd, TEXT("CreateThread Error\n"), NULL, MB_OK | MB_ICONEXCLAMATION);
    }

    return hThread;
}

VOID Control::moveToWaitOrGame(HWND hwnd, DWORD flags) {
    HWND insertAfter = 0;
    UINT paramFlag = 0;
    BOOL focusFlag = FALSE;
    RECT rect;

    /*将要切换到game模式*/
    if (flags == 0) {
        //insertAfter = HWND_TOPMOST;
        //insertAfter = HWND_TOP;
        //paramFlag = SWP_SHOWWINDOW;
        //insertAfter = HWND_BOTTOM;
        focusFlag = TRUE;
        rect = params.gameRect;
        this->waiting = FALSE;
    }/*将要切换到wait模式*/
    else if (flags == 1) {
        //insertAfter = HWND_NOTOPMOST;
        insertAfter = HWND_BOTTOM;
        paramFlag = SWP_NOACTIVATE;
        focusFlag = FALSE;
        rect = params.waitRect;
        this->waiting = TRUE;
    }

    SetWindowPos(
        hwnd,
        insertAfter,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        paramFlag
    );

    tool->setFocusHOS(focusFlag);
}


/**************************************************************************************/


/**************************     热键接口函数     **************************/


/**************************************************************************************/



/*CTRL_1 接口 移动窗口*/
void Control::moveWindow() {
    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
    RECT rect;
    HWND hwndHOS;
    hwndHOS = FindWindow(TEXT("Heroes of the Storm"), TEXT("《风暴英雄》"));
    if (hwndHOS) {
        BOOL ret = GetWindowRect(hwndHOS, &rect);
        if (!ret) {
            MessageBox(hwnd, TEXT("GetWindowRect Error\n"), NULL, MB_OK);
            return;
        }
        RECT rectWait, rectGame;
        rectWait = params.waitRect;
        rectGame = params.gameRect;

        if (rect.left != rectGame.left
            || rect.right != rectGame.right
            || rect.top != rectGame.top
            || rect.bottom != rectGame.bottom) {
            this->detectThreadHandle(&hRestartupThread);
            this->moveToWaitOrGame(hwndHOS, 0);
            //emit needPlaySound(2);
        }
        else {
            this->moveToWaitOrGame(hwndHOS, 1);
        }
        tool->detectedCursor(&params.limitLeftTop, &params.limitRightBottom, &params.gameRect);
    }

}

/*CTRL_3 接口*/
void Control::quitHOS() {
    this->pDm->UnBindWindow();
    this->detectThreadHandle(&hRestartupThread);
    /*关闭不检测，直接接触限制*/
    tool->relieveCursor();

    HANDLE hThread = newThread((PVOID)killHOSThreadProc);
    if (hThread) {
        CloseHandle(hThread);
    }
}



/*CTRL_4 接口*/
void Control::restartupHOS() {
    /*重开不检测，直接接触限制*/
    tool->relieveCursor();

    detectThreadHandle(&this->hRestartupThread);
    this->hRestartupThread = newThread((PVOID)restartupHOSThreadProc);

}


/*CTRL_F7 接口*/
void Control::inputAccount() {
    HANDLE hThread = newThread((PVOID)inputAccountThreadProc);
    if (hThread) {
        CloseHandle(hThread);
    }
    ///*调试*/
    //HWND hwndHOS = FindWindow(TEXT("Heroes of the Storm"), TEXT("《风暴英雄》"));
    //pDm->BindWindow((long)hwndHOS, "gdi", "windows", "windows", 0);

    //pDm->SetWindowSize((long)hwndHOS, 1040, 759);
    //pDm->UnBindWindow();
}


/**************************************************************************************/


/**************************                                 **************************/


/**************************************************************************************/


/*槽函数*/
//重启完毕执行的函数
void Control::onRestartupThreadEnd(int flag) {
    if (flag == 0) {
        CloseHandle(hRestartupThread);
        hRestartupThread = 0;
        /*延迟5秒重开*/
        Sleep(5000);
        restartupHOS();
    }
    else if (flag == 1) {
        CloseHandle(hRestartupThread);
        hRestartupThread = 0;
        /*手动模式结束时播放提示音*/
        this->tool->playSound(":/HOS_Script/Resources/mua.wav");
    }

}
