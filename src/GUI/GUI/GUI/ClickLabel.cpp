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
		emit clicked();//被单击时emit clicked()，触发相应的槽函数以打开对应的html网页文件
	}
	QLabel::mousePressEvent(event);
	
}