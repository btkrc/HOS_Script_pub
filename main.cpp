#include "HOS_Script.h"
#include <QtWidgets/QApplication>
#include<atlconv.h>

#define MUTEXTOKEN TEXT("防止多开")
#define MUTEXTOKEN_ERROR 1
#define MUTEXTOKEN_EXISTS 2

/*互斥对象在最后一个句柄被关闭时销毁
不要CloseHandle*/
DWORD preventMultiple(LPCTSTR mutex) {
    HANDLE hMutex;
    hMutex = CreateMutex(NULL, FALSE, mutex);
    DWORD ret = GetLastError();

    /*判断互斥体令牌是否存在*/        /*返回值只能判断函数是否执行成功*/
    if (!hMutex) {
        return MUTEXTOKEN_ERROR;
    }
    if (ret == ERROR_ALREADY_EXISTS) {
        return MUTEXTOKEN_EXISTS;
    }
    else {
        return 0;
    }

}

int main(int argc, char *argv[])
{
    /*防止多开*/
    if (!preventMultiple(MUTEXTOKEN)) {
        QApplication a(argc, argv);
        HOS_Script w;
        w.show();
        return a.exec();
    }
    else {
        MessageBox(NULL, TEXT("禁止多开"), TEXT("警告"), MB_OK|MB_ICONEXCLAMATION);
        return 0;
    }
}
