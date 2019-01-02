//#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS 1		//忽略编译器<hash_map>头文件被弃用的报错

#include <iostream>
#include <fstream>
#include <ext/hash_map>
#include <string>
#include <sstream>

#define MAXLENGTH 15	//最大匹配长度5个字符，UTF-8编码每个汉字3个字节
#define UTF8LENGTH 3	//UTF-8汉字编码所占字节数

using namespace std;
using namespace __gnu_cxx;

namespace __gnu_cxx{
	template<> struct hash<std::string>
	{
		size_t operator() (const std::string& x) const{
			return hash<const char*>() (x.c_str());
		}	
	};
}
hash_map<string, int> dictionary;	//词典

//输入文件过滤
void FiltrateFile(void) {
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
			if ((str[p] == 0x20) || (str[p] >= 48 && str[p] <= 59) || (str[p] >= 65 && str[p] <= 122) || (str[p] == '-')) {
				//过滤掉空格、数字、字母和‘-’
				if (p > p1)
					word += str.substr(p1, p - p1);
				p++;
				p1 = p;

			}
			else if (str[p] == 47) {
				//处理“/”
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
				//过滤掉空格、数字、字母和‘-’
				if (p > p1)
					word += str.substr(p1, p - p1);
				p++;
				p1 = p;
			}
			else if (str[p] == 0xffffffe3 && str[p + 1] == 0xfffff80 && str[p + 2] == 0xffffff80) {
				//过滤掉全角空格
				if (p > p1)
					word += str.substr(p1, p - p1);
				p += 3;
				p1 = p;
			}
			else {
				p += 3;
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
	fstream fout("dictout.txt");
	ifstream fin("traingdataPure.txt");
	typedef pair<string, int> wordpair;		//存入词典中的项目
	string word;
	
	if (!fin.is_open()) {
		cerr << "Fail!" << endl << "Can't open the input file." << endl;
		exit(-1);
	}

	while (getline(fin, str,'/')) {
		//dictionary.insert(wordpair(str, 1));
		fout << str << endl; 
	//	cout << str << endl;
		//UTF-8 to String
	}
	
	while(getline(fout,str)){
		istringstream istr(str);
		istr >> word;
		cout << word << endl;
		dictionary.insert(wordpair(word, 1));
	}
	
	fin.close();
	fout.close();
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
		string word = strin.substr(strin.length() - len, len);		//从最右端开始截取词语
		int n = (dictionary.find(word) != dictionary.end());		//在词典中查找相应的词语
		while (len > UTF8LENGTH && n == 0) {
			//word不在词典中
			len -= UTF8LENGTH;		//从左边起，减少一个汉字
			word = strin.substr(strin.length() - len, len);
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
		fout << line;
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
