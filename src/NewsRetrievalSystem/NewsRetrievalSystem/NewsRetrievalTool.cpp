#include "stdafx.h"
#include "NewsRetrievalTool.h"
#include "LinkedStack.h"
#include "io.h"
#include <fstream>//ִ���ļ���д�Ȼ�������ʱ��Ҫ�õ�
#include <iostream>//ִ���ļ���д�Ȼ�������ʱ��Ҫ�õ�
#include <sstream>//ִ���ļ���д�Ȼ�������ʱ��Ҫ�õ�
#include <string>//ִ���ļ���д�Ȼ�������ʱ��Ҫ�õ�
#include <locale>//ִ���ļ���д�Ȼ�������ʱ��Ҫ�õ�
#include <cwchar>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>


NewsRetrievalTool::NewsRetrievalTool(){
	initDictionary();//��ʼ���ʿ⡢tree��input_path��output_path
	initDocumentLinkedList();//��ʼ���ĵ�����

	//��ȡ���ݿ���������ҳ�ı�����Ϣ���洢��titles��:
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


NewsRetrievalTool::~NewsRetrievalTool(){//��������
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

void NewsRetrievalTool::initDictionary(){//ִ������ʿ�ȳ�ʼ������
	//��ȡ�����ļ���ȫ������
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

	//���ôʿ��ļ������·��
	int pos=str.indexOf(L"PATHDIR=",0);
	int pos1=str.indexOf(L"\"",pos)+1;
	int pos2=str.indexOf(L"\"",pos1);
	path=new wchar_t[pos2-pos1+1];
	path[pos2-pos1]=L'\0';
	wmemcpy(path,str.substring(pos1,pos2-pos1).wc_str(),pos2-pos1);
	//���������ļ������·��
	pos=str.indexOf(L"INPUT_PATH_DIR=",0);
	pos1=str.indexOf(L"\"",pos)+1;
	pos2=str.indexOf(L"\"",pos1);
	input_path=new wchar_t[pos2-pos1+1];
	input_path[pos2-pos1]=L'\0';
	wmemcpy(input_path,str.substring(pos1,pos2-pos1).wc_str(),pos2-pos1);pos=str.indexOf(L"INPUT_PATH_DIR=",0);
	//��������ļ������·��
	pos=str.indexOf(L"OUTPUT_PATH_DIR=",0);
	pos1=str.indexOf(L"\"",pos)+1;
	pos2=str.indexOf(L"\"",pos1);
	output_path=new wchar_t[pos2-pos1+1];
	output_path[pos2-pos1]=L'\0';
	wmemcpy(output_path,str.substring(pos1,pos2-pos1).wc_str(),pos2-pos1);

	//��ʼ���ʿ�
	std::wfstream wfs2((CharString(path)+L"�ʿ�.dic").wc_str(), std::fstream::in | std::fstream::out);
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
	while(pos+1<max){//���δ���ʿ��ļ���ÿһ��
		key=key0+prev;
		const int key_len=pos-prev;
		HashNode* newNode=new HashNode;
		delete[] newNode->str;
		newNode->str=new wchar_t[key_len+1];
		wmemcpy(newNode->str,key,key_len);
		newNode->str[key_len]=L'\0';
		volatile TDataType result=compute_hash(newNode->str,HashTable::HashTableSize);//�����ϣֵ

		//���¹�ϣ��
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
		tree=insert(tree, newNode->str).link;//���ؼ��ʲ��뵽����ƽ����tree��
		len++;
	}
	amount_of_words=len + 1;//���´ʿ�Ĵ���
}

void NewsRetrievalTool::initDocumentLinkedList()//��ʼ���ĵ�����
{
	documentList = new documentLinkedList(amount_of_words);//��̬�����ڴ�ռ�
	struct _wfinddata_t fileinfo;
	intptr_t ld;
	int* num = new int[amount_of_words];//��¼�ĵ���ÿ�����ʳ��ֵĴ���
	std::locale loc("Chinese-simplified");

	//����output_pathĿ¼�µ����зִʽ���ļ�
	if ((ld = _wfindfirst((CharString(output_path) + CharString(L"*.txt")).wc_str(), &fileinfo)) != -1) {
		do {
			std::set<int> exisitng_keywords;//��¼��ǰ���ڱ�����txt�ִʽ���ļ��г��ֵ����йؼ��ʣ�ʹ��std::set<int>��ֹ�ظ�
			memset(num, 0, sizeof(int)*amount_of_words);//ÿ������һ���µ�txt�ִʽ���ļ�ʱ������һ�γ�ʼ��

			//����txt�ļ����ļ�����ȡ�ĵ����DocID
			std::wstringstream ss;
			ss << CharString(fileinfo.name).substring(0, CharString(fileinfo.name).indexOf(L".", 0));
			int DocID;
			ss >> DocID;
			ss.clear();
			ss.str(L"");

			std::wifstream s((CharString(output_path) + fileinfo.name).wc_str());
			s.imbue(loc);
			std::wstring str;
			while (std::getline(s, str)) {//���ж�ȡtxt�ִʽ���ļ�
				AVL* avlNode = Search(tree, str.c_str());//��ѯ�������е���Ӧ�ڵ�
				if (num[avlNode->data.TermID]==0){
					avlNode->data.DF += 1;//����ڸ�txt�ļ��е�һ�ζ����ùؼ��ʣ������avlNode->data.DF
				}
				avlNode->data.Occur += 1;
				num[avlNode->data.TermID]++;//����num
				exisitng_keywords.insert(avlNode->data.TermID);//����ȡ�Ĺؼ��ʵ�ID����existing_keywords��
			}
			for (auto it = exisitng_keywords.begin(); it != exisitng_keywords.end(); it++) {
				documentList->Add(*it, DocID, num[*it]);//����¼���뵽�ĵ�������
			}
			s.close();
		} while (_wfindnext(ld, &fileinfo) == 0);
	}
	delete[] num;
	_findclose(ld);
}

void NewsRetrievalTool::extracrtAllInfo(bool write_into_file){//�����ͷִ�input_path·���µ�������ҳ�ļ�
	/*
	Parameters:
		write_into_file:Ϊtrueʱ������������浽output_path��Ӧ��Ŀ¼�ºͽ��зִ�
	Return Value:
		none
	*/
	struct _wfinddata_t fileinfo;
	intptr_t ld;
	if((ld=_wfindfirst((CharString(input_path)+CharString(L"*.html")).wc_str(),&fileinfo))!=-1){
		do{//����input_pathĿ¼�µ�����html�ļ�
			NewsInfo info;
			extractinfo(fileinfo.name,info,write_into_file);
		}while(_wfindnext(ld,&fileinfo)==0);
	}
	_findclose(ld);
}

void NewsRetrievalTool::extractinfo(const wchar_t* filename,NewsInfo& info, bool write_results_into_file){//ִ�н�����ҳ����
	/*
	Parameters:
		filename:���������·�����ļ���
		info:��Ž������
		write_results_into_file:�Ƿ�Ҫ�������Ľ��д��*.info�ļ��в����зִ�
	Return Value:
		none
	*/
	
	//��ȡhtml�ļ���ȫ������
	std::locale loc("Chinese-simplified");
	CharString str;
	std::wifstream s((CharString(input_path)+filename).wc_str());
	s.imbue(loc);
	std::wstring tmpStr;
	while (getline(s, tmpStr)) {
		str += tmpStr.c_str();
	}
	s.close();
	//������ҳ
	int startPos=0;//��ǰ�ڼ���L'<'��λ��
	if(str.substring(0,15)==CharString(L"<!DOCTYPE HTML>")){
		startPos=str.indexOf(L"<",str.indexOf(L">",0));
	}else{
		startPos=str.indexOf(L"<",0);
	}
	enum tagType{post_text,post_desc,endtext,endtext_clearfix,post_content_main,g_w1200_posr,artical_summary,hidden,p,h,otherdiv,blockElement,inlineElement,html};
	//enum tagType��html��ʾ<html>��ǩ��p��ʾ<p>��ǩ��blockElement��ʾ����p��div����Ŀ鼶Ԫ�أ�inlineElement��ʾ��ǶԪ��,h��ʾ<h1>/<h2>/.../<h6>
	//enum tagType��post_textΪclass="post_text"��<div>��endtextΪclass="end-text"��<div>��post_descΪclass="post_desc"��<div>��endtext_clearfixΪclass="end-text clearfix"��<div>��post_content_mainΪclass="post_content_main"��"ep-content-main"��<div>
	//enum tagType��artical_summaryΪclass="artical-summary"��<div>��g_w1200_posrΪclass="g-w1200 posr"��<div>��otherdiv��ʾ������<div>
	//enum tagType��hiddenΪclass="hidden"��textarea
	bool in_p=false;//��ʾ��ǰ�Ƿ��ڴ���p��ǩ�ڵ�����
	bool in_h=false;//��ʾ��ǰ�Ƿ��ڴ���h1/h2/.../h6��ǩ�ڵ�����
	bool in_post_text=false;//��ʾ��ǰ�Ƿ��ڴ���class="post_text"��div��ǩ�ڵ�����
	bool in_endtext=false;//��ʾ��ǰ�Ƿ��ڴ���class="end-text"��div��ǩ�ڵ�����
	bool in_endtext_clearfix=false;//��ʾ��ǰ�Ƿ��ڴ���class="end-text clearfix"��div��ǩ�ڵ�����
	bool in_post_desc=false;//��ʾ��ǰ�Ƿ��ڴ���class="post_desc"��div��ǩ�ڵ�����
	bool in_post_content_main=false;//��ʾ��ǰ�Ƿ��ڴ���class="post_content_main"��"ep-content-main"��div��ǩ�ڵ�����
	bool in_artical_summary=false;//��ʾ��ǰ�Ƿ��ڴ���class="artical-summary"��div��ǩ�ڵ�����
	bool in_g_w1200_posr=false;//��ʾ��ǰ�Ƿ��ڴ���class="g-w1200 posr"��div��ǩ�ڵ�����
	bool in_hidden=false;//��ʾ��ǰ�Ƿ��ڴ���class="post_desc"��textarea��ǩ�ڵ�����
	bool add_into_prev_string=false;//��ĳ��<p>...</p>�ڻ�������ǶԪ�أ����<p>...</p>�ڵ������ı���Ӧ�ñ��洢��һ����ͬ��CharString���bool�ͱ���Ϊtrueʱ�����֪����Ҫ�洢��һ����ͬ��CharString�<h1>/../<h6>��ǩ�����Ƶ����⣩

	bool isNormalFormat=str.indexOf(CharString(L"<body>"),0)>-1;//������ҳΪ������������6.html�ĸ�ʽʱ��isNormalFormatΪfalse������Ϊtrue����������6.html����ҳ����ȡ���⡢ʱ�䡢��Դ�ķ����Ƚ�����
	//��ȡ��������
	LinkedStack<tagType> tagStack;
	while(startPos>-1){
		if(str[startPos+1]==L'/'){//�ж��Ƿ���Ҫ��ջ
			tagType type;
			tagStack.top(type);
			if((!(str.compareSubStr(startPos,L"</p>"))) && type==p ){//��HTMLԴ�ļ���<p>���ܵ������ֶ���</p>���ף����ֲ��淶����ҳ��Ҫ���⴦��
				add_into_prev_string=false;
				tagStack.pop();
				in_p=false;
				continue;
			}
			if(str.compareSubStr(startPos,L"</p>") && type!=p){//��HTMLԴ�ļ���</p>���ܵ������ֶ���<p>���ף����ֲ��淶����ҳ��Ҫ���⴦��
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
			if(tmp!=startPos && (in_p || in_h) && (in_post_text || in_post_desc || in_artical_summary || in_endtext || in_endtext_clearfix || in_hidden)){//�ж��Ƿ�Ҫ��ȡ��info.text��
				if(add_into_prev_string){
					info.text.getLastElement()+=str.substring(tmp,startPos-tmp);
				}else{
					info.text.add(str.substring(tmp,startPos-tmp));
					add_into_prev_string=true;
				}
			}
		}else if(str.substring(startPos+1,4)==L"html"){//����<html>
			tagStack.push(html);
			startPos=str.indexOf(L"<",startPos+1);
		}else if(str.compareSubStr(startPos,L"<div")){//����<div>
			tagType type;
			tagStack.top(type);
			if(type==p){//��HTMLԴ�ļ���<p>���ܵ������ֶ���</p>���ף����ֲ��淶����ҳ��Ҫ���⴦��
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
					//��ȡ���ŵ�ʱ�䣺
					int tmp=str.indexOf(L":",startPos);
					info.time=str.substring(tmp-13,19);
					//��ȡ���ŵ���Դ��
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
		}else if(str.compareSubStr(startPos,L"<p>") || str.compareSubStr(startPos,L"<p ")){//����<p>
			tagType type;
			tagStack.top(type);
			if(type==p){//��HTMLԴ�ļ���<p>���ܵ������ֶ���</p>���ף����ֲ��淶����ҳ��Ҫ���⴦��
				tagStack.pop();
				in_p=false;
				add_into_prev_string=false;
			}			
			if(type==h){//��HTMLԴ�ļ���<h1>/.../<h6>���ܵ������ֶ���</h1>/.../</h6>���ף����ֲ��淶����ҳ��Ҫ���⴦��
				tagStack.pop();
				in_h=false;
				add_into_prev_string=false;
			}
			int tmp=str.indexOf(L">",startPos)+1;
			startPos=str.indexOf(L"<",startPos+1);
			in_p=true;
			tagStack.push(p);
			if(tmp!=startPos && (in_post_text || in_post_desc || in_endtext || in_artical_summary || in_endtext_clearfix || in_hidden)){//�ж��Ƿ�Ҫ��ȡ��info.text��
				add_into_prev_string=true;
				info.text.add(str.substring(tmp,startPos-tmp));
			}
		}else if(str.substring(startPos,4)==L"<!--"){//����ע��
			startPos=str.indexOf(L"<",str.indexOf(L"-->",startPos));
		}else if(str.compareSubStr(startPos,L"<h") &&  (CharString(L"123456").indexOf(str[startPos+2],0)!=-1) && (CharString(L" >").indexOf(str[startPos + 3], 0) != -1)){//����<h1>/.../<h6>
				tagType type;
				tagStack.top(type);
				if(type==p){//��HTMLԴ�ļ���<p>���ܵ������ֶ���</p>���ף����ֲ��淶����ҳ��Ҫ���⴦��
					tagStack.pop();
					in_p=false;
					add_into_prev_string=false;
				}
				if(type==h){//��HTMLԴ�ļ���<h1>/.../<h6>���ܵ������ֶ���</h1>/.../</h6>���ף����ֲ��淶����ҳ��Ҫ���⴦��
					tagStack.pop();
					in_h=false;
					add_into_prev_string=false;
				}
				int tmp=str.indexOf(L">",startPos)+1;
				startPos=str.indexOf(L"<",startPos+1);
				in_h=true;
				tagStack.push(h);
				if(tmp!=startPos && (in_post_text | in_post_desc | in_endtext | in_artical_summary | in_endtext_clearfix | in_hidden)){//�ж��Ƿ�Ҫ��ȡ��info.text��
					add_into_prev_string=true;
					info.text.add(str.substring(tmp,startPos-tmp));
				}
				if(tmp!=startPos && (in_post_content_main|in_g_w1200_posr)){//�ж��Ƿ���Ҫ��ȡ���ű���
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
				str2 == L"br" || str4 == L"base" || str6 == L"source" || str5 == L"embed") {//������಻��Գ��֡�������ջ�ı�ǩ
				int tmp = str.indexOf(L">", startPos) + 1;
				startPos = str.indexOf(L"<", startPos + 1);
				if (tmp != startPos && (in_p || in_h) && (in_post_text || in_post_desc || in_endtext || in_artical_summary || in_endtext_clearfix || in_hidden)) {//�ж��Ƿ�Ҫ��ȡ��info.text��
					if (add_into_prev_string) {
						info.text.getLastElement() += str.substring(tmp, startPos - tmp);
					}
					else {
						info.text.add(str.substring(tmp, startPos - tmp));
						add_into_prev_string = true;
					}
				}
			}
			else if (str8 == L"textarea") {//����class="hidden"��<textarea>��ǩ
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
				if (okay) {//����html��inline element
					//�ж��Ƿ���class="ptime"��<span>�����
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
					if (tmp != startPos && (in_p || in_h) && (in_post_text || in_post_desc || in_endtext || in_artical_summary || in_endtext_clearfix || in_hidden)) {//�ж��Ƿ�Ҫ��ȡ��������
						if (add_into_prev_string) {
							info.text.getLastElement() += str.substring(tmp, startPos - tmp);
						}
						else {
							info.text.add(str.substring(tmp, startPos - tmp));
							add_into_prev_string = true;
						}
					}
					if (tmp != startPos && in_h && (in_post_content_main || in_g_w1200_posr)) {//�ж��Ƿ�Ҫ��ȡ��info.title��
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
		info.time=L"ʱ�䲻��";
	}
	if(info.source.length()==0){
		info.source=L"��Դ����";
	}
	if(info.title.length()==0){
		info.title=L"���ⲻ��";
	}

	info.text.clearInvalidCharString();//�����Ч���ַ���

	//����write_results_into_file�����Ƿ񽫽������д���ļ��в����зִ�
	if(write_results_into_file){
		info.WriteIntoFile(filename,output_path);
		divideWords(info,20,filename);
	}
}

CharStringLinkedList NewsRetrievalTool::divideWords(NewsInfo& info,int  max_segmentation_length,const wchar_t* output_file_name){//ִ�зִʲ���
	/*
	Parameters:
		max_segmentation_length:���ִʳ���
	Return Value:
		����һ���ַ���������󣬴洢�ִʽ��
	*/
	int epoch=info.text.length();
	const Link* pointer=info.text.getHead();
	pointer=pointer->next;
	CharStringLinkedList list;

	while(epoch--){//�����ַ����������ÿ���ַ���
		const int len=pointer->data.length();
		int startPos=len-1;
		//�������ƥ��ִ��㷨��
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

	//�ִʽ������
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
		����filename�ļ����������н�����ҳ�����������д���ļ�
	Parameters:
		filename:����м��������Ĳ�ѯ�ļ�������
	Return Value:
		none
	*/

	std::wifstream s(filename);
	std::locale loc("Chinese-simplified");
	s.imbue(loc);
	std::wstring str;
	std::ofstream  fout((CharString(L"result")+CharString(filename).substring(5, CharString(filename).length()-5)).wc_str());//���ڽ��������д����Ӧ�ļ�
	fout.imbue(loc);
	while (std::getline(s, str)) {//���ж�ȡfilename�ļ������ݣ���ÿһ�е�������
		CharString cstr(str.c_str());
		int prev = 0;
		int pos = cstr.indexOf(L" ", prev);
		std::vector<RetrievalInfo> retrieInfoVec;//����������Ľ��
		AVL* avlNode;
		while(pos!=-1){
			avlNode = Search(tree, cstr.substring(prev, pos - prev));
			loop:
			if (avlNode != nullptr) {
				//����retrieInfoVec
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
		goto loop;//��������е����һ���ؼ���

		sort://����������
		std::sort(retrieInfoVec.begin(), retrieInfoVec.end(), greater);

		//���������д���ļ�
		for (auto it = retrieInfoVec.begin(); it != retrieInfoVec.end(); it++) {
			fout << "(" << it->ID << "," << it->numOfKeywords << ")" << " ";
		}
		if (retrieInfoVec.empty()) {
			fout << "��ѯʧ��";
		}
		fout << endl;
	}
	s.close();
}

void NewsRetrievalTool::recommendAll(const wchar_t * filename)
{
	/*
	Usage:
		���ݴ洢���ű�����ļ��������Ƽ��������
	Parameters:
		filename:�洢���ű�����ļ���
	Return Value:
		none
	*/

	std::locale loc("Chinese-simplified");
	std::wifstream s(filename);
	s.imbue(loc);
	std::wstring tmpStr;
	std::wofstream  fout((CharString(L"result") + CharString(filename).substring(5, CharString(filename).length() - 5)).wc_str());
	fout.imbue(loc);
	while(getline(s,tmpStr)){//���ж�ȡfilename����
		CharString str(tmpStr.c_str());
		bool valid = false;//��¼�������洢�ı����Ƿ���Ч
		int index;
		//�жϸ��д洢�ı����Ƿ���Ч
		for (int i = 0; i < DATABASE_SIZE; i++) {
			if (str == titles[i]) {
				valid = true;
				index = i;
				break;
			}
		}
		if (valid) {//����Ч�������Ӧ��ҳ�ķִʽ���ļ������Ƽ�
			recommend(fout, (CharString(output_path) + std::to_wstring(index).c_str() + CharString(L".txt")).wc_str(),index);
		}
		else {//����Ч�������ʾ��Ϣ
			fout << L"�����Ų������ݿ��л���������ʧ�ܣ��޷��Ƽ�"<<endl;
		}
	}
	s.close();
	fout.close();
}

void NewsRetrievalTool::recommend(std::wofstream& fout, const wchar_t* filename, int DocID,int max_to_recommend)
{
	/*
	Usage:
		������ҳ�ķִʽ���ļ����Ƽ������ҳ���Ƶ�������ҳ
	Parameters:
		fout:���ڽ��Ƽ����д���ļ������������
		filename:�ִʽ���ļ���
		DocID:�ĵ����
		max_to_recommend:���������Ƽ�����
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
	std::set<CharString> keywordSet;//�ؼ��ʼ��ϣ��洢filename�г��ֵĹؼ��ʣ�����std::set��ֹ�ظ�
	while (std::getline(s, str)) {//���ж�ȡfilename�ļ�
		CharString cstr(str.c_str());
		keywordSet.insert(cstr);//����keywordSet
	}
	std::set<int> keywordIDSet;//�ؼ��ʱ�ż���
	for (auto it = keywordSet.begin(); it != keywordSet.end(); it++) {
		AVL* node = Search(tree, *it);
		if (node != nullptr) {
			keywordIDSet.insert(node->data.TermID);
		}
	}
	if (keywordIDSet.empty()) {//��keywordIDSetΪ�գ���ʾ����ҳ����ʧ�ܣ������ʾ��Ϣ
		fout << L"�����Ž���ʧ�ܣ��޷��Ƽ�" << endl;
		return;
	}

	//�Լ����ؼ��ʵķ�ʽ��������ҳ��˼·�����ڼ�����ҳ��
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
	if (retrieInfoVec.empty()) {//���������������ҳ�������ʾ��Ϣ
		fout << L"��������ݣ��Ƽ�ʧ��" << endl;
		return;
	}
	int num = 0;
	//����Ƽ������
	for (auto it = retrieInfoVec.begin(); it != retrieInfoVec.end() && num<max_to_recommend; it++) {
		if (it->ID != DocID) {
			fout << "(" << it->ID << "," << titles[it->ID].wc_str() << ")" << " ";
			num++;
		}
	}
	fout << endl;
	return;
}

void NewsInfo::WriteIntoFile(const wchar_t* filename,const wchar_t* output_dir){//����ҳ�Ľ������д��output_dir·���µ�filename�ļ�
	CharString output_file_name(filename);
	output_file_name=CharString(output_dir)+output_file_name.substring(0,output_file_name.length()-4)+CharString(L"info");
	std::locale loc("Chinese-simplified");
	std::wofstream wfs(output_file_name.wc_str(), std::fstream::out | std::fstream::app);
	wfs.imbue(loc);
	wfs<<title<<std::endl;//д�����ű���
	wfs<<source<<std::endl;//д��������Դ
	wfs<<time<<std::endl;//д������ʱ��
	text.writeIntoFile(wfs);//д����������
	wfs.close();
}


bool greater(RetrievalInfo info1, RetrievalInfo info2)//���ڱȽ�RetrievalInfo�����ȼ�����std::sort��std::vector<RetrievalInfo>����ʱ��Ҫ�õ�
{
	//���ȸ���numOfCategoriesOfKeywords�Ƚ�
	if (info1.numOfCategoriesOfKeywords > info2.numOfCategoriesOfKeywords) {
		return true;
	}
	else if (info1.numOfCategoriesOfKeywords < info2.numOfCategoriesOfKeywords) {
		return false;
	}
	//numOfCategoriesOfKeywords���ʱ������numOfKeywords�Ƚ�
	else {
		if (info1.numOfKeywords > info2.numOfKeywords) {
			return true;
		}
		else if (info1.numOfKeywords < info2.numOfKeywords) {
			return false;
		}
		else {
			return false;//Ϊ��ʹ��<algorithm>��std::sort������Ϊstric weak order�������ﷵ��false
		}
	}
}
