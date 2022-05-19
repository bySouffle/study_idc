//
// Created by bysou on 2022/5/15.
//
//! 操作MySQL数据库的 封装

#ifndef __MYSQL_H_
#define __MYSQL_H_

// C/C++库常用头文件
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#include <mysql/mysql.h>   // mysql 基础库

// 把文件filename加载到buffer中，必须确保buffer足够大。
// 成功返回文件的大小，文件不存在或为空返回0。
//! 加载文件
//! \param filename 文件名
//! \param buffer   out 输出文件信息
//! \return 文件大小
//! \note   必须确保 buffer 足够大
unsigned long file_to_buff(const char *filename, char *buffer);

//!
//! \param filename
//! \param buffer
//! \param size
//! \return
bool buff_to_file(const char *filename, char *buffer, unsigned long size);

//! MySQL登录环境
typedef struct MysqlLoginENV
{
  char ip[32];       // MySQL数据库的ip地址。
  int  port;         // MySQL数据库的通信端口。
  char user[32];     // 登录MySQL数据库的用户名。
  char pass[32];     // 登录MySQL数据库的密码。
  char dbname[51];   // 登录后，缺省打开的数据库。
}MysqlLoginENV_t;

//! 每次调用MySQL接口函数返回的结果
typedef struct CdaDef
{
  int      rc;         // 返回值：0-成功；其它是失败，存放了MySQL的错误代码。
  unsigned long rpc;   // 如果是insert、update和delete，存放影响记录的行数，如果是select，存放结果集的行数。
  char     message[2048]; // 如果返回失败，存放错误描述信息。
}CdaDef_t;

//! MySQL数据库连接类
class Connection
{
 private:
  //! 设置数据库参数
  //! \param conn_str 解析ip,username,password,dbname,port
  //! \example    "127.0.0.1","bys","bys","db",3306
  void set_db_opt(char *conn_str);

  //! 设置字符集
  //! \param charset
  //! \note   要与数据库的一致，否则中文会出现乱码
  void character(char *charset);

  MysqlLoginENV m_env;      // 服务器环境句柄。

  char m_dbtype[21];   // 数据库种类，固定取值为"mysql"。
 public:
  int m_state;         // 与数据库的连接状态，0-未连接，1-已连接。

  CdaDef m_cda;       // 数据库操作的结果或最后一次执行SQL语句的结果。

  char m_sql[10241];   // SQL语句的文本，最长不能超过10240字节。

  Connection();        // 构造函数。
  ~Connection();        // 析构函数。

  //! 登录数据库
  //! \param conn_str             数据库的登录参数,格式:"ip,username,password,dbname,port"
  //! \param charset              数据库的字符集，如"utf8"、"gbk"，必须与数据库保持一致，否则会出现中文乱码的情况
  //! \param auto_commit_opt      是否启用自动提交，0-不启用，1-启用，缺省是不启用
  //! \return                     0-成功，其它失败，失败的代码在m_cda.rc中，失败的描述在m_cda.message中
  int connect_to_db(char *conn_str, char *charset, unsigned int auto_commit_opt=0);

  //! 提交事务
  //! \return     返回值：0-成功，其它失败，程序中一般不必关心返回值
  int commit();

  //! 回滚事务
  //! \return     返回值：0-成功，其它失败，程序中一般不必关心返回值
  int  rollback();

  //! 断开与数据库的连接
  //! \return 0-成功，其它失败，程序中一般不必关心返回值
  //! \note   断开与数据库的连接时，全部未提交的事务自动回滚
  int disconnect();

  //! 执行SQL语句
  //! \note   如果SQL语句不需要绑定输入和输出变量（无绑定变量、非查询语句），可以直接用此方法执行
  //! \param fmt  语句格式
  //! \param ...  不定长参数
  //! \return     0-成功，其它失败，失败的代码在m_cda.rc中，失败的描述在m_cda.message中
  //! \note        1. 如果成功的执行了非查询语句，在m_cda.rpc中保存了本次执行SQL影响记录的行数
  //! \note        2. 程序中必须检查execute方法的返回值
  //! \note        3. 在connection类中提供了execute方法，是为了方便程序员，在该方法中，也是用SqlStatement类来完成功能
  int execute(const char *fmt,...);

