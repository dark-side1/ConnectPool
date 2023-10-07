#include "connectPool.h"
#include <fstream>
#include <json/json.h>
#include <thread>

using namespace Json;

connectPool* connectPool::getConnectPool()
{
	static connectPool pool;//static�̰߳�ȫ
	return &pool;
}

shared_ptr<mysqlConnect> connectPool::getConnection()
{
    unique_lock<mutex> locker(m_mutexQ);
    while (m_connectQ.empty()) {
        if (cv_status::timeout == m_cond.wait_for(locker, chrono::milliseconds(m_timeout))) {
            //����Ϊ�ձ��������ȴ���m_timeoutʱ������δ������ݿ����ӣ��ٴ��ж϶����Ƿ�Ϊ�գ���������
            if (m_connectQ.empty()) continue;
        }
    }

    //ȡ�����ݿ����ӣ����ù���ָ�룬����������ʱ�������ݿ����������ݿ����ӳض���
    //��д��������
    shared_ptr<mysqlConnect> connptr(m_connectQ.front(), [this](mysqlConnect* conn) {
        unique_lock<mutex> locker(m_mutexQ);
        conn->refreshAliveTime();
        m_connectQ.push(conn);
        });
    m_connectQ.pop();
    //�������̣߳�ȡ�����Ӻ�֪ͨ�������̼߳�������
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
        //�ж����ݿ����ӳ���������Ƿ���
        //�����ӳ��������С����С��������������
        while (m_connectQ.size() >= m_minSize) {//todo
            m_cond.wait(locker);
        }
        addConnection();
        //�������̣߳��������Ӻ�֪ͨ��������
        m_cond.notify_all();
    }
}

//�������̣߳����ճ�ʱ�߳�
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
    //���������ļ�
    if (!parseJsonFile())   return;

    for (int i = 0; i < m_minSize; i++) {
        addConnection();
    }
    thread prodecer(&connectPool::produceConnection, this);
    thread recycler(&connectPool::recycleConnection, this);
    prodecer.detach();
    recycler.detach();
}