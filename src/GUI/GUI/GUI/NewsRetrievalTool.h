#ifndef _NEWSRETRIEVALTOOL_H_
#define _NEWSRETRIEVALTOOL_H_
#include "CharString.h"
#include "CharStringLinkedList.h"
#include "AVL.h"
#include "documentLinkedList.h"
#include "HashTable.h"
#include "stdafx.h"

#define DATABASE_SIZE 781//�������ݿ�洢��������

class Widget;

class NewsInfo{//��װ������Ϣ��ȡ���
	CharString title;//���ű���
	CharString source;//������Դ
	CharString time;//����ʱ��
	CharStringLinkedList text;//��������
	friend class NewsRetrievalTool;
public:
	void WriteIntoFile(const wchar_t* filename,const wchar_t* output_dir);//����ȡ���д���ļ�
};

class RetrievalInfo {//��װ�������
public:
	int ID;//��ҳ�ı��
	int numOfCategoriesOfKeywords;//��ҳ�г��ֵĲ�ͬ�ؼ��ʵ�������Ҳ�ǱȽ���ҳ���������������ȼ�������
	int numOfKeywords;//��ҳ�г�����һ����ѯ�ؼ��ʵ���Ŀ֮�ͣ�Ҳ�ǱȽ���ҳ��������Ľϵ����ȼ�������
public:
	friend bool greater(RetrievalInfo info1, RetrievalInfo info2);//�Ƚ�RetrievalInfo��С������ʱ��Ҫ�õ�
};

class NewsRetrievalTool
{
	wchar_t* path=nullptr;//��Ŵʿ��ļ������·��
	wchar_t* input_path=nullptr;//��������ļ������·��
	wchar_t* output_path=nullptr;//�������ļ������·��
	HashTable lexicon;//�ʿ�
	int amount_of_words;//�ʿ��������ʵ���Ŀ���ڹ��캯������initDictionary()ʱ��ʼ��
	AVL* tree=nullptr;//�ʿ�
	CharString titles[DATABASE_SIZE];//���ݿ��ڵ����ű���
	documentLinkedList* documentList=nullptr;//�ĵ�����
public:
	friend class Widget;
	NewsRetrievalTool();
	~NewsRetrievalTool();
	void extractinfo(const wchar_t* filename, NewsInfo& info, bool write_into_file=false);//ִ�н�����ҳ����
	void extracrtAllInfo(bool write_into_file=false);//�����ͷִ�input_path·���µ�������ҳ�ļ�
	void initDictionary();//ִ������ʿ�ȳ�ʼ������
	void initDocumentLinkedList();//ִ���ĵ�����ĳ�ʼ������
	CharStringLinkedList divideWords(NewsInfo& info,int max_segmentation_length,const wchar_t* output_file_name);//ִ�зִʲ���
	void retrieveByKeyword(const wchar_t* filename);//ִ�м�������
	void recommendAll(const wchar_t* filename=L"query2.txt");//�����ļ�ִ���Ƽ�����
	void recommend(std::wofstream& fout, const wchar_t* filename, int DocID, int max_to_recommend=5);//������ҳ���ִ���Ƽ�����
};


#endif