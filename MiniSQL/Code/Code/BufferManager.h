#ifndef _BUFFERMANAGER_H
#define _BUFFERMANAGER_H
#include "base.h"
#include <fstream>
struct insertPos {
	int Pos[2];
};
class buffer{
public:
	string filename;
	bool isWritten;
	bool isValid;
	bool isLocked;
	bool isLRU;
	int blockOffset;	//block offset in file, indicate position in file
	int LRUvalue;		
	char values[BLOCKSIZE + 1];
	buffer(){
		initialize();
	}
	void initialize(){
		isWritten = 0;
		isValid = 0;
		isLocked = 0;
		filename = "NULL";
		blockOffset = 0;
		LRUvalue = 0;
		memset(values,EMPTY,BLOCKSIZE); 
		values[BLOCKSIZE] = '\0';
	}
	string getdata(int pos, int length){
		string temp = "";
		if (pos >= 0) {
			for (int i = pos; i < pos + length; i++)
				temp += values[i];
		}
		return temp;
	}
};

class BufferManager
{
public:
	BufferManager();
	~BufferManager();
	void initialize(int i);//��ʼ�����Ϊi��block
	void Writeback(int num);//��block����д���ļ�
	int getbufferNum(string name, int offset);//��ȡָ��block���ڴ��еı��
	void readBlock(string name, int offset, int num); //���ļ����ȡ��block��
	void writeBlock(int num);//block����дʱ���ã������
	void useBlock(int num); //ʹ��block
	int getEmptyBuffer(string name, int i);//ȡ���ڴ��еĿ�block��ԭ��������name
	insertPos getInsertPosition(Table& tableinfor);//���ز������ݵĿ���λ��
	int IsDataInBuffer(string name, int offset);//�ҵ�ָ��block���ڴ��еı��
	void UpdateLRU();//ˢ��Least_Recent_Use��ֵ
	buffer bufferBlock[BLOCKNUM];
};


#endif
