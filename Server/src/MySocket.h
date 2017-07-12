#pragma once

#include <winsock2.h>
#include "My_MySQL.h"
#include "order_define.h"
#include "json/json.h"

#pragma comment (lib, "ws2_32.lib")  //���� ws2_32.dll

class MySocket
{
public:
	MySocket();
	~MySocket();

public:
	void InitSocket();
	void ListenClient();

private:
	// ����������Ҫ�ı���
	WSADATA wsaData;		// ���ڳ�ʼ��DLL
	SOCKET servSock;		// ���������׽���
	sockaddr_in sockAddr;	// �󶨵ĵ�ַ
	int nSize;
	string ip_addr;

	// ����mysql��Ҫ�ı���
	My_MySQL *Sql_data;
	string hostname;		// ����
	string root;			// �û���
	string passwd;			// ����
	int port;				// �˿�

	// ���ʹؼ���
	Json::Value stopsend;	// �������
	Json::Value datasend;	// ��������
	string stopsend_str;
	string datasend_str;
};

