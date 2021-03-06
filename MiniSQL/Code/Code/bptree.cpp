
#include"bptree.h"


int Maxchild(int length) {	//return the number of children in one block with different type
	return (BLOCKSIZE - MESSAGE) / (length + POINTERLENGTH) - 1;
}



index::index(string filename) :name(filename)	//filename is a .index string
{
	keylength[0] = 4;	//the length of int 
	keylength[1] = 4;	//the length of double
	keylength[2] = 20;	//the length of string 
	fstream _file;
	_file.open(filename, ios::in);

	if (!_file)	//existing already
	{
		_file.open(filename, ios::out);	//open this one
		Number = 0;						//re create
	}
	else
	{
		_file.seekp(0, ios::end);
		long size = _file.tellg();		//to calculate the size 
		Number = size / BLOCKSIZE;
		_file.close();

		if (Number == 0)
		{
			return;
		}
		char* currentBlock = new char[BLOCKSIZE];
		int buffernum = bf.getbufferNum(name, 0);
		bf.useBlock(buffernum);
		memcpy(currentBlock, bf.bufferBlock[buffernum].values, BLOCKSIZE);
		type = *(int*)(currentBlock + 20);
		maxchild = Maxchild(keylength[type]);	//calculate the maxchild 
		order = maxchild;
	}
}


void index::initialize(Data* key, int Addr, int ktype)
{
	char* root = new char[BLOCKSIZE];
	*(int*)(root) = Internal;
	*(int*)(root + FOUR_BYTE) = 0;			//node position
	*(int*)(root + FOUR_BYTE * 2) = -1;		//father node position
	*(int*)(root + FOUR_BYTE * 3) = 1;		//number of keys
	*(int*)(root + FOUR_BYTE * 4) = 0;		//delete or not
	*(int*)(root + FOUR_BYTE * 5) = ktype;	//type
	int NumOfKeys = *(int*)(root + 12);
	type = ktype;
	*(int*)(root + MESSAGE + keylength[type]) = NumOfKeys;//first member position

	switch (key->flag) {
	case -1:
		*(int*)(root + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH)) = ((Data_i*)key)->x;
		break;
	case 0:
		*(float*)(root + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH)) = ((Data_f*)key)->x;
		break;
	default:
		memcpy((char*)(root + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH)), ((Data_c*)key)->x.c_str(), ((Data_c*)key)->x.length() + 1);
		break;
	}

	*(int*)(root + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type]) = -1;
	*(int*)(root + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type] + FOUR_BYTE * 2) = NumOfKeys;
	*(int*)(root + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type] + FOUR_BYTE) = -1;
	*(int*)(root + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type] + FOUR_BYTE * 3) = 1;//leaf node position
	*(int*)(root + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type] + FOUR_BYTE * 4) = 0;//delete or not
	int buffernum = bf.getbufferNum(name, 0);
	memcpy(bf.bufferBlock[buffernum].values, root, BLOCKSIZE);
	bf.writeBlock(buffernum);
	bf.Writeback(buffernum);
	//writebuffer

	//the first leaf node
	//initialize the message part
	char* newBlock = new char[BLOCKSIZE];
	*(int*)(newBlock) = Leaf;
	*(int*)(newBlock + FOUR_BYTE) = 1;//node position
	*(int*)(newBlock + FOUR_BYTE * 2) = 0;//father node position
	*(int*)(newBlock + FOUR_BYTE * 3) = 1;//number of keys
	*(int*)(newBlock + FOUR_BYTE * 4) = 0;//delete or not

	NumOfKeys = *(int*)(newBlock + 12);
	*(int*)(newBlock + MESSAGE + keylength[type]) = NumOfKeys;
	if (key->flag == -1)
		*(int*)(newBlock + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH)) = ((Data_i*)key)->x;
	else if (key->flag == 0)
		*(float*)(newBlock + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH)) = ((Data_f*)key)->x;
	else
	{
		memcpy((char*)(newBlock + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH)), ((Data_c*)key)->x.c_str(), ((Data_c*)key)->x.length() + 1);
	}

	*(int*)(newBlock + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type]) = -1;
	*(int*)(newBlock + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = NumOfKeys;
	*(int*)(newBlock + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = Addr;
	*(int*)(newBlock + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
	buffernum = bf.getbufferNum(name, 1);
	memcpy(bf.bufferBlock[buffernum].values, newBlock, BLOCKSIZE);
	bf.writeBlock(buffernum);
	bf.Writeback(buffernum);

	maxchild = (BLOCKSIZE - MESSAGE) / (keylength[type] + POINTERLENGTH) - 1;
	order = maxchild;
	Number = 2;
	type = ktype;
	delete[] newBlock;
	delete[] root;
}

int index::find(Data* key)
{
	char* currentBlock = new char[BLOCKSIZE];	//currentBlock points to allocated space

	//buffer reader
	int buffernum = bf.getbufferNum(name, 0);
	bf.useBlock(buffernum);			//get the corresponding data in disk
	memcpy(currentBlock, bf.bufferBlock[buffernum].values, BLOCKSIZE);	//move the DB data from disk to memory


	int LeafType = *(int*)(currentBlock);	//the first int means the leaftype
	int Brother = *(int*)(currentBlock + MESSAGE + keylength[type]);	//pointer to brother
	int tempBro, position;

	while (LeafType == Internal)	//it's a internal , not leaf yet!
	{
		int NumOfKeys = *(int*)(currentBlock + 12);
		int i = 0;

		for (i = 0; i < NumOfKeys; i++)
		{
			if (type == 0)//0 means int; 
			{
				int NowKey = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
				tempBro = Brother;
				position = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
				Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
				if ((((Data_i*)key)->x < NowKey))
					break;
			}
			else if (type == 1)//1 means double
			{
				float NowKey = *(float*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
				tempBro = Brother;
				position = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
				Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
				if ((((Data_f*)key)->x < NowKey))
					break;
			}
			else //2 means string
			{
				string NowKey((char*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH)));
				tempBro = Brother;
				position = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
				Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
				if ((((Data_c*)key)->x.compare(NowKey)) < 0)
					break;
			}
		}
		if (i == NumOfKeys)
			position = *(int*)(currentBlock + MESSAGE + tempBro * (keylength[type] + POINTERLENGTH) + keylength[type] + 12);	//the last child

		if (position == -1)
		{
			return -1;	//error because block storage fault
		}

		int buffernum = bf.getbufferNum(name, position);	//go to the child block "position" points to the child
		bf.useBlock(buffernum);
		memcpy(currentBlock, bf.bufferBlock[buffernum].values, BLOCKSIZE);
		int temp_LeafType = *(int*)(currentBlock);//read out the type of the block
		if (temp_LeafType != LeafType) {
			return -1;//in one bptree the type is inconsistent
		}
		Brother = *(int*)(currentBlock + MESSAGE + keylength[type]);
		//readbuffer
	}

	//find the leaf!
	int NumOfKeys = *(int*)(currentBlock + 12);
	Brother = *(int*)(currentBlock + MESSAGE + keylength[type]);
	for (int i = 0; i < NumOfKeys; i++)
	{
		if (type == 0)
		{
			int NowKey = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
			tempBro = Brother;
			int Addr = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
			position = Addr;
			Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
			if ((((Data_i*)key)->x == NowKey) && *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) != 1)
			{
				delete[]currentBlock;
				return Addr;
			}
		}
		else if (type == 1)
		{
			float NowKey = *(float*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
			tempBro = Brother;
			int Addr = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
			position = Addr;
			Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
			if ((((Data_f*)key)->x == NowKey) && *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) != 1)
			{
				delete[]currentBlock;
				return Addr;
			}
		}
		else
		{
			string NowKey((char*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH)));
			tempBro = Brother;
			int Addr = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
			position = Addr;
			Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
			if ((((Data_c*)key)->x.compare(NowKey)) == 0 && *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) != 1)
			{
				delete[]currentBlock;
				return Addr;
			}
		}
	}
	delete[]currentBlock;
	return -1;

}

