#include "stdafx.h"
#include "NewsRetrievalTool.h"
#include "LinkedStack.h"
#include "io.h"
#include <fstream>//执行文件读写等基本操作时需要用到
#include <iostream>//执行文件读写等基本操作时需要用到
#include <sstream>//执行文件读写等基本操作时需要用到
#include <string>//执行文件读写等基本操作时需要用到
#include <locale>//执行文件读写等基本操作时需要用到
#include <cwchar>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>


NewsRetrievalTool::NewsRetrievalTool(){
	initDictionary();//初始化词库、tree、input_path和output_path
	initDocumentLinkedList();//初始化文档链表

	//读取数据库内所有网页的标题信息并存储到titles中:
	std::locale loc("Chinese-simplified");
	std::wstring tmpStr;
	for (int i = 0; i < DATABASE_SIZE; i++) {
		std::wifstream s((CharString(output_path) + std::to_wstring(i).c_str() + CharString(L".info")).wc_str());
		s.imbue(loc);
		getline(s, tmpStr);
		titles[i]=tmpStr.c_str();
		s.close();
	}
}


NewsRetrievalTool::~NewsRetrievalTool(){//析构函数
	if(input_path!=nullptr){
		delete[] input_path;
	}
	if(output_path!=nullptr){
		delete[] output_path;
	}
	if(path!=nullptr){
		delete[] path;
	}
	Delete(tree);
	if (documentList != nullptr) {
		delete documentList;
	}
}

void NewsRetrievalTool::initDictionary(){//执行载入词库等初始化操作
	//读取配置文件的全部内容
	std::locale loc("Chinese-simplified");
	std::wfstream wfs(L"path.Config", std::fstream::in | std::fstream::out);
	wfs.imbue(loc);
	std::wstringstream buffer;
	buffer.imbue(loc);
	buffer << wfs.rdbuf(); 
	std::wstring contents(buffer.str());
	wfs.close();
	std::wcout.imbue(loc);
	CharString str(contents.c_str());
	buffer.str(L"");

	//设置词库文件的相对路径
	int pos=str.indexOf(L"PATHDIR=",0);
	int pos1=str.indexOf(L"\"",pos)+1;
	int pos2=str.indexOf(L"\"",pos1);
	path=new wchar_t[pos2-pos1+1];
	path[pos2-pos1]=L'\0';
	wmemcpy(path,str.substring(pos1,pos2-pos1).wc_str(),pos2-pos1);
	//设置输入文件的相对路径
	pos=str.indexOf(L"INPUT_PATH_DIR=",0);
	pos1=str.indexOf(L"\"",pos)+1;
	pos2=str.indexOf(L"\"",pos1);
	input_path=new wchar_t[pos2-pos1+1];
	input_path[pos2-pos1]=L'\0';
	wmemcpy(input_path,str.substring(pos1,pos2-pos1).wc_str(),pos2-pos1);pos=str.indexOf(L"INPUT_PATH_DIR=",0);
	//设置输出文件的相对路径
	pos=str.indexOf(L"OUTPUT_PATH_DIR=",0);
	pos1=str.indexOf(L"\"",pos)+1;
	pos2=str.indexOf(L"\"",pos1);
	output_path=new wchar_t[pos2-pos1+1];
	output_path[pos2-pos1]=L'\0';
	wmemcpy(output_path,str.substring(pos1,pos2-pos1).wc_str(),pos2-pos1);

	//初始化词库
	std::wfstream wfs2((CharString(path)+L"词库.dic").wc_str(), std::fstream::in | std::fstream::out);
	wfs2.imbue(loc);
	std::wstringstream buffer2;
	buffer2.imbue(loc);
	buffer2 << wfs2.rdbuf(); 
	std::wstring contents2(buffer2.str());
	wfs2.close();
	std::wcout.imbue(loc);
	CharString str2(contents2.c_str());
	buffer2.str(L"");
	int prev=0;
	pos=str2.indexOf(L"\n",prev);
	const wchar_t* key0=&str2[0];
	const wchar_t* key;
	int max=str2.length();
	TDataType len = 0;
	while(pos+1<max){//依次处理词库文件的每一行
		key=key0+prev;
		const int key_len=pos-prev;
		HashNode* newNode=new HashNode;
		delete[] newNode->str;
		newNode->str=new wchar_t[key_len+1];
		wmemcpy(newNode->str,key,key_len);
		newNode->str[key_len]=L'\0';
		volatile TDataType result=compute_hash(newNode->str,HashTable::HashTableSize);//计算哈希值

		//更新哈希表
		if(lexicon.hashData[result].next==nullptr){
			lexicon.hashData[result].next=newNode;
		}else{
			newNode->next=lexicon.hashData[result].next;
			lexicon.hashData[result].next=newNode;
		}
		prev=pos+1;
		pos++;
		while(*(key0+(++pos))!=L'\n'){
		}
		tree=insert(tree, newNode->str).link;//将关键词插入到二叉平衡树tree中
		len++;
	}
	amount_of_words=len + 1;//更新词库的词数
}

