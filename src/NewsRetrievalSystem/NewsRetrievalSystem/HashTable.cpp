#include "stdafx.h"
#include "HashTable.h"


const TDataType HashTable::HashTableSize = (TDataType)275911;


HashNode::HashNode() {
	str = new wchar_t[1];
	str[0] = L'\0';
	next = nullptr;
}

HashNode::~HashNode() {
	delete[] str;
	str = nullptr;
	if (next != nullptr) {
		delete next;
		next = nullptr;
	}
}


HashTable::HashTable() {
	hashData = new HashNode[HashTableSize];
}

bool HashTable::inHashTable(const wchar_t* key) {
	TDataType result = hash(key, false);
	HashNode* pointer = hashData[result].next;
	while (pointer != nullptr) {
		if (wcscmp(key, pointer->str) == 0) {
			return true;
		}
		pointer = pointer->next;
	}
	return false;
}

TDataType HashTable::hash(const wchar_t* key, bool add_into_hashtable) {
	//计算哈希值
	TDataType result = compute_hash(key, HashTableSize);

	//如果add_into_hashtable为true,则将key添加到哈希表内，不检查是否重复
	if (add_into_hashtable) {
		HashNode* newNode = new HashNode;
		delete[] newNode->str;
		const int length = wcslen(key);
		newNode->str = new wchar_t[length + 1];
		wmemcpy(newNode->str, key, length + 1);

		if (hashData[result].next == nullptr) {
			hashData[result].next = newNode;
		}
		else {
			newNode->next = hashData[result].next;
			hashData[result].next = newNode;
		}
	}

	return result;
}

const HashNode*  HashTable::getFirstHashNode() const {
	return hashData;
}

HashTable::~HashTable() {
	if (hashData != nullptr) {
		delete[] hashData;
	}
}