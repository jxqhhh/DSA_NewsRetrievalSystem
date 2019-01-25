#pragma once

#include <QWidget>
#include "ui_QMyItem.h"
class QMyItem : public QWidget//用于自定义QListItemWidget
{
	Q_OBJECT

public:
	QMyItem(QString filename,QString infoFileName,QString title,QString abstract,QWidget *parent = Q_NULLPTR);
	~QMyItem();
private slots:
	void clicklabel_clicked();
private:
	Ui::QMyItem ui;
	QString filename;//对应的html新闻文件的文件名
	QString infoFileName;//对应的info分词结果文件的文件名
	QString title;//对应的新闻标题
	QString abstract;//对应的新闻的摘要
};