void index::insert(Data* key, int Addr)
{
	char* currentBlock = new char[BLOCKSIZE];

	if (find(key) != -1)
	{
		throw TableException("duplicating!!!");
		return;
	}

	//buff reader
	int buffernum = bf.getbufferNum(name, 0);
	bf.useBlock(buffernum);
	memcpy(currentBlock, bf.bufferBlock[buffernum].values, BLOCKSIZE);

	int LeafType = *(int*)(currentBlock);
	int Brother = *(int*)(currentBlock + MESSAGE + keylength[type]);
	int tempBro, position;

	//find the leaf node
	while (LeafType == Internal)
	{
		int NumOfKeys = *(int*)(currentBlock + 12);
		int i = 0;

		for (i = 0; i < NumOfKeys; i++)
		{
			if (type == 0)
			{
				int NowKey = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
				tempBro = Brother;
				position = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
				Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
				if ((((Data_i*)key)->x < NowKey))
					break;
			}
			else if (type == 1)
			{
				float NowKey = *(float*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
				tempBro = Brother;
				position = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
				Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
				if ((((Data_f*)key)->x < NowKey))
					break;
			}
			else
			{
				string NowKey((char*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH)));
				tempBro = Brother;
				position = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
				Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
				if ((((Data_c*)key)->x.compare(NowKey)) < 0)
					break;
			}
		}
		if (i == NumOfKeys)
			position = *(int*)(currentBlock + MESSAGE + tempBro * (keylength[type] + POINTERLENGTH) + keylength[type] + 12);
		if (position == -1)
		{	//there is need to new a block
			char* newBlock = new char[BLOCKSIZE];
			*(int*)(newBlock) = Leaf;	//define the type of the new block
			*(int*)(newBlock + 4) = Number++;	//one more block in this bptree
			*(int*)(newBlock + 8) = *(int*)(currentBlock + 4);
			*(int*)(newBlock + 12) = 1;
			*(int*)(newBlock + 16) = 0;

			*(int*)(newBlock + MESSAGE + keylength[type]) = *(int*)(newBlock + 12);
			if (key->flag == -1)
				*(int*)(newBlock + MESSAGE + (keylength[type] + POINTERLENGTH)) = ((Data_i*)key)->x;
			else if (key->flag == 0)
				*(float*)(newBlock + MESSAGE + (keylength[type] + POINTERLENGTH)) = ((Data_f*)key)->x;
			else
			{
				memcpy((char*)(newBlock + MESSAGE + (keylength[type] + POINTERLENGTH)), ((Data_c*)key)->x.c_str(), ((Data_c*)key)->x.length() + 1);
			}
			*(int*)(newBlock + MESSAGE + keylength[type] + POINTERLENGTH + keylength[type]) = -1;
			*(int*)(newBlock + MESSAGE + keylength[type] + POINTERLENGTH + keylength[type] + 4) = Addr;
			*(int*)(newBlock + MESSAGE + keylength[type] + POINTERLENGTH + keylength[type] + 8) = 1;
			*(int*)(newBlock + MESSAGE + (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
			position = Number - 1;
			int buffernum = bf.getbufferNum(name, position);
			memcpy(bf.bufferBlock[buffernum].values, newBlock, BLOCKSIZE);
			bf.writeBlock(buffernum);
			bf.Writeback(buffernum);

			*(int*)(currentBlock + MESSAGE + tempBro * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = Number - 1;
			buffernum = bf.getbufferNum(name, *(int*)(currentBlock + 4));
			memcpy(bf.bufferBlock[buffernum].values, currentBlock, BLOCKSIZE);
			bf.writeBlock(buffernum);
			bf.Writeback(buffernum);
			//writebuffer
			return;
		}
		int buffernum = bf.getbufferNum(name, position);
		bf.useBlock(buffernum);
		memcpy(currentBlock, bf.bufferBlock[buffernum].values, BLOCKSIZE);
		LeafType = *(int*)(currentBlock);
		Brother = *(int*)(currentBlock + MESSAGE + keylength[type]);
		//readbuffer;
	}


	int NumOfKeys = *(int*)(currentBlock + 12);
	Brother = *(int*)(currentBlock + MESSAGE + keylength[type]);
	if (NumOfKeys < maxchild)	//no need to split
	{
		*(int*)(currentBlock + 12) += 1;
		NumOfKeys++;
		int LastBro = 0;
		int i = 0;

		for (i = 0; i < NumOfKeys; i++)	//find the position to insert
		{
			if (type == 0)
			{
				if (Brother == -1 && i == NumOfKeys - 1)
					break;
				int NowKey = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
				if ((((Data_i*)key)->x < NowKey))
					break;
			}
			else if (type == 1)
			{
				if (Brother == -1 && i == NumOfKeys - 1)
					break;
				float NowKey = *(float*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
				if ((((Data_f*)key)->x < NowKey))
					break;
			}
			else
			{
				if (Brother == -1 && i == NumOfKeys - 1)
					break;
				string NowKey((char*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH)));
				if ((((Data_c*)key)->x.compare(NowKey)) < 0)
					break;
			}
			LastBro = Brother;
			Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
		}

		//inserting
		if (key->flag == -1)
			*(int*)(currentBlock + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH)) = ((Data_i*)key)->x;
		else if (key->flag == 0)
			*(float*)(currentBlock + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH)) = ((Data_f*)key)->x;
		else
		{
			memcpy((char*)(currentBlock + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH)), ((Data_c*)key)->x.c_str(), ((Data_c*)key)->x.length() + 1);
		}

		//set pointers to brother
		*(int*)(currentBlock + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type]) =
			*(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 8);
		*(int*)(currentBlock + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) =
			NumOfKeys;
		*(int*)(currentBlock + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) =
			Addr;
		*(int*)(currentBlock + MESSAGE + LastBro * (keylength[type] + POINTERLENGTH) + keylength[type]) =
			NumOfKeys;
		if (i == NumOfKeys - 1)
			*(int*)(currentBlock + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type]) = -1;

		//writing back to the memory
		int buffernum = bf.getbufferNum(name, *(int*)(currentBlock + 4));
		memcpy(bf.bufferBlock[buffernum].values, currentBlock, BLOCKSIZE);
		bf.writeBlock(buffernum);
		bf.Writeback(buffernum);
		//writebuffer
	}
	else	//there is need to split
	{
		*(int*)(currentBlock + 12) += 1;
		Data* mid = NULL;
		split(currentBlock, mid, key, Addr, 0, 0);
		bf.getbufferNum(name, 8);
	}

	delete[]currentBlock;
}