  ////////////////////////////////////////////////////////////////////
 protected:
  friend  class SqlStatement;
  // 以下成员变量和函数，除了SqlStatement类，在类的外部不需要调用它。
  MYSQL     *m_conn;   // MySQL数据库连接句柄。
  int m_auto_commit_opt; // 自动提交标志，0-关闭自动提交；1-开启自动提交。
  void err_report();   // 获取错误信息。
  ////////////////////////////////////////////////////////////////////
};

// 执行SQL语句前绑定输入或输出变量个数的最大值，256是很大的了，可以根据实际情况调整。
#define MAX_PARAMS  256

// 操作SQL语句类。
class SqlStatement
{
 private:
  MYSQL_STMT *m_handle; // SQL语句句柄。

  MYSQL_BIND params_in[MAX_PARAMS];            // 输入参数。
  unsigned long params_in_length[MAX_PARAMS];  // 输入参数的实际长度。
//  my_bool params_in_is_null[MAX_PARAMS];       // 输入参数是否为空。
  int params_in_is_null[MAX_PARAMS];       // 输入参数是否为空。

  unsigned max_bind_in;                         // 输入参数最大的编号。

  MYSQL_BIND params_out[MAX_PARAMS]; // 输出参数。

  CdaDef m_cda1;      // prepare() SQL语句的结果。

  Connection *m_conn;  // 数据库连接指针。
  int m_sql_type;       // SQL语句的类型，0-查询语句；1-非查询语句。
  int m_auto_commit_opt; // 自动提交标志，0-关闭；1-开启。
  void err_report();   // 错误报告。
  void initial();      // 初始化成员变量。
 public:
  int m_state;         // 与数据库连接的绑定状态，0-未绑定，1-已绑定。

  char m_sql[10241];   // SQL语句的文本，最长不能超过10240字节。

  CdaDef m_cda;       // 执行SQL语句的结果。

  SqlStatement();      // 构造函数。
  SqlStatement(Connection *conn);    // 构造函数，同时绑定数据库连接。

  ~SqlStatement();      // 析构函数。

  //! 绑定数据库连接
  //! \param conn 数据库连接connection对象的地址
  //! \return     0-成功，其它失败，只要conn参数是有效的，并且数据库的游标资源足够，connect方法不会返回失败.
  //!             程序中一般不必关心connect方法的返回值
  //! \note       每个SqlStatement只需要绑定一次，在绑定新的connection前，必须先调用disconnect方法
  int connect(Connection *conn);

  //! 取消与数据库连接的绑定
  //! \return 返回值：0-成功，其它失败，程序中一般不必关心返回值
  int disconnect();

  //! 准备SQL语句
  //! \param fmt 格式
  //! \param ... 可变参数
  //! \return 0-成功，其它失败，程序中一般不必关心返回值
  //! \note   如果SQL语句没有改变，只需要prepare一次就可以了
  int prepare(const char *fmt,...);

