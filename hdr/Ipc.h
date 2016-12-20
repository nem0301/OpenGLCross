#pragma once
#include <winsock2.h>

#define	IPC_MAX_SIZE 4096

#define IPC_PORT 		19000

//car
#define PositionRept_CA	0x1201

//acm
#define SensorRes_AC	0x2101

typedef struct {
	uint16_t hdr;
	int len;
	unsigned char data[0];
} ipc_t;

typedef struct {
	float x;
	float y;
} PositionRept;

typedef struct {
	float x;
	float y;
} SensorRes;

class Ipc
{
	WSADATA wsaData;
	int recvSock;
	int sendSock;
	struct sockaddr_in serv;
	struct sockaddr_in from;
	struct sockaddr_in client;
public:
	Ipc(const char* addr, unsigned short port)
	{
		// socket
		if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			printf("wsaData error\n");
		}

		recvSock = socket(AF_INET,SOCK_DGRAM,0);
		sendSock = socket(AF_INET,SOCK_DGRAM,0);
		if( recvSock == -1 || sendSock == -1 )
		{
			printf( "socket fail %d, %d\n", recvSock, sendSock);
		}

		memset(&serv, 0, sizeof(serv));
		serv.sin_family = AF_INET;
		serv.sin_port = htons(IPC_PORT);
		serv.sin_addr.s_addr = inet_addr(addr);

		memset(&from, 0, sizeof(from));
		from.sin_family = AF_INET;
		from.sin_port = htons(IPC_PORT);
		from.sin_addr.s_addr = htonl(INADDR_ANY);


		bind ( recvSock, (struct sockaddr*)&from, sizeof(from));
	}

	void sendIpc(uint16_t sigId, void *data, int len)
	{
		unsigned char ipc_msg[IPC_MAX_SIZE];
		ipc_t *ipc = (ipc_t *) ipc_msg;
		ipc->len = len;
		ipc->hdr = sigId;
		memcpy(ipc->data, (void*)data, len);

		sendto(sendSock,(const char*)ipc_msg,sizeof(ipc_t) + len, 0,
				(struct sockaddr *)&serv, IPC_MAX_SIZE);
	}

	void recvIpc()
	{
        unsigned char ipc_msg[IPC_MAX_SIZE];
	    ipc_t *ipc = (ipc_t*)ipc_msg;
		int l = sizeof(client);
		int rc = recvfrom(recvSock,(char*)ipc_msg,sizeof(ipc_t), 0,
				(struct sockaddr *)&client, &l);
	}

};