int* index::split(char* currentBlock, Data* mid, Data* key, int Addr, int leftpos, int rightpos)
{
	int NumOfKeys = *(int*)(currentBlock + 12);
	int LeafType = *(int*)(currentBlock);
	int father[2] = { 0, 0 };
	if ((*(int*)(currentBlock + 8) == -1) && NumOfKeys >= maxchild - 1)	//another times split
	{
		char* newBlock1 = new char[BLOCKSIZE];
		char* newBlock2 = new char[BLOCKSIZE];
		SplitInternal(newBlock1, newBlock2, currentBlock, mid, leftpos, rightpos);//mid need change
		leftpos = *(int*)(newBlock1 + 4);
		rightpos = *(int*)(newBlock2 + 4);

		*(int*)(newBlock1 + 8) = 0;
		*(int*)(newBlock2 + 8) = 0;

		if (type == 0)
		{
			mid = new Data_i(*(int*)(newBlock2 + MESSAGE + keylength[type] + POINTERLENGTH));
		}
		else if (type == 1)
		{
			mid = new Data_f(*(float*)(newBlock2 + MESSAGE + keylength[type] + POINTERLENGTH));
		}
		else
		{
			mid = new Data_c((char*)(newBlock2 + MESSAGE + keylength[type] + POINTERLENGTH));
		}

		int buffernum = bf.getbufferNum(name, *(int*)(newBlock1 + 4));
		memcpy(bf.bufferBlock[buffernum].values, newBlock1, BLOCKSIZE);
		bf.writeBlock(buffernum);
		bf.Writeback(buffernum);
		//writebuffer
		buffernum = bf.getbufferNum(name, *(int*)(newBlock2 + 4));
		memcpy(bf.bufferBlock[buffernum].values, newBlock2, BLOCKSIZE);
		bf.writeBlock(buffernum);
		bf.Writeback(buffernum);
		//writebuffer
		char* root = new char[BLOCKSIZE];
		*(int*)(root) = Internal;
		*(int*)(root + 4) = 0;//node position
		*(int*)(root + 8) = -1;//father node position
		*(int*)(root + 12) = 1;//number of keys
		*(int*)(root + 16) = 0;//delete or not
		*(int*)(root + 20) = type;//type

		int NumOfKeys = *(int*)(root + 12);
		*(int*)(root + MESSAGE + keylength[type]) = NumOfKeys;
		if (key->flag == -1)
			*(int*)(root + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH)) = ((Data_i*)mid)->x;
		else if (key->flag == 0)
			*(float*)(root + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH)) = ((Data_f*)mid)->x;
		else
		{
			memcpy((char*)(root + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH)), ((Data_c*)mid)->x.c_str(), ((Data_c*)mid)->x.length() + 1);
		}
		*(int*)(root + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type]) = -1;
		*(int*)(root + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = NumOfKeys;
		*(int*)(root + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = leftpos;
		*(int*)(root + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = rightpos;
		*(int*)(root + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not

		buffernum = bf.getbufferNum(name, *(int*)(root + 4));
		memcpy(bf.bufferBlock[buffernum].values, root, BLOCKSIZE);
		bf.writeBlock(buffernum);
		bf.Writeback(buffernum);
		//writebuffer;root

		father[0] = *(int*)(newBlock1 + 4);
		father[1] = *(int*)(newBlock2 + 4);

		delete[]root;
		delete[]newBlock1;
		delete[]newBlock2;
		return father;
	}
	else
	{
		if (LeafType == Leaf && NumOfKeys >= maxchild)
		{
			char* newBlock1 = new char[BLOCKSIZE];
			char* newBlock2 = new char[BLOCKSIZE];
			int* temp = NULL;
			SplitLeaf(newBlock1, newBlock2, currentBlock, key, Addr);
			*(int*)(currentBlock + 16) = 1;
			leftpos = *(int*)(newBlock1 + 4);
			rightpos = *(int*)(newBlock2 + 4);

			if (type == 0)
			{
				mid = new Data_i(*(int*)(newBlock2 + MESSAGE + keylength[type] + POINTERLENGTH));
			}
			else if (type == 1)
			{
				mid = new Data_f(*(float*)(newBlock2 + MESSAGE + keylength[type] + POINTERLENGTH));
			}
			else
			{
				mid = new Data_c((char*)(newBlock2 + MESSAGE + keylength[type] + POINTERLENGTH));
			}

			int buffernum = bf.getbufferNum(name, *(int*)(currentBlock + 8));
			bf.useBlock(buffernum);
			char* fatherBlock = new char[BLOCKSIZE];
			memcpy(fatherBlock, bf.bufferBlock[buffernum].values, BLOCKSIZE);
			//readbuffer;

			temp = split(fatherBlock, mid, key, Addr, leftpos, rightpos);

			*(int*)(newBlock1 + 8) = temp[0];
			*(int*)(newBlock2 + 8) = temp[1];

			buffernum = bf.getbufferNum(name, *(int*)(currentBlock + 4));
			memcpy(bf.bufferBlock[buffernum].values, currentBlock, BLOCKSIZE);
			bf.writeBlock(buffernum);
			bf.Writeback(buffernum);
			//writebuffer

			buffernum = bf.getbufferNum(name, *(int*)(newBlock1 + 4));
			memcpy(bf.bufferBlock[buffernum].values, newBlock1, BLOCKSIZE);
			bf.writeBlock(buffernum);
			bf.Writeback(buffernum);
			//writebuffer
			buffernum = bf.getbufferNum(name, *(int*)(newBlock2 + 4));
			memcpy(bf.bufferBlock[buffernum].values, newBlock2, BLOCKSIZE);
			bf.writeBlock(buffernum);
			bf.Writeback(buffernum);
			//writebuffer

			father[0] = *(int*)(newBlock1 + 4);
			father[1] = *(int*)(newBlock2 + 4);

			delete[]newBlock1;
			delete[]newBlock2;
			delete[]fatherBlock;
			return father;
		}
		else if (LeafType == Internal && NumOfKeys >= maxchild - 1)
		{
			char* newBlock1 = new char[BLOCKSIZE];
			char* newBlock2 = new char[BLOCKSIZE];
			SplitInternal(newBlock1, newBlock2, currentBlock, mid, leftpos, rightpos);//mid need be change
			leftpos = *(int*)(newBlock1 + 4);
			rightpos = *(int*)(newBlock2 + 4);
			*(int*)(currentBlock + 16) = 1;
			int* temp = NULL;

			if (type == 0)
			{
				mid = new Data_i(*(int*)(newBlock2 + MESSAGE + keylength[type] + POINTERLENGTH));
			}
			else if (type == 1)
			{
				mid = new Data_f(*(float*)(newBlock2 + MESSAGE + keylength[type] + POINTERLENGTH));
			}
			else
			{
				mid = new Data_c((char*)(newBlock2 + MESSAGE + keylength[type] + POINTERLENGTH));
			}

			int buffernum = bf.getbufferNum(name, *(int*)(currentBlock + 8));
			bf.useBlock(buffernum);
			char* fatherBlock = new char[BLOCKSIZE];
			memcpy(fatherBlock, bf.bufferBlock[buffernum].values, BLOCKSIZE);
			//readbuffer;

			temp = split(fatherBlock, mid, key, Addr, leftpos, rightpos);

			*(int*)(newBlock1 + 8) = temp[0];
			*(int*)(newBlock2 + 8) = temp[1];

			buffernum = bf.getbufferNum(name, *(int*)(currentBlock + 4));
			memcpy(bf.bufferBlock[buffernum].values, currentBlock, BLOCKSIZE);
			bf.writeBlock(buffernum);
			bf.Writeback(buffernum);
			//writebuffer

			buffernum = bf.getbufferNum(name, *(int*)(newBlock1 + 4));
			memcpy(bf.bufferBlock[buffernum].values, newBlock1, BLOCKSIZE);
			bf.writeBlock(buffernum);
			bf.Writeback(buffernum);
			//writebuffer
			buffernum = bf.getbufferNum(name, *(int*)(newBlock2 + 4));
			memcpy(bf.bufferBlock[buffernum].values, newBlock2, BLOCKSIZE);
			bf.writeBlock(buffernum);
			bf.Writeback(buffernum);
			//writebuffer

			father[0] = *(int*)(newBlock1 + 4);
			father[1] = *(int*)(newBlock2 + 4);

			delete[]newBlock1;
			delete[]newBlock2;
			delete[]fatherBlock;
			return father;
		}
		else if (LeafType == Internal && NumOfKeys < maxchild - 1)
		{
			*(int*)(currentBlock + 12) += 1;

			Internal_insert(currentBlock, mid, leftpos, rightpos);//insert into internal

			int buffernum = bf.getbufferNum(name, 8);
			buffernum = bf.getbufferNum(name, *(int*)(currentBlock + 4));
			memcpy(bf.bufferBlock[buffernum].values, currentBlock, BLOCKSIZE);
			bf.writeBlock(buffernum);
			bf.Writeback(buffernum);
			//writebuffer
			buffernum = bf.getbufferNum(name, 8);
			father[0] = *(int*)(currentBlock + 4);
			father[1] = *(int*)(currentBlock + 4);

			return father;
		}
	}
}

void  index::Internal_insert(char* currentBlock, Data* mid, int leftpos, int rightpos)
{
	int NumOfKeys = *(int*)(currentBlock + 12);
	int Brother = *(int*)(currentBlock + MESSAGE + keylength[type]);
	int LastBro = 0;
	int i = 0;

	for (i = 0; i < NumOfKeys; i++)	// find the current node
	{
		if (type == 0)
		{
			if (Brother == -1 && i == NumOfKeys - 1)	//the last one
				break;
			int NowKey = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
			if ((((Data_i*)mid)->x < NowKey))		//the right position
				break;
		}
		else if (type == 1)
		{
			if (Brother == -1 && i == NumOfKeys - 1)//the last one
				break;
			float NowKey = *(float*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
			if ((((Data_f*)mid)->x < NowKey))//the right position
				break;
		}
		else
		{
			if (Brother == -1 && i == NumOfKeys - 1)//the last one
				break;
			string NowKey;
			NowKey = (char*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
			if ((((Data_c*)mid)->x.compare(NowKey)) < 0)//the right position
				break;
		}
		//go to the later position
		LastBro = Brother;
		Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
	}

	//inserting 
	if (type == 0)
		*(int*)(currentBlock + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH)) = ((Data_i*)mid)->x;
	else if (type == 1)
		*(float*)(currentBlock + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH)) = ((Data_f*)mid)->x;
	else
	{
		memcpy((char*)(currentBlock + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH)), ((Data_c*)mid)->x.c_str(), ((Data_c*)mid)->x.length() + 1);
	}

	//fulfill the key part
	if (i == NumOfKeys - 1)
	{
		*(int*)(currentBlock + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type]) = -1;		//last one 
		*(int*)(currentBlock + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = leftpos;
		*(int*)(currentBlock + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = NumOfKeys;
		*(int*)(currentBlock + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = rightpos;
		*(int*)(currentBlock + MESSAGE + LastBro * (keylength[type] + POINTERLENGTH) + keylength[type]) = NumOfKeys;
		*(int*)(currentBlock + MESSAGE + LastBro * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = leftpos;
	}
	else
	{
		*(int*)(currentBlock + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type]) = Brother;	//not the last one 
		*(int*)(currentBlock + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = leftpos;
		*(int*)(currentBlock + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = NumOfKeys;
		*(int*)(currentBlock + MESSAGE + NumOfKeys * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = rightpos;
		*(int*)(currentBlock + MESSAGE + LastBro * (keylength[type] + POINTERLENGTH) + keylength[type]) = NumOfKeys;
		*(int*)(currentBlock + MESSAGE + LastBro * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = leftpos;
		*(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = rightpos;
	}
}

void index::SplitLeaf(char* block1, char* block2, char* currentBlock, Data* key, int Addr)
{
	int NumOfKeys = *(int*)(currentBlock + 12);
	int Brother = *(int*)(currentBlock + MESSAGE + keylength[type]);
	int Address, flag = 1;
	int i, position = 1;

	*(int*)(block1) = Leaf;
	*(int*)(block1 + 4) = Number++;
	*(int*)(block1 + 8) = 0;
	*(int*)(block1 + 12) = NumOfKeys / 2;
	*(int*)(block1 + 16) = 0;

	*(int*)(block2) = Leaf;
	*(int*)(block2 + 4) = Number++;
	*(int*)(block2 + 8) = 0;
	*(int*)(block2 + 12) = NumOfKeys - (int)(NumOfKeys / 2);
	*(int*)(block2 + 16) = 0;

	*(int*)(block1 + MESSAGE + keylength[type]) = 1;
	*(int*)(block2 + MESSAGE + keylength[type]) = 1;

	for (i = 0; i < NumOfKeys / 2; i++)
	{
		if (type == 0)
		{
			int NowKey = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
			Address = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
			if ((((Data_i*)key)->x < NowKey) && flag)
			{
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH)) = ((Data_i*)key)->x;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = position + 1;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = Addr;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
				position++;
				flag = 0;
			}
			else
			{
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH)) = NowKey;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = position + 1;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = Address;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
				position++;
				Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
			}
		}
		else if (type == 1)
		{
			float NowKey = *(float*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
			Address = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
			if ((((Data_f*)key)->x < NowKey) && flag)
			{
				*(float*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH)) = ((Data_f*)key)->x;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = position + 1;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = Addr;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
				position++;
				flag = 0;
			}
			else
			{
				*(float*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH)) = NowKey;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = position + 1;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = Address;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
				position++;
				Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
			}
		}
		else
		{
			string NowKey;
			NowKey = (char*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
			Address = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
			if ((((Data_c*)key)->x.compare(NowKey)) < 0 && flag)
			{
				memcpy((char*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH)), ((Data_c*)key)->x.c_str(), ((Data_c*)key)->x.length() + 1);
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = position + 1;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = Addr;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
				position++;
				flag = 0;
			}
			else
			{
				memcpy((char*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH)), NowKey.c_str(), NowKey.length() + 1);
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = position + 1;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = Address;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
				position++;
				Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
			}
		}
	}
	*(int*)(block1 + MESSAGE + (position - 1) * (keylength[type] + POINTERLENGTH) + keylength[type]) = -1;

	position = 1;
	for (; i < NumOfKeys; i++)
	{
		if (type == 0)
		{
			if (Brother == -1 && i == NumOfKeys - 1)
				break;
			int NowKey = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
			Address = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
			if ((((Data_i*)key)->x < NowKey) && flag)
			{
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH)) = ((Data_i*)key)->x;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = position + 1;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = Addr;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
				position++;
				flag = 0;
			}
			else
			{
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH)) = NowKey;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = position + 1;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = Address;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
				position++;
				Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
			}
		}
		else if (type == 1)
		{
			if (Brother == -1 && i == NumOfKeys - 1)
				break;
			float NowKey = *(float*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
			Address = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
			if ((((Data_f*)key)->x < NowKey) && flag)
			{
				*(float*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH)) = ((Data_f*)key)->x;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = position + 1;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = Addr;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
				position++;
				flag = 0;
			}
			else
			{
				*(float*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH)) = NowKey;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = position + 1;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = Address;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
				position++;
				Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
			}
		}
		else
		{
			if (Brother == -1 && i == NumOfKeys - 1)
				break;
			string NowKey;
			NowKey = (char*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
			Address = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
			if ((((Data_c*)key)->x.compare(NowKey)) < 0 && flag)
			{
				memcpy((char*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH)), ((Data_c*)key)->x.c_str(), ((Data_c*)key)->x.length() + 1);
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = position + 1;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = Addr;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
				position++;
				flag = 0;
			}
			else
			{
				memcpy((char*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH)), NowKey.c_str(), NowKey.length() + 1);
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = position + 1;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = Address;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
				position++;
				Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
			}
		}
	}
	if (flag)
	{
		if (key->flag == -1)
			*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH)) = ((Data_i*)key)->x;
		else if (key->flag == 0)
			*(float*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH)) = ((Data_f*)key)->x;
		else
		{
			memcpy((char*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH)), ((Data_c*)key)->x.c_str(), ((Data_c*)key)->x.length() + 1);
		}
		*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = -1;
		*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = Addr;
		*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
		*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
		position++;
	}
	*(int*)(block2 + MESSAGE + (position - 1) * (keylength[type] + POINTERLENGTH) + keylength[type]) = -1;
}