void NewsRetrievalTool::initDocumentLinkedList()//初始化文档链表
{
	documentList = new documentLinkedList(amount_of_words);//动态分配内存空间
	struct _wfinddata_t fileinfo;
	intptr_t ld;
	int* num = new int[amount_of_words];//记录文档中每个单词出现的次数
	std::locale loc("Chinese-simplified");

	//遍历output_path目录下的所有分词结果文件
	if ((ld = _wfindfirst((CharString(output_path) + CharString(L"*.txt")).wc_str(), &fileinfo)) != -1) {
		do {
			std::set<int> exisitng_keywords;//记录当前正在遍历的txt分词结果文件中出现的所有关键词，使用std::set<int>防止重复
			memset(num, 0, sizeof(int)*amount_of_words);//每当分析一个新的txt分词结果文件时，进行一次初始化

			//利用txt文件的文件名获取文档编号DocID
			std::wstringstream ss;
			ss << CharString(fileinfo.name).substring(0, CharString(fileinfo.name).indexOf(L".", 0));
			int DocID;
			ss >> DocID;
			ss.clear();
			ss.str(L"");

			std::wifstream s((CharString(output_path) + fileinfo.name).wc_str());
			s.imbue(loc);
			std::wstring str;
			while (std::getline(s, str)) {//逐行读取txt分词结果文件
				AVL* avlNode = Search(tree, str.c_str());//查询二叉树中的相应节点
				if (num[avlNode->data.TermID]==0){
					avlNode->data.DF += 1;//如果在该txt文件中第一次读到该关键词，则更新avlNode->data.DF
				}
				avlNode->data.Occur += 1;
				num[avlNode->data.TermID]++;//更新num
				exisitng_keywords.insert(avlNode->data.TermID);//将读取的关键词的ID加入existing_keywords中
			}
			for (auto it = exisitng_keywords.begin(); it != exisitng_keywords.end(); it++) {
				documentList->Add(*it, DocID, num[*it]);//将记录插入到文档链表内
			}
			s.close();
		} while (_wfindnext(ld, &fileinfo) == 0);
	}
	delete[] num;
	_findclose(ld);
}

void NewsRetrievalTool::extracrtAllInfo(bool write_into_file){//解析和分词input_path路径下的所有网页文件
	/*
	Parameters:
		write_into_file:为true时将解析结果保存到output_path对应的目录下和进行分词
	Return Value:
		none
	*/
	struct _wfinddata_t fileinfo;
	intptr_t ld;
	if((ld=_wfindfirst((CharString(input_path)+CharString(L"*.html")).wc_str(),&fileinfo))!=-1){
		do{//遍历input_path目录下的所有html文件
			NewsInfo info;
			extractinfo(fileinfo.name,info,write_into_file);
		}while(_wfindnext(ld,&fileinfo)==0);
	}
	_findclose(ld);
}

