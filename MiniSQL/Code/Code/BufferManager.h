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
	void initialize(int i);//初始化编号为i的block
	void Writeback(int num);//将block立刻写入文件
	int getbufferNum(string name, int offset);//获取指定block在内存中的编号
	void readBlock(string name, int offset, int num); //将文件块读取到block中
	void writeBlock(int num);//block被改写时调用，做标记
	void useBlock(int num); //使用block
	int getEmptyBuffer(string name, int i);//取得内存中的空block且原名不能是name
	insertPos getInsertPosition(Table& tableinfor);//返回插入数据的可行位置
	int IsDataInBuffer(string name, int offset);//找到指定block在内存中的编号
	void UpdateLRU();//刷新Least_Recent_Use的值
	buffer bufferBlock[BLOCKNUM];
};


#endif
