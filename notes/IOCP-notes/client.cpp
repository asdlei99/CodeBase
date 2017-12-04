#include <winsock2.h>  
#include <Windows.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")      // Socket������õĶ�̬���ӿ�  

#define DEFAULTPORT 6000
#define DEFAULTSIZE 1024 

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

	// ����socket������������ʽ�׽��֣������׽��ֺ�sockClient  
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);  
	if(sockClient == INVALID_SOCKET)
	{   
		printf("Error at socket():%ld\n", WSAGetLastError());   
		WSACleanup();   
		return -1;
	}   

	// ���׽���sockClient��Զ����������  
	// int connect( SOCKET s,  const struct sockaddr* name,  int namelen);  
	// ��һ����������Ҫ�������Ӳ������׽���  
	// �ڶ����������趨����Ҫ���ӵĵ�ַ��Ϣ  
	// ��������������ַ�ĳ���  
	SOCKADDR_IN addrSrv;  
	addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");      // ���ػ�·��ַ��127.0.0.1;   
	addrSrv.sin_family = AF_INET;  
	addrSrv.sin_port = htons(DEFAULTPORT);  
	while(SOCKET_ERROR == connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)))
	{  
		// �����û�����Ϸ�������Ҫ������  
		printf("����������ʧ�ܣ��Ƿ��������ӣ���Y/N):");  
		char choice;  
		while((choice = getchar()) && (!((choice != 'Y' && choice == 'N') || (choice == 'Y' && choice != 'N'))))
		{  
			printf("�����������������:");
		}  
		if (choice == 'Y'){  
			continue;  
		}  
		else
		{  
			printf("�˳�ϵͳ��...\n");  
			system("pause");  
			return 0;  
		}  
	}   
	printf("�ͻ�����׼��������ֱ�����������������������Ϣ��\n");  

	char buf[DEFAULTSIZE];
	int iRes = 0;
	while(true)
	{  
		gets(buf);
		if(buf[0] == 'q')
		{  
			break;  
		}  
		else
		{  
			printf("I Say:(\"quit\"to exit):%s\n",buf);  
			iRes = send(sockClient, buf, strlen(buf)+1, 0); // ������Ϣ
			memset(buf, 0, DEFAULTSIZE);
			iRes = recv(sockClient, buf, DEFAULTSIZE, 0);
			printf("Server Say:%s\n",buf);
		}  
	}  

	closesocket(sockClient);  
	WSACleanup();   // ��ֹ���׽��ֿ��ʹ��  
	printf("End linking...\n");   
	system("pause");  
	return 0;
}