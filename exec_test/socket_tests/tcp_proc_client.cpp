//
// Created by bysou on 2022/5/8.
//

#include "_public.h"

CTcpClient TcpClient;

bool heart();
bool exec_login();
bool exec_inquiry();

int main(int argc,char *argv[])
{
  if (argc!=3)
  {
    printf("Using:./tcp_proc_client server_ip port\n"
           "Example:./tcp_proc_client 127.0.0.1 54321\n\n"); return -1;
  }

  // 向服务端发起连接请求。
  if (TcpClient.ConnectToServer(argv[1],atoi(argv[2]))==false)
  {
    printf("TcpClient.ConnectToServer(%s,%s) failed.\n",argv[1],argv[2]); return -1;
  }

  // 登录业务。
  if (exec_login()==false) { printf("exec_login() failed.\n"); return -1; }

  sleep(3);

  // 我的账户（查询余额）。
  if (exec_inquiry()==false) { printf("exec_inquiry() failed.\n"); return -1; }

  sleep(10);

  for (int ii=3;ii<5;ii++)
  {
    if (heart()==false) break;

    sleep(ii);
  }

  // 我的账户（查询余额）。
  if (exec_inquiry()==false) { printf("exec_inquiry() failed.\n"); return -1; }

  return 0;
}

// 心跳。 
bool heart()
{
  char buffer[1024];

  SPRINTF(buffer,sizeof(buffer),"<opt_code>0x0001</opt_code>");
  printf("发送：%s\n",buffer);
  if (TcpClient.Write(buffer)==false) return false; // 向服务端发送请求报文。

  memset(buffer,0,sizeof(buffer));
  if (TcpClient.Read(buffer)==false) return false; // 接收服务端的回应报文。
  printf("接收：%s\n",buffer);

  return true;
}

// 登录业务。 
bool exec_login()
{
  char buffer[1024];

  SPRINTF(buffer,sizeof(buffer),"<opt_code>0x1000</opt_code><username>bys</username><passwd>zzz</passwd>");
  printf("发送：%s\n",buffer);
  if (TcpClient.Write(buffer)==false) return false; // 向服务端发送请求报文。

  memset(buffer,0,sizeof(buffer));
  if (TcpClient.Read(buffer)==false) return false; // 接收服务端的回应报文。
  printf("接收：%s\n",buffer);

  // 解析服务端返回的xml。
  int iretcode=-1;
  GetXMLBuffer(buffer,"opt_code",&iretcode);
  if (iretcode!=0x1000) { printf("登录失败。\n"); return false; }

  printf("登录成功。\n");

  return true;
}

// 我的账户（查询余额）。
bool exec_inquiry()
{
  char buffer[1024];

  SPRINTF(buffer,sizeof(buffer),"<opt_code>0x1001</opt_code><card_id>12345678</card_id>");
  printf("发送：%s\n",buffer);
  if (TcpClient.Write(buffer)==false) return false; // 向服务端发送请求报文。

  memset(buffer,0,sizeof(buffer));
  if (TcpClient.Read(buffer)==false) return false; // 接收服务端的回应报文。
  printf("接收：%s\n",buffer);

  // 解析服务端返回的xml。
  int iretcode=-1;
  GetXMLBuffer(buffer,"opt_code",&iretcode);
  if (iretcode!=0x1001) { printf("查询余额失败。\n"); return false; }

  double ye=0;
  GetXMLBuffer(buffer,"message",&ye);

  printf("查询余额成功(%.2f)。\n",ye);

  return true;
}


