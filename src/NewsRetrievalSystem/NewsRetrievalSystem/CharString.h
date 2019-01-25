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
	CharString& operator = (const CharString& str);//重载=操作符
	int indexOf(const CharString& T,int pos);//查找子串第一次出现的位置，查找失败则返回-1,查找成功则返回相应的位置（从0开始编号）;pos表示从主串的第pos个宽字符（pos从0开始编号）起开始查找
	CharString substring(int pos,int len) const;//截取字符串
	int length() const;//获取字符串长度
	friend wostream & operator << (wostream &out,CharString& str);//重载<<运算符
	bool operator == (const CharString& str);
	int cmp(const CharString& str);
	CharString operator += (const CharString& str);
	wchar_t& operator [] (int pos);
	bool invalid() const;//仅由L' '、L'\n'、L'\t'组成的字符串视为无效
	const wchar_t& operator [] (int pos) const;
	CharString operator + (const CharString& str);
	friend CharString concat(const CharString& first,const CharString& second);//连接字符串
	bool compareSubStr(int pos, const wchar_t* content);
	bool operator<(const CharString& str) const;
};

#endif