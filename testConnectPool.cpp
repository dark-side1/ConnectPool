//// ConnectPool.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
////
//
//#include <iostream>
//#include <stdio.h>
//#include <mysql.h>
//
//int main()
//{
//    // 1. 初始化连接环境
//    MYSQL* mysql = mysql_init(NULL);
//    if (mysql == NULL)
//    {
//        printf("mysql_init() error\n");
//        return -1;
//    }
//
//    // 2. 连接数据库服务器
//    mysql = mysql_real_connect(mysql, "127.0.0.1", "root", "123456", "test", 0, NULL, 0);
//    if (mysql == NULL)
//    {
//        printf("mysql_real_connect() error\n");
//        return -1;
//    }
//
//    printf("mysql api使用的默认编码: %s\n", mysql_character_set_name(mysql));
//
//    // 设置编码为utf8
//    mysql_set_character_set(mysql, "utf8");
//
//    printf("mysql api使用的修改之后的编码: %s\n", mysql_character_set_name(mysql));
//
//    printf("恭喜, 连接数据库服务器成功了...\n");
//
//    // 3. 执行一个sql语句
//    const char* sql = "select * from person";
//    // 执行这个sql语句
//    int ret = mysql_query(mysql, sql);
//    if (ret != 0)
//    {
//        printf("mysql_query() a失败了, 原因: %s\n", mysql_error(mysql));
//        return -1;
//    }
//
//    // 4. 取出结果集
//    MYSQL_RES* res = mysql_store_result(mysql);
//    if (res == NULL)
//    {
//        printf("mysql_store_result() 失败了, 原因: %s\n", mysql_error(mysql));
//        return -1;
//    }
//
//    // 5. 得到结果集中的列数
//    int num = mysql_num_fields(res);
//
//    // 6. 得到所有列的名字, 并且输出
//    MYSQL_FIELD* fields = mysql_fetch_fields(res);
//    for (int i = 0; i < num; ++i)
//    {
//        printf("%s\t\t", fields[i].name);
//    }
//    printf("\n");
//
//    // 7. 遍历结果集中所有的行
//    MYSQL_ROW row;
//    while ((row = mysql_fetch_row(res)) != NULL)
//    {
//        // 将当前行中的每一列信息读出
//        for (int i = 0; i < num; ++i)
//        {
//            printf("%s\t\t", row[i]);
//        }
//        printf("\n");
//    }
//
//    // 8. 释放资源 - 结果集
//    mysql_free_result(res);
//
//
//    // 9. 写数据库
//    // 以下三条是一个完整的操作, 对应的是一个事务
//    // 设置事务为手动提交
//    mysql_autocommit(mysql, 0);
//	int ret1 = mysql_query(mysql, "insert into person values(61, 10, 'man', 'asdf')");
//	int ret2 = mysql_query(mysql, "insert into person values(62, 10, 'man', 'fdg')");
//	int ret3 = mysql_query(mysql, "insert into person values(63, 10, 'man', 'zxc')");
//    printf("ret1 = %d, ret2 = %d, ret3 = %d\n", ret1, ret2, ret3);
//
//    if (ret1 == 0 && ret2 == 0 && ret3 == 0)
//    {
//        // 提交事务
//        mysql_commit(mysql);
//    }
//    else
//    {
//        mysql_rollback(mysql);
//    }
//
//    // 释放数据库资源
//    mysql_close(mysql);
//
//    return 0;
//}
