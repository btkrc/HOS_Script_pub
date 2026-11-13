#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_HOS_Script.h"
#include <QMouseEvent>
#include <QPoint>
#include <QDebug>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QRect>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultiMediaWidgets/QVideoWidget>
#include <QtMultiMedia/QMediaPlaylist>
#include <QUrl>
#include <QEvent>
#include <QApplication>
#include <QString>
#include<qt_windows.h>
#include "InformationWidget.h"
#include<QWindow>

/*win32功能函数*/
#include "control.h"

class HOS_Script : public QMainWindow
{
    Q_OBJECT

public:
    HOS_Script(QWidget *parent = Q_NULLPTR);
    ~HOS_Script();

    Tool *tool;


private:
    Ui::HOS_ScriptClass ui;
    //退出程序用
    QApplication *app;
    InformationWidget *informationWidget;
    Control *control;
    WId wid;
    QSettings *settings;

    enum SoundFlags{
        ah=1,
        mua=2
    };

    struct Login {
        QString userName;
        QString password;
    };

    QList<Login> logins;

    QString myStyleSheet;
    QSystemTrayIcon *systemTray;
    QPoint startCoor;

    //鼠标按下状态
    bool isDraging;

    void iniUi();
    void iniParams();
    void iniStyleSheet();
    void iniConnect();
    void iniSystemTray();


private slots:
    void onSystemTrayActivated(QSystemTrayIcon::ActivationReason reason);
    void onBtnMinimizedClicked();
    void onExitActionTriggered();
    void onBtnInformationClicked();
    void onBtnManualToggled(bool);
    void onBtnFastPatternToggled(bool);

    //重启线程结束的槽函数
    void onNeedPlaySound(int flag);

    //重写父类事件
protected:
    void mousePressEvent(QMouseEvent *event)override;
    void mouseMoveEvent(QMouseEvent *event)override;
    void mouseReleaseEvent(QMouseEvent *event)override;
    void enterEvent(QEvent *event)override;
    void leaveEvent(QEvent *event)override;
    //重写事件 处理windows消息
    bool nativeEvent(const QByteArray &eventType, void *message, long *result)override;
};
