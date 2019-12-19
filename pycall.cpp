#include <bits/stdc++.h>
#include <string>
using namespace std;
int processimage(int desc)
{
	string str = "python3 image_converter.py ";
	str = str + to_string(desc);
	system(str.c_str());

	return 0;
}
