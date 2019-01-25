#include "ClickLabel.h"
#include <qevent.h>
ClickLabel::ClickLabel(QWidget *parent)
	: QLabel(parent)
{
	ui.setupUi(this);
}

ClickLabel::~ClickLabel()
{
}

void ClickLabel::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton) {
		emit clicked();//������ʱemit clicked()��������Ӧ�Ĳۺ����Դ򿪶�Ӧ��html��ҳ�ļ�
	}
	QLabel::mousePressEvent(event);
	
}