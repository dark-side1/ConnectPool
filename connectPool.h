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
	static connectPool* getConnectPool();//懒汉模式——用到时创建
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

	//连接池中，可连接数范围
	int m_minSize;
	int m_maxSize;//已经通过空闲时长对连接进行回收了，不是应对读写需求频繁变化场景，感觉不需要设置最大上限
	//如果要判断最大连接数，还要记录当前连接数（要加锁保证线程安全）
	
	int m_timeout;		//超时时长
	int m_maxIdleTime;	//最大空闲时长

	queue<mysqlConnect*> m_connectQ;//队列存放数据库连接
	mutex m_mutexQ;//操作连接池锁住
	condition_variable m_cond;//生产者消费者，控制连接池连接个数
};