  //! 绑定输入变量的地址
  //! \param position     字段的顺序，从1开始，必须与prepare方法中的SQL的序号一一对应
  //! \param value        输入变量的地址，如果是字符串，内存大小应该是表对应的字段长度加1
  //! \return             0-成功，其它失败，程序中一般不必关心返回值
  //! \note           1)  如果SQL语句没有改变，只需要bind_out一次就可以了
  //! \note           2)  绑定输出变量的总数不能超过MAX_PARAMS个
  int bind_in(unsigned int position, int    *value);
  //! 绑定输入变量的地址
  //! \param position     字段的顺序，从1开始，必须与prepare方法中的SQL的序号一一对应
  //! \param value        输入变量的地址，如果是字符串，内存大小应该是表对应的字段长度加1
  //! \return             0-成功，其它失败，程序中一般不必关心返回值
  //! \note           1)  如果SQL语句没有改变，只需要bind_out一次就可以了
  //! \note           2)  绑定输出变量的总数不能超过MAX_PARAMS个
  int bind_in(unsigned int position,long   *value);
  //! 绑定输入变量的地址
  //! \param position     字段的顺序，从1开始，必须与prepare方法中的SQL的序号一一对应
  //! \param value        输入变量的地址，如果是字符串，内存大小应该是表对应的字段长度加1
  //! \return             0-成功，其它失败，程序中一般不必关心返回值
  //! \note           1)  如果SQL语句没有改变，只需要bind_out一次就可以了
  //! \note           2)  绑定输出变量的总数不能超过MAX_PARAMS个
  int bind_in(unsigned int position,unsigned int  *value);
  //! 绑定输入变量的地址
  //! \param position     字段的顺序，从1开始，必须与prepare方法中的SQL的序号一一对应
  //! \param value        输入变量的地址，如果是字符串，内存大小应该是表对应的字段长度加1
  //! \return             0-成功，其它失败，程序中一般不必关心返回值
  //! \note           1)  如果SQL语句没有改变，只需要bind_out一次就可以了
  //! \note           2)  绑定输出变量的总数不能超过MAX_PARAMS个
  int bind_in(unsigned int position,unsigned long *value);
  //! 绑定输入变量的地址
  //! \param position     字段的顺序，从1开始，必须与prepare方法中的SQL的序号一一对应
  //! \param value        输入变量的地址，如果是字符串，内存大小应该是表对应的字段长度加1
  //! \return             0-成功，其它失败，程序中一般不必关心返回值
  //! \note           1)  如果SQL语句没有改变，只需要bind_out一次就可以了
  //! \note           2)  绑定输出变量的总数不能超过MAX_PARAMS个
  int bind_in(unsigned int position,float *value);
  //! 绑定输入变量的地址
  //! \param position     字段的顺序，从1开始，必须与prepare方法中的SQL的序号一一对应
  //! \param value        输入变量的地址，如果是字符串，内存大小应该是表对应的字段长度加1
  //! \return             0-成功，其它失败，程序中一般不必关心返回值
  //! \note           1)  如果SQL语句没有改变，只需要bind_out一次就可以了
  //! \note           2)  绑定输出变量的总数不能超过MAX_PARAMS个
  int bind_in(unsigned int position,double *value);

  //! 绑定输入变量的地址
  //! \param position     字段的顺序，从1开始，必须与prepare方法中的SQL的序号一一对应
  //! \param value        输入变量的地址，如果是字符串，内存大小应该是表对应的字段长度加1
  //! \param len          如果输入变量的数据类型是字符串，用len指定它的最大长度，建议采用表对应的字段长度
  //! \return             0-成功，其它失败，程序中一般不必关心返回值
  //! \note           1)  如果SQL语句没有改变，只需要bind_out一次就可以了
  //! \note           2)  绑定输出变量的总数不能超过MAX_PARAMS个
  int bind_in(unsigned int position,char   *value,unsigned int len);

  //! 绑定BLOB字段
  //! \param position
  //! \param buffer   BLOB字段的内容
  //! \param size     BLOB字段的大小
  //! \return
  int bind_in_blob(unsigned int position, void *buffer, unsigned long *size);

