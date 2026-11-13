#include "HOS_Script.h"

HOS_Script::HOS_Script(QWidget *parent) : QMainWindow(parent) {
    ui.setupUi(this);
    this->iniUi();
    /*连接信号槽要用到SystemTray的指针，需要先初始化*/
    iniParams();
    this->iniSystemTray();
    this->iniStyleSheet();

    this->iniConnect();

    // this->control->iniDM();

    tool->playSound(":/HOS_Script/Resources/welcome.wav");
}

HOS_Script::~HOS_Script() {
    /*子窗口执行close()时会自动释放，但如果打开子窗口后直接退出程序就需要释放内存*/
    if (informationWidget) {
        delete informationWidget;
    }
    delete control;
    delete settings;
    delete tool;

    this->control->destroyDM();
}

/*初始化UI*/
void HOS_Script::iniUi() {

    /*将主窗口设置为无边框全透明，再用一个widget覆盖。*/
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_LayoutOnEntireRect, true);
    setWindowIcon(QIcon(":/HOS_Script/Resources/favicon.ico"));

    this->setWindowOpacity(0.9);
}

/*初始化成员*/
void HOS_Script::iniParams() {
    app = (QApplication *)QApplication::instance();
    informationWidget = nullptr;

    /*QSettings析构时会自动调用sync()，并且又定期事件自动执行，所以通常不需要自己调用sync()*/
    settings = new QSettings("config.ini", QSettings::IniFormat);
    settings->setIniCodec("utf-8");

    /*构造时直接注册热键*/
    wid = this->winId();
    tool = new Tool();
    control = new Control(wid, settings, tool, this);
}

void HOS_Script::iniStyleSheet() {
    this->myStyleSheet = QString("\
        #widget{\
            border-image:url(%1);\
            background-repeat:no-repeat;\
                }\
        QPushButton{\
            background-color:#222a30;\
            border-radius:10px;\
            border:none;\
            width:40px;\
            height:40px;\
            color:#ffffff;\
    }\
        QPushButton:hover{\
            background-color:pink;\
}\
        #groupBox{border:none;}");
    this->ui.widget->setStyleSheet(myStyleSheet.arg(":/HOS_Script/Resources/wz.png"));
}

/*连接信号和槽函数*/
void HOS_Script::iniConnect() {

    QObject::connect(ui.btnMinimized, SIGNAL(clicked()), this, SLOT(onBtnMinimizedClicked()));

    QObject::connect(this->systemTray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this,
                     SLOT(onSystemTrayActivated(QSystemTrayIcon::ActivationReason)));

    QObject::connect(ui.actionExit, SIGNAL(triggered(bool)), this, SLOT(onExitActionTriggered()));

    QObject::connect(ui.btnInformation, SIGNAL(clicked()), this, SLOT(onBtnInformationClicked()));
    QObject::connect(this->control, SIGNAL(needPlaySound(int)), this, SLOT(onNeedPlaySound(int)));
    QObject::connect(this->ui.btnManual, SIGNAL(toggled(bool)), this, SLOT(onBtnManualToggled(bool)));
    QObject::connect(this->ui.btnFastPattern, SIGNAL(toggled(bool)), this, SLOT(onBtnFastPatternToggled(bool)));
}

void HOS_Script::iniSystemTray() {
    /*系统托盘*/
    systemTray = new QSystemTrayIcon(this);
    systemTray->setIcon(QIcon(":/HOS_Script/Resources/favicon.ico"));
    systemTray->setVisible(true);
    systemTray->setToolTip(QString(QStringLiteral("BY Karcyril")));
    QMenu *menu = new QMenu(this);
    menu->addAction(ui.actionAuthor);
    menu->addAction(ui.actionExit);
    systemTray->setContextMenu(menu);
}

/*
**********************************************槽函数*******************************
******************************************************/

/*托盘槽函数*/
void HOS_Script::onSystemTrayActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger) {
        this->show();
        this->activateWindow();
    }
}

/*最小化槽函数*/
void HOS_Script::onBtnMinimizedClicked() {
    this->hide();
    this->systemTray->show();
}

/*信息按钮*/
void HOS_Script::onBtnInformationClicked() {
    if (!this->informationWidget) {
        /*指定父类*/
        this->informationWidget = new InformationWidget(this);
        this->informationWidget->show();
        this->informationWidget->move(QPoint(0, -50));
    } else {
        this->informationWidget->close();
        delete this->informationWidget;
        this->informationWidget = nullptr;
    }
}

