#ifndef _NEWSRETRIEVALTOOL_H_
#define _NEWSRETRIEVALTOOL_H_
#include "CharString.h"
#include "CharStringLinkedList.h"
#include "AVL.h"
#include "documentLinkedList.h"
#include "HashTable.h"
#include "stdafx.h"

#define DATABASE_SIZE 781//新闻数据库存储的新闻数

class Widget;

class NewsInfo{//封装新闻信息提取结果
	CharString title;//新闻标题
	CharString source;//新闻来源
	CharString time;//新闻时间
	CharStringLinkedList text;//新闻正文
	friend class NewsRetrievalTool;
public:
	void WriteIntoFile(const wchar_t* filename,const wchar_t* output_dir);//将提取结果写入文件
};

class RetrievalInfo {//封装检索结果
public:
	int ID;//网页的编号
	int numOfCategoriesOfKeywords;//网页中出现的不同关键词的种数，也是比较网页检索结果的最高优先级参照物
	int numOfKeywords;//网页中出现任一待查询关键词的数目之和，也是比较网页检索结果的较低优先级参照物
public:
	friend bool greater(RetrievalInfo info1, RetrievalInfo info2);//比较RetrievalInfo大小，排序时需要用到
};

class NewsRetrievalTool
{
	wchar_t* path=nullptr;//存放词库文件的相对路径
	wchar_t* input_path=nullptr;//存放输入文件的相对路径
	wchar_t* output_path=nullptr;//存放输出文件的相对路径
	HashTable lexicon;//词库
	int amount_of_words;//词库中所含词的数目，在构造函数调用initDictionary()时初始化
	AVL* tree=nullptr;//词库
	CharString titles[DATABASE_SIZE];//数据库内的新闻标题
	documentLinkedList* documentList=nullptr;//文档链表
public:
	friend class Widget;
	NewsRetrievalTool();
	~NewsRetrievalTool();
	void extractinfo(const wchar_t* filename, NewsInfo& info, bool write_into_file=false);//执行解析网页操作
	void extracrtAllInfo(bool write_into_file=false);//解析和分词input_path路径下的所有网页文件
	void initDictionary();//执行载入词库等初始化操作
	void initDocumentLinkedList();//执行文档链表的初始化操作
	CharStringLinkedList divideWords(NewsInfo& info,int max_segmentation_length,const wchar_t* output_file_name);//执行分词操作
	void retrieveByKeyword(const wchar_t* filename);//执行检索操作
	void recommendAll(const wchar_t* filename=L"query2.txt");//根据文件执行推荐操作
	void recommend(std::wofstream& fout, const wchar_t* filename, int DocID, int max_to_recommend=5);//根据网页编号执行推荐操作
};


#endif