void index::SplitInternal(char* block1, char* block2, char* currentBlock, Data* mid, int leftpos, int rightpos)
{
	*(int*)(currentBlock + 12) += 1;
	int NumOfKeys = *(int*)(currentBlock + 12);
	int Brother = *(int*)(currentBlock + MESSAGE + keylength[type]);
	int Addr1, Addr2;
	int i, position = 1, flag = 1, LastBro = 0;

	*(int*)(block1) = Internal;
	*(int*)(block1 + 4) = Number++;
	*(int*)(block1 + 8) = 0;
	*(int*)(block1 + 12) = NumOfKeys / 2;
	*(int*)(block1 + 16) = 0;

	*(int*)(block2) = Internal;
	*(int*)(block2 + 4) = Number++;
	*(int*)(block2 + 8) = 0;
	*(int*)(block2 + 12) = NumOfKeys - (int)(NumOfKeys / 2);
	*(int*)(block2 + 16) = 0;

	*(int*)(block1 + MESSAGE + keylength[type]) = 1;
	*(int*)(block2 + MESSAGE + keylength[type]) = 1;

	for (i = 0; i < NumOfKeys / 2; i++)
	{
		if (type == 0)
		{
			int NowKey = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
			Addr1 = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
			Addr2 = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 12);
			if ((((Data_i*)mid)->x <= NowKey) && flag)
			{
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH)) = ((Data_i*)mid)->x;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = position + 1;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = leftpos;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = rightpos;
				*(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = rightpos;
				*(int*)(block1 + MESSAGE + (position - 1) * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = leftpos;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
				position++;
				flag = 0;
			}
			else
			{
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH)) = NowKey;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = position + 1;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = Addr1;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = Addr2;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
				LastBro = Brother;
				Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
				position++;
			}
		}
		else if (type == 1)
		{
			int NowKey = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
			Addr1 = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
			Addr2 = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 12);
			if ((((Data_f*)mid)->x <= NowKey) && flag)
			{
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH)) = ((Data_f*)mid)->x;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = position + 1;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = leftpos;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = rightpos;
				*(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = rightpos;
				*(int*)(block1 + MESSAGE + (position - 1) * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = leftpos;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
				position++;
				flag = 0;
			}
			else
			{
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH)) = NowKey;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = position + 1;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = Addr1;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = Addr2;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
				LastBro = Brother;
				Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
				position++;
			}
		}

		else
		{
			string NowKey;
			NowKey = (char*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
			Addr1 = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
			Addr2 = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 12);
			if ((((Data_c*)mid)->x.compare(NowKey)) <= 0 && flag)
			{
				memcpy((char*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH)), ((Data_c*)mid)->x.c_str(), ((Data_c*)mid)->x.length() + 1);
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = position + 1;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = leftpos;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = rightpos;
				*(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = rightpos;
				*(int*)(block1 + MESSAGE + (position - 1) * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = leftpos;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
				position++;
				flag = 0;
			}
			else
			{
				memcpy((char*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH)), NowKey.c_str(), NowKey.length() + 1);
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = position + 1;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = Addr1;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = Addr2;
				*(int*)(block1 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
				LastBro = Brother;
				Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
				position++;
			}
		}
	}

	position = 1;
	int j = i;
	for (; i < NumOfKeys; i++)
	{
		if (type == 0)
		{
			if (Brother == -1 && i == NumOfKeys - 1)
				break;
			int NowKey = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
			Addr1 = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
			Addr2 = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 12);
			if ((((Data_i*)mid)->x <= NowKey) && flag)
			{
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH)) = ((Data_i*)mid)->x;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = position + 1;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = leftpos;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = rightpos;
				*(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = rightpos;
				if (i == j)
				{
					*(int*)(block1 + MESSAGE + *(int*)(block1 + 12) * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = leftpos;
				}
				else *(int*)(block2 + MESSAGE + (position - 1) * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = leftpos;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
				position++;
				flag = 0;
			}
			else
			{
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH)) = NowKey;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = position + 1;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = Addr1;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = Addr2;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
				LastBro = Brother;
				Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
				position++;
			}
		}
		else if (type == 1)
		{
			if (Brother == -1 && i == NumOfKeys - 1)
				break;
			int NowKey = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
			Addr1 = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
			Addr2 = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 12);
			if ((((Data_f*)mid)->x <= NowKey) && flag)
			{
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH)) = ((Data_f*)mid)->x;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = position + 1;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = leftpos;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = rightpos;
				*(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = rightpos;
				if (i == j)
				{
					*(int*)(block1 + MESSAGE + *(int*)(block1 + 12) * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = leftpos;
				}
				else *(int*)(block2 + MESSAGE + (position - 1) * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = leftpos;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
				position++;
				flag = 0;
			}
			else
			{
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH)) = NowKey;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = position + 1;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = Addr1;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = Addr2;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
				LastBro = Brother;
				Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
				position++;
			}
		}

		else
		{
			if (Brother == -1 && i == NumOfKeys - 1)
				break;
			string NowKey;
			NowKey = (char*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
			Addr1 = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
			Addr2 = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 12);
			if ((((Data_c*)mid)->x.compare(NowKey)) <= 0 && flag)
			{
				memcpy((char*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH)), ((Data_c*)mid)->x.c_str(), ((Data_c*)mid)->x.length() + 1);
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = position + 1;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = leftpos;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = rightpos;
				*(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = rightpos;
				if (i == j)
				{
					*(int*)(block1 + MESSAGE + *(int*)(block1 + 12) * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = leftpos;
				}
				else *(int*)(block2 + MESSAGE + (position - 1) * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = leftpos;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
				position++;
				flag = 0;
			}
			else
			{
				memcpy((char*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH)), NowKey.c_str(), NowKey.length() + 1);
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = position + 1;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = Addr1;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = Addr2;
				*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
				LastBro = Brother;
				Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
				position++;
			}
		}
	}
	if (flag)
	{
		if (mid->flag == -1)
			*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH)) = ((Data_i*)mid)->x;
		else if (mid->flag == 0)
			*(float*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH)) = ((Data_f*)mid)->x;
		else
		{
			memcpy((char*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH)), ((Data_c*)mid)->x.c_str(), ((Data_c*)mid)->x.length() + 1);
		}
		*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type]) = -1;
		*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 4) = leftpos;
		*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 8) = position;
		*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 0;//delete or not
		*(int*)(block2 + MESSAGE + position * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = rightpos;
		*(int*)(block2 + MESSAGE + (position - 1) * (keylength[type] + POINTERLENGTH) + keylength[type] + 12) = leftpos;
		position++;
	}
}

