#pragma once

#include <QWidget>
#include "ui_InformationWidget.h"

class InformationWidget : public QWidget
{
    Q_OBJECT

public:
    InformationWidget(QWidget *parent = Q_NULLPTR);
    ~InformationWidget();

private:
    Ui::InformationWidget ui;

    void iniUi();
    void iniConnect();
};
