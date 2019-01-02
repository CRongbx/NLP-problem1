//#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS 1		//���Ա�����<hash_map>ͷ�ļ������õı���

#include <iostream>
#include <fstream>
#include <ext/hash_map>
#include <string>
#include <sstream>

#define MAXLENGTH 15	//���ƥ�䳤��5���ַ���UTF-8����ÿ������3���ֽ�
#define UTF8LENGTH 3	//UTF-8���ֱ�����ռ�ֽ���

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
hash_map<string, int> dictionary;	//�ʵ�

//�����ļ�����
void FiltrateFile(void) {
	ifstream fin("traingdata.txt");
	ofstream fout("traingdataPure.txt");

	if (!fin.is_open()) {
		//�ļ���ʧ��
		cerr << "Fail!" << endl << "Can't open the input file." << endl;
		exit(-1);
	}

	string str;
	while (getline(fin, str)) {
		int p1 = 0, p = 0;
		string word;
		for (p = 0; p < str.length(); ) {
			if ((str[p] == 0x20) || (str[p] >= 48 && str[p] <= 59) || (str[p] >= 65 && str[p] <= 122) || (str[p] == '-')) {
				//���˵��ո����֡���ĸ�͡�-��
				if (p > p1)
					word += str.substr(p1, p - p1);
				p++;
				p1 = p;

			}
			else if (str[p] == 47) {
				//����/��
				p++;
			}
			else if (str[p] == 0xffffffe3 && str[p + 1] == 0xfffff80 && str[p + 2] == 0xffffff80) {
				//���˵�ȫ�ǿո�
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

//���������ı���ʹ�ı���ֻ�о��ӣ��޷ָ��������ֵ������ַ���
void FiltrateTestFile() {
	ifstream fin("testdata.txt");
	ofstream fout("testin.txt");
	if (!fin.is_open()) {
		//�ļ���ʧ��
		cerr << "Fail�� Can't open the input file." << endl;
		exit(-1);
	}

	string str;
	while (getline(fin, str,'/')) {
		int p1 = 0, p = 0;
		string word;
		for (p = 0; p < str.length();) {
			if ((str[p] == 0x20) || (str[p] >= 48 && str[p] <= 59) || (str[p] >= 65 && str[p] <= 122) || (str[p] == '-') 
				||(str[p] == '{')||(str[p] == '}')) {
				//���˵��ո����֡���ĸ�͡�-��
				if (p > p1)
					word += str.substr(p1, p - p1);
				p++;
				p1 = p;
			}
			else if (str[p] == 0xffffffe3 && str[p + 1] == 0xfffff80 && str[p + 2] == 0xffffff80) {
				//���˵�ȫ�ǿո�
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
//��ѵ���ı���ȡ�������ʵ�
void GetDictionary(void) {
	string str;
	fstream fout("dictout.txt");
	ifstream fin("traingdataPure.txt");
	typedef pair<string, int> wordpair;		//����ʵ��е���Ŀ
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

//�������ƥ��
string BackwardMaxMatch(string strin) {
	string strout ;		//�ִʽ��
	while (!strin.empty()) {
		size_t len = strin.length();	//���봮����
		if (len > MAXLENGTH) {
			//���봮���ȴ��ڹ涨�������ﳤ��
			len = MAXLENGTH;	//ֻ���������ﳤ�ȷ�Χ�ڵ�����
		}
		string word = strin.substr(strin.length() - len, len);		//�����Ҷ˿�ʼ��ȡ����
		int n = (dictionary.find(word) != dictionary.end());		//�ڴʵ��в�����Ӧ�Ĵ���
		while (len > UTF8LENGTH && n == 0) {
			//word���ڴʵ���
			len -= UTF8LENGTH;		//������𣬼���һ������
			word = strin.substr(strin.length() - len, len);
			n = (dictionary.find(word) != dictionary.end());
		}
		word += "/";	//��ӷָ�����/��
		strout = word + strout;
		strin = strin.substr(0, strin.length() - len);	//���봮��ɾȥ�ѻ��ֵĲ���
	}
	return strout;
}

void ChineseSegment(void) {
	ofstream fout("outcome.txt");			//��������ķִʽ��
	ifstream fin("testin.txt");
	if (!fin.is_open()) {
		cerr << "Fail!" << endl << "Can't open the input file." << endl;
		exit(-1);
	}

	string str;
	while (getline(fin, str)) {
		//���������е�ÿһ�У���������ƥ���㷨���к���ִ�
		string line = "";	//ÿһ�зִʵĽ��
		line = BackwardMaxMatch(str);
		fout << line;
	}
}

int main(void) {
	FiltrateFile();		//����ѵ���ı�
	GetDictionary();	//��ȡ�ʵ�
	FiltrateTestFile();	//ȥ���Ѿ��ָ���ı������еķָ���
	ChineseSegment();	//����ִ�

	system("pause");
	return 0;
}
