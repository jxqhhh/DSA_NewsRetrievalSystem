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
	void add(TELemType& str);//�������������
	void add(int pos,TELemType& data);//ʹ�ò�����λ��Ϊpos��pos�ķ�Χ��0��ʼ��
	bool remove(TELemType& str);//ɾ���ɹ�����True�����򷵻�False
	bool remove(int pos);//ɾ���ɹ�����True�����򷵻�False
	int search(TELemType& str);//����ĳԪ��λ�ã�����ʧ���򷵻�-1
	int length() const;
	void clearInvalidCharString();//ɾ����������Ч���ַ���������L' '��L'\n'��L'\t'��ɵ��ַ����ͳ���Ϊ0���ַ�����Ϊ��Ч��
	CharString& getLastElement() const;//�������һ��Ԫ�ص�����
	void writeIntoFile(std::wofstream& wfs) const;
	const Link* getHead() const;
	Link* getFirst() const;
};

#endif
