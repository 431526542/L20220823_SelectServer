#include<iostream>
#include<WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main()
{
	WSAData WsaData;

	WSAStartup(MAKEWORD(2, 2), &WsaData);

	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN ServerSockAddr;
	memset(&ServerSockAddr, 0, sizeof(SOCKADDR_IN));
	ServerSockAddr.sin_family = PF_INET;
	ServerSockAddr.sin_addr.s_addr = INADDR_ANY;
	ServerSockAddr.sin_port = htons(12345);

	bind(ServerSocket, (SOCKADDR*)&ServerSockAddr, sizeof(SOCKADDR_IN));

	listen(ServerSocket, 0);

	fd_set Reads;
	fd_set CopyReads;

	TIMEVAL Timeout;
	Timeout.tv_sec = 0;
	Timeout.tv_usec = 0;

	FD_ZERO(&Reads); //�ʱ�ȭ �Լ�
	FD_ZERO(&CopyReads); //�ʱ�ȭ �Լ�

	FD_SET(ServerSocket, &Reads);//����, Reads������ ��Ĺ�Ѱ��� �ִ´�.

	while (1)
	{
		CopyReads = Reads;
		int ChangedNumber = select(0, &CopyReads, 0, 0, &Timeout);//����, , , ��ٸ��� �ð�

		if (ChangedNumber == 0)
		{
			//cout << "Process" << endl; //��û�� ���� ������ ����.
		}
		else
		{
			for (int i = 0; i < (int)Reads.fd_count; ++i)
			{
				if (FD_ISSET(Reads.fd_array[i], &CopyReads))
				{
					if (Reads.fd_array[i] == ServerSocket)
					{
						SOCKADDR_IN ClientSockAddr;
						memset(&ClientSockAddr, 0, sizeof(SOCKADDR_IN));
						int ClientSockAddrLength = sizeof(SOCKADDR_IN);

						int ClientSocket = accept(ServerSocket, (SOCKADDR*)&ClientSockAddr, &ClientSockAddrLength);
						FD_SET(ClientSocket, &Reads);
						cout << "connect client : " << Reads.fd_array[i] << endl;
					}
					else
					{
						//clientsocket
						char Buffer[1024] = { 0, };
						int RecvSize = recv(Reads.fd_array[i], Buffer, 1024, 0);

						if (RecvSize < 0)
						{
							//disconnect or error
							FD_CLR(Reads.fd_array[i], &Reads);
							closesocket(Reads.fd_array[i]);
							cout << "disconnect client : " << Reads.fd_array[i] << endl;
						}
						else
						{
							//������ ����
							cout << "client recv : " << Buffer << endl;
							for (int j = 0; j < (int)Reads.fd_count; ++j)
							{
								if (Reads.fd_array[j] != ServerSocket)
								{
									int sentSize = send(Reads.fd_array[j], Buffer, RecvSize, 0);
								}
							}
						}
					}
				}
			}	
		}
	}
	closesocket(ServerSocket);
	WSACleanup();
	return 0;
}