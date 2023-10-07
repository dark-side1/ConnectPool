#pragma once
#include<iostream>
#include<string>
#include<mysql.h>
#include<chrono>

using namespace std;
using namespace chrono;

class mysqlConnect
{
public:
	//��ʼ�����ݿ�����
	mysqlConnect();
	//�ͷ����ݿ�����
	~mysqlConnect();
    //�������ݿ�
    bool connect(string user, string passwd, string dbName, string ip, unsigned short port = 3306);
    //�������ݿ�����
    bool update(string sql);
    //��ѯ���ݿ�����
    bool query(string sql);
    //������ѯ�õ��Ľ����
    bool next();
    //�õ�������е��ֶ�ֵ
    string value(int index);
    //�������
    bool transaction();
    //�ύ����
    bool commit();
    //����ع�
    bool rollback();
    //ˢ����ʼ�Ŀ���ʱ���
    void refreshAliveTime();
    //�������Ӵ�����ʱ��
    long long getAliveTime();
private:
    MYSQL* m_conn = nullptr;//���ݿ����
    MYSQL_RES* m_result = nullptr;//���ݿ�����
    MYSQL_ROW m_row = nullptr;//�ṹΪMYSQL_ROW����һ�н��
    void freeResult();//�ͷŽ����
    steady_clock::time_point m_alivetime;
};

