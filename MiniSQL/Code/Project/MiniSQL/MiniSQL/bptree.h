#ifndef _BPTREE_H_
#define _BPTREE_H_
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "BufferManager.h"

#define BLOCKSIZE 4096
#define MESSAGE 24
#define FOUR_BYTE 4  
//pointer length 20 means that
//the front 20byte of the block records the 
#define POINTERLENGTH 20
//block�ڽṹ��ǰ20λ��¼��ǰ�ڵ���Ϣ�����ֵ��Ϣ��Ϊ�Ŀ飺��һ��Ϊ��ֵ���ڶ���Ϊ�ֵܼ�ֵλ��
//������ΪҶ�ӽڵ���У���¼�ļ���Ӧλ�ã���Ӧ���ڽڵ��¼��һ���ַ(���Լ�С��)�����Ŀ�Ϊ��ǰ���λ��,�����ר�żǱ��Լ���ĵ�ַ
//�ڵ��һ���ֿճ��ص�������¼��һ����ֵλ��
//��ž���0��ʼ

using namespace std;

enum nodetype { Internal, Leaf };
extern BufferManager bf;
class index {
public:
	int Number;			//��¼��ǰindex�ļ������Ŀ��
	int keylength[3];	////��Ӧ�������͵ļ�ֵ��int float string

private:
	int maxchild;
	int order;
	int type;//0 int; 1 double; 2 string
	string name;
public:
	index(string filename);
	~index() {};
	void initialize(Data* key, int Addr, int ktype);
	int find(Data* key);
	void insert(Data* key, int Addr);
	int* split(char* currentBlock, Data* mid, Data* key, int Addr, int leftpos, int rightpos);
	void Internal_insert(char* currentBlock, Data* mid, int leftpos, int rightpos);
	void SplitLeaf(char* block1, char* block2, char* currentBlock, Data* key, int Addr);
	void SplitInternal(char* block1, char* block2, char* currentBlock, Data* mid, int leftpos, int rightpos);
	void Delete(Data* key);
	int* Range(Data* key1, Data* key2);
};

#endif