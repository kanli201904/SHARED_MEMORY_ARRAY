// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 SHAREDMAPARRAY_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// SHAREDMAPARRAY_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef SHAREDMAPARRAY_EXPORTS
#define SHAREDMAPARRAY_API __declspec(dllexport)
#else
#define SHAREDMAPARRAY_API __declspec(dllimport)
#endif

#include <string>
#include <iostream>

#ifdef WIN32
#include "windows.h"
#endif

#define  SHM_SIZE  100001 * 1024
#define  ARR_MAX_LEN  100000
#define  ARR_MAX_SIZE ARR_MAX_LEN+1
#define  ARR_BUFFER_LEN 1024
#define  SHM_MARKET   "shm_market"

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef struct Shm_Array {
	int head;
	int tail;
	char sMsg[][1024];
	Shm_Array()
	{
		head = tail = 0;
		// 初始化二维数组
		sMsg[0][0] = 0;
	}
} SHM_ARRAY;

enum MarketDataType
{
	MSG_STATISTICS = 0, //统计数据
	MSG_HIGH_LOW = 1, //高低数据
	MSG_ORDER_BOOK1 = 2, //盘口1
	MSG_ORDER_BOOK5 = 3, //盘口2-5
	MSG_ORDER_BOOK10 = 4, //盘口6-10
	MSG_FILLTICK = 5, //逐笔
	MSG_OPEN_INTEREST = 6, //持仓量
	MSG_TRADING_STATUS = 7, //状态
	MSG_BROKER_LIST = 8, //经济商列表
	MSG_NOMINAL_PRICE = 9, //按盘价
	MSG_STUTAS_DATA = 10 //状态数据
};

typedef struct msg_data_t
{
	uint32 message_len;			//消息长度
	MarketDataType  data_type;  //消息类型MarketDataType
	uint8 message_data[0];		//消息体
} MSG_DATA_T;

// 此类是从 dll 导出的
class SHAREDMAPARRAY_API CSharedMapArray {
public:
	virtual ~CSharedMapArray();
	CSharedMapArray();

	bool Create();
	bool Open();
	bool Empty();
	bool Full();
	bool Pop(std::string& sValue);
	bool Push(const std::string& sValue);
	int  Lenth();

private:
	SHM_ARRAY *shmArray = NULL;

#ifdef WIN32

	HANDLE hFileMapping;
	LPVOID lpShareMemory;
#else
	// mmap
	void *      pSharedMemory;
#endif

};

