// SharedMapArray.cpp : 定义 DLL 的导出函数。
//

#include "SharedMapArray.h"
#include "string.h"

using namespace std;

CSharedMapArray::CSharedMapArray()
{
#ifdef _WIN32	
	hFileMapping = nullptr;

	lpShareMemory = nullptr;
#else
	pSharedMemory = nullptr;
	OpenMmap();
#endif

}

CSharedMapArray::~CSharedMapArray()
{
#ifdef _WIN32
	//release share memory 

	if (NULL != lpShareMemory)
	{
		UnmapViewOfFile(lpShareMemory);
	}
	if (NULL != hFileMapping)
	{
		CloseHandle(hFileMapping);
	}

#else
	if (nullptr != pSharedMemory)
	{
		munmap(pSharedMemory, 4096);	// 释放
	}
#endif // WIN32

}

bool CSharedMapArray::Create()
{
	hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		SHM_SIZE,
		LPCTSTR(SHM_MARKET));

	if (NULL == hFileMapping)
	{
		std::cout << "CreateMap create file map fail:" << GetLastError() << std::endl;
		return false;
	}

	return Attach();
}

bool CSharedMapArray::Open()
{
	// Read
	hFileMapping = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, LPCTSTR(SHM_MARKET));
	if (NULL == hFileMapping)
	{
		std::cout << "Open create file map fail:" << GetLastError() << std::endl;
		return false;
	}

	return Attach();
}

bool CSharedMapArray::Attach()
{
	lpShareMemory = (char*)MapViewOfFile(hFileMapping,
		FILE_MAP_ALL_ACCESS,
		0,
		0,   //memory start address 
		0);      //all memory space 

	if (NULL == lpShareMemory)
	{
		std::cout << "MapViewOfFile" << GetLastError() << std::endl;
		return false;
	}

	shmArray = (SHM_ARRAY *)lpShareMemory;
	return true;
}
//int CSharedMapArray::OpenMmap()
//{
//
//#ifdef WIN32
//	//
//#else
//	int fd = open("mapping_file", O_RDWR | O_CREAT, 0664);
//	pSharedMemory = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
//	if(ptr == MAP_FAILED)
//	{
//		perror("mmap");
//		exit(1);
//	}
//#endif // WIN32
//	return 0;
//}


bool CSharedMapArray::Empty()
{
	return (shmArray->head == shmArray->tail);
}

bool CSharedMapArray::Full()
{
	return (((shmArray->tail + 1) % ARR_MAX_SIZE) == shmArray->head);
}

bool CSharedMapArray::Pop(std::string& sValue)
{
	sValue = shmArray->sMsg[shmArray->head];

	// move the head
	int iTemp = shmArray->head + 1;

	shmArray->head = (iTemp% ARR_MAX_SIZE);  //队头指针+1

	return true;
}

bool CSharedMapArray::Push(const std::string& sValue)
{
	if (shmArray == NULL)
		return false;

	int iMinSize = sValue.length();
	if (iMinSize > (ARR_BUFFER_LEN - 1))
	{
		iMinSize = ARR_BUFFER_LEN - 1;
	}

	memcpy(shmArray->sMsg[shmArray->tail], sValue.c_str(), iMinSize);
	shmArray->sMsg[shmArray->tail][iMinSize] = 0;

	// move the tail
	shmArray->tail = (shmArray->tail + 1) % ARR_MAX_SIZE;

	return true;
}

int CSharedMapArray::Lenth()
{
	if (shmArray->tail > shmArray->head)
		return shmArray->tail - shmArray->head;
	else
		ARR_MAX_LEN - (shmArray->head - shmArray->tail);
}
