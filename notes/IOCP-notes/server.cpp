#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")//Socket������õĶ�̬���ӿ�

#define DEFAULTPORT 6000
#define DEFAULTSIZE 1024

/** 
 * �ṹ�����ƣ�PER_IO_DATA 
 * �ṹ�幦�ܣ��ص�I/O��Ҫ�õ��Ľṹ�壬��ʱ��¼IO����
 **/  
typedef struct  
{  
    OVERLAPPED overlapped;
    char buffer[DEFAULTSIZE];  
    int BufferLen;  
    bool readflag;  
}PER_IO_DATA, *LPPER_IO_DATA;  
  
/** 
 * �ṹ�����ƣ�PER_HANDLE_DATA 
 * �ṹ��洢����¼�����׽��ֵ����ݣ��������׽��ֵı������׽��ֵĶ�Ӧ�Ŀͻ��˵ĵ�ַ�� 
 * �ṹ�����ã��������������Ͽͻ���ʱ����Ϣ�洢���ýṹ���У�֪���ͻ��˵ĵ�ַ�Ա��ڻطá� 
 **/  
typedef struct  
{  
    SOCKET socket;  
    SOCKADDR_IN ClientAddr;  
}PER_SOCKET_DATA, *LPPER_SOCKET_DATA; 

DWORD WINAPI ServerWorkThread(LPVOID CompletionPortID)
{
	HANDLE CompletionPort = (HANDLE)CompletionPortID;  
	DWORD BytesTransferred; 
	LPPER_SOCKET_DATA PerSocketData = NULL;  
	LPPER_IO_DATA PerIoData = NULL;  
	DWORD RecvBytes;  
	DWORD Flags = 0;  
	BOOL bRet = false;
	while(true)
	{   
		bRet = GetQueuedCompletionStatus(CompletionPort, &BytesTransferred, (PULONG_PTR)&PerSocketData, (LPOVERLAPPED*)&PerIoData, INFINITE);  
		if(bRet == FALSE)
		{  
			printf("GetQueuedCompletionStatus Error:%d\n", GetLastError());
			if(PerSocketData != NULL)
			{
				if(PerSocketData->socket != NULL)
					closesocket(PerSocketData->socket);
				free(PerSocketData);
			}
			if(PerIoData != NULL)
				free(PerIoData);
			continue;
		}  
		//PerIoData = (LPPER_IO_DATA)CONTAINING_RECORD(IpOverlapped, PER_IO_DATA, overlapped);  

		//http://blog.csdn.net/mylovepan/article/details/8204126
		//http://bbs.csdn.net/topics/390719212
		//1.�رմ��ڻ���ֹͣ����windows���ͷž���ģ�socket�ᱻ�رղ���Է����ͶϿ�����
		//2.�Է��ر�socket��WSARecv������GetQueuedCompletionStatus����ֵ���棬�����յ����ֽ���BytesTransferred��0�������жϡ�
		//GetQueuedCompletionStatus����TRUE���Ҷ�ȡ�������ݳ���Ϊ0ʱ,�ر��׽���
		if(BytesTransferred == 0)
		{  
			printf("BytesTransferred==0:%d\n", GetLastError());
			if(PerSocketData != NULL)
			{
				if(PerSocketData->socket != NULL)
					closesocket(PerSocketData->socket);
				free(PerSocketData);
			}
			if(PerIoData != NULL)
				free(PerIoData);
			continue;
		}  

		int iRes = 0;
		Flags = 0;
		if(PerIoData->readflag == true)
		{
		// ��ʼ���ݴ����������Կͻ��˵�����
		printf("%s says: %s\n", inet_ntoa(PerSocketData->ClientAddr.sin_addr), PerIoData->buffer);
		//send(PerSocketData->socket, PerIoData->buffer, strlen(PerIoData->buffer)+1, 0);
		memset(&PerIoData->overlapped, 0, sizeof(PerIoData->overlapped));
		PerIoData->readflag = false;
		WSABUF wsabuf;
		wsabuf.buf = PerIoData->buffer;
		wsabuf.len = sizeof(PerIoData->buffer);
		iRes = WSASend(PerSocketData->socket, &wsabuf, 1, &RecvBytes, Flags, &PerIoData->overlapped, NULL);
		}
		else
		{
		//Ϊ��һ���ص����ý�����I/O��������
		memset(PerIoData, 0, sizeof(PER_IO_DATA)); // ����ڴ�
		PerIoData->readflag = true;
		WSABUF wsabuf;
		wsabuf.buf = PerIoData->buffer;
		wsabuf.len = sizeof(PerIoData->buffer);
		iRes = WSARecv(PerSocketData->socket, &wsabuf, 1, &RecvBytes, &Flags, &PerIoData->overlapped, NULL);
		}
	}

	return 0;
}

DWORD WINAPI ServerSendThread(LPVOID IpParam);  

