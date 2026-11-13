#include "InformationWidget.h"

InformationWidget::InformationWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    iniUi();
}

InformationWidget::~InformationWidget()
{
}

void InformationWidget::iniUi() {
    /*设置此属性，此指针指向的内存在关闭时会自动释放*/
    setAttribute(Qt::WA_DeleteOnClose);
    setStyleSheet("#widget{background-color:rgba(0,0,0,0.5);border-radius:10px;}\
QPushButton{color:orange;background-color:black;border-radius:10px;}");
}