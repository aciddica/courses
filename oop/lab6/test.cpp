#include<iostream>
#include<stdlib.h>
#include"MyVector.h"
using namespace std;
int main()
{
    Vector<int> t1;//test no parameter constructor 
    cout << "no parameter constructor: " << t1.size() << endl;
    if (t1.empty()) cout << "empty" << endl;//check empty()
    else cout << "not empty" << endl;
    Vector<int> t2(10);//test constructor with size
    cout << "size constructor: " << t2.size() << endl;
    t1.push_back(1);
    t1.push_back(2);
    if (t1.empty()) cout << "empty" << endl;//check empty
    else cout << "not empty" << endl;
    Vector<int> t3(t1);
    cout << "copy constructor: " << t3.size() << endl;
    cout << "t3: ";
    for (int i = 0; i < t3.size(); i++)
        cout << t3[i] << " ";
    cout << endl;
    t1.push_back(3);//inflate() is called
    t1.push_back(4);
    cout << "t1: ";
    for (int i = 0; i < t1.size(); i++)//check data in the vector
        cout << t1[i] <<" ";
    cout << endl;
    t3.clear();
    if (t3.empty()) cout << "empty" << endl;//check clear()
    else cout << "not empty" << endl;
    cout << t1[100] << endl;//test out of range
    cout << t1.at(2) << endl;
    cout << t1.at(100);
    system("pause");
    return 0;

}