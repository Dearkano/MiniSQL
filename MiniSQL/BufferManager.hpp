
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

#define MAX_BLOCK 200
#define BLOCK_SIZE 4096

typedef struct m_string Str;

class BufferManager
{
public:
  Str **read(string tableName);
  int update(string tableName, Str **newData, int row, int col);
  char *getpage(string tableName, int blockId);
  int modifypage(string tableName, int blockId, char *newData);
  int createNewTable(string tableName);
  int createDB(string DBName);
};

typedef struct blockInfo BlockInfo;
typedef struct fileInfo FileInfo;

FileInfo *DB;

FileInfo *get_file_info(const string DB_Name, const string fileName, const int m_fileType);
BlockInfo *get_block_inbuffer(const FileInfo *file_temp, const int blockNum);
BlockInfo *remove_block_inbuffer(BlockInfo *m_blockInfo);
BlockInfo *findBlock(string DB_Name);
void replace(FileInfo *m_fileInfo, BlockInfo *m_blockInfo);
BlockInfo *readBlock(FileInfo *file_temp, string DB_Name, const int m_blockNum, BlockInfo *block_temp);
void writeBlock(string DB_Name, BlockInfo *block);
void closeDatabase(string DB_Name, bool m_flag);
void closeFile(string DB_Name, FileInfo *file_temp)