void NewsRetrievalTool::extractinfo(const wchar_t* filename,NewsInfo& info, bool write_results_into_file){//执行解析网页操作
	/*
	Parameters:
		filename:不包含相对路径的文件名
		info:存放解析结果
		write_results_into_file:是否要将解析的结果写到*.info文件中并进行分词
	Return Value:
		none
	*/
	
	//读取html文件的全部内容
	std::locale loc("Chinese-simplified");
	CharString str;
	std::wifstream s((CharString(input_path)+filename).wc_str());
	s.imbue(loc);
	std::wstring tmpStr;
	while (getline(s, tmpStr)) {
		str += tmpStr.c_str();
	}
	s.close();
	//解析网页
	int startPos=0;//当前在检查的L'<'的位置
	if(str.substring(0,15)==CharString(L"<!DOCTYPE HTML>")){
		startPos=str.indexOf(L"<",str.indexOf(L">",0));
	}else{
		startPos=str.indexOf(L"<",0);
	}
	enum tagType{post_text,post_desc,endtext,endtext_clearfix,post_content_main,g_w1200_posr,artical_summary,hidden,p,h,otherdiv,blockElement,inlineElement,html};
	//enum tagType：html表示<html>标签，p表示<p>标签，blockElement表示除了p和div以外的块级元素，inlineElement表示内嵌元素,h表示<h1>/<h2>/.../<h6>
	//enum tagType：post_text为class="post_text"的<div>，endtext为class="end-text"的<div>，post_desc为class="post_desc"的<div>，endtext_clearfix为class="end-text clearfix"的<div>，post_content_main为class="post_content_main"或"ep-content-main"的<div>
	//enum tagType：artical_summary为class="artical-summary"的<div>，g_w1200_posr为class="g-w1200 posr"的<div>，otherdiv表示其他的<div>
	//enum tagType：hidden为class="hidden"的textarea
	bool in_p=false;//表示当前是否在处理p标签内的内容
	bool in_h=false;//表示当前是否在处理h1/h2/.../h6标签内的内容
	bool in_post_text=false;//表示当前是否在处理class="post_text"的div标签内的内容
	bool in_endtext=false;//表示当前是否在处理class="end-text"的div标签内的内容
	bool in_endtext_clearfix=false;//表示当前是否在处理class="end-text clearfix"的div标签内的内容
	bool in_post_desc=false;//表示当前是否在处理class="post_desc"的div标签内的内容
	bool in_post_content_main=false;//表示当前是否在处理class="post_content_main"或"ep-content-main"的div标签内的内容
	bool in_artical_summary=false;//表示当前是否在处理class="artical-summary"的div标签内的内容
	bool in_g_w1200_posr=false;//表示当前是否在处理class="g-w1200 posr"的div标签内的内容
	bool in_hidden=false;//表示当前是否在处理class="post_desc"的textarea标签内的内容
	bool add_into_prev_string=false;//若某个<p>...</p>内还含有内嵌元素，则该<p>...</p>内的所有文本都应该被存储在一个共同的CharString里。该bool型变量为true时程序会知道需要存储在一个共同的CharString里（<h1>/../<h6>标签有类似的问题）

	bool isNormalFormat=str.indexOf(CharString(L"<body>"),0)>-1;//若该网页为类似于样例中6.html的格式时，isNormalFormat为false，否则为true。对于形似6.html的网页，提取标题、时间、来源的方法比较特殊
	//获取新闻正文
	LinkedStack<tagType> tagStack;
	while(startPos>-1){
		if(str[startPos+1]==L'/'){//判断是否需要退栈
			tagType type;
			tagStack.top(type);
			if((!(str.compareSubStr(startPos,L"</p>"))) && type==p ){//在HTML源文件中<p>可能单独出现而无</p>配套，这种不规范的网页需要特殊处理
				add_into_prev_string=false;
				tagStack.pop();
				in_p=false;
				continue;
			}
			if(str.compareSubStr(startPos,L"</p>") && type!=p){//在HTML源文件中</p>可能单独出现而无<p>配套，这种不规范的网页需要特殊处理
				startPos=str.indexOf(L"<",startPos+1);
				continue;
			}
			tagStack.pop();
			if(in_post_text){
				int a=1;
				a++;
			}
			if(type==post_text){
				in_post_text=false;
			}else if(type==p){
				in_p=false;
				add_into_prev_string=false;
			}else if(type==post_desc){
				in_post_desc=false;
				int tmp1=str.indexOf(L">",startPos)+1;
				int tmp2=str.indexOf(L"<",tmp1);
				info.text.add(str.substring(tmp1,tmp2-tmp1));
			}else if(type==endtext){
				in_endtext=false;
			}else if(type==endtext_clearfix){
				in_endtext_clearfix=false;
			}else if(type==h){
				in_h=false;
				add_into_prev_string=false;
			}else if(type==hidden){
				in_hidden=false;
			}else if(type==post_content_main){
				in_post_content_main=false;
			}else if(type==g_w1200_posr){
				in_g_w1200_posr=false;
			}else if(type==artical_summary){
				in_artical_summary=false;
			}
			int tmp=str.indexOf(L">",startPos)+1;
			startPos=str.indexOf(L"<",startPos+1);
			if(tmp!=startPos && (in_p || in_h) && (in_post_text || in_post_desc || in_artical_summary || in_endtext || in_endtext_clearfix || in_hidden)){//判断是否要提取到info.text内
				if(add_into_prev_string){
					info.text.getLastElement()+=str.substring(tmp,startPos-tmp);
				}else{
					info.text.add(str.substring(tmp,startPos-tmp));
					add_into_prev_string=true;
				}
			}
		}else if(str.substring(startPos+1,4)==L"html"){//处理<html>
			tagStack.push(html);
			startPos=str.indexOf(L"<",startPos+1);
		}else if(str.compareSubStr(startPos,L"<div")){//处理<div>
			tagType type;
			tagStack.top(type);
			if(type==p){//在HTML源文件中<p>可能单独出现而无</p>配套，这种不规范的网页需要特殊处理
				add_into_prev_string=false;
				tagStack.pop();
				in_p=false;
			}
			int index1=str.indexOf(L"class=\"",startPos);
			int index2=str.indexOf(L">",startPos);
			if(index1>-1 && index1<index2){
				index1+=7;
				int index3=str.indexOf(L"\"",index1);
				CharString s=str.substring(index1,index3-index1);
				if(s==L"post_text"){
					tagStack.push(post_text);
					in_post_text=true;
				}else if(s==L"end-text"){
					tagStack.push(endtext);
					in_endtext=true;
				}else if(s==L"post_desc"){
					tagStack.push(post_desc);
					in_post_desc=true;
					int tmp1=str.indexOf(L">",startPos)+1;
					int tmp2=str.indexOf(L"<",tmp1);
					info.text.add(str.substring(tmp1,tmp2-tmp1));
				}else if(s==L"end-text clearfix"){
					tagStack.push(endtext_clearfix);
					in_endtext_clearfix=true;
				}else if(s==L"post_content_main" || s==L"ep-content-main"){
					tagStack.push(post_content_main);
					in_post_content_main=true;
				}else if(s==L"g-w1200 posr"){
					tagStack.push(g_w1200_posr);
					in_g_w1200_posr=true;
				}else if((s==L"post_time_source" || s==L"ep-time-soure cDGray")){
					//获取新闻的时间：
					int tmp=str.indexOf(L":",startPos);
					info.time=str.substring(tmp-13,19);
					//获取新闻的来源：
					int tmp1=str.indexOf(L">",str.indexOf(L"<a",tmp))+1;
					int tmp2=str.indexOf(L"<",tmp1);
					int tmp3=str.indexOf(L">",tmp2)+1;
					int tmp4=str.indexOf(L"<",tmp3);
					info.source+=str.substring(tmp1,tmp2-tmp1);
					info.source+=str.substring(tmp3,tmp4-tmp3);
				}else if(s==L"artical-summary"){
					tagStack.push(artical_summary);
					in_artical_summary=true;
				}else{
					tagStack.push(otherdiv);
				}
			}else{
				tagStack.push(otherdiv);
			}
			startPos=str.indexOf(L"<",startPos+1);
		}else if(str.compareSubStr(startPos,L"<p>") || str.compareSubStr(startPos,L"<p ")){//处理<p>
			tagType type;
			tagStack.top(type);
			if(type==p){//在HTML源文件中<p>可能单独出现而无</p>配套，这种不规范的网页需要特殊处理
				tagStack.pop();
				in_p=false;
				add_into_prev_string=false;
			}			
			if(type==h){//在HTML源文件中<h1>/.../<h6>可能单独出现而无</h1>/.../</h6>配套，这种不规范的网页需要特殊处理
				tagStack.pop();
				in_h=false;
				add_into_prev_string=false;
			}
			int tmp=str.indexOf(L">",startPos)+1;
			startPos=str.indexOf(L"<",startPos+1);
			in_p=true;
			tagStack.push(p);
			if(tmp!=startPos && (in_post_text || in_post_desc || in_endtext || in_artical_summary || in_endtext_clearfix || in_hidden)){//判断是否要提取到info.text内
				add_into_prev_string=true;
				info.text.add(str.substring(tmp,startPos-tmp));
			}
		}else if(str.substring(startPos,4)==L"<!--"){//处理注释
			startPos=str.indexOf(L"<",str.indexOf(L"-->",startPos));
		}else if(str.compareSubStr(startPos,L"<h") &&  (CharString(L"123456").indexOf(str[startPos+2],0)!=-1) && (CharString(L" >").indexOf(str[startPos + 3], 0) != -1)){//处理<h1>/.../<h6>
				tagType type;
				tagStack.top(type);
				if(type==p){//在HTML源文件中<p>可能单独出现而无</p>配套，这种不规范的网页需要特殊处理
					tagStack.pop();
					in_p=false;
					add_into_prev_string=false;
				}
				if(type==h){//在HTML源文件中<h1>/.../<h6>可能单独出现而无</h1>/.../</h6>配套，这种不规范的网页需要特殊处理
					tagStack.pop();
					in_h=false;
					add_into_prev_string=false;
				}
				int tmp=str.indexOf(L">",startPos)+1;
				startPos=str.indexOf(L"<",startPos+1);
				in_h=true;
				tagStack.push(h);
				if(tmp!=startPos && (in_post_text | in_post_desc | in_endtext | in_artical_summary | in_endtext_clearfix | in_hidden)){//判断是否要提取到info.text内
					add_into_prev_string=true;
					info.text.add(str.substring(tmp,startPos-tmp));
				}
				if(tmp!=startPos && (in_post_content_main|in_g_w1200_posr)){//判断是否需要提取新闻标题
					add_into_prev_string=true;
					info.title+=(str.substring(tmp,startPos-tmp));
				}
		}
		else {
			CharString str2 = str.substring(startPos + 1, 2);
			CharString str3 = str.substring(startPos + 1, 3);
			CharString str4 = str.substring(startPos + 1, 4);
			CharString str5 = str.substring(startPos + 1, 5);
			CharString str6 = str.substring(startPos + 1, 6);
			CharString str7 = str.substring(startPos + 1, 7);
			CharString str8 = str.substring(startPos + 1, 8);
			if (str5 == L"input" || str3 == L"img" || str4 == L"meta" || str5 == L"param" ||
				str2 == L"br" || str4 == L"base" || str6 == L"source" || str5 == L"embed") {//处理八类不配对出现、无需入栈的标签
				int tmp = str.indexOf(L">", startPos) + 1;
				startPos = str.indexOf(L"<", startPos + 1);
				if (tmp != startPos && (in_p || in_h) && (in_post_text || in_post_desc || in_endtext || in_artical_summary || in_endtext_clearfix || in_hidden)) {//判断是否要提取到info.text内
					if (add_into_prev_string) {
						info.text.getLastElement() += str.substring(tmp, startPos - tmp);
					}
					else {
						info.text.add(str.substring(tmp, startPos - tmp));
						add_into_prev_string = true;
					}
				}
			}
			else if (str8 == L"textarea") {//处理class="hidden"的<textarea>标签
				int index1 = str.indexOf(L"class=\"", startPos);
				int index2 = str.indexOf(L">", startPos);
				startPos = str.indexOf(L"<", startPos + 1);
				if (index1 > -1 && index1 < index2) {
					index1 += 7;
					int tmp = str.indexOf(L"\"", index1);
					if (str.substring(index1, tmp - index1) == L"hidden") {
						in_hidden = true;
						tagStack.push(hidden);
					}
					else {
						tagStack.push(inlineElement);
					}
				}
				else {
					tagStack.push(inlineElement);
				}
			}
			else {
				bool okay = false;
				if (str2 == L"a " || str2 == L"a>" || str2 == L"b " || str2 == L"b>" || str2 == L"u " || str2 == L"u>" || str2 == L"q " || str2 == L"q>" || str2 == L"i " || str2 == L"i>" ||
					str2 == L"s " || str2 == L"s>" || str3 == L"em " || str3 == L"em>" || str3 == L"tt " || str3 == L"tt>" || str4 == L"sup " || str4 == L"sup>" ||
					str4 == L"var " || str4 == L"var>" || str4 == L"bdo " || str4 == L"bdo>" ||
					str4 == L"big " || str4 == L"big>" || str4 == L"dfn " || str4 == L"dfn>" ||
					str4 == L"kbd " || str4 == L"kbd>" || str4 == L"sub " || str4 == L"sub>" ||
					str5 == L"abbr " || str5 == L"abbr>" || str5 == L"font " || str5 == L"font>" ||
					str5 == L"span " || str5 == L"span>" || str5 == L"samp " || str5 == L"samp>" ||
					str5 == L"cite " || str5 == L"cite>" || str5 == L"code " || str5 == L"code>" ||
					str6 == L"input " || str6 == L"input>" || str6 == L"label " || str6 == L"label>" || str6 == L"small " || str6 == L"small>" ||
					str7 == L"strike " || str7 == L"strike>" || str7 == L"strong " || str7 == L"strong>" ||
					str7 == L"select " || str7 == L"select>" || str8 == L"acronym " || str8 == L"acronym>") {
					okay = true;
				}
				if (okay) {//处理html的inline element
					//判断是否是class="ptime"的<span>的情况
					if (info.time.length() == 0 && str.substring(startPos + 1, 5) == L"span ") {
						int index1 = str.indexOf(L"class=\"", startPos) + 7;
						int index2 = str.indexOf(L"\"", index1);
						if (str.substring(index1, index2 - index1) == L"ptime") {
							int index3 = str.indexOf(L">", index2) + 1;
							int index4 = str.indexOf(L"<", index3);
							info.time = str.substring(index3, index4 - index3);
						}
					}

					int tmp = str.indexOf(L">", startPos) + 1;
					startPos = str.indexOf(L"<", startPos + 1);
					tagStack.push(inlineElement);
					if (tmp != startPos && (in_p || in_h) && (in_post_text || in_post_desc || in_endtext || in_artical_summary || in_endtext_clearfix || in_hidden)) {//判断是否要提取到正文内
						if (add_into_prev_string) {
							info.text.getLastElement() += str.substring(tmp, startPos - tmp);
						}
						else {
							info.text.add(str.substring(tmp, startPos - tmp));
							add_into_prev_string = true;
						}
					}
					if (tmp != startPos && in_h && (in_post_content_main || in_g_w1200_posr)) {//判断是否要提取到info.title内
						if (add_into_prev_string) {
							info.title += str.substring(tmp, startPos - tmp);
						}
						else {
							info.title += (str.substring(tmp, startPos - tmp));
							add_into_prev_string = true;
						}
					}
				}
				else {
					tagStack.push(blockElement);
					startPos = str.indexOf(L"<", startPos + 1);
				}
			}
			startPos = str.indexOf(L"<", startPos);
		}
	}

	if(info.time.length()==0){
		info.time=L"时间不详";
	}
	if(info.source.length()==0){
		info.source=L"来源不详";
	}
	if(info.title.length()==0){
		info.title=L"标题不详";
	}

	info.text.clearInvalidCharString();//清除无效的字符串

	//根据write_results_into_file决定是否将解析结果写到文件中并进行分词
	if(write_results_into_file){
		info.WriteIntoFile(filename,output_path);
		divideWords(info,20,filename);
	}
}

