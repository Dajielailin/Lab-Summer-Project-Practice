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
	char recvBuf[MAXBYTE] = { 0 };
	int insertflag = -1;

	while (1) {
		datasend.clear();
		recvfrom(servSock, recvBuf, MAXBYTE, 0, (SOCKADDR*)&sockAddr, &nSize);
		int order_list = -1;
		Json::Value val;
		Json::Reader reader;
		if (reader.parse(recvBuf, val)) {
			order_list = val["order_type"].asInt();
		}
		Sql_data->SetUserName(val["user"].asString());
		vector<string> res;
		// ��ѯ���ݿ�
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
		// ��ѯ��
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
		// �������ݿ�
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
		// ������
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
		// ɾ�����ݿ�
		else if (order_list == DELETEDB_ORDER) {
			datasend["order_type"] = DELETEDB_DATA;
			string dbname = val["db_name"].asString();
			Sql_data->SetDataBaseName(dbname);
			int flag = Sql_data->DeleteDataBase();
			datasend["value"] = flag;
			datasend_str = datasend.toStyledString();
			sendto(servSock, datasend_str.c_str(), datasend_str.length() + 1, \
				0, (SOCKADDR*)&sockAddr, nSize);
		}
		// ɾ����
		else if (order_list == DELETETB_ORDER) {
			datasend["order_type"] = DELETETB_DATA;
			string dbname = val["db_name"].asString();
			string tbname = val["tb_name"].asString();
			Sql_data->SetDataBaseName(dbname);
			Sql_data->SetTableName(tbname);
			int flag = Sql_data->DeleteTable();
			datasend["value"] = flag;
			datasend_str = datasend.toStyledString();
			sendto(servSock, datasend_str.c_str(), datasend_str.length() + 1, \
				0, (SOCKADDR*)&sockAddr, nSize);
		}
		// ��������
		else if (order_list == INSERTTB_ORDER) {
			//cout << recvBuf << endl;
			datasend["order_type"] = INSERTTB_DATA;
			datasend_str = datasend.toStyledString();
			sendto(servSock, datasend_str.c_str(), datasend_str.length() + 1, \
				0, (SOCKADDR*)&sockAddr, nSize);

			datasend.clear();
			int flag0 = INSERTTB_VALUE, flag = -1;
			datasend["order_type"] = RINSRTTB_DATA;
			if (false == Sql_data->ExistTable())
				flag0 = NEXISTB_VALUE;
			while (1) {
				recvfrom(servSock, recvBuf, MAXBYTE, 0, (SOCKADDR*)&sockAddr, &nSize);
				if (reader.parse(recvBuf, val)) {
					order_list = val["order_type"].asInt();
				}
				cout << recvBuf << endl;
				if (INSERTTB_DATA == order_list) {
					string x = val["x"].asString();
					string y = val["y"].asString();
					string dbname = val["db_name"].asString();
					string tbname = val["tb_name"].asString();
					Sql_data->SetDataBaseName(dbname);
					Sql_data->SetTableName(tbname);
					flag = Sql_data->InsertTable(x, y);
					if (flag == NEXISTB_VALUE) {
						flag0 = flag;
						continue;
					}
					else if (flag == FINSRTTB_VALUE) {
						if (flag0 != NEXISTB_VALUE)
							flag0 = flag;
						continue;
					}
				}
				else if (EXITINTB_DATA == order_list) {
					datasend["value"] = flag0;
					datasend_str = datasend.toStyledString();
					cout << datasend_str << endl;
					sendto(servSock, datasend_str.c_str(), datasend_str.length() + 1, \
						0, (SOCKADDR*)&sockAddr, nSize);
					break;
				}
			}
		}
		sendto(servSock, stopsend_str.c_str(), stopsend_str.length() + 1, \
			0, (SOCKADDR*)&sockAddr, nSize);
	}
}