/*程序退出槽函数*/
void HOS_Script::onExitActionTriggered() {
    this->app->quit();
}

/*处理需要播放音乐信号*/
void HOS_Script::onNeedPlaySound(int flag) {
    if (flag == this->SoundFlags::ah) {
        tool->playSound(":/HOS_Script/Resources/ah.wav");
    } else if (flag == this->SoundFlags::mua) {
        tool->playSound(":/HOS_Script/Resources/mua.wav");
    }
}

// 手动按钮
void HOS_Script::onBtnManualToggled(bool b) {
    if (b) {
        QString cs = QString("\
        #btnManual{\
            background-color:pink;\
                }\
 ");
        ui.btnManual->setStyleSheet(cs);
    } else if (!b) {
        QString cs = QString("\
        #btnManual{\
            background-color:#222a30;\
                }\
        #btnManual:hover{\
            background-color:pink;\
}\
 ");
        ui.btnManual->setStyleSheet(cs);
    }

    /*设置手动标志*/
    control->setManual(b);
}

/*快速模式按钮*/
void HOS_Script::onBtnFastPatternToggled(bool b) {
    if (b) {
        QString cs = QString("\
        #btnFastPattern{\
            background-color:pink;\
                }\
 ");
        ui.btnFastPattern->setStyleSheet(cs);
    } else if (!b) {
        QString cs = QString("\
        #btnFastPattern{\
            background-color:#222a30;\
                }\
        #btnFastPattern:hover{\
            background-color:pink;\
}\
 ");
        ui.btnFastPattern->setStyleSheet(cs);
    }

    /*设置手动标志*/
    control->setFastPattern(b);
}

/*************************************************************************/

/*拖动窗口*/
void HOS_Script::mousePressEvent(QMouseEvent *event) {
    startCoor = event->pos(); // 保存鼠标相对窗口的坐标
    /*由于子组件不会触发此事件,所以要进行判断是否在主区域按下鼠标*/
    isDraging = true;
    QWidget::mousePressEvent(event);
}

void HOS_Script::mouseMoveEvent(QMouseEvent *event) {
    /*将返回值与鼠标左键枚举值进行位与运算*/
    if (event->buttons() & Qt::LeftButton && isDraging) {
        this->move(event->globalPos() - startCoor);
    }
    QWidget::mouseMoveEvent(event);
}

void HOS_Script::mouseReleaseEvent(QMouseEvent *event) {
    isDraging = false;
    QWidget::mouseReleaseEvent(event);
}

/*******************************************************/

void HOS_Script::enterEvent(QEvent *event) {
    ui.groupBox->show();
}
void HOS_Script::leaveEvent(QEvent *event) {
    ui.groupBox->hide();
}

/*重写事件 处理windows消息*/
bool HOS_Script::nativeEvent(const QByteArray &eventType, void *message, long *result) {
    MSG *msg = (MSG *)message;

    switch (msg->message) {
    case WM_HOTKEY: {
        switch (msg->wParam) {
        case Control::CTRL_1:
            /*处理Ctrl + 1*/
            control->moveWindow();
            break;
        case Control::CTRL_2:
            /*处理Ctrl + 2*/
            tool->relieveCursor();
            onNeedPlaySound(this->SoundFlags::mua);
            break;
        case Control::CTRL_3:
            /*处理Ctrl + 3*/
            control->setPainting(FALSE);
            this->control->quitHOS();
            break;
        case Control::CTRL_4:
            /*处理Ctrl + 4*/
            control->setPainting(FALSE);
            this->control->restartupHOS();
            break;
        case Control::CTRL_F7:
            /*处理Ctrl + F7*/
            this->control->inputAccount();
            break;
        case Control::CTRL_F1:
            if (control->isPainting()) {
                control->setPainting(FALSE);
            } else {
                control->paintCirle(1);
            }
            control->heqiangshidan();
            break;
        /*乱斗单号挂机送人头*/
        case Control::CTRL_F2:
            control->luandouguaji();
            break;
        }
    }
    case WM_PAINT: {
        break;
    }
    default: {
        break;
    }
    }

    /*不用处理默认事件
    返回false表示由qt处理，返回true则不处理*/
    return false;
}