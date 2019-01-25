#pragma once

#include <QLabel>
#include "ui_ClickLabel.h"

class ClickLabel : public QLabel
{
	Q_OBJECT

public:
	ClickLabel(QWidget *parent = Q_NULLPTR);
	~ClickLabel();

private:
	Ui::ClickLabel ui;
signals:
	void clicked();//被单击时发出clicked()信号
protected:
	virtual void mousePressEvent(QMouseEvent* event);//处理QMouseEvent
};