CharStringLinkedList NewsRetrievalTool::divideWords(NewsInfo& info,int  max_segmentation_length,const wchar_t* output_file_name){//执行分词操作
	/*
	Parameters:
		max_segmentation_length:最大分词长度
	Return Value:
		返回一个字符串链表对象，存储分词结果
	*/
	int epoch=info.text.length();
	const Link* pointer=info.text.getHead();
	pointer=pointer->next;
	CharStringLinkedList list;

	while(epoch--){//遍历字符串链表里的每个字符串
		const int len=pointer->data.length();
		int startPos=len-1;
		//逆向最大匹配分词算法：
		loop1:
		while(startPos+1>=max_segmentation_length){
			for(int i=max_segmentation_length;i>0;i--){
				if(lexicon.inHashTable(pointer->data.substring(startPos-i+1,i).wc_str())){
					list.add(pointer->data.substring(startPos-i+1,i));
					startPos-=i;
					goto loop1;
				}
			}
			startPos--;
		}
		loop2:
		while(startPos>=0){
			for(int i=startPos+1;i>0;i--){
				if(lexicon.inHashTable(pointer->data.substring(startPos-i+1,i).wc_str())){
					list.add(pointer->data.substring(startPos-i+1,i));
					startPos-=i;
					goto loop2;
				}
			}
			startPos--;
		}
		pointer=pointer->next;
	}

	//分词结果保存
	CharString output_file(output_file_name);
	output_file=CharString(output_path)+output_file.substring(0,output_file.length()-4)+CharString(L"txt");
	std::locale loc("Chinese-simplified");
	std::wofstream wfs(output_file.wc_str(), std::fstream::out | std::fstream::app);
	wfs.imbue(loc);
	list.writeIntoFile(wfs);
	return list;
}