int main(int argc, char* argv[])
{
	//����socket��̬���ӿ�
	WORD wVersionRequested = MAKEWORD(2, 2); //����2.2�汾��WinSock��  
	WSADATA wsaData;    //����Windows Socket�Ľṹ��Ϣ
	DWORD err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)//����׽��ֿ��Ƿ�����ɹ�
	{  
		printf("Request Windows Socket Library Error!\n");  
		system("pause");
		return -1;
	}  
	if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)//����Ƿ�����������汾���׽��ֿ�
	{
		WSACleanup();  
		printf("Request Windows Socket Version 2.2 Error!\n");  
		system("pause");  
		return -1;  
	}

	//����IOCP���ں˶���  
    /** 
     * ��Ҫ�õ��ĺ�����ԭ�ͣ� 
     * HANDLE WINAPI CreateIoCompletionPort( 
     *    __in   HANDLE FileHandle,     // �Ѿ��򿪵��ļ�������߿վ����һ���ǿͻ��˵ľ�� 
     *    __in   HANDLE ExistingCompletionPort, // �Ѿ����ڵ�IOCP��� 
     *    __in   ULONG_PTR CompletionKey,   // ��ɼ���������ָ��I/O��ɰ���ָ���ļ�
     *    __in   DWORD NumberOfConcurrentThreads // ��������ͬʱִ������߳�����һ���ƽ���CPU������*2 
     * ); 
     **/  
    HANDLE completionPort = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0);  
    if (completionPort == NULL)//����IO�ں˶���ʧ��  
	{    
        printf("CreateIoCompletionPort failed. Error:%d\n", GetLastError());  
        system("pause");  
        return -1;  
    }

	// ����IOCP�߳�--�߳����洴���̳߳�  
	// ȷ���������ĺ�������  
	SYSTEM_INFO mySysInfo;  
	GetSystemInfo(&mySysInfo);  
	// ���ڴ������ĺ������������߳�  
	for(DWORD i = 0; i < (mySysInfo.dwNumberOfProcessors * 2); ++i)
	{  
		//�����������������̣߳�������ɶ˿ڴ��ݵ����߳�  
		HANDLE ThreadHandle = CreateThread(NULL, 0, ServerWorkThread, completionPort, 0, NULL);  
		if(ThreadHandle == NULL){  
			printf("Create Thread Handle failed. Error:%d\n", GetLastError());  
			system("pause");  
			return -1;  
		}  
		CloseHandle(ThreadHandle);  
	}

	// ������ʽ�׽���  
	SOCKET srvSocket = socket(AF_INET, SOCK_STREAM, 0);  
	// ��SOCKET������  
	SOCKADDR_IN srvAddr;  
	srvAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);  
	srvAddr.sin_family = AF_INET;  
	srvAddr.sin_port = htons(DEFAULTPORT);
	int bindResult = bind(srvSocket, (SOCKADDR*)&srvAddr, sizeof(SOCKADDR));  
	if(bindResult == SOCKET_ERROR)
	{  
		printf("Bind failed. Error:", GetLastError());  
		system("pause");  
		return -1;  
	}  
	// ��SOCKET����Ϊ����ģʽ  
	int listenResult = listen(srvSocket, 10);  
	if(listenResult == SOCKET_ERROR)
	{  
		printf("Listen failed. Error:%d\n", GetLastError());  
		system("pause");  
		return -1;  
	}

	//��ʼ����IO����  
	printf("��������׼�����������ڵȴ��ͻ��˵Ľ���...\n");
	while(true)
	{
		SOCKET acceptSocket;  
		//�������ӣ���������ɶˣ����������AcceptEx()  
		SOCKADDR_IN saRemote;  
		int RemoteLen = sizeof(saRemote);  
		acceptSocket = accept(srvSocket, (SOCKADDR*)&saRemote, &RemoteLen);  
		if(acceptSocket == SOCKET_ERROR)// ���տͻ���ʧ��  
		{   
			printf("Accept Socket Error:%d\n", GetLastError());  
			system("pause");  
			return -1;  
		}
		//�����������׽��ֹ����ĵ����������Ϣ�ṹ  
		LPPER_SOCKET_DATA PerSocketData = (LPPER_SOCKET_DATA)malloc(sizeof(PER_SOCKET_DATA));  //�ڶ���Ϊ���PerHandleData����ָ����С���ڴ�  
		PerSocketData->socket = acceptSocket;
		memcpy(&PerSocketData->ClientAddr, &saRemote, RemoteLen);
		// �������׽��ֺ���ɶ˿ڹ���  
		CreateIoCompletionPort((HANDLE)(PerSocketData->socket), completionPort, (DWORD)PerSocketData, 0); 

		// ��ʼ�ڽ����׽����ϴ���I/Oʹ���ص�I/O����  
		// ���½����׽�����Ͷ��һ�������첽  
		// WSARecv��WSASend������ЩI/O������ɺ󣬹������̻߳�ΪI/O�����ṩ����
		// ��I/O��������(I/O�ص�)
		LPPER_IO_DATA PerIoData = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));  
		memset(PerIoData, 0, sizeof(PER_IO_DATA));  
		PerIoData->readflag = true;    // read  

		DWORD RecvBytes;  
		DWORD Flags = 0;
		WSABUF wsabuf;
		wsabuf.buf = PerIoData->buffer;
		wsabuf.len = sizeof(PerIoData->buffer);
		WSARecv(PerSocketData->socket, &wsabuf, 1, &RecvBytes, &Flags, &PerIoData->overlapped, NULL);
	}

	system("pause");
	return 0;
}