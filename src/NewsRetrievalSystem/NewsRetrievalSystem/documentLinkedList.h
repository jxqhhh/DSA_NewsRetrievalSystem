
#ifndef _DOCUMENTLINKEDLIST_H_
#define _DOCUMENTLINKEDLIST_H_

#include "stdafx.h"
class NewsRetrivalTool;
class Widget;
struct DocInfoNode {//记录单词在某篇文档中的出现次数和该文档的ID
	int DocID;//出现该单词的文档ID
	int Times;//在该文档的出现次数
	DocInfoNode* next = nullptr;
};
struct ducumentLinkedListNode{
	int TermID;//单词ID
	DocInfoNode* infoNode=nullptr;//记录该单词在各文档的出现情况
};
class documentLinkedList{//文档链表中，每个单词对应的documentLinkedListNode要求始终依DocID从大到小先后排列
	ducumentLinkedListNode* data;//数组的每一个成员都是某一个单词对应的相关文档的链表
	int size = 0;//data的容量
public:
	static const int ADD_TIMES;//静态成员常量，作为Edit函数的mode参数使用
	static const int SUBTRACT_TIMES;//静态成员常量，作为Edit函数的mode参数使用
	documentLinkedList(TDataType size_);
	~documentLinkedList();
	friend class NewsRetrievalTool;//声明友元类
	friend class Widget;//声明友元类
	void Add(int TermID, int DocID, int Times);/*添加文档（考虑本次作业的需求，对于同一单词对应的documentLinkedListNode，插入时可以保证DocID大的比小的晚插入以及每个DocID只插入一次记录，
												这样无需排序便可满足插入后文档链表里每个单词的documentLinkedListNode后面接的DocInfoNode按DocID从大到小先后排列）*/
	DocInfoNode* Search(int TermID,int DocID);//查找文档
	void Edit(int TermID, int DocID,int param,int mode);//修改文档
	void Remove(int DocID);//删除某文档
	void clearRecord(int DocID, int TermID);//移除特定文档的关于特定关键词的记录
};

#endif