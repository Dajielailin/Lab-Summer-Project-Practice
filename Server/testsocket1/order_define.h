#ifndef ORDER_DEFINE_H
#define ORDER_DEFINE_H

//  ����
#define QUERYDB_ORDER   1   //  ��ѯ���ݿ���
#define QUERYTB_ORDER   2   //  ��ѯ����
#define SIGNIN_ORDER    3   //  ��¼
#define VISIT_ORDER     4   //  ����
#define CREATEDB_ORDER	5   //  �½����ݿ�
#define CREATETB_ORDER	6   //  �½���
#define DELETEDB_ORDER	7   //  ɾ�����ݿ�
#define DELETETB_ORDER	8   //  ɾ����
#define INSERTTB_ORDER	9   //  ��������
#define STOPINTB_ORDER	10  //  ֹͣ����
#define QUERY_ORDER		11  //  ��ѯ

//  ������������
#define DATABASE_DATA   1   //  ���ݿ����б�
#define TABLE_DATA      2   //  �����б�

#define RETURNTB_DATA	3   //  �½����ݿⷵ��
#define RETURNDB_DATA	4   //  �½�����
#define DELETEDB_DATA	5   //  ɾ�����ݿⷵ��
#define DELETETB_DATA	6   //  ɾ������
#define INSERTTB_DATA	7   //  �������ݱ��
#define RINSRTTB_DATA   8   //  �������ݷ���
#define EXITINTB_DATA	9   //  ����������
#define QUERY_DATA		10  //  ��ѯ����

#define EXIT_DATA       20  //  ��������

//  ����ֵ
#define EXISTDB_VALUE	0   //  ���ݿ��Ѵ���
#define FAILCDB_VALUE	1   //  �������ݿ�ʧ��
#define SUCSCDB_VALUE	2   //  �������ݿ�ɹ�
#define EXISTTB_VALUE	0   //  ���Ѵ���
#define FAILCTB_VALUE	1   //  ������ʧ��
#define SUCSCTB_VALUE	2   //  ������ɹ�
#define NEXISDB_VALUE	0   //  ���ݿⲻ����
#define FAILDDB_VALUE	1   //  ɾ�����ݿ�ʧ��
#define SUCSDDB_VALUE	2   //  ɾ�����ݿ�ɹ�
#define NEXISTB_VALUE	0   //  ������
#define FAILDTB_VALUE	1   //  ɾ����ʧ��
#define SUCSDTB_VALUE	2   //  ɾ����ɹ�
#define INSERTTB_VALUE	1   //  ���������ݳɹ�
#define FINSRTTB_VALUE	2   //  ����������ʧ��
#define SUCSQTB_VALUE	1   //  ��ѯ�ɹ�
#define FAILQTB_VALUE	2   //  ��ѯʧ��

#endif // ORDER_DEFINE_H
