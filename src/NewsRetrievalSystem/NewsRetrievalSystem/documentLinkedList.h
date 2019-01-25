
#ifndef _DOCUMENTLINKEDLIST_H_
#define _DOCUMENTLINKEDLIST_H_

#include "stdafx.h"
class NewsRetrivalTool;
class Widget;
struct DocInfoNode {//��¼������ĳƪ�ĵ��еĳ��ִ����͸��ĵ���ID
	int DocID;//���ָõ��ʵ��ĵ�ID
	int Times;//�ڸ��ĵ��ĳ��ִ���
	DocInfoNode* next = nullptr;
};
struct ducumentLinkedListNode{
	int TermID;//����ID
	DocInfoNode* infoNode=nullptr;//��¼�õ����ڸ��ĵ��ĳ������
};
class documentLinkedList{//�ĵ������У�ÿ�����ʶ�Ӧ��documentLinkedListNodeҪ��ʼ����DocID�Ӵ�С�Ⱥ�����
	ducumentLinkedListNode* data;//�����ÿһ����Ա����ĳһ�����ʶ�Ӧ������ĵ�������
	int size = 0;//data������
public:
	static const int ADD_TIMES;//��̬��Ա��������ΪEdit������mode����ʹ��
	static const int SUBTRACT_TIMES;//��̬��Ա��������ΪEdit������mode����ʹ��
	documentLinkedList(TDataType size_);
	~documentLinkedList();
	friend class NewsRetrievalTool;//������Ԫ��
	friend class Widget;//������Ԫ��
	void Add(int TermID, int DocID, int Times);/*����ĵ������Ǳ�����ҵ�����󣬶���ͬһ���ʶ�Ӧ��documentLinkedListNode������ʱ���Ա�֤DocID��ı�С��������Լ�ÿ��DocIDֻ����һ�μ�¼��
												�����������������������ĵ�������ÿ�����ʵ�documentLinkedListNode����ӵ�DocInfoNode��DocID�Ӵ�С�Ⱥ����У�*/
	DocInfoNode* Search(int TermID,int DocID);//�����ĵ�
	void Edit(int TermID, int DocID,int param,int mode);//�޸��ĵ�
	void Remove(int DocID);//ɾ��ĳ�ĵ�
	void clearRecord(int DocID, int TermID);//�Ƴ��ض��ĵ��Ĺ����ض��ؼ��ʵļ�¼
};

#endif