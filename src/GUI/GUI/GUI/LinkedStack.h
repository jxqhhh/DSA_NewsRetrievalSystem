#ifndef _LINKEDSTACK_H
#define _LINKEDSTACK_H

#include "stdafx.h"
template<class T> struct LNode{
	T data;
	LNode<T>* prev;
};

template<class T> class LinkedStack{
	LNode<T>* base;
	LNode<T>* front;
	int length;
public:
	LinkedStack();
	LinkedStack(LinkedStack& stack);
	LinkedStack& operator = (LinkedStack& stack);//÷ÿ‘ÿ∏≥÷µ‘ÀÀ„∑˚
	~LinkedStack();
	void push(T data);//»Î’ª
	bool pop(T& data);//≥ˆ’ª
	bool pop();//≥ˆ’ª
	bool top(T& data) const;//ªÒ»°’ª∂•‘™Àÿ≤¢±£¥ÊµΩT& data÷–
	int getLength() const;
	bool empty() const;//≈–∂œ’ª «∑Òø’
};

#include "stdafx.h"
#include <new>
#include <cstdlib> /* exit, EXIT_FAILURE */
#include <Windows.h>
#include <iostream>
using std::cout;
using std::endl;

template<class T> int LinkedStack<T>::getLength() const{
	return length;
}

template<class T> bool LinkedStack<T>::empty() const{
	return length==0;
}

template<class T> LinkedStack<T>& LinkedStack<T>::operator=(LinkedStack& stack){
	LNode<T>* cur=front;
	LNode<T>* p;
	while(cur!=nullptr){
		p=cur;
		cur=cur->prev;
		delete p;
	}
	LNode<T>* pointer1=new (std::nothrow) LNode<T>;
	if(!pointer1){
		cout<<"Failed to create a new LNode object..."<<endl;//≈–∂œ «∑Òƒ⁄¥Ê∑÷≈‰ ß∞‹
		cout<<"Exiting..."<<endl;
		Sleep(3000);
		exit(EXIT_FAILURE);
	}
	front=pointer1;
	LNode<T>* pointer2=stack.front;
	pointer1->data=pointer2->data;
	pointer1->prev=nullptr;
	while(pointer2->prev!=nullptr){
		pointer1->prev=new (std::nothrow) LNode<T>;
		if(!(pointer1->prev)){
			cout<<"Failed to create a new LNode object..."<<endl;//≈–∂œ «∑Òƒ⁄¥Ê∑÷≈‰ ß∞‹
			cout<<"Exiting..."<<endl;
			Sleep(3000);
			exit(EXIT_FAILURE);
		}
		pointer1=pointer1->prev;
		pointer2=pointer2->prev;
		pointer1->data=pointer2->data;
		pointer1->prev=nullptr;
	}
	base=pointer1;
	length=stack.length;
}

template<class T> LinkedStack<T>::LinkedStack(){
	length=0;
	base=new (std::nothrow) LNode<T>;
	if(!base){
		cout<<"Failed to create a new LNode object..."<<endl;//≈–∂œ «∑Òƒ⁄¥Ê∑÷≈‰ ß∞‹
		cout<<"Exiting..."<<endl;
		Sleep(3000);
		exit(EXIT_FAILURE);
	}else{
		base->prev=nullptr;
		front=base;
	}
}

template<class T> LinkedStack<T>::LinkedStack(LinkedStack& stack){
	LNode<T>* pointer1=new (std::nothrow) LNode<T>;
	if(!pointer1){
		cout<<"Failed to create a new LNode object..."<<endl;//≈–∂œ «∑Òƒ⁄¥Ê∑÷≈‰ ß∞‹
		cout<<"Exiting..."<<endl;
		Sleep(3000);
		exit(EXIT_FAILURE);
	}
	front=pointer1;
	LNode<T>* pointer2=stack.front;
	pointer1->data=pointer2->data;
	pointer1->prev=nullptr;
	while(pointer2->prev!=nullptr){
		pointer1->prev=new (std::nothrow) LNode<T>;
		if(!(pointer1->prev)){
			cout<<"Failed to create a new LNode object..."<<endl;//≈–∂œ «∑Òƒ⁄¥Ê∑÷≈‰ ß∞‹
			cout<<"Exiting..."<<endl;
			Sleep(3000);
			exit(EXIT_FAILURE);
		}
		pointer1=pointer1->prev;
		pointer2=pointer2->prev;
		pointer1->data=pointer2->data;
		pointer1->prev=nullptr;
	}
	base=pointer1;
	length=stack.length;
}

template<class T> void LinkedStack<T>::push(T data){
	LNode<T>* newFront=new (std::nothrow) LNode<T>;
	if(!newFront){
		cout<<"Failed to create a new LNode object..."<<endl;//≈–∂œ «∑Òƒ⁄¥Ê∑÷≈‰ ß∞‹
		cout<<"Exiting..."<<endl;
		Sleep(3000);
		exit(EXIT_FAILURE);
	}
	newFront->prev=front;
	newFront->data=data;
	front=newFront;
	length++;
}

template<class T> bool LinkedStack<T>::pop(T& data){
	if(front==base){
		return false;
	}
	data=front->data;
	LNode<T>* p=front;
	front=front->prev;
	delete p;
	length--;
	return true;
}

template<class T> bool LinkedStack<T>::pop(){
	if(front==base){
		return false;
	}
	LNode<T>* p=front;
	front=front->prev;
	delete p;
	length--;
	return true;
}

template<class T> bool LinkedStack<T>::top(T& data) const{
	if(front==base){
		return false;
	}
	data=front->data;
	return true;
}

template<class T> LinkedStack<T>::~LinkedStack(){
	LNode<T>* cur=front;
	LNode<T>* p;
	while(cur!=nullptr){
		p=cur;
		cur=cur->prev;
		delete p;
	}
}

#endif