void NewsRetrievalTool::retrieveByKeyword(const wchar_t* filename)
{
	/*
	Usage:
		根据filename文件的内容逐行进行网页检索并将结果写入文件
	Parameters:
		filename:需进行检索操作的查询文件的名称
	Return Value:
		none
	*/

	std::wifstream s(filename);
	std::locale loc("Chinese-simplified");
	s.imbue(loc);
	std::wstring str;
	std::ofstream  fout((CharString(L"result")+CharString(filename).substring(5, CharString(filename).length()-5)).wc_str());//用于将检索结果写入相应文件
	fout.imbue(loc);
	while (std::getline(s, str)) {//逐行读取filename文件的内容，对每一行单独操作
		CharString cstr(str.c_str());
		int prev = 0;
		int pos = cstr.indexOf(L" ", prev);
		std::vector<RetrievalInfo> retrieInfoVec;//保存检索到的结果
		AVL* avlNode;
		while(pos!=-1){
			avlNode = Search(tree, cstr.substring(prev, pos - prev));
			loop:
			if (avlNode != nullptr) {
				//更新retrieInfoVec
				DocInfoNode* node=documentList->data[avlNode->data.TermID].infoNode;
				while (node != nullptr) {
					bool already_existing = false;
					int DocID = node->DocID;
					for (auto it = retrieInfoVec.begin(); it != retrieInfoVec.end(); it++) {
						if ((*it).ID == DocID) {
							it->numOfCategoriesOfKeywords += 1;
							it->numOfKeywords += node->Times;
								already_existing = true;
							break;
						}
					}
					if (!already_existing) {
						RetrievalInfo result;
						result.ID = DocID;
						result.numOfCategoriesOfKeywords = 1;
						result.numOfKeywords = node->Times;
						retrieInfoVec.push_back(result);
					}
					node = node->next;
				}
				
			}
			if (pos == -1) {
				goto sort;
			}
			prev = pos + 1;
			pos = cstr.indexOf(L" ", prev);
		}
		avlNode = Search(tree, cstr.substring(prev, cstr.length() - prev));
		goto loop;//处理改行中的最后一个关键词

		sort://排序检索结果
		std::sort(retrieInfoVec.begin(), retrieInfoVec.end(), greater);

		//将检索结果写入文件
		for (auto it = retrieInfoVec.begin(); it != retrieInfoVec.end(); it++) {
			fout << "(" << it->ID << "," << it->numOfKeywords << ")" << " ";
		}
		if (retrieInfoVec.empty()) {
			fout << "查询失败";
		}
		fout << endl;
	}
	s.close();
}

