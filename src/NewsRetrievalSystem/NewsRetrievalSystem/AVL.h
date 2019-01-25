#ifndef _AVL_H
#define _AVL_H
#include "CharString.h"
#include "stdafx.h"
struct AVLNodeData{//�ڵ�洢�ĵ��ʵ���Ϣ
	TDataType TermID;//����ID
	CharString Term;//����
	int DF=0;//���ʳ����ڶ���ƪ������
	int Occur=0;//�����ܵĳ��ִ���
};

struct AVL{//ƽ��������Ľڵ�
	AVLNodeData data;//�ڵ�洢����Ϣ
	int bf;//ƽ������
	AVL* lchild=nullptr;//����
	AVL* rchild = nullptr;//�Һ���
};

struct returnInfo {//��װ��ӽڵ�ĺ����ķ���ֵ
	AVL* link=nullptr;//��ӽڵ����������µĸ��ڵ��ָ��
	bool height_growth;//��¼�����ĸ߶��ھ����������������
};

returnInfo insert(AVL*& link,CharString str);//��ӽڵ�
AVL* Search(AVL* link, CharString str);//���ҽڵ�
AVL* Adjust(AVL* link);//����������ʹ��ƽ��
void Delete(AVL* link);//�ͷ�AVL�ڴ�ռ�
AVL* llRotate(AVL* link);//����ת
AVL* rrRotate(AVL* link);//�ҵ���ת
AVL* lrRotate(AVL* link);//�������˫��ת
AVL* rlRotate(AVL* link);//���Һ���˫��ת
CharString tree_to_string(AVL* tree);//��ƽ���������CharString�����ʾ

#endif