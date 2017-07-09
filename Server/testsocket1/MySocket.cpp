#include "MySocket.h"

MySocket::MySocket() :
	hostname("localhost"),
	root("root"),
	passwd("930930"),
	port(3306),
	ip_addr("115.156.217.168")
{
}

MySocket::~MySocket()
{
	// ɾ��MySql����
	delete Sql_data;
	// �ر��׽���
	closesocket(servSock);
	// ��ֹ DLL ��ʹ��
	WSACleanup();
}

void MySocket::InitSocket() {
	//��ʼ�� DLL
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//�����׽���
	servSock = socket(AF_INET, SOCK_DGRAM, 0);
	nSize = sizeof(SOCKADDR);
	//���׽���
	memset(&sockAddr, 0, sizeof(sockAddr));		// ÿ���ֽڶ���0���
	sockAddr.sin_family = AF_INET;				// ʹ��IPv4��ַ
	sockAddr.sin_addr.S_un.S_addr = inet_addr(ip_addr.c_str());  //�����IP��ַ
	sockAddr.sin_port = htons(1234);			// �˿�
	bind(servSock, (SOCKADDR*)&sockAddr, nSize);

	Sql_data = new My_MySQL();
	Sql_data->InitMySql(hostname, port, root, passwd);

	stopsend["order_type"] = EXIT_DATA;
	stopsend_str = stopsend.toStyledString();
}

//��ͻ��˷�������
void MySocket::ListenClient() {
	char recvBuf[100];

	while (1) {
		datasend.clear();
		recvfrom(servSock, recvBuf, 100, 0, (SOCKADDR*)&sockAddr, &nSize);
		int order_list = -1;
		Json::Value val;
		Json::Reader reader;
		//std::cout << recvBuf;
		if (reader.parse(recvBuf, val)) {
			order_list = val["order_type"].asInt();
		}
		Sql_data->SetUserName(val["user"].asString());
		vector<string> res;
		if (order_list == QUERYDB_ORDER) {
			datasend["order_type"] = DATABASE_DATA;
			res = Sql_data->GetMysqlDataBase();
			for each (string var in res) {
				datasend["database"] = var;
				datasend_str = datasend.toStyledString();
				sendto(servSock, datasend_str.c_str(), datasend_str.length() + 1, \
					0, (SOCKADDR*)&sockAddr, nSize);
			}
		}
		else if (order_list == QUERYTB_ORDER) {
			datasend["order_type"] = TABLE_DATA;
			string dbname = val["db_name"].asString();
			Sql_data->SetDataBaseName(dbname);
			res = Sql_data->GetMysqlTable();
			for each (string var in res) {
				datasend["table"] = var;
				datasend_str = datasend.toStyledString();
				sendto(servSock, datasend_str.c_str(), datasend_str.length() + 1, \
					0, (SOCKADDR*)&sockAddr, nSize);
			}
		}
		else if (order_list == CREATEDB_ORDER) {
			datasend["order_type"] = RETURNDB_DATA;
			string dbname = val["db_name"].asString();
			Sql_data->SetDataBaseName(dbname);
			int flag = Sql_data->CreateDataBase();
			datasend["value"] = flag;
			datasend_str = datasend.toStyledString();
			sendto(servSock, datasend_str.c_str(), datasend_str.length() + 1, \
				0, (SOCKADDR*)&sockAddr, nSize);
		}
		else if (order_list == CREATETB_ORDER) {
			datasend["order_type"] = RETURNTB_DATA;
			string dbname = val["db_name"].asString();
			string tbname = val["tb_name"].asString();
			Sql_data->SetDataBaseName(dbname);
			Sql_data->SetTableName(tbname);
			int flag = Sql_data->CreateTable();
			datasend["value"] = flag;
			datasend_str = datasend.toStyledString();
			sendto(servSock, datasend_str.c_str(), datasend_str.length() + 1, \
				0, (SOCKADDR*)&sockAddr, nSize);
		}
		sendto(servSock, stopsend_str.c_str(), stopsend_str.length() + 1, \
			0, (SOCKADDR*)&sockAddr, nSize);
	}
	// ɾ�����ݿ����
	delete Sql_data;
}