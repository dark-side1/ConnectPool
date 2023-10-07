#include "mysqlConnect.h"

mysqlConnect::mysqlConnect()
{
    //mysql ������ʼ��
    m_conn = mysql_init(nullptr);
    //mysql �ַ������� utf8
    mysql_set_character_set(m_conn, "utf8");
}

mysqlConnect::~mysqlConnect()
{
    if (m_conn != nullptr) {
        mysql_close(m_conn);
    }
    freeResult();
}

bool mysqlConnect::connect(string user, string passwd, string dbName, string ip, unsigned short port)
{
    MYSQL* ptr = mysql_real_connect(m_conn, ip.c_str(), user.c_str(), passwd.c_str(), dbName.c_str(), port, nullptr, 0);
    return ptr != nullptr;
}

bool mysqlConnect::update(string sql)
{
    if (mysql_query(m_conn, sql.c_str())) return false;
    return true;
}

bool mysqlConnect::query(string sql)
{
    freeResult();
    if (mysql_query(m_conn, sql.c_str())) return false;
    m_result = mysql_store_result(m_conn);
    return true;
}

bool mysqlConnect::next()
{
    if (m_result != nullptr) {
        m_row = mysql_fetch_row(m_result);
        if (m_row != nullptr) return true;
    }
    return false;
}

string mysqlConnect::value(int index)
{
    int listCount = mysql_num_fields(m_result);
    if (index >= listCount || index < 0)    return string();
    char* val = m_row[index];//����ֵΪchar* ��'\0'
    unsigned long length = mysql_fetch_lengths(m_result)[index];//ȡ����index�е����Գ���
    return string(val, length);//ȥ��'\0'
}

bool mysqlConnect::transaction()
{
    return mysql_autocommit(m_conn, false);
}

bool mysqlConnect::commit()
{
    return mysql_commit(m_conn);
}

bool mysqlConnect::rollback()
{
    return mysql_rollback(m_conn);
}

void mysqlConnect::refreshAliveTime()
{
    m_alivetime = steady_clock::now();
}

long long mysqlConnect::getAliveTime()
{
    nanoseconds res = steady_clock::now() - m_alivetime;
    milliseconds millisec = duration_cast<milliseconds>(res);
    return millisec.count();
}

void mysqlConnect::freeResult()
{
    if (m_result) {
        mysql_free_result(m_result);
        m_result = nullptr;
    }
}
