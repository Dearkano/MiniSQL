/*
1.   根据需要，读取指定的数据到系统缓冲区或将缓冲区中的数据写出到文件；
2.   实现缓冲区的替换算法，当缓冲区满时选择合适的页进行替换；
3.   记录缓冲区中各页的状态，如是否被修改过等；
4.   提供缓冲区页的pin功能，及锁定缓冲区的页，不允许替换出去。
    为提高磁盘I/O操作的效率，缓冲区与文件系统交互的单位是块，块的大小应为文件系统与磁盘交互单位的整数倍，一般可定为4KB或8KB。
*/
#include "BufferManager.hpp"
using namespace std;

struct fileInfo
{
    int type;         //0->datafile 1->indexfile 文件类型 数据文件或索引文件
    string fileName;  //file name文件名
    int recordAmount; //amount of the record 记录数
    int freeNum;      //the number of free block 空块号
    int recordLength; //记录长度
    FileInfo *next;   //指向下一个文件的指针
    BlockInfo *first; //文件指向的第一个块
    int row;          //文件行数
    int col;          //文件列数
};

struct blockInfo //一个block 存一条记录 每个字段256Byte, 最多存15个字段
{
    int blockNum;    // the block number of the block,which indicate it when it be newed
    bool dirtyBit;   // 0 -> flase， 1 -> indicate dirty, write back
    BlockInfo *next; // the pointer point to next block
    FileInfo *file;  // the pointer point to the file, which the block belongs to
    int charNum;     // the number of chars in the block
    char *cBlock;    // the array space for storingthe records in the block in buffer
    int iTime;       // it indicate the ageof the block in use
    int lock;        // prevent the blockfrom replacing
};

struct m_string
{
  public:
    static const int str_size = 256;
    char str[str_size];

  public:
    m_string() = default;

    m_string(const m_string &) = default;

    m_string &operator=(const m_string &) = default;

    m_string &operator=(const std::string &std_str)
    {
        memcpy(str, std_str.c_str(), std_str.size());
        return *this;
    }

    m_string &operator=(const char *c_str)
    {
        memcpy(str, c_str, strlen(c_str));
        return *this;
    }

    bool operator!=(const m_string &obj)
    {
        return strcmp(this->str, obj.str) != 0;
    }

    bool operator==(const m_string &obj)
    {
        return strcmp(this->str, obj.str) == 0;
    }

    bool operator>(const m_string &obj)
    {
        return strcmp(this->str, obj.str) > 0;
    }

    bool operator<(const m_string &obj)
    {
        return strcmp(this->str, obj.str) < 0;
    }

    bool operator>=(const m_string &obj)
    {
        return strcmp(this->str, obj.str) >= 0;
    }

    bool operator<=(const m_string &obj)
    {
        return strcmp(this->str, obj.str) <= 0;
    }

    friend std::ostream &operator<<(std::ostream &out, const m_string &obj)
    {
        out << obj.str;
        return out;
    }

    friend std::istream &operator>>(std::istream &in, m_string &obj)
    {
        in >> obj.str;
        return in;
    }

    ~m_string() = default;
};
//
Str **BufferManager::read(string tableName)
{
    FileInfo *fTemp = get_file_info("DBNAME", tableName, 0);
    BlockInfo *bTemp = fTemp->first;
    int row = fTemp->row;
    int col = fTemp->col;
    //定义一个三维字符串数组
    Str **re = new Str *[row];
    int i = 0;
    for (i = 0; i < row; i++)
    {
        re[i] = new Str[col];
    }
    int j = 0;
    char *p = bTemp->cBlock;
    for (i = 0; i < fTemp->row; i++)
    {
        for (j = 0; j < fTemp->col; j++)
        {
            strncpy(re[i][j].str, p, 256);
            p += 256;
        }
        bTemp = bTemp->next;
        if (bTemp == NULL)
        {
            return re;
        }
        return re;
    }
    return re;
}

