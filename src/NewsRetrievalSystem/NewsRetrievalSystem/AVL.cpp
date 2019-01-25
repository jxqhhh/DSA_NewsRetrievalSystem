#include "stdafx.h"
#include "AVL.h"

static TDataType id = 0;//��¼�Ѿ�����Ľڵ���

returnInfo insert(AVL*& link, CharString str)//����ڵ�
{
	/*
	Parameters:
		link:����linkΪ���ڵ��ƽ��������ϲ���
		str:����ڵ����洢�Ĺؼ���
	Return Value:
		����һ��returnInfo�ṹ�壬returnInfo��link��ʾ����������µĸ��ڵ㣬returnInfo��height_growth��ʾ��������ĸ߶���������
	*/
	if (link == nullptr) {//��Ϊnullptr��ֱ���½��ڵ���Ϊ���ڵ�
		link = new AVL;
		link->bf = 0;
		link->data.Term = str;
		returnInfo info;
		link->data.TermID = id;
		id += 1;
		info.link = link;//info.link��Ϊ�µĸ��ڵ�
		info.height_growth = true;//�ӿ�����Ϊ��һ���ڵ㣬���ĸ߶������ӣ�����info.height_growth=true
		return info;
	}
	int result = str.cmp(link->data.Term);
	if (result == 0) {//result==0��ʾ��Ҫ���뵽ƽ��������Ĺؼ�����ƽ����������Ѿ����ڣ��������
		returnInfo info;
		info.link = link;//��Ϊδ���룬�����ĸ��ڵ���ȻΪlink
		info.height_growth = false;//��Ϊ�������ʵ��δ���룬���ĸ߶Ȳ�����
		return info;
	}
	else if (result > 0) {
		returnInfo cInfo;
		cInfo=insert(link->rchild, str);//�ݹ飬������������
		link->rchild = cInfo.link;//����link->rchildΪ���������µĸ��ڵ�
		returnInfo info;
		info.link = link;
		info.height_growth = false;
		if (cInfo.height_growth) {//��������������������߶����ӣ�����ж���ĵ���
			link->bf -= 1;
			if (link->bf == -1) {//�������������߶����Ӳ�����link->bf��Ϊ-1������linkΪ���ڵ�����߶����ӣ�info.height_growth��Ϊtrue
				info.height_growth = true;
			}
			if (link->bf == -2) {//�������������߶����Ӳ�����link->bf��Ϊ-2������Ҫ����ƽ�⣬���߶Ȳ��䵫���ڵ���Ҫ�޸�ΪAdjust(link)
				info.link = Adjust(link);
			}
		}
		return info;
	}
	else {
		returnInfo cInfo = insert(link->lchild, str);//�ݹ飬������������
		link->lchild = cInfo.link;//����link->lchildΪ���������µĸ��ڵ�
		returnInfo info;
		info.link = link;
		info.height_growth = false;
		if (cInfo.height_growth) {//��������������������߶����ӣ�����ж���ĵ���
			link->bf += 1;
			if (link->bf == 1) {//�������������߶����Ӳ�����link->bf��Ϊ1������linkΪ���ڵ�����߶����ӣ�info.height_growth��Ϊtrue
				info.height_growth = true;
			}
			if (link->bf == 2) {//�������������߶����Ӳ�����link->bf��Ϊ2������Ҫ����ƽ�⣬���߶Ȳ��䵫���ڵ���Ҫ�޸�ΪAdjust(link)
				info.link = Adjust(link);
			}
		}
		return info;
	}
}

AVL* Search(AVL* link, CharString str)//���ҽڵ�
{
	/*
	Parameters:
		link:����linkΪ���ڵ��ƽ��������Ͻ��в��ҽڵ����
		str:���ҽ���Ŀ�����ҵ��洢�ĵ��ʵ���str�Ľڵ�
	Return Value:
		����ʧ���򷵻�nullptr�����ҳɹ��򷵻������ҵ�Ŀ��ڵ�
	*/
	if (link == nullptr) {
		return nullptr;//����ʧ��
	}
	int result = str.cmp(link->data.Term);//�Ƚ�str��link�洢�ĵ��ʵĴ�С��ϵ
	if (result==0) {
		return link;//���ҳɹ�
	}else if (result > 0) {
		return Search(link->rchild, str);//���������ݹ����
	}else {
		return Search(link->lchild, str);//���������ݹ����
	}
}