void NewsRetrievalTool::recommendAll(const wchar_t * filename)
{
	/*
	Usage:
		根据存储新闻标题的文件，逐行推荐相关新闻
	Parameters:
		filename:存储新闻标题的文件名
	Return Value:
		none
	*/

	std::locale loc("Chinese-simplified");
	std::wifstream s(filename);
	s.imbue(loc);
	std::wstring tmpStr;
	std::wofstream  fout((CharString(L"result") + CharString(filename).substring(5, CharString(filename).length() - 5)).wc_str());
	fout.imbue(loc);
	while(getline(s,tmpStr)){//逐行读取filename内容
		CharString str(tmpStr.c_str());
		bool valid = false;//记录改行所存储的标题是否有效
		int index;
		//判断改行存储的标题是否有效
		for (int i = 0; i < DATABASE_SIZE; i++) {
			if (str == titles[i]) {
				valid = true;
				index = i;
				break;
			}
		}
		if (valid) {//若有效则根据相应网页的分词结果文件进行推荐
			recommend(fout, (CharString(output_path) + std::to_wstring(index).c_str() + CharString(L".txt")).wc_str(),index);
		}
		else {//若无效则输出提示信息
			fout << L"该新闻不在数据库中或解析其标题失败，无法推荐"<<endl;
		}
	}
	s.close();
	fout.close();
}