int update(string tableName, Str **newData, int row, int col)
{
    FileInfo *fTemp = get_file_info("DBNAME", tableName, 0);
    if (fTemp == NULL)
        return 0;
    fTemp->row = row;
    fTemp->col = col;
    int i;
    int j;
    BlockInfo *bTemp = fTemp->first;
    if (bTemp == NULL)
    {
        fTemp->first = findBlock("DB_Name");
        if (fTemp->first == NULL)
        {
            return -1;
        }
    }
    bTemp = fTemp->first;
    char *p;
    for (i = 0; i < row; i++)
    {
        p = bTemp->cBlock;
        for (j = 0; j < col; j++)
        {
            strncpy(p, newData[i][j].str, 256);
            p += 256;
        }
        if (bTemp->next == NULL && i != row - 1)
        {
            bTemp->next = findBlock("DB_Name");
            if (bTemp->next == NULL)
            {
                return -1;
            }
        }
        bTemp = bTemp->next;
    }
    //如果更新完成后面仍有多余的块节点（删除了数据），把块从文件节点下移除
    BlockInfo *temp;
    if (bTemp != NULL)
    {
        temp = bTemp;
        bTemp = bTemp->next;
        temp->next == NULL;
    }
    return 1;
}

char *BufferManager::getpage(string tableName, int BlockId)
{
    FileInfo *fTemp = get_file_info("DBNAME", tableName, 0);
    if (fTemp == NULL)
    {
        return NULL;
    }
    BlockInfo *bTemp = fTemp->first;
    int mark = 0;
    char *re = NULL;
    while (bTemp != NULL)
    {
        if (bTemp->blockNum == BlockId)
        {
            re = (char *)malloc(bTemp->charNum + 1);
            strncpy(re, bTemp->cBlock, bTemp->charNum);
            mark = 1;
            break;
        }
        bTemp = bTemp->next;
    }
    if (mark == 1)
    {
        return re;
    }
}

int BufferManager::modifypage(string tableName, int BlockId, char *newData)
{
    FileInfo *fTemp = get_file_info("DBNAME", tableName, 0);
    if (fTemp == NULL)
    {
        return 0;
    }
    BlockInfo *bTemp = fTemp->first;
    while (bTemp != NULL)
    {
        if (bTemp->blockNum == BlockId)
        {
            bTemp->charNum = strlen(newData);
            strncpy(bTemp->cBlock, newData, bTemp->charNum);
            return 1;
        }
    }
    return 0;
}

int BufferManager::createDB(string DBName)
{
    DB = (FileInfo *)malloc(sizeof(struct fileInfo));
    DB->fileName = "DBNAME";
    DB->next == NULL;
    return 1;
}

int BufferManager::createNewTable(string tableName)
{
    FileInfo *fTemp = (FileInfo *)malloc(sizeof(struct fileInfo));
    DB->next = fTemp;
    fTemp->col = 0;
    fTemp->row = 0;
    fTemp->first = NULL;
    fTemp->type = 0;
    return 1;
}

//返回一个数据库的起始地址
FileInfo *findFileInfo(string DB_Name)
{
    if (DB == NULL) //数据库的地址
        return NULL;
    return DB->next;
}

FileInfo *get_file_info(const string DB_Name, const string fileName, const int m_fileType)
{
    FileInfo *file_temp = findFileInfo(DB_Name);
    while (NULL != file_temp) //遍历所有的file节点
    {
        if (file_temp->fileName == fileName && file_temp->type == m_fileType)
        {
            return file_temp;
        }
        file_temp = file_temp->next;
    }
    //没有找到这个文件
    cout << "buffer::get_file_info::can't find the file" << endl;
    return NULL;
}

//在内存中查找这个数据块
BlockInfo *get_block_inbuffer(const FileInfo *file_temp, const int blockNum)
{
    if (NULL == file_temp)
        return NULL;
    blockInfo *block_temp = file_temp->first;
    //遍历该file节点下所有block
    while (NULL != block_temp)
    {
        if (block_temp->blockNum == blockNum)
        {
            return block_temp;
        }
        block_temp = block_temp->next;
    }
    return NULL;
}

