#include "Widget.h"
#include <qlineedit.h>
#include <cwchar>
#include <qmessagebox.h>
#include <QMyItem.h>
#include <set>
Widget::Widget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

void Widget::pushButton_retrie_Click() {//�ۺ�������������ť�����ʱ����
	//�������������NewsRetrievalTool��ļ�������retrieveByKeyword���
	ui.listWidget->clear();
	QLineEdit* edit=ui.tabWidget->findChild<QLineEdit *>("lineRetrie");
	CharString input=edit->text().toStdWString().c_str();
	int prev = 0;
	int pos = input.indexOf(L" ", prev);
	std::vector<RetrievalInfo> retrieInfoVec;
	bool to_end = false;
	AVL* avlNode;
	while (pos != -1) {
		avlNode = Search(tool.tree, input.substring(prev, pos - prev));
		loop:
		if (avlNode != nullptr) {
			DocInfoNode* node = tool.documentList->data[avlNode->data.TermID].infoNode;
			while (node != nullptr) {
				bool already_existing = false;
				int DocID = node->DocID;
				for (auto it = retrieInfoVec.begin(); it != retrieInfoVec.end(); it++) {
					if ((*it).ID == DocID) {
						it->numOfCategoriesOfKeywords += 1;
						it->numOfKeywords += node->Times;
						already_existing = true;
						break;
					}
				}
				if (!already_existing) {
					RetrievalInfo result;
					result.ID = DocID;
					result.numOfCategoriesOfKeywords = 1;
					result.numOfKeywords = node->Times;
					retrieInfoVec.push_back(result);
				}
				node = node->next;
			}

		}
		if (pos == -1) {
			goto sort;
		}
		prev = pos + 1;
		pos = input.indexOf(L" ", prev);
	}
	avlNode = Search(tool.tree, input.substring(prev, input.length() - prev));
	goto loop;
	
	sort:
	std::sort(retrieInfoVec.begin(), retrieInfoVec.end(), greater);
	for (auto it = retrieInfoVec.begin(); it != retrieInfoVec.end(); it++) {
		QListWidgetItem* pListWidgetItem = new QListWidgetItem;
		QWidget* item = new QMyItem(QString::fromStdWString((CharString(tool.input_path)+CharString(std::to_wstring(it->ID).c_str())+CharString(L".html")).wc_str()), 
			QString::fromStdWString((CharString(tool.output_path) + CharString(std::to_wstring(it->ID).c_str()) + CharString(L".info")).wc_str()),
			QString::fromStdWString(tool.titles[it->ID].wc_str()), 
			QString::fromStdWString(L" "));
		pListWidgetItem->setSizeHint(QSize(500, 210));
		ui.listWidget->addItem(pListWidgetItem);
		ui.listWidget->setItemWidget(pListWidgetItem,item);
	}
	if (retrieInfoVec.empty()) {
		QMessageBox::about(NULL, QString::fromStdWString(L"��ʾ"), QString::fromStdWString(L"����ʧ�ܣ�"));
	}
}

void Widget::pushButton_Recommend_Click()//�ۺ��������Ƽ���ť�����ʱ����
{
	//�����Ƽ�������NewsRetrievalTool����Ƽ�����recommend���
	ui.listWidget_2->clear();
	QLineEdit* edit = ui.tabWidget->findChild<QLineEdit *>("lineRecommend");
	CharString input = edit->text().toStdWString().c_str();
	int max_to_recommend = 5;
	std::locale loc("Chinese-simplified");
	bool valid = false;
	int index;
	for (int i = 0; i < DATABASE_SIZE; i++) {
		if (tool.titles[i] == input) {
			valid = true;
			index = i;
			break;
		}
	}
	if (!valid) {
		QMessageBox::about(NULL, QString::fromStdWString(L"��ʾ"), QString::fromStdWString(L"�����Ų������ݿ��л���������ʧ�ܣ��޷��Ƽ�"));
		return;
	}
	std::wifstream s((CharString(tool.output_path) + CharString(std::to_wstring(index).c_str()) + CharString(L".txt")).wc_str());
	s.imbue(loc);
	std::set<CharString> keywordSet;
	std::set<int> keywordIDSet;
	std::wstring str;
	while (std::getline(s, str)) {
		CharString cstr(str.c_str());
		keywordSet.insert(cstr);
	}
	for (auto it = keywordSet.begin(); it != keywordSet.end(); it++) {
		AVL* node = Search(tool.tree, *it);
		if (node != nullptr) {
			keywordIDSet.insert(node->data.TermID);
		}
	}
	if (keywordIDSet.empty()) {
		QMessageBox::about(NULL, QString::fromStdWString(L"��ʾ"), QString::fromStdWString(L"�����Ž���ʧ�ܣ��޷��Ƽ�"));
		return;
	}
	std::vector<RetrievalInfo> retrieInfoVec;
	for (auto it = keywordIDSet.begin(); it != keywordIDSet.end(); it++) {
		DocInfoNode* node = tool.documentList->data[*it].infoNode;
		while (node != nullptr) {
			bool already_existing = false;
			int DocID = node->DocID;
			for (auto it2 = retrieInfoVec.begin(); it2 != retrieInfoVec.end(); it2++) {
				if ((*it2).ID == DocID) {
					it2->numOfCategoriesOfKeywords += 1;
					it2->numOfKeywords += node->Times;
					already_existing = true;
					break;
				}
			}
			if (!already_existing) {
				RetrievalInfo result;
				result.ID = DocID;
				result.numOfCategoriesOfKeywords = 1;
				result.numOfKeywords = node->Times;
				retrieInfoVec.push_back(result);
			}
			node = node->next;
		}
	}
	sort:
	std::sort(retrieInfoVec.begin(), retrieInfoVec.end(), greater);
	if (retrieInfoVec.empty()) {
		QMessageBox::about(NULL, QString::fromStdWString(L"��ʾ"), QString::fromStdWString(L"��������ݣ��Ƽ�ʧ��"));
		return;
	}
	int num = 0;
	for (auto it = retrieInfoVec.begin(); it != retrieInfoVec.end() && num < max_to_recommend; it++) {
		if (it->ID != index) {
			QListWidgetItem* pListWidgetItem = new QListWidgetItem;
			QWidget* item = new QMyItem(QString::fromStdWString((CharString(tool.input_path) + CharString(std::to_wstring(it->ID).c_str()) + CharString(L".html")).wc_str()),
				QString::fromStdWString((CharString(tool.output_path) + CharString(std::to_wstring(it->ID).c_str()) + CharString(L".info")).wc_str()),
				QString::fromStdWString(tool.titles[it->ID].wc_str()),
				QString::fromStdWString(L" "));
			pListWidgetItem->setSizeHint(QSize(500, 210));
			ui.listWidget_2->addItem(pListWidgetItem);
			ui.listWidget_2->setItemWidget(pListWidgetItem, item);
			num++;
		}
	}
	return;
}
