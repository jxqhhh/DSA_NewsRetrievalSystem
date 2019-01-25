#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_
#include "stdafx.h"

class HashNode {//哈希表的拉链法实现中的链表结点
public:
	wchar_t* str;
	HashNode* next;
	HashNode();
	~HashNode();
};


class HashTable {//哈希表类
	HashNode* hashData;//存储哈希表的数组（拉链法实现）
public:
	HashTable();
	static const TDataType HashTableSize;
	bool inHashTable(const wchar_t* key);//检查key是否已经存储在当前的哈希表中
	TDataType hash(const wchar_t* key, bool add_into_hashtable = false);//哈希函数，返回值为key对应的哈希值，根据add_into_hashtable决定是否插入到哈希表内
	const HashNode* getFirstHashNode() const;//返回指向hashData的指针
	~HashTable();
	friend class NewsRetrievalTool;
};



inline TDataType compute_hash(const wchar_t* key, TDataType hashTableSize) {//应用内联函数计算哈希值，确保统一的计算方法
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