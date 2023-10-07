#pragma once
#include<queue>
#include<iostream>
#include<mutex>
#include <condition_variable>
#include"mysqlConnect.h"

using namespace std;

class connectPool
{
public:
	static connectPool* getConnectPool();//����ģʽ�����õ�ʱ����
	connectPool(const connectPool& obj) = delete;
	connectPool& operator=(const connectPool& obj) = delete;
	shared_ptr<mysqlConnect> getConnection();

private:
	connectPool();
	bool parseJsonFile();
	void addConnection();
	void produceConnection();
	void recycleConnection();
	~connectPool();

	string m_ip;
	string m_user;
	string m_passwd;
	string m_dbName;
	unsigned short m_port;

	//���ӳ��У�����������Χ
	int m_minSize;
	int m_maxSize;//�Ѿ�ͨ������ʱ�������ӽ��л����ˣ�����Ӧ�Զ�д����Ƶ���仯�������о�����Ҫ�����������
	//���Ҫ�ж��������������Ҫ��¼��ǰ��������Ҫ������֤�̰߳�ȫ��
	
	int m_timeout;		//��ʱʱ��
	int m_maxIdleTime;	//������ʱ��

	queue<mysqlConnect*> m_connectQ;//���д�����ݿ�����
	mutex m_mutexQ;//�������ӳ���ס
	condition_variable m_cond;//�����������ߣ��������ӳ����Ӹ���
};

