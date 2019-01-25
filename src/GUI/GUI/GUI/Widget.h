#pragma once

#include <QtWidgets/QWidget>
#include "ui_Widget.h"
#include "NewsRetrievalTool.h"
class Widget : public QWidget
{
	Q_OBJECT

public:
	Widget(QWidget *parent = Q_NULLPTR);
public slots:
	void pushButton_retrie_Click();
	void pushButton_Recommend_Click();
private:
	Ui::WidgetClass ui;
	NewsRetrievalTool tool;
};
