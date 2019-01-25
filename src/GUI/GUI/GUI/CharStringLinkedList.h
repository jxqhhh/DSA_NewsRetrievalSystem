#ifndef _CHARSTRINGLINKEDLIST_H_
#define _CHARSTRINGLINKEDLIST_H_

#include "CharString.h"

#include "stdafx.h"
#include <fstream>
typedef CharString TELemType;

typedef struct Link{
	TELemType data;
	Link* next;
} Link;

class CharStringLinkedList
{
	Link* head;
	Link* tail;
	int len;
public:
	CharStringLinkedList();
	~CharStringLinkedList();
	CharStringLinkedList(const CharStringLinkedList& list);
	void add(TELemType& str);//在链表的最后插入
	void add(int pos,TELemType& data);//使得插入后的位置为pos（pos的范围从0开始）
	bool remove(TELemType& str);//删除成功返回True，否则返回False
	bool remove(int pos);//删除成功返回True，否则返回False
	int search(TELemType& str);//查找某元素位置，查找失败则返回-1
	int length() const;
	void clearInvalidCharString();//删除链表中无效的字符串（仅由L' '、L'\n'、L'\t'组成的字符串和长度为0的字符串视为无效）
	CharString& getLastElement() const;//返回最后一个元素的引用
	void writeIntoFile(std::wofstream& wfs) const;
	const Link* getHead() const;
	Link* getFirst() const;
};

#endif
