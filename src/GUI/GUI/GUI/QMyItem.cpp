#include "QMyItem.h"
#include <QUrl>
#include <qdesktopservices.h>
#include <qmessagebox.h>
#include "CharString.h"
#include <fstream>
QMyItem::QMyItem(QString filename, QString infoFileName, QString title, QString abstract, QWidget *parent)
	: QWidget(parent),filename(filename),title(title),abstract(abstract)
{
	ui.setupUi(this);
	connect(ui.label, SIGNAL(clicked()), this, SLOT(clicklabel_clicked()));//连接信号与槽
	ui.label->setText(title);
	ui.textBrowser->setText(abstract);
	ui.label->show();
	std::locale loc("Chinese-simplified");
	std::wifstream input(infoFileName.toStdWString());
	input.imbue(loc);
	std::wstring str;
	std::wstring tmp;
	std::getline(input, tmp);
	std::getline(input, tmp);
	std::getline(input, tmp);
	while (std::getline(input, tmp)) {
		str += tmp;
		if (str.length() > 200) {
			break;
		}
	}
	CharString cstr(str.c_str());
	int pos = cstr.indexOf(L"。", 0);
	if (pos != -1) {
		int tmptmp = 0;
		while (tmptmp != -1) {
			pos = tmptmp;
			tmptmp = cstr.indexOf(L"。", pos + 1);
		}
		str = cstr.substring(0, pos + 1).wc_str();
	}
	ui.textBrowser->setText(QString::fromStdWString(str));
	input.close();
}

QMyItem::~QMyItem()
{
}

void QMyItem::clicklabel_clicked() {//槽函数，ui.label被点击时触发
	QDesktopServices::openUrl(QUrl::fromLocalFile(filename));//打开对应的html文件
}