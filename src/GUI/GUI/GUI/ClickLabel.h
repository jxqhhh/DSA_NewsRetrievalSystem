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
	void clicked();//������ʱ����clicked()�ź�
protected:
	virtual void mousePressEvent(QMouseEvent* event);//����QMouseEvent
};
