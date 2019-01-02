#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS 1		//忽略编译器<hash_map>头文件被弃用的报错

#include <iostream>
#include <fstream>
#include <hash_map>
#include <string>
#include <codecvt>
#include <wchar.h>
#include <Windows.h>

#define MAXLENGTH 15	//最大匹配长度5个字符，UTF-8编码每个汉字3个字节
#define UTF8LENGTH 3	//UTF-8汉字编码所占字节数

using namespace std;
using namespace stdext;

hash_map<string, int> dictionary;	//词典

string UTF8ToString(const string str) {
	wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	wstring wString = conv.from_bytes(str);
	int	iTextLen = WideCharToMultiByte(CP_ACP, 0, wString.c_str(), -1, NULL, 0, NULL, NULL);
	std::vector<char> vecText(iTextLen, '\0');
	::WideCharToMultiByte(CP_ACP, 0, wString.c_str(), -1, &(vecText[0]), iTextLen, NULL, NULL);
	std::string strText = &(vecText[0]);
	return strText;
}

//输入文件过滤
void FiltrateFile(void) {
	//读入时是UTF-8编码，一个汉字3字节
	ifstream fin("traingdata.txt");
	ofstream fout("traingdataPure.txt");

	if (!fin.is_open()) {
		//文件打开失败
		cerr << "Fail!" << endl << "Can't open the input file." << endl;
		exit(-1);
	}

	string str;
	while (getline(fin, str)) {
		int p1 = 0, p = 0;
		string word;
		for (p = 0; p < str.length(); ) {
			if ((str[p] == 0x20) || (str[p] >= 48 && str[p] <= 59) || (str[p] >= 65 && str[p] <= 122) ||
				(str[p] == '-')||(str[p] == '{')||(str[p] == '}')) {
				//过滤掉空格、数字、字母、‘-’和｛｝
				if (p > p1)
					word += str.substr(p1, p - p1);
				p++;
				p1 = p;

			}
			else if (str[p] == 47) {
				//处理“/”
				p++;
			}
			else if (str[p] <= 0xEF && str[p] >= 0xE0 && str[p + 1] >= 0x80 && str[p] <= 0xBF
				&& str[p + 2] >= 0x80 && str[p+2] <= 0xEF) {
				//过滤掉全角空格
				if (p > p1)
					word += str.substr(p1, p - p1);
				p += UTF8LENGTH;
				p1 = p;
			}
			else {
				p += UTF8LENGTH;
			}
		}
		word += str.substr(p1, p - p1);
		fout << word;
	}

	fin.close();
	fout.close();
}

//过滤输入文本，使文本中只有句子，无分隔符、数字等其他字符。
void FiltrateTestFile() {
	ifstream fin("testdata.txt");
	ofstream fout("testin.txt");
	if (!fin.is_open()) {
		//文件打开失败
		cerr << "Fail！ Can't open the input file." << endl;
		exit(-1);
	}

	string str;
	while (getline(fin, str,'/')) {
		int p1 = 0, p = 0;
		string word;
		for (p = 0; p < str.length();) {
			if ((str[p] == 0x20) || (str[p] >= 48 && str[p] <= 59) || (str[p] >= 65 && str[p] <= 122) || (str[p] == '-') 
				||(str[p] == '{')||(str[p] == '}')) {
				//过滤掉空格、数字、字母和‘-’'{}'
				if (p > p1)
					word += str.substr(p1, p - p1);
				p++;
				p1 = p;
			}
			else if (str[p] == '\n') {
				p++;
			}
			else if (str[p] == 0xffffffe3 && str[p + 1] == 0xfffff80 && str[p + 2] == 0xffffff80) {
				//过滤掉全角空格
				if (p > p1)
					word += str.substr(p1, p - p1);
				p += UTF8LENGTH;
				p1 = p;
			}
			else {
				p += UTF8LENGTH;
			}
		}
		word += str.substr(p1, p - p1);
		fout << word;
	}
	fin.close();
	fout.close();
}
//从训练文本提取词语，读入词典
void GetDictionary(void) {
	string str;
	ofstream fout("dictout.txt");
	ifstream fin("traingdataPure.txt");
	typedef pair<string, int> wordpair;		//存入词典中的项目

	if (!fin.is_open()) {
		cerr << "Fail!" << endl << "Can't open the input file." << endl;
		exit(-1);
	}

	while (getline(fin, str,'/')) {
		//utf8-->string
		wstring_convert<std::codecvt_utf8<wchar_t>> conv;
		wstring wString = conv.from_bytes(str);
		int	iTextLen = WideCharToMultiByte(CP_ACP, 0, wString.c_str(), -1, NULL, 0, NULL, NULL);
		std::vector<char> vecText(iTextLen, '\0');
		::WideCharToMultiByte(CP_ACP, 0, wString.c_str(), -1, &(vecText[0]), iTextLen, NULL, NULL);
		std::string strText = &(vecText[0]);

		dictionary.insert(wordpair(strText, 1));
		fout << str << endl; 

	}
	fin.close();
}

//后向最大匹配
string BackwardMaxMatch(string strin) {
	string strout ;		//分词结果
	while (!strin.empty()) {
		size_t len = strin.length();	//输入串长度
		if (len > MAXLENGTH) {
			//输入串长度大于规定的最大词语长度
			len = MAXLENGTH;	//只处理最大词语长度范围内的语言
		}
		string str = strin.substr(strin.length() - len, len);		//从最右端开始截取词语
		string word = UTF8ToString(str);
		
		int n = (dictionary.find(word) != dictionary.end());		//在词典中查找相应的词语
		while (len > UTF8LENGTH && n == 0) {
			//word不在词典中
			len -= UTF8LENGTH;		//从左边起，减少一个汉字
			str = strin.substr(strin.length() - len, len);
			word = UTF8ToString(str);

			n = (dictionary.find(word) != dictionary.end());
		}
		word += "/";	//添加分隔符“/”
		strout = word + strout;
		strin = strin.substr(0, strin.length() - len);	//输入串中删去已划分的部分
	}
	return strout;
}

void ChineseSegment(void) {
	ofstream fout("outcome.txt");			//最终输出的分词结果
	ifstream fin("testin.txt");
	if (!fin.is_open()) {
		cerr << "Fail!" << endl << "Can't open the input file." << endl;
		exit(-1);
	}

	string str;
	while (getline(fin, str)) {
		//读入语料中的每一行，用向后最大匹配算法进行汉语分词
		string line = "";	//每一行分词的结果
		line = BackwardMaxMatch(str);
		fout << line << endl;
	}
}

int main(void) {
	FiltrateFile();		//过滤训练文本
	GetDictionary();	//获取词典
	FiltrateTestFile();	//去除已经分割的文本材料中的分隔符
	ChineseSegment();	//汉语分词

	system("pause");
	return 0;
}