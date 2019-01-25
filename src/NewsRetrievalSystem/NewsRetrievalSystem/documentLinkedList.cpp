#include "stdafx.h"
#include "documentLinkedList.h"
#include <iostream>

const int documentLinkedList::ADD_TIMES = 100;
const int documentLinkedList::SUBTRACT_TIMES = 200;

documentLinkedList::documentLinkedList(TDataType size_)//构造函数
{
	/*
	Parameters:
		size_:文档链表的容量
	*/
	if (size_ <= 0) {
		std::wcout << L"文档链表的容量不能为0或负数，由于错误的参数，程序之后可能会错误运行..." << std::endl;
		size_ = 100;
	}
	this->size = size_;
	data = new ducumentLinkedListNode[size];
	for (int i = 0; i < size; i++) {
		data[i].TermID = i;
	}
}


documentLinkedList::~documentLinkedList()//析构函数
{
	for (int i = 0; i < size; i++) {
		DocInfoNode* link = data[i].infoNode;
		DocInfoNode* tmp;
		while (link != nullptr) {
			tmp = link;
			link = link->next;
			delete tmp;
		}
	}
	delete[] data;
}

void documentLinkedList::Add(int TermID, int DocID, int Times)
{
	/*
	Usage:
		添加文档
		考虑本次作业的需求，对于同一单词对应的documentLinkedListNode，插入时可以保证DocID大的比小的晚插入以及每个DocID只执行一次插入，
		这样无需排序便可满足插入后文档链表里每个单词的documentLinkedListNode后面接的DocInfoNode按DocID从大到小先后排列）
	Parameters:
		TermID:插入的记录里的关键词ID
		DocID:插入的记录里的文档ID
		Times:插入的记录里的TermID在DocID号文档中出现的次数
	Return Value:
		none
	*/
	DocInfoNode* node = data[TermID].infoNode;
	if (node == nullptr) {
		data[TermID].infoNode = new DocInfoNode;
		node = data[TermID].infoNode;
		node->DocID = DocID;
		node->Times = Times;
	}
	else {
		data[TermID].infoNode = new DocInfoNode;
		data[TermID].infoNode->DocID = DocID;
		data[TermID].infoNode->Times = Times;
		data[TermID].infoNode->next = node;
	}
	return;
}

DocInfoNode* documentLinkedList::Search(int TermID, int DocID)
{
	/*
	Usage:
		搜索文档
	Parameters:
		TermID:所查找记录的关键词ID
		DocID:所查找记录的文档ID
	Return Value:
		若查找成功，返回一个DocInfoNode对象，对应一条记录；若查找失败，返回nullptr
	*/
	if (size < 0 || TermID >= size) {
		return nullptr;
	}
	DocInfoNode *node = data[TermID].infoNode;
	while (node != nullptr) {
		if (node->DocID == DocID) {
			return node;
		}
		if (node->DocID < DocID) {
			return nullptr;
		}
		node = node->next;
	}
	return nullptr;
}

void documentLinkedList::Edit(int TermID, int DocID, int param,int mode)
{
	/*
	Usage:
		修改文档
	Parameters:
		TermID:需要修改的记录对应的关键词ID
		DocID:需要修改的记录对应的文档ID
		param:修改时的参数
		mode:修改模式，合法取值为documentLinkedList::ADD_TIMES和documentLinkedList::SUBTRACT_TIMES
	Return Value:
		none
	*/
	DocInfoNode* target = Search(TermID, DocID);//首先查找相应的节点
	if (target != nullptr) {//查找不失败，则根据mode的取值对记录进行相应修改
		if (mode == ADD_TIMES) {
			target->Times += param;
		}
		else if (mode == SUBTRACT_TIMES) {
			target->Times -= param;
		}
	}
}

void documentLinkedList::Remove(int DocID)
{
	/*
	Usage:
		删除某文档
	Parameters:
		DocID:删除文档编号等于DocID的记录
	Return Value:
		none
	*/
	for(int i = 0; i < size; i++) {
		clearRecord(DocID, i);//调用clearRecord，移除文档编号为DocID且关键词编号为i的所有记录
	}
}

void documentLinkedList::clearRecord(int DocID, int TermID)
{
	/*
	Usage:
		移除特定文档的关于特定关键词的记录
	Parameters:
		DocID:移除的记录的文档编号应等于DocID
		TermID:移除的记录的关键词编号应等于TermID
	Return Value:
		none
	*/
	DocInfoNode* node = data[TermID].infoNode;
	if (node == nullptr) {
		return;
	}
	if (node->DocID == DocID) {
		data[TermID].infoNode = node->next;
		delete node;
		return;
	}
	while (node != nullptr) {
		if (node->DocID < DocID) {
			return;
		}
		if(node->next!=nullptr){
			if (node->next->DocID == DocID) {
				DocInfoNode* to_delete = node->next;
				node->next = node->next->next;
				delete to_delete;
			}
		}
		node = node->next;
	}
}