void index::Delete(Data* key)
{
	char* currentBlock = new char[BLOCKSIZE];

	int buffernum = bf.getbufferNum(name, 0);
	bf.useBlock(buffernum);
	memcpy(currentBlock, bf.bufferBlock[buffernum].values, BLOCKSIZE);
	//readbuffer;
	int LeafType = *(int*)(currentBlock);
	int Brother = *(int*)(currentBlock + MESSAGE + keylength[type]);
	int tempBro, position;

	while (LeafType == Internal)
	{
		int NumOfKeys = *(int*)(currentBlock + 12);
		int i = 0;

		for (i = 0; i < NumOfKeys; i++)
		{
			if (type == 0)
			{
				int NowKey = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
				tempBro = Brother;
				position = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
				Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
				if ((((Data_i*)key)->x < NowKey))
					break;
			}
			else if (type == 1)
			{
				float NowKey = *(float*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
				tempBro = Brother;
				position = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
				Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
				if ((((Data_f*)key)->x < NowKey))
					break;
			}
			else
			{
				string NowKey;
				NowKey = (char*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
				tempBro = Brother;
				position = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
				Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
				if ((((Data_c*)key)->x.compare(NowKey)) < 0)
					break;
			}
		}
		if (i == NumOfKeys)
			position = *(int*)(currentBlock + MESSAGE + tempBro * (keylength[type] + POINTERLENGTH) + keylength[type] + 12);
		if (position == -1)
		{
			throw TableException("invalid delete!!!");
		}
		int buffernum = bf.getbufferNum(name, position);
		bf.useBlock(buffernum);
		memcpy(currentBlock, bf.bufferBlock[buffernum].values, BLOCKSIZE);
		LeafType = *(int*)(currentBlock);
		Brother = *(int*)(currentBlock + MESSAGE + keylength[type]);
		//readbuffer
	}

	int NumOfKeys = *(int*)(currentBlock + 12);
	Brother = *(int*)(currentBlock + MESSAGE + keylength[type]);
	int i = 0;
	for (i = 0; i < NumOfKeys; i++)
	{
		if (type == 0)
		{
			int NowKey = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
			tempBro = Brother;
			int Addr = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + FOUR_BYTE);
			position = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + FOUR_BYTE);
			Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
			if ((((Data_i*)key)->x == NowKey) && *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) != 1)
			{
				*(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 1;
				int buffernum = bf.getbufferNum(name, *(int*)(currentBlock + 4));
				memcpy(bf.bufferBlock[buffernum].values, currentBlock, BLOCKSIZE);
				break;
			}
			else
			{
				throw TableException("Wrong delete!");
			}
		}
		else if (type == 1)
		{
			float NowKey = *(float*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
			tempBro = Brother;
			int Addr = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + FOUR_BYTE);
			position = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + FOUR_BYTE);
			Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
			if ((((Data_f*)key)->x == NowKey) && *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) != 1)
			{
				*(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 1;
				int buffernum = bf.getbufferNum(name, *(int*)(currentBlock + FOUR_BYTE));
				memcpy(bf.bufferBlock[buffernum].values, currentBlock, BLOCKSIZE);
				break;
			}
			else
			{
				throw TableException("Wrong delete!");
			}
		}
		else
		{
			string NowKey;
			NowKey = (char*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
			tempBro = Brother;
			int Addr = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + FOUR_BYTE);
			position = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + FOUR_BYTE);
			Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
			if ((((Data_c*)key)->x.compare(NowKey)) == 0 && *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) != 1)
			{
				*(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) = 1;
				int buffernum = bf.getbufferNum(name, *(int*)(currentBlock + FOUR_BYTE));
				memcpy(bf.bufferBlock[buffernum].values, currentBlock, BLOCKSIZE);
				break;
			}
		}

		if (i == NumOfKeys)
		{
			throw TableException("Wrong delete!");
		}
	}
	delete[]currentBlock;
}

