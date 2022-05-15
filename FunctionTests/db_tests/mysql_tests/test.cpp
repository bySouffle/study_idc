//
// Created by bysou on 2022/5/15.
//

#include "db/_mysql.h"
#include "gtest/gtest.h"

using namespace std;

TEST(file_to_buff, test) {
  char buff[1024]{};
  cout << file_to_buff("/tmp/zzz.log", buff) << "\n";
  cout << string(buff) << "\n";
}

TEST(buff_to_file, test) {
  char buff[1024]{};
  file_to_buff("/tmp/zzz.log", buff);
  cout << buff_to_file("/tmp/1.log", buff, strlen(buff) + 1) << "\n";
}

TEST(create_table, test) {
//  $ docker run --name z-mysql -p 3306:3306  -e MYSQL_ROOT_PASSWORD=2020 -d mysql:latest
  connection conn;
  if (conn.connect_to_db("172.20.92.218,root,2020,mysql,3306", "utf8") != 0) {
    printf("connect database failed.\n%s\n", conn.m_cda.message);
  }

  SqlStatement st(&conn);

  st.prepare("create table zz("
             "id bigint(10),"
             "name varchar(30),"
             "timestamp   datetime,"
             "mem  longtext,"
             "pic  longblob,"
             "primary key (id))"
  );
  /*
  1、int prepare(const char *fmt,...)，SQL语句可以多行书写。
  2、SQL语句最后的分号可有可无，建议不要写（兼容性考虑）。
  3、SQL语句中不能有说明文字。
  4、可以不用判断stmt.prepare()的返回值，stmt.execute()时再判断。
  */

  // 执行SQL语句，一定要判断返回值，0-成功，其它-失败。
  // 失败代码在stmt.m_cda.rc中，失败描述在stmt.m_cda.message中。
  if (st.execute() != 0) {
    printf("stmt.execute() failed.\n%s\n%s\n", st.m_sql, st.m_cda.message);
    return;
  }
  printf("create table zz ok.\n");

}

TEST(inset_table, test) {
  connection conn;
  if (conn.connect_to_db("172.20.92.218,root,2020,mysql,3306", "utf8") != 0) {
    printf("connect database failed.\n%s\n", conn.m_cda.message);
  }

  struct st_zz {
    long id;        //
    char name[31];  //
    char btime[20]; //
  } stzzs;

  SqlStatement st(&conn);  // 操作SQL语句的对象。

  // 准备插入表的SQL语句。
  st.prepare("insert into zz(id,name,timestamp) "
             "values(:1+1,:2,str_to_date(:3,'%%Y-%%m-%%d %%H:%%i:%%s'))");
  //insert into girls(id,name,weight,btime) values(?+1,?,?+45.35,to_date(?,'yyyy-mm-dd hh24:mi:ss'))");
  /*
    注意事项：
    1、参数的序号从1开始，连续、递增，参数也可以用问号表示，但是，问号的兼容性不好，不建议；
    2、SQL语句中的右值才能作为参数，表名、字段名、关键字、函数名等都不能作为参数；
    3、参数可以参与运算或用于函数的参数；
    4、如果SQL语句的主体没有改变，只需要prepare()一次就可以了；
    5、SQL语句中的每个参数，必须调用bindin()绑定变量的地址；
    6、如果SQL语句的主体已改变，prepare()后，需重新用bindin()绑定变量；
    7、prepare()方法有返回值，一般不检查，如果SQL语句有问题，调用execute()方法时能发现；
    8、bind_in()方法的返回值固定为0，不用判断返回值；
    9、prepare()bind_in()之后，每调用一次execute()，就执行一次SQL语句，SQL语句的数据来自被绑定变量的值。
  */
  st.bind_in(1, &stzzs.id);
  st.bind_in(2, stzzs.name, 30);
  st.bind_in(3, stzzs.btime, 19);

  // 向表中插入5条测试数据。
  for (int ii = 0; ii < 5; ii++) {
    memset(&stzzs, 0, sizeof(struct st_zz));         // 结构体变量初始化。

    // 为结构体变量的成员赋值。
    stzzs.id = ii;                                     // 。
    sprintf(stzzs.name, "作者%05d_zz", ii + 1);         // 。
    sprintf(stzzs.btime, "2021-08-25 10:33:%02d", ii); // 。

    // 执行SQL语句，一定要判断返回值，0-成功，其它-失败。
    // 失败代码在stmt.m_cda.rc中，失败描述在stmt.m_cda.message中。
    if (st.execute() != 0) {
      printf("stmt.execute() failed.\n%s\n%s\n", st.m_sql, st.m_cda.message);
      return;
    }

    printf("成功插入了%ld条记录。\n", st.m_cda.rpc); // stmt.m_cda.rpc是本次执行SQL影响的记录数。
  }

  printf("insert table zz ok.\n");

  conn.commit();   // 提交数据库事务。
}