//将块从文件节点下移除
BlockInfo *remove_block_inbuffer(BlockInfo *m_blockInfo)
{
    FileInfo *file_temp = m_blockInfo->file;
    int blockNum = m_blockInfo->blockNum;
    BlockInfo *block_last = NULL;
    BlockInfo *block_temp = file_temp->first;
    while (NULL != block_temp)
    {
        if (block_temp == m_blockInfo)
        {
            //find the node wanted
            break;
        }
        block_last = block_temp;
        block_temp = block_temp->next;
    }
    if (NULL != block_temp)
    {
        if (NULL == block_last)
        {
            file_temp->first = block_temp->next; //头节点
        }
        else
        {
            block_last->next = block_temp->next; //中间节点&叶节点
        }
        //返回前一个节点
        return block_last;
    }
    //没找着
    return NULL;
}

//返回可用的块
BlockInfo *findBlock(string DB_Name)
{
    static int i = 0; //记录现有block数字
                      //搜索空块
    FileInfo *ptr = findFileInfo(DB_Name);
    BlockInfo *newblock;
    while (NULL != ptr)
    {
        int freeBlock = ptr->freeNum;
        if (freeBlock > 0)
        {
            blockInfo *block_temp = get_block_inbuffer(ptr, freeBlock);
            if (NULL != block_temp)
            {
                blockInfo *block_last = remove_block_inbuffer(block_temp); //从链表中移除
                                                                           //更新freeNum
                string freeNum_rec(block_temp->cBlock);
                ptr->freeNum = stoi(freeNum_rec.substr(0, 3));
                return block_temp;
            }
            //file记录了freeNUM 但是找不到这个block
            cout << "WANING  buffer::findBlock:the freeNum in record may be wrong" << endl;
        }
        ptr = ptr->next;
    }
    if (i < MAX_BLOCK)
    {
        //申请内存
        newblock = new BlockInfo;
        i++;
        if (NULL != newblock)
        {
            newblock->cBlock = new char[BLOCK_SIZE];
            if (NULL != newblock->cBlock)
                return newblock;
        }
    }
    //释放内存
    //哇这里写的MRU
    int time = -1;
    BlockInfo *temp = NULL;
    ptr = findFileInfo(DB_Name);
    while (NULL != ptr)
    {
        blockInfo *block_ptr = ptr->first;
        while (NULL != block_ptr)
        {
            if (block_ptr->iTime > time)
            {
                time = block_ptr->iTime;
                temp = block_ptr;
            }
            block_ptr = block_ptr->next;
        }
        ptr = ptr->next;
    }
    temp->iTime = 1;
    remove_block_inbuffer(temp);
    return temp;
}

//把block连到file下
void replace(FileInfo *m_fileInfo, BlockInfo *m_blockInfo)
{
    //加在最前面
    m_blockInfo->file = m_fileInfo;
    m_blockInfo->next = m_fileInfo->first;
    m_fileInfo->first = m_blockInfo;
}

//返回数据
BlockInfo *get_file_block(string DB_Name, string Table_Name, const int fileType, const int blockNum)
{
    BlockInfo *block_temp;
    FileInfo *file_temp = get_file_info(DB_Name, Table_Name, fileType);

    if (NULL != (block_temp = get_block_inbuffer(file_temp, fileType)))
    {
        if (1 == block_temp->lock) //判断这个块有没有被锁住
            cout << "buffer::the block has been locked" << endl;
        return block_temp;
    }
    if (NULL == file_temp) //找不到这个文件
    {
        cout << "buffer::get_file_block::wrong information" << endl;
        return NULL;
    }
    block_temp = file_temp->first;
    while (NULL != block_temp)
    {
        if (block_temp->blockNum == blockNum)
        {
            if (0 != block_temp->charNum) //如果不是空块
            {
                block_temp->iTime++; //调用次数++
                return block_temp;
            }
            else
            {
                block_temp = readBlock(file_temp, DB_Name, blockNum, block_temp);
                block_temp->iTime = 1; //调用次数初始化
                return block_temp;
            }
        }
        block_temp = block_temp->next;
    }
    cout << "buffer::get_file_block::can't find this block >>> begin read" << endl;
    block_temp = readBlock(file_temp, DB_Name, blockNum, NULL);
    replace(file_temp, block_temp); //将该block增加到头节点下
    block_temp->iTime = 1;          //调用次数初始化
    return block_temp;
}