int* index::Range(Data* key1, Data* key2)
{
	char* currentBlock = new char[BLOCKSIZE];

	int* re = new int[1000];
	re[0] = -1;
	int buffernum = bf.getbufferNum(name, 0);
	bf.useBlock(buffernum);
	memcpy(currentBlock, bf.bufferBlock[buffernum].values, BLOCKSIZE);
	//readbuffer;
	int LeafType = *(int*)(currentBlock);
	int Brother = *(int*)(currentBlock + MESSAGE + keylength[type]);
	int tempBro, position;

	while (LeafType == Internal)
	{
		int NumOfKeys = *(int*)(currentBlock + 12);
		int i = 0;

		for (i = 0; i < NumOfKeys; i++)
		{
			if (type == 0)
			{
				int NowKey = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
				tempBro = Brother;
				position = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + FOUR_BYTE);
				Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
				if ((((Data_i*)key1)->x < NowKey))
					break;
			}
			else if (type == 1)
			{
				float NowKey = *(float*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
				tempBro = Brother;
				position = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + FOUR_BYTE);
				Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
				if ((((Data_f*)key1)->x < NowKey))
					break;
			}
			else
			{
				string NowKey((char*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH)));
				tempBro = Brother;
				position = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + FOUR_BYTE);
				Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
				if ((((Data_c*)key1)->x.compare(NowKey)) < 0)
					break;
			}
		}
		if (i == NumOfKeys)
			position = *(int*)(currentBlock + MESSAGE + tempBro * (keylength[type] + POINTERLENGTH) + keylength[type] + 12);
		if (position == -1)
		{
			return re;
		}
		int buffernum = bf.getbufferNum(name, position);
		bf.useBlock(buffernum);
		memcpy(currentBlock, bf.bufferBlock[buffernum].values, BLOCKSIZE);
		LeafType = *(int*)(currentBlock);
		Brother = *(int*)(currentBlock + MESSAGE + keylength[type]);
		//readbuffer
	}
	int NumOfKeys = *(int*)(currentBlock + 12);
	Brother = *(int*)(currentBlock + MESSAGE + keylength[type]);
	for (int i = 0; i < NumOfKeys; i++)
	{
		if (type == 0)
		{
			int NowKey = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
			tempBro = Brother;
			int Addr = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
			position = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
			Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
			if ((((Data_i*)key1)->x == NowKey) && *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + FOUR_BYTE * 4) != 1)
			{
				int j = 0;
				while (Brother != -1 && NowKey <= ((Data_i*)key2)->x)
				{
					re[j++] = Addr;
					Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
					NowKey = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
					Addr = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + FOUR_BYTE);
				}
				re[j] = -1;
				delete[]currentBlock;
				return re;
			}
		}
		else if (type == 1)
		{
			float NowKey = *(float*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
			tempBro = Brother;
			int Addr = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + FOUR_BYTE);
			position = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + FOUR_BYTE);
			Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
			if ((((Data_f*)key1)->x == NowKey) && *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) != 1)
			{
				int j = 0;
				while (Brother != -1 && NowKey <= ((Data_f*)key2)->x)
				{
					re[j++] = Addr;
					Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
					NowKey = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
					Addr = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 4);
				}
				re[j] = -1;
				delete[]currentBlock;
				return re;
			}
		}
		else
		{
			string NowKey((char*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH)));
			tempBro = Brother;
			int Addr = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + FOUR_BYTE);
			position = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + FOUR_BYTE);
			Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
			if ((((Data_c*)key1)->x.compare(NowKey)) == 0 && *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + 16) != 1)
			{
				int j = 0;
				while (Brother != -1 && (((Data_c*)key2)->x.compare(NowKey)) >= 0)
				{
					re[j++] = Addr;
					Brother = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type]);
					NowKey = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH));
					Addr = *(int*)(currentBlock + MESSAGE + Brother * (keylength[type] + POINTERLENGTH) + keylength[type] + FOUR_BYTE);
				}
				re[j] = -1;
				delete[]currentBlock;
				return re;
			}
		}
	}
	return re;
	delete[]currentBlock;
}