TEST(delete_table, test) {
  connection conn;
  if (conn.connect_to_db("172.20.92.218,root,2020,mysql,3306", "utf8") != 0) {
    printf("connect database failed.\n%s\n", conn.m_cda.message);
  }
  int iminid, imaxid;

  SqlStatement st(&conn);
  // 准备删除表的SQL语句。
  st.prepare("delete from zz where id>=:1 and id<=:2");
  // 为SQL语句绑定输入变量的地址，bindin方法不需要判断返回值。
  st.bind_in(1, &iminid);
  st.bind_in(2, &imaxid);
  iminid = 4;    // 指定待删除记录的最小id的值。
  imaxid = 5;    // 指定待删除记录的最大id的值。

  // 执行SQL语句，一定要判断返回值，0-成功，其它-失败。
  // 失败代码在stmt.m_cda.rc中，失败描述在stmt.m_cda.message中。
  if (st.execute() != 0) {
    printf("stmt.execute() failed.\n%s\n%s\n", st.m_sql, st.m_cda.message);
    return;
  }

  // 请注意，stmt.m_cda.rpc变量非常重要，它保存了SQL被执行后影响的记录数。
  printf("本次删除了zz表%ld条记录。\n", st.m_cda.rpc);

  conn.commit();   // 提交数据库事务。

}

TEST(update_table, test) {
  connection conn;
  if (conn.connect_to_db("172.20.92.218,root,2020,mysql,3306", "utf8") != 0) {
    printf("connect database failed.\n%s\n", conn.m_cda.message);
  }
  SqlStatement st(&conn);

  struct st_zz {
    long id;        //
    char name[31];  //
    char btime[20]; //
  } stzzs;
  st.prepare("update zz "
             "set name=:1,"
             "timestamp=str_to_date(:2,'%%Y-%%m-%%d %%H:%%i:%%s') "
             "where id=:3");
  st.bind_in(1, stzzs.name, 30);
  st.bind_in(2, stzzs.btime, 19);
  st.bind_in(3, &stzzs.id);

  // 模拟超女数据，修改超女信息表中的全部记录。
  for (int ii = 0; ii < 5; ii++) {
    memset(&stzzs, 0, sizeof(struct st_zz));         // 结构体变量初始化。

    // 为结构体变量的成员赋值。
    stzzs.id = ii + 1;                                   //  。
    sprintf(stzzs.name, "修改%05dxx", ii + 1);         //  。
    sprintf(stzzs.btime, "2021-10-02 11:25:%02d", ii); //  。

    // 执行SQL语句，一定要判断返回值，0-成功，其它-失败。
    // 失败代码在stmt.m_cda.rc中，失败描述在stmt.m_cda.message中。
    if (st.execute() != 0) {
      printf("stmt.execute() failed.\n%s\n%s\n", st.m_sql, st.m_cda.message);
      return;
    }

    printf("成功修改了%ld条记录。\n", st.m_cda.rpc); // stmt.m_cda.rpc是本次执行SQL影响的记录数。
  }

  printf("update table girls ok.\n");

  conn.commit();   // 提交数据库事务。

}

TEST(search_table, test) {
  connection conn;
  if (conn.connect_to_db("172.20.92.218,root,2020,mysql,3306", "utf8") != 0) {
    printf("connect database failed.\n%s\n", conn.m_cda.message);
  }
  SqlStatement st(&conn);

  struct st_zz {
    long id;        //
    char name[31];  //
    char btime[20]; //
  } stzzs;

  /*
    注意事项：
    1、如果SQL语句的主体没有改变，只需要prepare()一次就可以了；
    2、结果集中的字段，调用bind_out()绑定变量的地址；
    3、bind_out()方法的返回值固定为0，不用判断返回值；
    4、如果SQL语句的主体已改变，prepare()后，需重新用bind_out()绑定变量；
    5、调用execute()方法执行SQL语句，然后再循环调用next()方法获取结果集中的记录；
    6、每调用一次next()方法，从结果集中获取一条记录，字段内容保存在已绑定的变量中。
  */
  int iminid, imaxid;
  st.prepare("select"
             " id,name,date_format(timestamp,'%%Y-%%m-%%d %%H:%%i:%%s') "
             "from zz "
             "where id>=:1 and id<=:2");
  st.bind_in(1, &iminid);
  st.bind_in(2, &imaxid);
  st.bind_out(1, &stzzs.id);
  st.bind_out(2, stzzs.name, 30);
  st.bind_out(3, stzzs.btime, 19);

  iminid = 1;
  imaxid = 3;

  if (st.execute() != 0) {
    printf("stmt.execute() failed.\n%s\n%s\n", st.m_sql, st.m_cda.message);
    return;
  }

  while (true) {
    memset(&stzzs, 0, sizeof(struct st_zz));         // 结构体变量初始化。

    // 从结果集中获取一条记录，一定要判断返回值，0-成功，1403-无记录，其它-失败。
    // 在实际开发中，除了0和1403，其它的情况极少出现。
    if (st.next() != 0) break;

    // 把获取到的记录的值打印出来。
    printf("id=%ld,name=%s,btime=%s\n", stzzs.id, stzzs.name, stzzs.btime);
  }

  printf("本次查询了girls表%ld条记录。\n", st.m_cda.rpc);

}