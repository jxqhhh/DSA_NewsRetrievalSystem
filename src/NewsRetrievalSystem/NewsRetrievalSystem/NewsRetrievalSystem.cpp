// NewsRetrievalSystem.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "NewsRetrievalTool.h"
#include "AVL.h"
#include <iostream>
int _tmain(int argc, _TCHAR* argv[]){
	NewsRetrievalTool tool;
	tool.retrieveByKeyword(L"query1.txt");
	tool.recommendAll(L"query2.txt");
	return 0;
}

