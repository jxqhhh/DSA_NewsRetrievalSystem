#pragma once

#include <QWidget>
#include "ui_QMyItem.h"
class QMyItem : public QWidget//�����Զ���QListItemWidget
{
	Q_OBJECT

public:
	QMyItem(QString filename,QString infoFileName,QString title,QString abstract,QWidget *parent = Q_NULLPTR);
	~QMyItem();
private slots:
	void clicklabel_clicked();
private:
	Ui::QMyItem ui;
	QString filename;//��Ӧ��html�����ļ����ļ���
	QString infoFileName;//��Ӧ��info�ִʽ���ļ����ļ���
	QString title;//��Ӧ�����ű���
	QString abstract;//��Ӧ�����ŵ�ժҪ
};