  //! 把结果集的字段与变量的地址绑定
  //! \param position position：字段的顺序，从1开始，与SQL的结果集字段一一对应
  //! \param value    输出变量的地址，如果是字符串，内存大小应该是表对应的字段长度加1
  //! \return         0-成功，其它失败，程序中一般不必关心返回值
  //! \note           1)  如果SQL语句没有改变，只需要bind_out一次就可以了
  //! \note           2)  绑定输出变量的总数不能超过MAX_PARAMS个
  int bind_out(unsigned int position, int    *value);
  //! 把结果集的字段与变量的地址绑定
  //! \param position position：字段的顺序，从1开始，与SQL的结果集字段一一对应
  //! \param value    输出变量的地址，如果是字符串，内存大小应该是表对应的字段长度加1
  //! \return         0-成功，其它失败，程序中一般不必关心返回值
  //! \note           1)  如果SQL语句没有改变，只需要bind_out一次就可以了
  //! \note           2)  绑定输出变量的总数不能超过MAX_PARAMS个
  int bind_out(unsigned int position, long   *value);
  //! 把结果集的字段与变量的地址绑定
  //! \param position position：字段的顺序，从1开始，与SQL的结果集字段一一对应
  //! \param value    输出变量的地址，如果是字符串，内存大小应该是表对应的字段长度加1
  //! \return         0-成功，其它失败，程序中一般不必关心返回值
  //! \note           1)  如果SQL语句没有改变，只需要bind_out一次就可以了
  //! \note           2)  绑定输出变量的总数不能超过MAX_PARAMS个
  int bind_out(unsigned int position, unsigned int  *value);
  //! 把结果集的字段与变量的地址绑定
  //! \param position position：字段的顺序，从1开始，与SQL的结果集字段一一对应
  //! \param value    输出变量的地址，如果是字符串，内存大小应该是表对应的字段长度加1
  //! \return         0-成功，其它失败，程序中一般不必关心返回值
  //! \note           1)  如果SQL语句没有改变，只需要bind_out一次就可以了
  //! \note           2)  绑定输出变量的总数不能超过MAX_PARAMS个
  int bind_out(unsigned int position, unsigned long *value);
  //! 把结果集的字段与变量的地址绑定
  //! \param position position：字段的顺序，从1开始，与SQL的结果集字段一一对应
  //! \param value    输出变量的地址，如果是字符串，内存大小应该是表对应的字段长度加1
  //! \return         0-成功，其它失败，程序中一般不必关心返回值
  //! \note           1)  如果SQL语句没有改变，只需要bind_out一次就可以了
  //! \note           2)  绑定输出变量的总数不能超过MAX_PARAMS个
  int bind_out(unsigned int position, float *value);
  //! 把结果集的字段与变量的地址绑定
  //! \param position position：字段的顺序，从1开始，与SQL的结果集字段一一对应
  //! \param value    输出变量的地址，如果是字符串，内存大小应该是表对应的字段长度加1
  //! \return         0-成功，其它失败，程序中一般不必关心返回值
  //! \note           1)  如果SQL语句没有改变，只需要bind_out一次就可以了
  //! \note           2)  绑定输出变量的总数不能超过MAX_PARAMS个
  int bind_out(unsigned int position, double *value);
  //! 把结果集的字段与变量的地址绑定
  //! \param position position：字段的顺序，从1开始，与SQL的结果集字段一一对应
  //! \param value    输出变量的地址，如果是字符串，内存大小应该是表对应的字段长度加1
  //! \param len      如果输出变量的数据类型是字符串，用len指定它的最大长度，建议采用表对应的字段长度
  //! \return         0-成功，其它失败，程序中一般不必关心返回值
  //! \note           1)  如果SQL语句没有改变，只需要bind_out一次就可以了
  //! \note           2)  绑定输出变量的总数不能超过MAX_PARAMS个
  int bind_out(unsigned int position, char   *value, unsigned int len);

  //! 绑定BLOB字段
  //! \param position
  //! \param buffer       buffer用于存放BLOB字段的内容
  //! \param buffer_size  buffer占用内存的大小
  //! \param size         结果集中BLOB字段实际的大小
  //! \return
  //! \note               一定要保证buffer足够大，防止内存溢出
  int bind_out_blob(unsigned int position, void *buffer, unsigned long buffer_size, unsigned long *size);

  //! 执行SQL语句
  //! \return  返回值：0-成功，其它失败，失败的代码在m_cda.rc中，失败的描述在m_cda.message中
  //! \note    1) 如果成功的执行了insert、update和delete语句，在m_cda.rpc中保存了本次执行SQL影响记录的行数
  //! \note    2) 程序中必须检查execute方法的返回值
  int execute();

  //! 执行SQL语句 (如果SQL语句不需要绑定输入和输出变量（无绑定变量、非查询语句），可以直接用此方法执行)
  //! \param fmt 格式
  //! \param ... 参数
  //! \return    0-成功，其它失败，失败的代码在m_cda.rc中，失败的描述在m_cda.message中
  //! \note    1) 如果成功的执行了非查询语句，在m_cda.rpc中保存了本次执行SQL影响记录的行数
  //! \note    2) 程序中必须检查execute方法的返回值
  int execute(const char *fmt,...);

  //! 从结果集中获取一条记录 (如果执行的SQL语句是查询语句，调用execute方法后，会产生一个结果集（存放在数据库的缓冲区中）)
  //! || next方法从结果集中获取一条记录，把字段的值放入已绑定的输出变量中
  //! \return 0-成功，1403-结果集已无记录，其它-失败，失败的代码在m_cda.rc中，失败的描述在m_cda.message中
  //!\note    1. 返回失败的原因主要有两种：
  //!\note    1）与数据库的连接已断开；2）绑定输出变量的内存太小
  //!\note    2. 每执行一次next方法，m_cda.rpc的值加1
  //!\note    3. 程序中必须检查next方法的返回值
  int next();
};

#endif // __MYSQL_H_
