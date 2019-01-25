#include "stdafx.h"
#include "AVL.h"

static TDataType id = 0;//记录已经插入的节点数

returnInfo insert(AVL*& link, CharString str)//插入节点
{
	/*
	Parameters:
		link:在以link为根节点的平衡二叉树上插入
		str:插入节点所存储的关键词
	Return Value:
		返回一个returnInfo结构体，returnInfo的link表示插入后树的新的根节点，returnInfo的height_growth表示插入后树的高度有无增加
	*/
	if (link == nullptr) {//若为nullptr，直接新建节点作为父节点
		link = new AVL;
		link->bf = 0;
		link->data.Term = str;
		returnInfo info;
		link->data.TermID = id;
		id += 1;
		info.link = link;//info.link设为新的根节点
		info.height_growth = true;//从空树变为有一个节点，树的高度有增加，故令info.height_growth=true
		return info;
	}
	int result = str.cmp(link->data.Term);
	if (result == 0) {//result==0表示需要插入到平衡二叉树的关键词在平衡二叉树中已经存在，无需插入
		returnInfo info;
		info.link = link;//因为未插入，故树的根节点仍然为link
		info.height_growth = false;//因为这种情况实际未插入，树的高度不增加
		return info;
	}
	else if (result > 0) {
		returnInfo cInfo;
		cInfo=insert(link->rchild, str);//递归，在右子树插入
		link->rchild = cInfo.link;//更新link->rchild为右子树的新的根节点
		returnInfo info;
		info.link = link;
		info.height_growth = false;
		if (cInfo.height_growth) {//若右子树插入后右子树高度增加，需进行额外的调整
			link->bf -= 1;
			if (link->bf == -1) {//若右子树插入后高度增加并导致link->bf变为-1，则以link为根节点的树高度增加，info.height_growth设为true
				info.height_growth = true;
			}
			if (link->bf == -2) {//若右子树插入后高度增加并导致link->bf变为-2，则需要调整平衡，树高度不变但根节点需要修改为Adjust(link)
				info.link = Adjust(link);
			}
		}
		return info;
	}
	else {
		returnInfo cInfo = insert(link->lchild, str);//递归，在左子树插入
		link->lchild = cInfo.link;//更新link->lchild为左子树的新的根节点
		returnInfo info;
		info.link = link;
		info.height_growth = false;
		if (cInfo.height_growth) {//若左子树插入后左子树高度增加，需进行额外的调整
			link->bf += 1;
			if (link->bf == 1) {//若左子树插入后高度增加并导致link->bf变为1，则以link为根节点的树高度增加，info.height_growth设为true
				info.height_growth = true;
			}
			if (link->bf == 2) {//若左子树插入后高度增加并导致link->bf变为2，则需要调整平衡，树高度不变但根节点需要修改为Adjust(link)
				info.link = Adjust(link);
			}
		}
		return info;
	}
}

AVL* Search(AVL* link, CharString str)//查找节点
{
	/*
	Parameters:
		link:在以link为根节点的平衡二叉树上进行查找节点操作
		str:查找结点的目标是找到存储的单词等于str的节点
	Return Value:
		查找失败则返回nullptr，查找成功则返回所查找的目标节点
	*/
	if (link == nullptr) {
		return nullptr;//查找失败
	}
	int result = str.cmp(link->data.Term);//比较str和link存储的单词的大小关系
	if (result==0) {
		return link;//查找成功
	}else if (result > 0) {
		return Search(link->rchild, str);//在右子树递归查找
	}else {
		return Search(link->lchild, str);//在左子树递归查找
	}
}

AVL* Adjust(AVL* link)//调整二叉树使其平衡
{
	/*
	Parameters:
		tree:需要调整使其平衡的平衡二叉树的根节点
	Return Value:
		返回调整平衡后的平衡二叉树的新的根节点
	*/

	//根据平衡因子决定采用何种方式进行平衡旋转
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

void Delete(AVL* link)//释放平衡二叉树的内存空间
{
	/*
	Parameters:
		tree:需要释放空间的平衡二叉树的根节点
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

AVL* llRotate(AVL* link)//左单旋转
{
	/*
	Parameters:
		tree:需要左单旋转使其平衡的平衡二叉树的根节点
	Return Value:
		返回调整平衡后的平衡二叉树的新的根节点
	*/
	AVL* C = link->rchild;
	link->rchild = C->lchild;
	C->lchild = link;
	C->bf = 0;
	link->bf = 0;
	return C;
}

AVL* rrRotate(AVL* link)//右单旋转
{
	/*
	Parameters:
		tree:需要右单旋转使其平衡的平衡二叉树的根节点
	Return Value:
		返回调整平衡后的平衡二叉树的新的根节点
	*/
	AVL* B = link->lchild;
	link->lchild = B->rchild;
	B->rchild = link;
	B->bf = 0;
	link->bf = 0;
	return B;
}

AVL* lrRotate(AVL* link)//先左旋后右旋
{	
	/*
	Parameters:
		tree:需要先左旋后右旋使其平衡的平衡二叉树的根节点
	Return Value:
		返回调整平衡后的平衡二叉树的新的根节点
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

AVL* rlRotate(AVL* link)//先右旋后左旋
{
	/*
	Parameters:
		tree:需要先右旋后左旋旋转使其平衡的平衡二叉树的根节点
	Return Value:
		返回调整平衡后的平衡二叉树的新的根节点
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

CharString tree_to_string(AVL * tree) {// 将平衡二叉树用CharString广义表示
	/*
	Parameters:
		tree:需要转化为广义表达的平衡二叉树的根节点
	Return Value:
		返回AVL的广义表达
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