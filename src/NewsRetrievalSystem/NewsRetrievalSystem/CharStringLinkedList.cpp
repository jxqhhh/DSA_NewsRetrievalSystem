#include "stdafx.h"
#include "CharStringLinkedList.h"
#include <iostream>
#include <locale>
using std::cout;
using std::endl;
using std::wcout;

CharStringLinkedList::CharStringLinkedList()
{
	head=new Link;
	head->next=nullptr;
	tail=head;
	len=0;
}


CharStringLinkedList::~CharStringLinkedList()
{
	while(len--){
		Link* current=head->next;
		head->next=current->next;
		delete current;
		current=nullptr;
	}
	delete head;
	head=nullptr;
}

CharStringLinkedList::CharStringLinkedList(const CharStringLinkedList& list){
	head=new Link;
	head->next=nullptr;
	tail=head;
	len=list.len;
	Link* pointer1=head;
	Link* pointer2=list.head;
	for(int i=0;i<len;i++){
		pointer1->next=new Link;
		pointer1=pointer1->next;
		tail=pointer1;
		pointer2=pointer2->next;
		pointer1->next=nullptr;
		pointer1->data=pointer2->data;
	}
}

void CharStringLinkedList::add(TELemType& data){//�������������
	add(len,data);
}

void CharStringLinkedList::add(int pos,TELemType& data){//ʹ�ò�����λ��Ϊpos��pos�ķ�Χ��0��ʼ��
	if(pos>len){
		cout<<"Error!"<<endl;
		return;
	}
	Link* current=head;
	for(int j=0;j<pos;j++){
		current=current->next;
	}
	Link* newLink=new Link;
	newLink->data=data;
	newLink->next=current->next;
	current->next=newLink;
	if(pos==len){
		tail=tail->next;
	}
	len++;
}

bool CharStringLinkedList::remove(TELemType& str){//ɾ���ɹ�����True�����򷵻�False
	Link* current=head;
	if(len==0){
		return false;
	}
	current=current->next;
	Link* prev =head;
	for(int i=0;i<len;i++){
		if(current->data==str){
			prev->next=current->next;
			delete current;
			current=nullptr;
			len--;
			return true;
		}
		prev=current;
		current=current->next;
	}
	return false;
}


bool CharStringLinkedList::remove(int pos){//ɾ���ɹ�����True�����򷵻�False
	if(pos<0 || pos>=len){
		return false;
	}
	Link* current=head->next;
	Link* prev=head;
	for(int i=0;i<pos;i++){
		prev=current;
		current=current->next;
	}
	prev->next=current->next;
	delete current;
	current=nullptr;
	len--;
	return true;
}

int CharStringLinkedList::search(TELemType& str){//����ĳԪ��λ�ã�����ʧ���򷵻�-1
	Link* current=head;
	if(len==0){
		return -1;
	}
	current=current->next;
	for(int i=0;i<len;i++){
		if(current->data==str){
			return i;
		}
		current=current->next;
	}
	return -1;
}

int CharStringLinkedList::length() const{
	return len;
}

void CharStringLinkedList::writeIntoFile(std::wofstream& wfs) const{//���δ�ӡ�洢�Ŀ��ַ���
	int epoch=len;
	Link* pointer=head;
	while(epoch--){
		pointer=pointer->next;
		wfs<<(*pointer).data<<std::endl;
	}
}

void CharStringLinkedList::clearInvalidCharString(){//ɾ����������Ч���ַ���������L' '��L'\n'��L'\t'��ɵ��ַ�����Ϊ��Ч��
	Link* prev=head;
	Link* cur=head->next;
	while(cur!=nullptr){
		if(cur->data.invalid()){
			prev->next=cur->next;
			delete cur;
			len--;
			cur=prev->next;
		}else{
			prev=cur;
			cur=prev->next;
		}
	}
}

CharString& CharStringLinkedList::getLastElement() const{
	return tail->data;
}

const Link* CharStringLinkedList::getHead() const{
	return head;
}

Link* CharStringLinkedList::getFirst() const {
	return head;
}
