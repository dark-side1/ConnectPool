#include "connectPool.h"
#include <fstream>
#include <json/json.h>
#include <thread>

using namespace Json;

connectPool* connectPool::getConnectPool()
{
	static connectPool pool;//static线程安全
	return &pool;
}

shared_ptr<mysqlConnect> connectPool::getConnection()
{
    unique_lock<mutex> locker(m_mutexQ);
    while (m_connectQ.empty()) {
        if (cv_status::timeout == m_cond.wait_for(locker, chrono::milliseconds(m_timeout))) {
            //队列为空被阻塞，等待了m_timeout时长后仍未获得数据库链接，再次判断队列是否为空，继续阻塞
            if (m_connectQ.empty()) continue;
        }
    }

    //取出数据库链接，利用共享指针，在析构函数时回收数据库链接至数据库连接池队列
    //重写析构函数
    shared_ptr<mysqlConnect> connptr(m_connectQ.front(), [this](mysqlConnect* conn) {
        unique_lock<mutex> locker(m_mutexQ);
        conn->refreshAliveTime();
        m_connectQ.push(conn);
        });
    m_connectQ.pop();
    //消费者线程，取出链接后通知生产者线程继续生产
    m_cond.notify_all();
    return connptr;
}

bool connectPool::parseJsonFile()
{
    ifstream ifs("dbconf.json");
    Reader rd;
    Value root;
    rd.parse(ifs, root);
    if (root.isObject()) {
        m_ip = root["ip"].asString();
        m_port = root["port"].asInt();
        m_user = root["userName"].asString();
        m_passwd = root["password"].asString();
        m_dbName = root["dbName"].asString();
        m_minSize = root["minSize"].asInt();
        m_maxSize = root["maxSize"].asInt();
        m_maxIdleTime = root["maxIdTime"].asInt();
        m_timeout = root["timeout"].asInt();
        return true;
    }
    return false;
}

void connectPool::addConnection()
{
    mysqlConnect* conn = new mysqlConnect;
    conn->connect(m_user, m_passwd, m_dbName, m_ip, m_port);
    conn->refreshAliveTime();
    m_connectQ.push(conn);
}

void connectPool::produceConnection()
{
    while (true)
    {
        unique_lock<mutex> locker(m_mutexQ);
        //判断数据库连接池里的连接是否够用
        //当连接池里的连接小于最小连接数，不够用
        while (m_connectQ.size() >= m_minSize) {//todo
            m_cond.wait(locker);
        }
        addConnection();
        //生产者线程，生产链接后通知阻塞请求
        m_cond.notify_all();
    }
}

//管理者线程，回收超时线程
void connectPool::recycleConnection()
{
    while (true)
    {
        this_thread::sleep_for(chrono::milliseconds(500));
        unique_lock<mutex> locker(m_mutexQ);
        while (m_connectQ.size() > m_minSize) {
            mysqlConnect* conn = m_connectQ.front();
            if (conn->getAliveTime() >= m_maxIdleTime) {
                m_connectQ.pop();
                delete conn;
            }
            else break;
        }
    }
}

connectPool::~connectPool()
{
    while (!m_connectQ.empty()) {
        mysqlConnect* conn = m_connectQ.front();
        m_connectQ.pop();;
        delete conn;
    }
}

connectPool::connectPool() {
    //加载配置文件
    if (!parseJsonFile())   return;

    for (int i = 0; i < m_minSize; i++) {
        addConnection();
    }
    thread prodecer(&connectPool::produceConnection, this);
    thread recycler(&connectPool::recycleConnection, this);
    prodecer.detach();
    recycler.detach();
}