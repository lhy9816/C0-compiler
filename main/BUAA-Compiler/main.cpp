#include "Compiler.h"

using namespace std;

int main() {
	char path[500] = "���й��⼯/16231086/16231086_test.txt";//*/ { "16231137_test.txt" };
	//cout << "�������ļ�·����" << endl;
	//cin >> path;
	Compiler *C = new Compiler(path);
	C->begin();
	system("pause");
}