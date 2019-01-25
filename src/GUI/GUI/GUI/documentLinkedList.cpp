#include "stdafx.h"
#include "documentLinkedList.h"
#include <iostream>

const int documentLinkedList::ADD_TIMES = 100;
const int documentLinkedList::SUBTRACT_TIMES = 200;

documentLinkedList::documentLinkedList(TDataType size_)//���캯��
{
	/*
	Parameters:
		size_:�ĵ����������
	*/
	if (size_ <= 0) {
		std::wcout << L"�ĵ��������������Ϊ0���������ڴ���Ĳ���������֮����ܻ��������..." << std::endl;
		size_ = 100;
	}
	this->size = size_;
	data = new ducumentLinkedListNode[size];
	for (int i = 0; i < size; i++) {
		data[i].TermID = i;
	}
}


documentLinkedList::~documentLinkedList()//��������
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
		����ĵ�
		���Ǳ�����ҵ�����󣬶���ͬһ���ʶ�Ӧ��documentLinkedListNode������ʱ���Ա�֤DocID��ı�С��������Լ�ÿ��DocIDִֻ��һ�β��룬
		�����������������������ĵ�������ÿ�����ʵ�documentLinkedListNode����ӵ�DocInfoNode��DocID�Ӵ�С�Ⱥ����У�
	Parameters:
		TermID:����ļ�¼��Ĺؼ���ID
		DocID:����ļ�¼����ĵ�ID
		Times:����ļ�¼���TermID��DocID���ĵ��г��ֵĴ���
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
		�����ĵ�
	Parameters:
		TermID:�����Ҽ�¼�Ĺؼ���ID
		DocID:�����Ҽ�¼���ĵ�ID
	Return Value:
		�����ҳɹ�������һ��DocInfoNode���󣬶�Ӧһ����¼��������ʧ�ܣ�����nullptr
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
		�޸��ĵ�
	Parameters:
		TermID:��Ҫ�޸ĵļ�¼��Ӧ�Ĺؼ���ID
		DocID:��Ҫ�޸ĵļ�¼��Ӧ���ĵ�ID
		param:�޸�ʱ�Ĳ���
		mode:�޸�ģʽ���Ϸ�ȡֵΪdocumentLinkedList::ADD_TIMES��documentLinkedList::SUBTRACT_TIMES
	Return Value:
		none
	*/
	DocInfoNode* target = Search(TermID, DocID);//���Ȳ�����Ӧ�Ľڵ�
	if (target != nullptr) {//���Ҳ�ʧ�ܣ������mode��ȡֵ�Լ�¼������Ӧ�޸�
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
		ɾ��ĳ�ĵ�
	Parameters:
		DocID:ɾ���ĵ���ŵ���DocID�ļ�¼
	Return Value:
		none
	*/
	for(int i = 0; i < size; i++) {
		clearRecord(DocID, i);//����clearRecord���Ƴ��ĵ����ΪDocID�ҹؼ��ʱ��Ϊi�����м�¼
	}
}

void documentLinkedList::clearRecord(int DocID, int TermID)
{
	/*
	Usage:
		�Ƴ��ض��ĵ��Ĺ����ض��ؼ��ʵļ�¼
	Parameters:
		DocID:�Ƴ��ļ�¼���ĵ����Ӧ����DocID
		TermID:�Ƴ��ļ�¼�Ĺؼ��ʱ��Ӧ����TermID
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
