#include "Compiler.h"

using namespace std;

int main() {
	char path[500] = "大佬公测集/16231086/16231086_test.txt";//*/ { "16231137_test.txt" };
	//cout << "请输入文件路径：" << endl;
	//cin >> path;
	Compiler *C = new Compiler(path);
	C->begin();
	system("pause");
}