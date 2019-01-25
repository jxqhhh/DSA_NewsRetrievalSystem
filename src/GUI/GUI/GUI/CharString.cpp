#include "stdafx.h"
#include "CharString.h"
#include <cwchar>
#include <new>
#include <iostream>
#include <cstdlib> /* exit, EXIT_FAILURE */
#include <Windows.h>

using std::wistream;
using std::wostream;
using std::wcout;
using std::cout;
using std::endl;

CharString::CharString(){//默认构造函数
	data=new (std::nothrow) wchar_t[1];
	if(!data){
		cout<<"Failed to create a new CharString object..."<<endl;//判断是否内存分配失败
		cout<<"Exiting1..."<<endl;
		Sleep(3000);
		exit(EXIT_FAILURE);
	}
	data[0]=L'\0';
}

CharString::CharString(wchar_t c)
{
	data = new wchar_t[2];
	data[0] = c;
	data[1] = L'\0';
}

CharString::CharString(const wchar_t* charArray){//参数为const wchar_t*的构造函数
	const int length=wcslen(charArray);
	data=new (std::nothrow) wchar_t[length+1];
	if(!data){
		cout<<"Failed to create a new CharString object..."<<endl;//判断是否内存分配失败
		cout<<"Exiting2..."<<endl;
		Sleep(3000);
		exit(EXIT_FAILURE);
	}
	wmemcpy(data,charArray,wcslen(charArray)+1);
}

CharString::CharString(const CharString& str){//拷贝构造函数
	const int length=wcslen(str.data);
	data=new (std::nothrow) wchar_t[length+1];
	if(!data){
		cout<<"Failed to create a new CharString object..."<<endl;//判断是否内存分配失败
		cout<<"Exiting3..."<<endl;
		Sleep(3000);
		exit(EXIT_FAILURE);
	}
	wmemcpy(data,str.data,wcslen(str.data)+1);
}

const wchar_t* CharString::wc_str(){//转换为C语言风格的宽字符字符串
	return data;
}


CharString::~CharString(){//析构函数
	if(data!=nullptr){
		delete[] data;
		data=nullptr;
	}
}

CharString& CharString::operator = (const CharString& str){//重载=操作符
	if(data!=nullptr){
		delete[] data;
		data=nullptr;
	}
	const int length=wcslen(str.data);
	data=new (std::nothrow) wchar_t[length+1];
	if(!data){
		cout<<"Failed to create a new CharString object..."<<endl;//判断是否内存分配失败
		cout<<"Exiting4..."<<endl;
		Sleep(3000);
		exit(EXIT_FAILURE);
	}
	wmemcpy(data,str.data,wcslen(str.data)+1);
	return *this;
}

CharString CharString::operator + (const CharString& str){
	const int length1=wcslen(data);
	const int length2=wcslen(str.data);
	wchar_t* wcharArray=new (std::nothrow) wchar_t[length1+length2+1];
	wmemcpy(wcharArray,data,length1);
	wmemcpy(wcharArray+length1,str.data,length2+1);
	CharString newStr(wcharArray);
	delete[] wcharArray;
	return newStr;
}

bool CharString::compareSubStr(int pos, const wchar_t * content)
{
	int len = wcslen(content);
	if (pos < 0 || pos + len > wcslen(data)) {
		return false;
	}
	wchar_t* p = data + pos;
	for (int i = 0; i < len; i++) {
		if ((*p)!= *(content + i)) {
			return false;
		}
		p++;
	}
	return true;
}

bool CharString::operator<(const CharString & str) const
{
	return wcscmp(data, str.data) < 0;
}

CharString CharString::substring(int pos,int len) const{//截取字符串(pos从0开始)
	const int length=wcslen(data);
	if(pos>length-1 || len<=0){//pos大于length-1时或len==0时返回空串
		CharString str;
		return str;
	}
	if((pos+len)>length){//len过大时修改为允许的最大值
		len=length-pos;
	}
	wchar_t* wcharArray=new (std::nothrow) wchar_t[len+1];
	if(!wcharArray){
		cout<<"Failed to create a new CharString object..."<<endl;//判断是否内存分配失败
		cout<<"Exiting5..."<<endl;
		Sleep(3000);
		exit(EXIT_FAILURE);
	}
	wmemcpy(wcharArray,data+pos,len);
	wcharArray[len]=L'\0';
	CharString str(wcharArray);
	delete[] wcharArray;
	return str;
}

int CharString::length() const{
	return wcslen(data);
}

wostream& operator << (wostream& out,CharString& str){
	out<<str.data;
	return out;
}

bool CharString::operator == (const CharString& str){
	return wcscmp(data,str.data)==0;
}

int CharString::cmp(const CharString& str) {
	return wcscmp(data, str.data);
}

CharString CharString::operator += (const CharString& str){
	wchar_t* newData=new wchar_t[wcslen(data)+wcslen(str.data)+1];
	wmemcpy(newData,data,wcslen(data));
	wmemcpy(newData+wcslen(data),str.data,wcslen(str.data)+1);
	delete[] data;
	data=newData;
	return *this;
}

bool CharString::invalid() const{//仅由L' '、L'\n'、L'\t'组成的字符串或第一个宽字符为L'\0'的字符串视为无效
	int len=wcslen(data);
	if(len==0){
		return false;
	}
	wchar_t* pointer=data;
	while(len--){
		wchar_t tmp=*pointer;
		if((tmp!=L' ') & (tmp!=L'\t') & (tmp!=L'\n')){
			return false;
		}
		pointer++;
	}
	return true;
}

wchar_t& CharString::operator [] (int pos){
	return *(data+pos);
}

const wchar_t& CharString::operator [] (int pos) const{
	return *(data+pos);
}

CharString concat(const CharString& first,const CharString& second){
	int length1=first.length();
	int length2=second.length();
	const int size=length1+length2+1;
	const wchar_t* data=new (std::nothrow) wchar_t[size];
	if(!data){
		cout<<"Failed to create a new CharString object..."<<endl;//判断是否内存分配失败
		cout<<"Exiting6..."<<endl;
		Sleep(3000);
		exit(EXIT_FAILURE);
	}
	wchar_t* pointer=const_cast<wchar_t*>(data);
	wmemcpy(pointer,first.data,length1);
	wmemcpy(pointer+length1,second.data,length2+1);
	CharString str(data);
	delete[] data;
	return str;
}

int CharString::indexOf(const CharString& str,int pos){//查找子串第一次出现的位置，查找失败则返回-1,查找成功则返回相应的位置（从0开始编号）;pos表示从主串的第pos个宽字符（pos从0开始编号）起开始查找
	if((pos>wcslen(data)) | pos<0){
		return -1;
	}
	int* next=new int[wcslen(str.data)];
	memset(next,0,sizeof(int)*wcslen(str.data));
	next[0]=-1;
	for(int i=2;i<wcslen(str.data);i++){
		for(int j=i-1;j>0;j--){
			if(str.substring(0,j)==str.substring(i-j,j)){
				next[i]=j;
				break;
			}
		}
	}
	int i1=pos;
	int j1=0;
	const int len1=wcslen(data);
	const int len2=wcslen(str.data);
	while(i1<len1 & j1<len2){
		if(j1==-1 | *(data+i1)==*(str.data+j1)){
			i1++;
			j1++;
		}else{
			j1=next[j1];
		}
	}
	delete next;
	next=nullptr;
	if(j1==len2){
		//cout<<i1-len2<<endl;
		return i1-len2;
	}
	return -1;
}