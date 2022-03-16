#ifndef Catalog_h
#define Catalog_h
#include "base.h"
#include "BufferManager.h"
extern BufferManager bf;

class CatalogManager {
public:
    CatalogManager() {};
    void Catalog_Create_Table(string s, vector<Attribute> attr, short primary, vector<Index> index);//create table
    bool ExistTable(std::string s);//check if a table already exists
    Table* GetTable(std::string s);//get the table from the system
    void Catalog_Create_Index(std::string tname, std::string aname, std::string iname);//create index
    void Catalog_Create_Index2(std::string tname, std::string aname, std::string iname);//create index
    void Catalog_Drop_Table(std::string t);//drop table
    void Catalog_Drop_Index(std::string tname, std::string iname);//drop index
    void Catalog_Drop_Index2(std::string iname);//drop index
    void changeblock(std::string tname, int bn);//change block we use
    void Mem_Move_Attribute(vector<Attribute>& attr, char* Block, int& pos, int RW);//move the attribute from or to the memory,RW denotes to read an attribute from the memory of to write an attribute to the memory; 0 denotes read and 1 denotes write
    void Mem_Move_Index(vector<Index>& ind, char* Block, int& pos, int RW);//move the index from or to the memory
};



#endif /* Catalog_h */