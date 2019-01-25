#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_
#include "stdafx.h"

class HashNode {//��ϣ���������ʵ���е�������
public:
	wchar_t* str;
	HashNode* next;
	HashNode();
	~HashNode();
};


class HashTable {//��ϣ����
	HashNode* hashData;//�洢��ϣ������飨������ʵ�֣�
public:
	HashTable();
	static const TDataType HashTableSize;
	bool inHashTable(const wchar_t* key);//���key�Ƿ��Ѿ��洢�ڵ�ǰ�Ĺ�ϣ����
	TDataType hash(const wchar_t* key, bool add_into_hashtable = false);//��ϣ����������ֵΪkey��Ӧ�Ĺ�ϣֵ������add_into_hashtable�����Ƿ���뵽��ϣ����
	const HashNode* getFirstHashNode() const;//����ָ��hashData��ָ��
	~HashTable();
	friend class NewsRetrievalTool;
};



inline TDataType compute_hash(const wchar_t* key, TDataType hashTableSize) {//Ӧ���������������ϣֵ��ȷ��ͳһ�ļ��㷽��
	long long int result = 0;
	const int length = wcslen(key);
	for (int i = 0; i < length; i++) {
		result += result * 131 + (TDataType)*(key + i);
		result %= hashTableSize;
		if (result < 0 || result >= hashTableSize) {
			result = hashTableSize / (i + 4);
		}
	}
	return result;
}

#endif