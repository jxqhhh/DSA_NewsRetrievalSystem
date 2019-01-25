#ifndef _CHARSTRING_H
#define _CHARSTRING_H

#include <iostream>

#include "stdafx.h"
using std::wostream;

class CharString{
	wchar_t* data;
public:
	CharString();
	CharString(wchar_t c);
	CharString(const wchar_t* charArray);
	CharString(const CharString& str);
	const wchar_t* wc_str(); 
	~CharString();
	CharString& operator = (const CharString& str);//����=������
	int indexOf(const CharString& T,int pos);//�����Ӵ���һ�γ��ֵ�λ�ã�����ʧ���򷵻�-1,���ҳɹ��򷵻���Ӧ��λ�ã���0��ʼ��ţ�;pos��ʾ�������ĵ�pos�����ַ���pos��0��ʼ��ţ���ʼ����
	CharString substring(int pos,int len) const;//��ȡ�ַ���
	int length() const;//��ȡ�ַ�������
	friend wostream & operator << (wostream &out,CharString& str);//����<<�����
	bool operator == (const CharString& str);
	int cmp(const CharString& str);
	CharString operator += (const CharString& str);
	wchar_t& operator [] (int pos);
	bool invalid() const;//����L' '��L'\n'��L'\t'��ɵ��ַ�����Ϊ��Ч
	const wchar_t& operator [] (int pos) const;
	CharString operator + (const CharString& str);
	friend CharString concat(const CharString& first,const CharString& second);//�����ַ���
	bool compareSubStr(int pos, const wchar_t* content);
	bool operator<(const CharString& str) const;
};

#endif