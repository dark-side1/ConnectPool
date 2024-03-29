#include "connectPool.h"
#include "mysqlConnect.h"
/*
1.单线程：使用/不使用连接池
    不使用：非连接池，单线程，用时：51715724300纳秒，51715毫秒
    使用：连接池，单线程，用时：7416616900纳秒，7416毫秒
2.多线程：使用/不使用连接池
    不使用：非连接池，多线程，用时：15593241700纳秒，15593毫秒
    使用：连接池，多线程，用时：4201836400纳秒，4201毫秒
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
    cout << "非连接池，单线程，用时：" << length.count() << "纳秒，" << length.count() / 1000000 << "毫秒" << endl;
#else
    connectPool* pool = connectPool::getConnectPool();
    steady_clock::time_point begin = steady_clock::now();
    op2(pool, 0, 5000);
    steady_clock::time_point end = steady_clock::now();
    auto length = end - begin;
    cout << "连接池，单线程，用时：" << length.count() << "纳秒，" << length.count() / 1000000 << "毫秒" << endl;
#endif
}

void test2() {
#if 1
    mysqlConnect conn;
    conn.connect("root", "123456", "test", "127.0.0.1");
    //op1里面会创建数据库连接，多个线程使用相同的用户名和密码去登陆MySQL，会被拒绝掉一些
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
    cout << "非连接池，多线程，用时：" << length.count() << "纳秒，" << length.count() / 1000000 << "毫秒" << endl;
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
    cout << "连接池，多线程，用时：" << length.count() << "纳秒，" << length.count() / 1000000 << "毫秒" << endl;
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
	//query();//连接测试
    test2();
    return 0;
}
