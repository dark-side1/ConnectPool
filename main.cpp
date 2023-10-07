#include "connectPool.h"
#include "mysqlConnect.h"
/*
1.���̣߳�ʹ��/��ʹ�����ӳ�
    ��ʹ�ã������ӳأ����̣߳���ʱ��51715724300���룬51715����
    ʹ�ã����ӳأ����̣߳���ʱ��7416616900���룬7416����
2.���̣߳�ʹ��/��ʹ�����ӳ�
    ��ʹ�ã������ӳأ����̣߳���ʱ��15593241700���룬15593����
    ʹ�ã����ӳأ����̣߳���ʱ��4201836400���룬4201����
*/
void op1(int begin, int end) {
    for (int i = begin; i < end; i++) {
        mysqlConnect conn;
        conn.connect("root", "123456", "test", "127.0.0.1");
        char sql[1024] = { 0 };
        sprintf(sql, "insert into person values(%d, 18, 'man', 'starry')", i);
        conn.update(sql);
    }
}

void op2(connectPool* pool, int begin, int end) {
    for (int i = begin; i < end; i++) {
        shared_ptr<mysqlConnect> conn = pool->getConnection();
        char sql[1024] = { 0 };
        sprintf(sql, "insert into person values(%d, 18, 'man', 'starry')", i);
        //        string sql = "insert into person values(" + to_string(i) + ", 18, 'man', 'yll')"; 
        //      conn->update(sql.c_str());
        conn->update(sql);
    }
}

void test1() {
#if 1
    steady_clock::time_point begin = steady_clock::now();
    op1(0, 5000);
    steady_clock::time_point end = steady_clock::now();
    auto length = end - begin;
    cout << "�����ӳأ����̣߳���ʱ��" << length.count() << "���룬" << length.count() / 1000000 << "����" << endl;
#else
    connectPool* pool = connectPool::getConnectPool();
    steady_clock::time_point begin = steady_clock::now();
    op2(pool, 0, 5000);
    steady_clock::time_point end = steady_clock::now();
    auto length = end - begin;
    cout << "���ӳأ����̣߳���ʱ��" << length.count() << "���룬" << length.count() / 1000000 << "����" << endl;
#endif
}

void test2() {
#if 1
    mysqlConnect conn;
    conn.connect("root", "123456", "test", "127.0.0.1");
    //op1����ᴴ�����ݿ����ӣ�����߳�ʹ����ͬ���û���������ȥ��½MySQL���ᱻ�ܾ���һЩ
    steady_clock::time_point begin = steady_clock::now();
    thread t1(op1, 0, 1000);
    thread t2(op1, 1000, 2000);
    thread t3(op1, 2000, 3000);
    thread t4(op1, 3000, 4000);
    thread t5(op1, 4000, 5000);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    steady_clock::time_point end = steady_clock::now();
    auto length = end - begin;
    cout << "�����ӳأ����̣߳���ʱ��" << length.count() << "���룬" << length.count() / 1000000 << "����" << endl;
#else
    steady_clock::time_point begin = steady_clock::now();
    connectPool* pool = connectPool::getConnectPool();
    thread t1(op2, pool, 0, 1000);
    thread t2(op2, pool, 1000, 2000);
    thread t3(op2, pool, 2000, 3000);
    thread t4(op2, pool, 3000, 4000);
    thread t5(op2, pool, 4000, 5000);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    steady_clock::time_point end = steady_clock::now();
    auto length = end - begin;
    cout << "���ӳأ����̣߳���ʱ��" << length.count() << "���룬" << length.count() / 1000000 << "����" << endl;
#endif 
}

int query() {
    mysqlConnect conn;
    conn.connect("root", "123456", "test", "127.0.0.1");
    string sql = "insert into person values(6, 18, 'man', 'starry')";
    bool flag = conn.update(sql);
    cout << "flag value" << flag << endl;

    sql = "select * from person";
    conn.query(sql);
    while (conn.next()) {
        cout << conn.value(0) << ", "
            << conn.value(1) << ", "
            << conn.value(2) << ", "
            << conn.value(3) << endl;
    }
    return 0;
}

int main() {
	//query();//���Ӳ���
    test2();
    return 0;
}
