#include "MySocket.h"

MySocket::MySocket() :
	hostname("localhost"),
	root("root"),
	passwd("930930"),
	port(3306),
	ip_addr("115.156.217.168")
	//ip_addr("10.11.73.61")
{
}

MySocket::~MySocket()
{
	// 删除MySql对象
	delete Sql_data;
	// 关闭套接字
	closesocket(servSock);
	// 终止 DLL 的使用
	WSACleanup();
}

void MySocket::InitSocket() {
	//初始化 DLL
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//创建套接字
	//servSock = socket(AF_INET, SOCK_DGRAM, 0);//基于UDP协议
	servSock = socket(AF_INET,SOCK_STREAM,0);	//基于TCP协议
	nSize = sizeof(SOCKADDR);
	//绑定套接字
	memset(&sockAddr, 0, sizeof(sockAddr));		// 每个字节都用0填充
	sockAddr.sin_family = AF_INET;				// 使用IPv4地址
	sockAddr.sin_addr.S_un.S_addr = inet_addr(ip_addr.c_str());  //具体的IP地址
	sockAddr.sin_port = htons(1235);			// 端口
	bind(servSock, (SOCKADDR*)&sockAddr, nSize);

	Sql_data = new My_MySQL();
	Sql_data->InitMySql(hostname, port, root, passwd);

	stopsend["order_type"] = EXIT_DATA;
	stopsend_str = stopsend.toStyledString();
}