AVL* Adjust(AVL* link)//����������ʹ��ƽ��
{
	/*
	Parameters:
		tree:��Ҫ����ʹ��ƽ���ƽ��������ĸ��ڵ�
	Return Value:
		���ص���ƽ����ƽ����������µĸ��ڵ�
	*/

	//����ƽ�����Ӿ������ú��ַ�ʽ����ƽ����ת
	int bf1 = link->bf;
	if (bf1 == 2) {
		int bf2 = link->lchild->bf;
		if (bf2 == 1) {
			return rrRotate(link);
		}
		else {
			return lrRotate(link);
		}
	}
	else {
		int bf2 = link->rchild->bf;
		if (bf2 == -1) {
			return llRotate(link);
		}
		else {
			return rlRotate(link);
		}
	}
}

void Delete(AVL* link)//�ͷ�ƽ����������ڴ�ռ�
{
	/*
	Parameters:
		tree:��Ҫ�ͷſռ��ƽ��������ĸ��ڵ�
	Return Value:
		none
	*/
	if (link == nullptr) {
		return;
	}
	Delete(link->lchild);
	Delete(link->rchild);
	delete link;
}

AVL* llRotate(AVL* link)//����ת
{
	/*
	Parameters:
		tree:��Ҫ����תʹ��ƽ���ƽ��������ĸ��ڵ�
	Return Value:
		���ص���ƽ����ƽ����������µĸ��ڵ�
	*/
	AVL* C = link->rchild;
	link->rchild = C->lchild;
	C->lchild = link;
	C->bf = 0;
	link->bf = 0;
	return C;
}

AVL* rrRotate(AVL* link)//�ҵ���ת
{
	/*
	Parameters:
		tree:��Ҫ�ҵ���תʹ��ƽ���ƽ��������ĸ��ڵ�
	Return Value:
		���ص���ƽ����ƽ����������µĸ��ڵ�
	*/
	AVL* B = link->lchild;
	link->lchild = B->rchild;
	B->rchild = link;
	B->bf = 0;
	link->bf = 0;
	return B;
}

AVL* lrRotate(AVL* link)//������������
{	
	/*
	Parameters:
		tree:��Ҫ������������ʹ��ƽ���ƽ��������ĸ��ڵ�
	Return Value:
		���ص���ƽ����ƽ����������µĸ��ڵ�
	*/
	AVL* B = link->lchild;
	AVL* E = B->rchild;
	AVL* F = E->lchild;
	AVL* G = E->rchild;
	B->rchild = F;
	link->lchild = G;
	E->lchild = B;
	E->rchild = link;
	E->bf = 0;
	B->bf = 0;
	link->bf = 1;
	return E;
}

AVL* rlRotate(AVL* link)//������������
{
	/*
	Parameters:
		tree:��Ҫ��������������תʹ��ƽ���ƽ��������ĸ��ڵ�
	Return Value:
		���ص���ƽ����ƽ����������µĸ��ڵ�
	*/
	AVL* C = link->rchild;
	AVL* D = C->lchild;
	AVL* G = D->rchild;
	AVL* F = D->lchild;
	C->lchild = G;
	link->rchild = F;
	D->lchild = link;
	D->rchild = C;
	D->bf = 0;
	C->bf = 0;
	link->bf = 1;
	return D;
}

CharString tree_to_string(AVL * tree) {// ��ƽ���������CharString�����ʾ
	/*
	Parameters:
		tree:��Ҫת��Ϊ�������ƽ��������ĸ��ڵ�
	Return Value:
		����AVL�Ĺ�����
	*/
	CharString str;
	if (tree == nullptr) {
		return str;
	}
	str += tree->data.Term;
	if (tree->rchild == nullptr && tree->lchild == nullptr) {
		return str;
	}
	str += L"(";
	str += tree_to_string(tree->lchild);
	str += L",";
	str += tree_to_string(tree->rchild);
	str += L")";
	return str;
}