void NewsRetrievalTool::recommend(std::wofstream& fout, const wchar_t* filename, int DocID,int max_to_recommend)
{
	/*
	Usage:
		根据网页的分词结果文件，推荐与该网页相似的其他网页
	Parameters:
		fout:用于将推荐结果写入文件的输出流对象
		filename:分词结果文件名
		DocID:文档编号
		max_to_recommend:最大的新闻推荐数量
	Return Value:
		none
	*/
	if (max_to_recommend <= 0) {
		max_to_recommend = 5;
	}
	std::locale loc("Chinese-simplified");
	std::wifstream s(filename);
	s.imbue(loc);
	std::wstring str;
	std::set<CharString> keywordSet;//关键词集合，存储filename中出现的关键词，利用std::set防止重复
	while (std::getline(s, str)) {//逐行读取filename文件
		CharString cstr(str.c_str());
		keywordSet.insert(cstr);//更新keywordSet
	}
	std::set<int> keywordIDSet;//关键词编号集合
	for (auto it = keywordSet.begin(); it != keywordSet.end(); it++) {
		AVL* node = Search(tree, *it);
		if (node != nullptr) {
			keywordIDSet.insert(node->data.TermID);
		}
	}
	if (keywordIDSet.empty()) {//若keywordIDSet为空，表示该网页解析失败，输出提示信息
		fout << L"该新闻解析失败，无法推荐" << endl;
		return;
	}

	//以检索关键词的方式获得相关网页，思路类似于检索网页：
	std::vector<RetrievalInfo> retrieInfoVec;
	for (auto it = keywordIDSet.begin(); it != keywordIDSet.end(); it++) {
		DocInfoNode* node = documentList->data[*it].infoNode;
		while (node != nullptr) {
			bool already_existing = false;
			int DocID = node->DocID;
			for (auto it2 = retrieInfoVec.begin(); it2 != retrieInfoVec.end(); it2++) {
				if ((*it2).ID == DocID) {
					it2->numOfCategoriesOfKeywords += 1;
					it2->numOfKeywords += node->Times;
					already_existing = true;
					break;
				}
			}
			if (!already_existing) {
				RetrievalInfo result;
				result.ID = DocID;
				result.numOfCategoriesOfKeywords = 1;
				result.numOfKeywords = node->Times;
				retrieInfoVec.push_back(result);
			}
			node = node->next;
		}
	}
	sort:
	std::sort(retrieInfoVec.begin(), retrieInfoVec.end(), greater);
	if (retrieInfoVec.empty()) {//若检索不到相关网页，输出提示信息
		fout << L"无相关内容，推荐失败" << endl;
		return;
	}
	int num = 0;
	//输出推荐结果：
	for (auto it = retrieInfoVec.begin(); it != retrieInfoVec.end() && num<max_to_recommend; it++) {
		if (it->ID != DocID) {
			fout << "(" << it->ID << "," << titles[it->ID].wc_str() << ")" << " ";
			num++;
		}
	}
	fout << endl;
	return;
}