//向客户端发送数据
void MySocket::ListenClient() {

	int len = sizeof(SOCKADDR);
	//将套接字设为监听模式，准备接受客户请求
	listen(servSock, 5);	//设置可接受连接请求的最大个数为5

	//创建一个SOCKADDR变量用来存储客户端的IP地址，端口信息
	SOCKADDR_IN clientAddr;

	/*先等待客户端发起连接请求
	listen（）函数不会发生阻塞，（即使接收不到链接请求也不会一直等待）故可以放在循环中
	accept返回一个新套接字用来与客户端通信（serverSocket-旧套接字仍用于监听客户端的链接请求）
	如果服务器没有接收到一个来自客户端的连接请求，就会陷入阻塞（一直在改句等待）
	*/
	SOCKET sockConnect = accept(servSock, (SOCKADDR*)&clientAddr, &len);

	//注意客户端与服务器连接上后，后面的发送接收都要使用accept（）返回的新套接字sockConnect
	int insertflag = -1;
	const int maxlen = 10000;
	const int maxbyte = 128 * 1024;
	/*Sql_data->SetUserName("zjh");
	Sql_data->SetDataBaseName("zjh");
	Sql_data->CreateDataBase();
	Sql_data->SetTableName("ruili");
	Sql_data->CreateTable();
	Sql_data->ExistTable();
	Sql_data->MyQuery("START TRANSACTION");
	for (int i = 0; i < 10000; ++i) {
		Sql_data->InsertTable("x", "y");
	}
	Sql_data->MyQuery("COMMIT");
	Sql_data->CloseMySql();
	*//**/
	while (1) {
		datasend.clear();
		char recvBuf[maxbyte] = { 0 };
		//基于UDP协议的接受函数
		//recvfrom(servSocket, recvBuf, maxbyte, 0, (SOCKADDR*)&sockAddr, &nSize);

		//基于TCP的接受函数
		recv(sockConnect, recvBuf, maxbyte, 0);
		//cout << recvBuf << endl;
		int order_list = -1;
		Json::Value val;
		Json::Reader reader;
		
		if (reader.parse(recvBuf, val)) {
			order_list = val["order_type"].asInt();
		}
		cout << val["user"].asCString();
		Sql_data->SetUserName(val["user"].toStyledString());
		vector<string> res;
		// 查询数据库
		if (order_list == QUERYDB_ORDER) {
			datasend["order_type"] = DATABASE_DATA;
			res = Sql_data->GetMysqlDataBase();
			for each (string var in res) {
				datasend["database"] = var;
				datasend_str = datasend.toStyledString();
				cout << datasend_str << endl;
				//基于UDP协议的发送函数
				//sendto(servSocket, datasend_str.c_str(), datasend_str.length() + 1, \
					0, (SOCKADDR*)&sockAddr, nSize);
				//基于TCP协议的发送函数
				send(sockConnect, datasend_str.c_str(), datasend_str.length() + 1, 0);

			}
		}
		// 查询表
		else if (order_list == QUERYTB_ORDER) {
			datasend["order_type"] = TABLE_DATA;
			string dbname = val["db_name"].asString();
			Sql_data->SetDataBaseName(dbname);
			res = Sql_data->GetMysqlTable();
			for each (string var in res) {
				datasend["table"] = var;
				datasend_str = datasend.toStyledString();
				//UDP
				//sendto(servSock, datasend_str.c_str(), datasend_str.length() + 1, \
					0, (SOCKADDR*)&sockAddr, nSize);
				//TCP
				send(sockConnect, datasend_str.c_str(), datasend_str.length() + 1, 0);
			}
		}
		// 创建数据库
		else if (order_list == CREATEDB_ORDER) {
			datasend["order_type"] = RETURNDB_DATA;
			string dbname = val["db_name"].asString();
			Sql_data->SetDataBaseName(dbname);
			int flag = Sql_data->CreateDataBase();
			datasend["value"] = flag;
			datasend_str = datasend.toStyledString();
			//UDP
			//sendto(servSock, datasend_str.c_str(), datasend_str.length() + 1, \
				0, (SOCKADDR*)&sockAddr, nSize);
			//TCP
			send(sockConnect, datasend_str.c_str(), datasend_str.length() + 1, 0);
		}
		// 创建表
		else if (order_list == CREATETB_ORDER) {
			datasend["order_type"] = RETURNTB_DATA;
			string dbname = val["db_name"].asString();
			string tbname = val["tb_name"].asString();
			cout << tbname << endl;
			Sql_data->SetDataBaseName(dbname);
			Sql_data->SetTableName(tbname);
			int flag = Sql_data->CreateTable();
			datasend["value"] = flag;
			datasend_str = datasend.toStyledString();
			//UDP
			//sendto(servSock, datasend_str.c_str(), datasend_str.length() + 1, \
				0, (SOCKADDR*)&sockAddr, nSize);
			//TCP
			send(sockConnect, datasend_str.c_str(), datasend_str.length() + 1, 0);
		}
		// 删除数据库
		else if (order_list == DELETEDB_ORDER) {
			datasend["order_type"] = DELETEDB_DATA;
			string dbname = val["db_name"].asString();
			Sql_data->SetDataBaseName(dbname);
			int flag = Sql_data->DeleteDataBase();
			datasend["value"] = flag;
			datasend_str = datasend.toStyledString();
			//UDP
			//sendto(servSock, datasend_str.c_str(), datasend_str.length() + 1, \
				0, (SOCKADDR*)&sockAddr, nSize);
			//TCP
			send(sockConnect, datasend_str.c_str(), datasend_str.length() + 1, 0);
		}
		// 删除表
		else if (order_list == DELETETB_ORDER) {
			datasend["order_type"] = DELETETB_DATA;
			string dbname = val["db_name"].asString();
			string tbname = val["tb_name"].asString();
			Sql_data->SetDataBaseName(dbname);
			Sql_data->SetTableName(tbname);
			int flag = Sql_data->DeleteTable();
			datasend["value"] = flag;
			datasend_str = datasend.toStyledString();
			//UDP
			//sendto(servSock, datasend_str.c_str(), datasend_str.length() + 1, \
				0, (SOCKADDR*)&sockAddr, nSize);
			//TCP
			send(sockConnect, datasend_str.c_str(), datasend_str.length() + 1, 0);
		}
		// 插入数据
		else if (order_list == INSERTTB_ORDER) {
			//cout << recvBuf << endl;
			datasend["order_type"] = INSERTTB_DATA;
			datasend_str = datasend.toStyledString();
			//UDP
			//sendto(servSock, datasend_str.c_str(), datasend_str.length() + 1, \
				0, (SOCKADDR*)&sockAddr, nSize);
			//TCP
			send(sockConnect, datasend_str.c_str(), datasend_str.length() + 1, 0);

			datasend.clear();
			string dbname = val["db_name"].asString();
			string tbname = val["tb_name"].asString();
			Sql_data->SetDataBaseName(dbname);
			Sql_data->SetTableName(tbname);
			int flag0 = INSERTTB_VALUE, flag = -1;
			if (false == Sql_data->ExistTable()) {
				flag0 = NEXISTB_VALUE;
			}
			Sql_data->MyQuery("START TRANSACTION");
			int i = 0;
			while (1) {
				memset(recvBuf, 0, maxbyte);
				order_list = -1;
				val.clear();
				//UDP
				//recvfrom(servSock, recvBuf, maxbyte, 0, (SOCKADDR*)&sockAddr, &nSize);
				//TCP
				recv(sockConnect,recvBuf,maxbyte,0);

				if (reader.parse(recvBuf, val)) {
					order_list = val["order_type"].asInt();
				}
				vector<string> vx, vy;
				cout << (++i) << "次" << order_list << endl;
				if (INSERTTB_DATA == order_list) {
					cout << "正在插入..." << endl;
					string x = val["x"].asString();
					string y = val["y"].asString();
					SplitString(x, vx, " ");
					SplitString(y, vy, " ");
					if (vx.empty() || vy.empty()) continue;
					cout << "len:" << vx.size() << endl;
					for (int i = 0; i < vx.size(); ++i) {
						if (vx[i].empty() || vy[i].empty()) continue;
						flag = Sql_data->InsertTable(vx[i], vy[i]);
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
				}
				else if (EXITINTB_DATA == order_list) {
					cout << "收到结束" << endl;
					Sql_data->MyQuery("COMMIT");
					Sql_data->CloseMySql();
					datasend["order_type"] = RINSRTTB_DATA;
					datasend["value"] = flag0;
					datasend_str = datasend.toStyledString();
					cout << "插入结束" << endl;
					//UDP
					//sendto(servSock, datasend_str.c_str(), datasend_str.length() + 1, \
						0, (SOCKADDR*)&sockAddr, nSize);
					//TCP
					send(sockConnect, datasend_str.c_str(), datasend_str.length() + 1, 0);
					break;
				}
			}
		}
		else if (order_list == QUERY_ORDER) {
			datasend["order_type"] = QUERY_DATA;
			int content = val["content"].asInt();
			string dbname = val["db_name"].asString();
			string tbname = val["tb_name"].asString();
			Sql_data->SetDataBaseName(dbname);
			Sql_data->SetTableName(tbname);
			int count = 0;
			int len = 0;
			string temp = "";
			if (TABLENAME_QUERY == content) {
				vector<string> res = Sql_data->QueryTableByName();
				len = res.size();
				for each (string var in res) {
					temp += var;
					count++;
					if (count % maxlen == 0) {
						datasend["table_name"] = temp;
						temp = "";
						datasend_str = datasend.toStyledString();
						//UDP
						//sendto(servSock, datasend_str.c_str(), datasend_str.length() + 1, \
							0, (SOCKADDR*)&sockAddr, nSize);

						//TCP
						send(sockConnect, datasend_str.c_str(), datasend_str.length() + 1, 0);
					}
					else if (count < len){
						temp += " ";
					}
				}
				if (count % maxlen) {
					datasend["table_name"] = temp;
					datasend_str = datasend.toStyledString();
					//cout << datasend_str << endl;
					//UDP
					//sendto(servSock, datasend_str.c_str(), datasend_str.length() + 1, \
						0, (SOCKADDR*)&sockAddr, nSize);
					//TCP
					send(sockConnect, datasend_str.c_str(), datasend_str.length() + 1, 0);
				}
			}
			else if (USERNAME_QUERY == content) {
				vector<string> res = Sql_data->QueryTableUserName();
				len = res.size();
				for each (string var in res) {
					temp += var;
					count++;
					if (count % maxlen == 0) {
						datasend["user_name"] = temp;
						temp = "";
						datasend_str = datasend.toStyledString();
						cout << datasend_str << endl;
						//UDP
						//sendto(servSock, datasend_str.c_str(), datasend_str.length() + 1, \
							0, (SOCKADDR*)&sockAddr, nSize);
						//TCP
						send(sockConnect, datasend_str.c_str(), datasend_str.length() + 1, 0);
					}
					else if (count < len) {
						temp += " ";
					}
				}
				if (count % maxlen) {
					datasend["user_name"] = temp;
					datasend_str = datasend.toStyledString();
					//cout << datasend_str << endl;
					//UDP
					//sendto(servSock, datasend_str.c_str(), datasend_str.length() + 1, \
						0, (SOCKADDR*)&sockAddr, nSize);
					//TCP
					send(sockConnect, datasend_str.c_str(), datasend_str.length() + 1, 0);
				}
			}
			else if (XYCONTENT_QUERY == content) {
				vector<pair<string, string>> res = Sql_data->QueryTableByCont();
				len = res.size();
				string tempx = "", tempy = "";
				for each (pair<string, string> var in res) {
					tempx += var.first;
					tempy += var.second;
					count++;
					if (count % maxlen == 0) {
						datasend["x"] = tempx;
						datasend["y"] = tempy;
						tempx = "";
						tempy = "";
						datasend_str = datasend.toStyledString();
						cout << datasend_str << endl;
						//UDP
						//sendto(servSock, datasend_str.c_str(), datasend_str.length() + 1, \
							0, (SOCKADDR*)&sockAddr, nSize);
						//TCP
						send(sockConnect, datasend_str.c_str(), datasend_str.length() + 1, 0);
					}
					else if (count < len)
						tempx += " ", tempy += " ";
				}
				if (count % maxlen) {
					datasend["x"] = tempx;
					datasend["y"] = tempy;
					datasend_str = datasend.toStyledString();
					//cout << datasend_str << endl;
					//UDP
					//sendto(servSock, datasend_str.c_str(), datasend_str.length() + 1, \
						0, (SOCKADDR*)&sockAddr, nSize);
					//TCP
					send(sockConnect, datasend_str.c_str(), datasend_str.length() + 1, 0);
				}
			}
			cout << "查询成功！" << endl;
		}
		//UCP
		//sendto(servSock, stopsend_str.c_str(), stopsend_str.length() + 1, \
			0, (SOCKADDR*)&sockAddr, nSize);
		//TCP
		send(sockConnect, datasend_str.c_str(), datasend_str.length() + 1, 0);
	}/**/
}

// 切割字符串
void SplitString(const string& s, vector<string>& v, const string& c)
{
	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}