//已经判断内存中不存在这一个块、 读取数据
BlockInfo *readBlock(FileInfo *file_temp, string DB_Name, const int m_blockNum, BlockInfo *block_temp)
{
    //如果没有提供块，则需要自己找一个块
    if (block_temp == NULL)
    {
        block_temp = findBlock(DB_Name);
        block_temp->blockNum = m_blockNum;
        block_temp->dirtyBit = 0;
    }
    //读取文件
    string fpath;
    if (0 == file_temp->type)
    {
        fpath = "..\\" + DB_Name + "\\table\\" + file_temp->fileName + ".txt";
    }
    else
    {
        fpath = "..\\" + DB_Name + "\\index\\" + file_temp->fileName + ".txt";
    }
    fstream file(fpath, ios::in | ios::out);
    if (!file.is_open()) //检查打开是否成功
    {
        cout << "buffer::readBlock::can't find the file" + fpath << endl;
        return NULL;
    }
    int offset = (m_blockNum - 1) * BLOCK_SIZE; //计算偏移量
    file.seekp(offset, ios::beg);               //从起始位置开始
    block_temp->cBlock = new char[BLOCK_SIZE];  //申请内存
    file.read(block_temp->cBlock, BLOCK_SIZE);  //读取文件
    file.close();
    return block_temp;
}

//把block指针所指向的块的内容写回磁盘。
void writeBlock(string DB_Name, BlockInfo *block)
{
    if (!block->dirtyBit)
        return;

    fileInfo *file_temp = block->file;
    string m_fileName = block->file->fileName;
    int m_blockNum = block->blockNum;
    string fpath;
    if (0 == file_temp->type)
    {
        fpath = "..\\" + DB_Name + "\\table\\" + m_fileName + ".txt";
    }
    else
    {
        fpath = "..\\" + DB_Name + "\\index\\" + m_fileName + ".txt";
    }
    fstream file(fpath, ios::in | ios::out);
    if (!file.is_open()) //检查打开是否成功
    {
        cout << "buffer::writeBlock::can't find the file" + fpath << endl;
        return;
    }
    int offset = (m_blockNum - 1) * BLOCK_SIZE;
    file.seekp(offset, ios::beg);
    string temp = block->cBlock;
    file.write(block->cBlock, temp.length());
    file.close();
}

//调用closeFile(DB_Name,filename,fileType,m_flag),逐个关闭文件。
void closeDatabase(string DB_Name, bool m_flag)
{
    //release Block_Info
    fileInfo *ptr = findFileInfo(DB_Name);
    while (NULL != ptr)
    {
        closeFile(DB_Name, ptr);
        ptr = ptr->next;
    }

    //release File_info
    ptr = findFileInfo(DB_Name);
    while (NULL != ptr)
    {
        fileInfo *ptr_next = ptr->next;
        delete ptr;
        ptr = ptr_next;
    }
}

//关闭文件
void closeFile(string DB_Name, FileInfo *file_temp)
{
    if (NULL == file_temp)
    {
        cout << "buffer::closeFile:can't find the file" << endl;
        return;
    }
    blockInfo *block_temp = file_temp->first;
    blockInfo *block_delete;

    //依次删除该file下的block
    while (NULL != block_temp)
    {
        writeBlock(DB_Name, block_temp);
        block_delete = block_temp;
        block_temp = block_temp->next; //指向下一个节点
        delete[] block_delete->cBlock; //释放内存
        delete block_delete;           //删除节点
    }
}
