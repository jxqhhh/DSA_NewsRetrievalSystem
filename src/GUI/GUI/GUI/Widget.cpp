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

void Widget::pushButton_retrie_Click() {//槽函数，当检索按钮被点击时触发
	//具体检索方法与NewsRetrievalTool类的检索函数retrieveByKeyword相近
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
		QMessageBox::about(NULL, QString::fromStdWString(L"提示"), QString::fromStdWString(L"检索失败！"));
	}
}

void Widget::pushButton_Recommend_Click()//槽函数，当推荐按钮被点击时触发
{
	//具体推荐方法与NewsRetrievalTool类的推荐函数recommend相近
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
		QMessageBox::about(NULL, QString::fromStdWString(L"提示"), QString::fromStdWString(L"该新闻不在数据库中或解析其标题失败，无法推荐"));
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
		QMessageBox::about(NULL, QString::fromStdWString(L"提示"), QString::fromStdWString(L"该新闻解析失败，无法推荐"));
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
		QMessageBox::about(NULL, QString::fromStdWString(L"提示"), QString::fromStdWString(L"无相关内容，推荐失败"));
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
