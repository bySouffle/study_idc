//
// Created by bys on 5/25/21.
//
//! _ftp.h，ftp cpp 封装

#ifndef __FTP_H
#define __FTP_H

#include "_public.h"
#include "ftplib.h"

class Cftp
{
public:
  netbuf *m_ftpconn;   // ftp连接句柄。
  unsigned int m_size; // 文件的大小，单位：字节。
  char m_mtime[21];    // 文件的修改时间，格式：yyyymmddhh24miss。

  // 以下三个成员变量用于存放login方法登录失败的原因。
  bool m_connectfailed;    // 连接失败。
  bool m_loginfailed;      // 登录失败，用户名和密码不正确，或没有登录权限。
  bool m_optionfailed;     // 设置传输模式失败。

  Cftp();  // 类的构造函数。
 ~Cftp();  // 类的析构函数。

  void initdata();   // 初始化m_size和m_mtime成员变量。

  //! 登录ftp服务器
  //! \param host         host：ftp服务器ip地址和端口，中间用":"分隔，如"192.168.1.1:21"
  //! \param username     登录ftp服务器用户名
  //! \param password     登录ftp服务器的密码
  //! \param imode        传输模式，1-FTPLIB_PASSIVE是被动模式，2-FTPLIB_PORT是主动模式，缺省是被动模式
  //! \return             true 成功 | false 失败
  bool login(const char *host,const char *username,const char *password,const int imode=FTPLIB_PASSIVE);

  //! 注销
  //! \return
  bool logout();

  //! 获取ftp服务器上文件的时间
  //! \param remote_filename 待获取的文件名
  //! \return false-失败；true-成功，获取到的文件时间存放在m_mtime成员变量中
  bool mtime(const char *remote_filename);

  //! 获取ftp服务器上文件的大小
  //! \param remote_filename 待获取的文件名
  //! \return false-失败；true-成功，获取到的文件大小存放在m_size成员变量中
  bool size(const char *remote_filename);

  //! 改变ftp服务器的当前工作目录
  //! \param remote_dir ftp服务器上的目录名
  //! \return true-成功；false-失败
  bool chdir(const char *remote_dir);

  //! 在ftp服务器上创建目录
  //! \param remote_dir ftp服务器上待创建的目录名
  //! \return true-成功；false-失败
  bool mkdir(const char *remote_dir);

  //! 删除ftp服务器上的目录
  //! \param remote_dir ftp服务器上待删除的目录名
  //! \return true-成功；如果权限不足、目录不存在或目录不为空会返回false
  bool rmdir(const char *remote_dir);

  //! 发送NLST命令列出ftp服务器目录中的子目录名和文件名
  //! \param remote_dir       ftp服务器的目录名
  //! \param list_filename    用于保存从服务器返回的目录和文件名列表
  //! \return                 true-成功；false-失败
  //! \note   如果列出的是ftp服务器当前目录，remote_dir 用"","*","."都可以，但是，不规范的ftp服务器可能有差别
  bool nlist(const char *remote_dir,const char *list_filename);

  //! 从ftp服务器上获取文件
  //! \param remote_filename  待获取ftp服务器上的文件名
  //! \param local_filename   保存到本地的文件名
  //! \param bCheckMTime      文件传输完成后，是否核对远程文件传输前后的时间，保证文件的完整性
  //! \return     true-成功；false-失败
  //! \note       文件在传输的过程中，采用临时文件命名的方法，即在local_filename后加".tmp"，在传输完成后才正式改为local_filename
  bool get(const char *remote_filename,const char *local_filename,const bool bCheckMTime=true);

  //! 向ftp服务器发送文件
  //! \param local_filename   本地待发送的文件名
  //! \param remote_filename  发送到ftp服务器上的文件名
  //! \param bCheckSize       文件传输完成后，是否核对本地文件和远程文件的大小，保证文件的完整性
  //! \return     true-成功；false-失败
  //! \note       文件在传输的过程中，采用临时文件命名的方法，即在remote_filename后加".tmp"，在传输完成后才正式改为remote_filename
  bool put(const char *local_filename,const char *remote_filename,const bool bCheckSize=true);

  //! 删除ftp服务器上的文件
  //! \param remote_filename 待删除的ftp服务器上的文件名
  //! \return true-成功；false-失败
  bool ftpdelete(const char *remote_filename);

  //! 重命名ftp服务器上的文件
  //! \param src_remote_filename ftp服务器上的原文件名
  //! \param dst_remote_filename ftp服务器上的目标文件名
  //! \return     true-成功；false-失败
  bool ftprename(const char *src_remote_filename,const char *dst_remote_filename);

  /* =================> 以下三个方法如果理解不了就算了，可以不启用 <=======================  */

  //! 发送LIST命令列出ftp服务器目录中的文件
  //! \param remote_dir       ftp服务器的目录名
  //! \param list_filename    用于保存从服务器返回的目录和文件名列表
  //! \return                 true-成功；false-失败
  //! \sa     bool nlist(const char *remote_dir,const char *list_filename);
  bool dir(const char *remote_dir,const char *list_filename);

  //! 向ftp服务器发送site命令
  //! \param command 命令的内容
  //! \return true-成功；false-失败
  bool site(const char *command);

  //! 获取服务器返回信息的最后一条(return a pointer to the last response received)
  //! \return
  char *response();
};

#endif
