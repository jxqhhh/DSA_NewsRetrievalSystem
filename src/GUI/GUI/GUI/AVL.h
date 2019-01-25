#ifndef _AVL_H
#define _AVL_H
#include "CharString.h"
#include "stdafx.h"
struct AVLNodeData{//节点存储的单词的信息
	TDataType TermID;//单词ID
	CharString Term;//单词
	int DF=0;//单词出现在多少篇文章中
	int Occur=0;//单词总的出现次数
};

struct AVL{//平衡二叉树的节点
	AVLNodeData data;//节点存储的信息
	int bf;//平衡因子
	AVL* lchild=nullptr;//左孩子
	AVL* rchild = nullptr;//右孩子
};

struct returnInfo {//封装添加节点的函数的返回值
	AVL* link=nullptr;//添加节点后的子树的新的根节点的指针
	bool height_growth;//记录子树的高度在经过插入后有无增长
};

returnInfo insert(AVL*& link,CharString str);//添加节点
AVL* Search(AVL* link, CharString str);//查找节点
AVL* Adjust(AVL* link);//调整二叉树使其平衡
void Delete(AVL* link);//释放AVL内存空间
AVL* llRotate(AVL* link);//左单旋转
AVL* rrRotate(AVL* link);//右单旋转
AVL* lrRotate(AVL* link);//先左后右双旋转
AVL* rlRotate(AVL* link);//先右后左双旋转
CharString tree_to_string(AVL* tree);//将平衡二叉树用CharString广义表示

#endif