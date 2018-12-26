#include <iostream>
#include <fstream>
#include <sstream>			//istringstream
//#include <hash_map>
#include <string>

#define LMAX 15	//最大匹配长度5个字符，UTF-8编码每个汉字3个字节
using namespace std;

//hash_map<string, int> dictionary;	//词典

//输入文件过滤
void FiltrateFile(void) {
	ifstream fin("traingdata.txt");
	ofstream fout("traingdataPure.txt");
	if (!fin.is_open()) {
		//文件打开失败
		cerr << "Fail!" << endl << "Can't open the input file." << endl;
		exit(-1);
	}

	string str, word;
	while (getline(fin, str)) {
		int p1 = 0, p = 0;
		for (p = 0; p < str.length();) {
			if ((str[p]==0x20)||(str[p] >= 48 && str[p] <= 59) || (str[p] >= 65 && str[p] <= 122) || (str[p] == '-')) {
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
			else if (str[p]==0xffffffe3 && str[p + 1] == 0xfffff80 && str[p + 2] == 0xffffff80) {
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

int main(void) {
	FiltrateFile();
	
	system("pause");
	return 0;
}