void NewsInfo::WriteIntoFile(const wchar_t* filename,const wchar_t* output_dir){//将网页的解析结果写入output_dir路径下的filename文件
	CharString output_file_name(filename);
	output_file_name=CharString(output_dir)+output_file_name.substring(0,output_file_name.length()-4)+CharString(L"info");
	std::locale loc("Chinese-simplified");
	std::wofstream wfs(output_file_name.wc_str(), std::fstream::out | std::fstream::app);
	wfs.imbue(loc);
	wfs<<title<<std::endl;//写入新闻标题
	wfs<<source<<std::endl;//写入新闻来源
	wfs<<time<<std::endl;//写入新闻时间
	text.writeIntoFile(wfs);//写入新闻正文
	wfs.close();
}


bool greater(RetrievalInfo info1, RetrievalInfo info2)//用于比较RetrievalInfo的优先级，用std::sort对std::vector<RetrievalInfo>排序时需要用到
{
	//优先根据numOfCategoriesOfKeywords比较
	if (info1.numOfCategoriesOfKeywords > info2.numOfCategoriesOfKeywords) {
		return true;
	}
	else if (info1.numOfCategoriesOfKeywords < info2.numOfCategoriesOfKeywords) {
		return false;
	}
	//numOfCategoriesOfKeywords相等时，利用numOfKeywords比较
	else {
		if (info1.numOfKeywords > info2.numOfKeywords) {
			return true;
		}
		else if (info1.numOfKeywords < info2.numOfKeywords) {
			return false;
		}
		else {
			return false;//为了使用<algorithm>的std::sort排序，需为stric weak order，故这里返回false
		}
	}
}
