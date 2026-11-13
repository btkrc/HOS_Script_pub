#include "control.h"
/*CTRL_3 关闭 友元线程函数*/
DWORD killHOSThreadProc(LPVOID lpParams) {
    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
    Control *pControl = static_cast<Control *>(lpParams);
    pControl->killHOSProc();
    return 0;
}

/*CTRL_4 重新启动 友元线程函数*/
DWORD restartupHOSThreadProc(LPVOID lpParams) {
    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
    SetProcessDPIAware();
    Control *pControl = static_cast<Control *>(lpParams);
    Tool *tool = pControl->tool;
    DWORD curAccount;
    /*获取屏幕HDC*/
    HDC hdc = GetDC(NULL);
    HANDLE hGame;
    DWORD HOSProcID;
    // 执行时长记数，用于发生意外时退出线程
    DWORD secondCound;
    // 重新点击开始记数
    DWORD reclickCound;
    HWND hwndHOS;
    /*上一窗口句柄*/
    HWND old;
    /*加载动画*/
    BOOL loadover = FALSE;

    hwndHOS = FindWindow(TEXT("Heroes of the Storm"), TEXT("《风暴英雄》"));
    /*关闭前先判断是否还有游戏进程*/
    if (hwndHOS) {
        /*移动窗口*/
        pControl->moveToWaitOrGame(hwndHOS, 1);
    }

    /*如果遍历到ID，则将目标进程终止。*/
    pControl->killHOSProc();
    Sleep(3000);

    /*检测Cookies*/
    pControl->detectCookies();

    if (pControl->createProc(&hGame, pControl->params.HOSPath)) {
        CloseHandle(hGame);
    }

    /*检测游戏窗口*/
    while (TRUE) {
        hwndHOS = FindWindow(TEXT("Heroes of the Storm"), TEXT("《风暴英雄》"));
        if (hwndHOS) {
            pControl->pDm->BindWindow((long)hwndHOS, "gdi", "windows", "windows", 0);
            RECT rect;
            GetWindowRect(hwndHOS, &rect);
            /*绕过加载窗口*/
            if ((rect.right - rect.left) > 700 && (rect.bottom - rect.top) > 500) {
                pControl->moveToWaitOrGame(hwndHOS, 1);
                break;
            } else {
                Sleep(1000);
                continue;
            }
        } else {
            Sleep(1000);
            continue;
        }
    }

    /*判断登陆按钮*/
    secondCound = 0;
    while (TRUE) {

        /*国服*/

        // 判断登录按钮
        if (!pControl->pDm->CmpColor(463, 588, "599afd", 1)) {

            /*等待游戏加载*/
            loadover = FALSE;
            while (TRUE) {
                /*等待加载动画的圈转完，每秒检测一次
                如果不等待直接进入可能会失败*/
                for (int i = 0; i < 10; i++) {
                    if (!pControl->pDm->CmpColor(534, 384, "182089", 1)) {
                        if (!pControl->pDm->CmpColor(517, 417, "221678", 1)) {
                            if (!pControl->pDm->CmpColor(488, 388, "222590", 1)) {
                                loadover = TRUE;
                            }
                        }
                    }
                    Sleep(100);
                }
                if (loadover) {
                    break;
                }
            }

            //    /*国际服可能登录页面异常*/
            if (!pControl->pDm->CmpColor(463, 588, "15171e", 1) || !pControl->pDm->CmpColor(463, 588, "599afd", 1)) {

                /*等待游戏加载*/
                loadover = FALSE;
                while (TRUE) {
                    /*等待加载动画的圈转完，每秒检测一次
                    如果不等待直接进入可能会失败*/
                    for (int i = 0; i < 10; i++) {
                        if (!pControl->pDm->CmpColor(534, 384, "15171e", 1) ||
                            !pControl->pDm->CmpColor(534, 384, "141b73", 1)) {
                            if (!pControl->pDm->CmpColor(517, 417, "15171e", 1) ||
                                !pControl->pDm->CmpColor(517, 417, "1c1264", 1)) {
                                if (!pControl->pDm->CmpColor(488, 388, "15171e", 1) ||
                                    !pControl->pDm->CmpColor(488, 388, "1d207a", 1)) {
                                    loadover = TRUE;
                                }
                            }
                        }
                        Sleep(100);
                    }
                    if (loadover) {
                        break;
                    }
                }

                /*加载完毕后延迟2秒进入游戏*/
                Sleep(2000);
                curAccount = pControl->params.currentIndex;
#ifdef MYDEBUG
                qDebug() << "pre submit:" << pControl->params.currentIndex << '\n';
#endif // MYDEBUG
                tool->sendTabToWnd(hwndHOS);
                tool->sendStringToWnd(hwndHOS, pControl->params.userInfos.at(curAccount).username);
                tool->sendTabToWnd(hwndHOS);
                tool->sendStringToWnd(hwndHOS, pControl->params.userInfos.at(curAccount).password);
                tool->sendEnterToWnd(hwndHOS);
                /*指向下一帐号*/
                pControl->raiseCurrentIndex();
                Sleep(2000);

                break;
            } // 超过时间没有检测到登录按钮
            if (secondCound >= SECONDFORWAITHOS) {
                pControl->pDm->UnBindWindow();
                /*发送信号，不管是否手动状态都重新启动*/
                emit pControl->restartupThreadEnd(0);
#ifdef MYDEBUG
                qDebug() << L"login timeout:" << pControl->params.currentIndex << '\n';
#endif // MYDEBUG
                return 0;
            }
            /*延迟1秒后重新检测*/
            Sleep(1000);
            secondCound++;
        }

        /*检测游戏是否进入*/
        secondCound = 0;
        while (TRUE) {
            if (!pControl->pDm->CmpColor(64, 20, "9793fd", 1)) {
                break;
            }
            // 超时退出
            if (secondCound >= SECONDFORWAITHOS) {
                pControl->dropCurrentIndex();
                pControl->pDm->UnBindWindow();
                /*发送信号，不管是否手动状态都重新启动*/
                emit pControl->restartupThreadEnd(0);
                return 0;
            }
            secondCound++;
            Sleep(1000);
        }

        /*选择模式*/
        secondCound = 0;
        while (TRUE) {
            if (secondCound >= SECONDFORWAITHOS) {
                pControl->dropCurrentIndex();
                pControl->pDm->UnBindWindow();
                /*发送信号，标记是否是手动状态*/
                if (!pControl->manual) {
                    emit pControl->restartupThreadEnd(0);
                } else {
                    emit pControl->restartupThreadEnd(1);
                }
                return 0;
            }
            /*天命乱斗*/
            if (!pControl->fastPattern) {
                tool->sendF2ToWnd(hwndHOS);
                old = GetForegroundWindow();
                pControl->pDm->SetWindowState((long)hwndHOS, 1);
                pControl->pDm->MoveTo(238, 60);
                pControl->pDm->LeftClick();
                pControl->pDm->SetWindowState((long)old, 1);
                secondCound++;
                Sleep(1000);
                // 最后确认模式
                if (!pControl->pDm->CmpColor(238, 60, "d1e4f9", 0.9)) {
                    break;
                } /*新号可能会有五角心*/
                else if (!pControl->pDm->CmpColor(252, 62, "d4e3f8", 0.9)) {
                    break;
                }

            }
            /*快速模式*/
            else if (pControl->fastPattern) {
                tool->sendF2ToWnd(hwndHOS);
                old = GetForegroundWindow();
                pControl->pDm->SetWindowState((long)hwndHOS, 1);
                pControl->pDm->MoveTo(78, 60);
                pControl->pDm->LeftClick();
                Sleep(500);
                pControl->pDm->MoveTo(501, 352);
                pControl->pDm->LeftClick();
                Sleep(500);
                /*选择随机角色*/
                pControl->pDm->MoveTo(179, 163);
                pControl->pDm->LeftClick();
                Sleep(500);
                pControl->pDm->MoveTo(501, 352);
                pControl->pDm->LeftClick();
                pControl->pDm->SetWindowState((long)old, 1);
                Sleep(1500);
                // 最后确认模式
                if (!pControl->pDm->CmpColor(74, 62, "dbe9fe", 0.9)) {
                    break;
                } /*信号旁边有个五角星，重新检测快速模式*/
                else if (!pControl->pDm->CmpColor(93, 66, "cdd4ee", 0.9)) {
                    break;
                }
                secondCound += 3;
            }
        }

        /*快速模式&&手动模式下不点击开始，直接返回*/
        if (pControl->manual && pControl->fastPattern) {
            pControl->pDm->UnBindWindow();
            emit pControl->restartupThreadEnd(1);
            return 0;
        }

        /*检测开始按钮*/
        secondCound = 0;
        reclickCound = 0;
        while (TRUE) {
            // 判断开始按钮
            if (!pControl->pDm->CmpColor(494, 651, "bccafe", 0.9)) {
                old = GetForegroundWindow();
                pControl->pDm->SetWindowState((long)hwndHOS, 1);
                pControl->pDm->MoveTo(420, 655);
                pControl->pDm->LeftClick();
                pControl->pDm->SetWindowState((long)old, 1);
                /*手动直接退出*/
                if (pControl->manual) {
                    emit pControl->restartupThreadEnd(1);
                    pControl->pDm->UnBindWindow();
                    return 0;
                }
                while (TRUE) {
                    Sleep(200);
                    /*判断游戏是否进入
                    1. 游戏进入黑屏
                    2. 天命乱斗选人界面
                    3. 快速游戏英雄边框*/
                    if (!pControl->pDm->CmpColor(490, 590, "010101", 0.9) ||
                        !pControl->pDm->CmpColor(490, 590, "ffffff", 0.9) ||
                        !pControl->pDm->CmpColor(87, 601, "bce0ee", 0.9)) {
                        pControl->pDm->UnBindWindow();
                        emit pControl->restartupThreadEnd(0);
#ifdef MYDEBUG
                        qDebug() << "normal restartup:" << pControl->params.currentIndex << '\n';
#endif // MYDEBUG
                        return 0;
                    } // 重新点击开始
                    else if (!pControl->pDm->CmpColor(494, 651, "bccafe", 0.9)) {
                        /*重新点击后延迟一秒，如果十次都失败则退出线程*/
                        if (reclickCound >= 10) {
                            /*游戏未开始，退回上一帐号*/
                            pControl->dropCurrentIndex();
                            pControl->pDm->UnBindWindow();
                            /*发送信号，标记是否是手动状态*/
                            if (!pControl->manual) {
                                emit pControl->restartupThreadEnd(0);
                            } else {
                                emit pControl->restartupThreadEnd(1);
                            }
                            return 0;
                        }
                        HWND old = GetForegroundWindow();
                        pControl->pDm->SetWindowState((long)hwndHOS, 1);
                        pControl->pDm->MoveTo(492, 666);
                        pControl->pDm->LeftClick();
                        pControl->pDm->SetWindowState((long)old, 1);
                        reclickCound++;
                        Sleep(1000);
                    }
                }
            }
            // 超过时间没有检测到开始按钮
            else if (secondCound >= SECONDFORWAITHOS) {
                pControl->dropCurrentIndex();
                pControl->pDm->UnBindWindow();
                /*发送信号，标记是否是手动状态*/
                if (!pControl->manual) {
                    emit pControl->restartupThreadEnd(0);
                } else {
                    emit pControl->restartupThreadEnd(1);
                }
#ifdef MYDEBUG
                qDebug() << "startup timeout:" << pControl->params.currentIndex << '\n';
#endif // MYDEBUG
                return 0;
            }
            secondCound++;
            Sleep(1000);
        }
        pControl->pDm->UnBindWindow();
        emit pControl->restartupThreadEnd(0);
        return 0;
    }

    /*Ctrl + F7输入帐号线程函数*/
    DWORD inputAccountThreadProc(LPVOID lpParams) {
        Control *pControl = (Control *)lpParams;
        DWORD curAccount = pControl->params.currentIndex;
        HWND hwndHOS = FindWindow(TEXT("Heroes of the Storm"), TEXT("《风暴英雄》"));
        if (hwndHOS) {
            SendMessage(hwndHOS, WM_SYSKEYUP, VK_CONTROL, 0);
            pControl->tool->sendTabToWnd(hwndHOS);
            pControl->tool->sendStringToWnd(hwndHOS, pControl->params.userInfos.at(curAccount).username);
            pControl->tool->sendTabToWnd(hwndHOS);
            pControl->tool->sendStringToWnd(hwndHOS, pControl->params.userInfos.at(curAccount).password);
            pControl->tool->sendEnterToWnd(hwndHOS);
            /*指向下一帐号*/
            pControl->raiseCurrentIndex();
        }

        return 0;
    }

    /* Ctrl F1 荷枪实弹线程函数*/
    DWORD heqiangshidanThreadProc(LPVOID lpParams) {
        Control *pControl = (Control *)lpParams;
        while (TRUE) {
            if ((!pControl->pDm->CmpColor(742, 754, "3ffe7a", 1)) ||
                (!pControl->pDm->CmpColor(530, 631, "baf7fe", 1))) {
                Sleep(380);
                pControl->pDm->KeyPress(68);
            }
        }
        return 0;
    }

    /*ctrl F2 乱斗单号挂机送人头 线程函数*/
    DWORD luandouguajiThreadProc(LPVOID lpParams) {
        Control *pControl = (Control *)lpParams;
        VARIANT x, y;
        DWORD result = 0;
        HANDLE hwndHOS = FindWindow(TEXT("Heroes of the Storm"), TEXT("《风暴英雄》"));
        VariantInit(&x);
        VariantInit(&y);
        int INTx, INTy = 0;
        HWND old = GetForegroundWindow();
        int picLeft, picRight, picTop, picBottom;
        /*开始查找敌方基地相对坐标*/
        while (TRUE) {
            //    /*末日塔*/
            result = pControl->pDm->FindPic(951, 664, 974, 685, "./1.bmp", "202020", 0.5, 0, &x, &y);
            if (result != -1) {
                /*大漠的变参需要强转后使用*/
                INTx = (_variant_t)x;
                INTy = (_variant_t)y;
                break;
            }
            result = pControl->pDm->FindPic(1151, 662, 1175, 684, "./1.bmp", "202020", 0.5, 0, &x, &y);
            if (result != -1) {
                INTx = (_variant_t)x;
                INTy = (_variant_t)y;
                break;
            }
        }

        picLeft = 717;
        picTop = 495;
        picRight = 1009;
        picBottom = 708;

        while (TRUE) {
            result = pControl->pDm->FindPic(604, 397, 1018, 713, "./4.bmp", "000000", 0.7, 0, &x, &y);
            if (result != -1) {
                break;
            }
        }
        while (TRUE) {
            pControl->pDm->SetWindowState((long)hwndHOS, 1);
            pControl->pDm->MoveTo((int)(_variant_t)x, (int)(_variant_t)y);
            pControl->pDm->RightClick();
            pControl->pDm->SetWindowState((long)old, 1);
            INTx = (_variant_t)x;
            INTy = (_variant_t)y;
            qDebug() << INTx << "   ,   " << INTy;
            Sleep(2000);
        }

        while (TRUE) {
            result = pControl->pDm->FindColor(604, 397, 1018, 713, "473352", 1, 0, &x, &y);
            if (result) {
                break;
            }
        }
        while (TRUE) {
            pControl->pDm->SetWindowState((long)hwndHOS, 1);
            pControl->pDm->MoveTo((int)(_variant_t)x, (int)(_variant_t)y);
            pControl->pDm->RightClick();
            pControl->pDm->SetWindowState((long)old, 1);
            INTx = (_variant_t)x;
            INTy = (_variant_t)y;
            qDebug() << INTx << "   ,   " << INTy;
            Sleep(2000);
        }

        return 0;
    }