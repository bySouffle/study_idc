//
// Created by bys on 5/24/21.
//
//! _public.cpp，此程序是开发框架公用函数和类的声明文件。



#include "_public.h"

char *STRCPY(char* dest,const size_t destlen,const char* src)
{
  if (dest==0) return 0;    // 判断空指针。
  memset(dest,0,destlen);   // 初始化dest。
  // memset(dest,0,sizeof(dest));   // 不能这么写，在64位系统中，sizeof(dest)永远是8。
  if (src==0) return dest;

  if (strlen(src)>destlen-1) strncpy(dest,src,destlen-1);
  else strcpy(dest,src);

  return dest;
}

char *STRNCPY(char* dest,const size_t destlen,const char* src,size_t n)
{
  if (dest==0) return 0;    // 判断空指针。
  memset(dest,0,destlen);   // 初始化dest。
  // memset(dest,0,sizeof(dest));   // 不能这么写，在64位系统中，sizeof(dest)永远是8。
  if (src==0) return dest;

  if (n>destlen-1) strncpy(dest,src,destlen-1);
  else strncpy(dest,src,n);

  return dest;
}

char *STRCAT(char* dest,const size_t destlen,const char* src)
{
  if (dest==0) return 0;    // 判断空指针。
  if (src==0) return dest;

  unsigned int left=destlen-1-strlen(dest);

  if (strlen(src)>left) { strncat(dest,src,left); dest[destlen-1]=0; }
  else strcat(dest,src);

  return dest;
}

char *STRNCAT(char* dest,const size_t destlen,const char* src,size_t n)
{
  if (dest==0) return 0;    // 判断空指针。
  if (src==0) return dest;

  size_t left=destlen-1-strlen(dest);

  if (n>left) { strncat(dest,src,left); dest[destlen-1]=0; }
  else strncat(dest,src,n);

  return dest;
}

int SPRINTF(char *dest,const size_t destlen,const char *fmt,...)
{
  if (dest==0) return -1;    // 判断空指针。

  memset(dest,0,destlen);
  // memset(dest,0,sizeof(dest));   // 不能这么写，在64位系统中，sizeof(dest)永远是8。

  va_list arg;
  va_start(arg,fmt);
  int ret=vsnprintf(dest,destlen,fmt,arg );
  va_end(arg);

  return ret;
}

int SNPRINTF(char *dest,const size_t destlen,size_t n,const char *fmt,...)
{
  if (dest==0) return -1;    // 判断空指针。

  memset(dest,0,destlen);
  // memset(dest,0,sizeof(dest));   // 不能这么写，在64位系统中，sizeof(dest)永远是8。

  int len=n;
  if (n>destlen) len=destlen;

  va_list arg;
  va_start(arg,fmt);
  int ret=vsnprintf(dest,len,fmt,arg );
  va_end(arg);

  return ret;
}

void DeleteLChar(char *str,const char chr)
{
  if (str == 0) return;
  if (strlen(str) == 0) return;

  char strTemp[strlen(str)+1];

  int iTemp=0;

  memset(strTemp,0,sizeof(strTemp));
  strcpy(strTemp,str);

  while ( strTemp[iTemp] == chr )  iTemp++;

  memset(str,0,strlen(str)+1);

  strcpy(str,strTemp+iTemp);

  return;
}

void DeleteRChar(char *str,const char chr)
{
  if (str == 0) return;
  if (strlen(str) == 0) return;

  int istrlen = strlen(str);

  while (istrlen>0)
  {
    if (str[istrlen-1] != chr) break;

    str[istrlen-1]=0;

    istrlen--;
  }
}

void DeleteLRChar(char *str,const char chr)
{
  DeleteLChar(str,chr);
  DeleteRChar(str,chr);
}

void ToUpper(char *str)
{
  if (str == 0) return;

  if (strlen(str) == 0) return;

  int istrlen=strlen(str);

  for (int ii=0;ii<istrlen;ii++)
  {
    if ( (str[ii] >= 'a') && (str[ii] <= 'z') ) str[ii]=str[ii] - 32;
  }
}

void ToUpper(std::string &str)
{
  if (str.empty()) return;

  char strtemp[str.size()+1];

  STRCPY(strtemp,sizeof(strtemp),str.c_str());

  ToUpper(strtemp);

  str=strtemp;

  return;
}

void ToLower(char *str)
{
  if (str == 0) return;

  if (strlen(str) == 0) return;

  int istrlen=strlen(str);

  for (int ii=0;ii<istrlen;ii++)
  {
    if ( (str[ii] >= 'A') && (str[ii] <= 'Z') ) str[ii]=str[ii] + 32;
  }
}

void ToLower(std::string &str)
{
  if (str.empty()) return;

  char strtemp[str.size()+1];

  STRCPY(strtemp,sizeof(strtemp),str.c_str());

  ToLower(strtemp);

  str=strtemp;

  return;
}

void UpdateStr(char *str,const char *str1,const char *str2,bool bloop)
{
  if (str == 0) return;
  if (strlen(str) == 0) return;
  if ( (str1 == 0) || (str2 == 0) ) return;

  // 如果bloop为true并且str2中包函了str1的内容，直接返回，因为会进入死循环，最终导致内存溢出。
  if ( (bloop==true) && (strstr(str2,str1)!= nullptr) ) return;

  // 尽可能分配更多的空间，但仍有可能出现内存溢出的情况，最好优化成string。
  int ilen=strlen(str)*10;
  if (ilen<1000) ilen=1000;

  char strTemp[ilen];

  char *strStart=str;

  char *strPos=0;

  while (true)
  {
    if (bloop == true)
    {
      strPos=strstr(str,str1);
    }
    else
    {
      strPos=strstr(strStart,str1);
    }

    if (strPos == 0) break;

    memset(strTemp,0,sizeof(strTemp));
    STRNCPY(strTemp,sizeof(strTemp),str,strPos-str);
    STRCAT(strTemp,sizeof(strTemp),str2);
    STRCAT(strTemp,sizeof(strTemp),strPos+strlen(str1));
    strcpy(str,strTemp);

    strStart=strPos+strlen(str2);
  }
}

void PickNumber(const char *src,char *dest,const bool bsigned,const bool bdot)
{
  if (dest==0) return;    // 判断空指针。
  if (src==0) { strcpy(dest,""); return; }

  char strtemp[strlen(src)+1];
  memset(strtemp,0,sizeof(strtemp));
  strcpy(strtemp,src);
  DeleteLRChar(strtemp,' ');

  int ipossrc,iposdst,ilen;
  ipossrc=iposdst=ilen=0;

  ilen=strlen(strtemp);

  for (ipossrc=0;ipossrc<ilen;ipossrc++)
  {
    if ( (bsigned==true) && (strtemp[ipossrc] == '+') )
    {
      dest[iposdst++]=strtemp[ipossrc]; continue;
    }

    if ( (bsigned==true) && (strtemp[ipossrc] == '-') )
    {
      dest[iposdst++]=strtemp[ipossrc]; continue;
    }

    if ( (bdot==true) && (strtemp[ipossrc] == '.') )
    {
      dest[iposdst++]=strtemp[ipossrc]; continue;
    }

    if (isdigit(strtemp[ipossrc])) dest[iposdst++]=strtemp[ipossrc];
  }

  dest[iposdst]=0;
}

bool MatchStr(const std::string &str,const std::string &rules)
{
  // 如果用于比较的字符是空的，返回false
  if (rules.size() == 0) return false;

  // 如果被比较的字符串是"*"，返回true
  if (rules == "*") return true;

  int ii,jj;
  int  iPOS1,iPOS2;
  CCmdStr CmdStr,CmdSubStr;

  std::string strFileName,strMatchStr;

  strFileName=str;
  strMatchStr=rules;

  // 把字符串都转换成大写后再来比较
  ToUpper(strFileName);
  ToUpper(strMatchStr);

  CmdStr.SplitToCmd(strMatchStr,",");

  for (ii=0;ii<CmdStr.CmdCount();ii++)
  {
    // 如果为空，就一定要跳过，否则就会被配上
    if (CmdStr.m_vCmdStr[ii].empty() == true) continue;

    iPOS1=iPOS2=0;
    CmdSubStr.SplitToCmd(CmdStr.m_vCmdStr[ii],"*");

    for (jj=0;jj<CmdSubStr.CmdCount();jj++)
    {
      // 如果是文件名的首部
      if (jj == 0)
      {
        if (strncmp(strFileName.c_str(),CmdSubStr.m_vCmdStr[jj].c_str(),CmdSubStr.m_vCmdStr[jj].size()) != 0) break;
      }

      // 如果是文件名的尾部
      if (jj == CmdSubStr.CmdCount()-1)
      {
        if (strcmp(strFileName.c_str()+strFileName.size()-CmdSubStr.m_vCmdStr[jj].size(),CmdSubStr.m_vCmdStr[jj].c_str()) != 0) break;
      }

      iPOS2=strFileName.find(CmdSubStr.m_vCmdStr[jj],iPOS1);

      if (iPOS2 < 0) break;

      iPOS1=iPOS2+CmdSubStr.m_vCmdStr[jj].size();
    }

    if (jj==CmdSubStr.CmdCount()) return true;
  }

  return false;
}

CFile::CFile()   // 类的构造函数
{
  m_fp=0;
  m_bEnBuffer=true;
  memset(m_filename,0,sizeof(m_filename));
  memset(m_filenametmp,0,sizeof(m_filenametmp));
}

void CFile::Close()
{
  if (m_fp==0) return;    // 判断空指针。

  fclose(m_fp);  // 关闭文件指针

  m_fp=0;
  memset(m_filename,0,sizeof(m_filename));

  // 如果存在临时文件，就删除它。
  if (strlen(m_filenametmp)!=0) remove(m_filenametmp);

  memset(m_filenametmp,0,sizeof(m_filenametmp));
}

// 判断文件是否已打开
bool CFile::IsOpened()
{
  if (m_fp==0) return false;    // 判断空指针。

  return true;
}

// 关闭文件指针，并删除文件
bool CFile::CloseAndRemove()
{
  if (m_fp==0) return true;    // 判断空指针。

  fclose(m_fp);  // 关闭文件指针

  m_fp=0;

  if (remove(m_filename) != 0) { memset(m_filename,0,sizeof(m_filename)); return false; }

  memset(m_filename,0,sizeof(m_filename));
  memset(m_filenametmp,0,sizeof(m_filenametmp));

  return true;
}

CFile::~CFile()   // 类的析构函数
{
  Close();
}

// 打开文件，参数与FOPEN相同，打开成功true，失败返回false
bool CFile::Open(const char *filename,const char *openmode,bool bEnBuffer)
{
  Close();

  if ( (m_fp=FOPEN(filename,openmode)) == 0 ) return false;

  memset(m_filename,0,sizeof(m_filename));

  STRNCPY(m_filename,sizeof(m_filename),filename,300);

  m_bEnBuffer=bEnBuffer;

  return true;
}

// 专为改名而打开文件，参数与fopen相同，打开成功true，失败返回false
bool CFile::OpenForRename(const char *filename,const char *openmode,bool bEnBuffer)
{
  Close();

  memset(m_filename,0,sizeof(m_filename));
  STRNCPY(m_filename,sizeof(m_filename),filename,300);

  memset(m_filenametmp,0,sizeof(m_filenametmp));
  SNPRINTF(m_filenametmp,sizeof(m_filenametmp),300,"%s.tmp",m_filename);

  if ( (m_fp=FOPEN(m_filenametmp,openmode)) == 0 ) return false;

  m_bEnBuffer=bEnBuffer;

  return true;
}

// 关闭文件并改名
bool CFile::CloseAndRename()
{
  if (m_fp==0) return false;    // 判断空指针。

  fclose(m_fp);  // 关闭文件指针

  m_fp=0;

  if (rename(m_filenametmp,m_filename) != 0)
  {
    remove(m_filenametmp);
    memset(m_filename,0,sizeof(m_filename));
    memset(m_filenametmp,0,sizeof(m_filenametmp));
    return false;
  }

  memset(m_filename,0,sizeof(m_filename));
  memset(m_filenametmp,0,sizeof(m_filenametmp));

  return true;
}

// 调用fprintf向文件写入数据
void CFile::Fprintf(const char *fmt,...)
{
  if ( m_fp == 0 ) return;

  va_list arg;
  va_start( arg, fmt );
  vfprintf( m_fp, fmt, arg );
  va_end( arg );

  if ( m_bEnBuffer == false ) fflush(m_fp);
}

// 调用fgets从文件中读取一行，bDelCRT=true删除换行符，false不删除，缺省为false
bool CFile::Fgets(char *buffer,const int readsize,bool bdelcrt)
{
  if ( m_fp == 0 ) return false;

  memset(buffer,0,readsize+1);  // 调用者必须保证buffer的空间足够，否则这里会内存溢出。

  if (fgets(buffer,readsize,m_fp) == 0) return false;

  if (bdelcrt==true)
  {
    DeleteRChar(buffer,'\n'); DeleteRChar(buffer,'\r');  // 如果文件是windows格式，还要删除'\r'。
  }

  return true;
}

// 从文件文件中读取一行
// strEndStr是一行数据的结束标志，如果为空，则以换行符"\n"为结束标志。
bool CFile::FFGETS(char *buffer,const int readsize,const char *endbz)
{
  if ( m_fp == 0 ) return false;

  return FGETS(m_fp,buffer,readsize,endbz);
}

// 调用fread从文件中读取数据。
size_t CFile::Fread(void *ptr, size_t size)
{
  if ( m_fp == 0 ) return -1;

  return fread(ptr,1,size,m_fp);
}

// 调用fwrite向文件中写数据
size_t CFile::Fwrite(const void *ptr, size_t size )
{
  if ( m_fp == 0 ) return -1;

  size_t tt=fwrite(ptr,1,size,m_fp);

  if ( m_bEnBuffer == false ) fflush(m_fp);

  return tt;
}


// 从文本文件中读取一行。
// fp：已打开的文件指针。
// buffer：用于存放读取的内容。
// readsize：本次打算读取的字节数，如果已经读取到了结束标志，函数返回。
// endbz：行内容结束的标志，缺省为空，表示行内容以"\n"为结束标志。
// 返回值：true-成功；false-失败，一般情况下，失败可以认为是文件已结束。
bool FGETS(const FILE *fp,char *buffer,const int readsize,const char *endbz)
{
  if ( fp == 0 ) return false;

  memset(buffer,0,readsize+1);   // 调用者必须保证buffer的空间足够，否则这里会内存溢出。

  char strline[readsize+1];

  while (true)
  {
    memset(strline,0,sizeof(strline));

    if (fgets(strline,readsize,(FILE *)fp) == 0) break;

    // 防止buffer溢出
    if ( (strlen(buffer)+strlen(strline)) >= (unsigned int)readsize ) break;

    strcat(buffer,strline);

    if (endbz == 0) return true;
    else if (strstr(strline,endbz)!= 0) return true;
    // 以上代码可以不用strstr，可以优化为只比较最后的内容。
  }

  return false;
}


CCmdStr::CCmdStr()
{
  m_vCmdStr.clear();
}

CCmdStr::CCmdStr(const std::string &buffer,const char *sepstr,const bool bdelspace)
{
  m_vCmdStr.clear();

  SplitToCmd(buffer,sepstr,bdelspace);
}

// 把字符串拆分到m_vCmdStr容器中。
// buffer：待拆分的字符串。
// sepstr：buffer字符串中字段内容的分隔符，注意，分隔符是字符串，如","、" "、"|"、"~!~"。
// bdelspace：是否删除拆分后的字段内容前后的空格，true-删除；false-不删除，缺省不删除。
void CCmdStr::SplitToCmd(const std::string &buffer,const char *sepstr,const bool bdelspace)
{
  // 清除所有的旧数据
  m_vCmdStr.clear();

  int iPOS=0;
  std::string srcstr,substr;

  srcstr=buffer;

  while ( (iPOS=srcstr.find(sepstr)) >= 0)
  {
    substr=srcstr.substr(0,iPOS);

    if (bdelspace == true)
    {
      char str[substr.length()+1];
      STRCPY(str,sizeof(str),substr.c_str());

      DeleteLRChar(str,' ');

      substr=str;
    }

    m_vCmdStr.push_back(substr);

    iPOS=iPOS+strlen(sepstr);

    srcstr=srcstr.substr(iPOS,srcstr.size()-iPOS);

  }

  substr=srcstr;

  if (bdelspace == true)
  {
    char str[substr.length()+1];
    STRCPY(str,sizeof(str),substr.c_str());

    DeleteLRChar(str,' ');

    substr=str;
  }

  m_vCmdStr.push_back(substr);

  return;
}

int CCmdStr::CmdCount()
{
  return m_vCmdStr.size();
}

bool CCmdStr::GetValue(const int inum,char *value,const int ilen)
{
  if ( (inum>=(int)m_vCmdStr.size()) || (value==0) ) return false;

  if (ilen>0) memset(value,0,ilen+1);   // 调用者必须保证value的空间足够，否则这里会内存溢出。

  if ( (m_vCmdStr[inum].length()<=(unsigned int)ilen) || (ilen==0) )
  {
    strcpy(value,m_vCmdStr[inum].c_str());
  }
  else
  {
    strncpy(value,m_vCmdStr[inum].c_str(),ilen); value[ilen]=0;
  }

  return true;
}

bool CCmdStr::GetValue(const int inum,int *value)
{
  if ( (inum>=(int)m_vCmdStr.size()) || (value==0) ) return false;

  (*value) = 0;

  if (inum >= (int)m_vCmdStr.size()) return false;

  (*value) = atoi(m_vCmdStr[inum].c_str());

  return true;
}

bool CCmdStr::GetValue(const int inum,unsigned int *value)
{
  if ( (inum>=(int)m_vCmdStr.size()) || (value==0) ) return false;

  (*value) = 0;

  if (inum >= (int)m_vCmdStr.size()) return false;

  (*value) = atoi(m_vCmdStr[inum].c_str());

  return true;
}


bool CCmdStr::GetValue(const int inum,long *value)
{
  if ( (inum>=(int)m_vCmdStr.size()) || (value==0) ) return false;

  (*value) = 0;

  if (inum >= (int)m_vCmdStr.size()) return false;

  (*value) = atol(m_vCmdStr[inum].c_str());

  return true;
}

bool CCmdStr::GetValue(const int inum,unsigned long *value)
{
  if ( (inum>=(int)m_vCmdStr.size()) || (value==0) ) return false;

  (*value) = 0;

  if (inum >= (int)m_vCmdStr.size()) return false;

  (*value) = atol(m_vCmdStr[inum].c_str());

  return true;
}

bool CCmdStr::GetValue(const int inum,double *value)
{
  if ( (inum>=(int)m_vCmdStr.size()) || (value==0) ) return false;

  (*value) = 0;

  if (inum >= (int)m_vCmdStr.size()) return false;

  (*value) = (double)atof(m_vCmdStr[inum].c_str());

  return true;
}

bool CCmdStr::GetValue(const int inum,bool *value)
{
  if ( (inum>=(int)m_vCmdStr.size()) || (value==0) ) return false;

  (*value) = 0;

  if (inum >= (int)m_vCmdStr.size()) return false;

  char strTemp[11];
  memset(strTemp,0,sizeof(strTemp));
  strncpy(strTemp,m_vCmdStr[inum].c_str(),10);

  ToUpper(strTemp);  // 转换为大写来判断。
  if (strcmp(strTemp,"TRUE")==0) (*value)=true;

  return true;
}

CCmdStr::~CCmdStr()
{
  m_vCmdStr.clear();
}

bool GetXMLBuffer(const char *xmlbuffer,const char *fieldname,char *value,const int ilen)
{
  if (value==0) return false;    // 判断空指针。

  if (ilen>0) memset(value,0,ilen+1);   // 调用者必须保证value的空间足够，否则这里会内存溢出。

  char *start=0,*end=0;
  char m_SFieldName[51],m_EFieldName[51];

  int m_NameLen = strlen(fieldname);

  SNPRINTF(m_SFieldName,sizeof(m_SFieldName),50,"<%s>",fieldname);
  SNPRINTF(m_EFieldName,sizeof(m_EFieldName),50,"</%s>",fieldname);

  start=0; end=0;

  start = (char *)strstr(xmlbuffer,m_SFieldName);

  if (start != 0)
  {
    end   = (char *)strstr(start,m_EFieldName);
  }

  if ((start==0) || (end == 0))
  {
    return false;
  }

  int   m_ValueLen = end - start - m_NameLen - 2 ;

  if ( ((m_ValueLen) <= ilen) || (ilen == 0) )
  {
    strncpy(value,start+m_NameLen+2,m_ValueLen); value[m_ValueLen]=0;
  }
  else
  {
    strncpy(value,start+m_NameLen+2,ilen); value[ilen]=0;
  }

  DeleteLRChar(value,' ');

  return true;
}

bool GetXMLBuffer(const char *xmlbuffer,const char *fieldname,bool *value)
{
  if (value==0) return false;    // 判断空指针。

  (*value) = false;

  char strTemp[51];

  memset(strTemp,0,sizeof(strTemp));

  if (GetXMLBuffer(xmlbuffer,fieldname,strTemp,10) == true)
  {
    ToUpper(strTemp);  // 转换为大写来判断。
    if (strcmp(strTemp,"TRUE")==0) { (*value)=true; return true; }
  }

  return false;
}

bool GetXMLBuffer(const char *xmlbuffer,const char *fieldname,int *value)
{
  if (value==0) return false;    // 判断空指针。

  (*value) = 0;

  char strTemp[51];

  memset(strTemp,0,sizeof(strTemp));

  if (GetXMLBuffer(xmlbuffer,fieldname,strTemp,50) == true)
  {
    //  20220508  兼容16进制字符串转换
//    (*value) = atoi(strTemp);
    if ( (strTemp[0]=='0') && (strTemp[1]=='x'||strTemp[1]=='X')){
      (*value) = strtol(strTemp, nullptr, 16);
    } else{
      (*value) = strtol(strTemp, nullptr, 10);
    }
     return true;
  }

  return false;
}

bool GetXMLBuffer(const char *xmlbuffer,const char *fieldname,unsigned int *value)
{
  if (value==0) return false;    // 判断空指针。

  (*value) = 0;

  char strTemp[51];

  memset(strTemp,0,sizeof(strTemp));

  if (GetXMLBuffer(xmlbuffer,fieldname,strTemp,50) == true)
  {
    (*value) = (unsigned int)atoi(strTemp); return true;
  }

  return false;
}

bool GetXMLBuffer(const char *xmlbuffer,const char *fieldname,long *value)
{
  if (value==0) return false;    // 判断空指针。

  (*value) = 0;

  char strTemp[51];

  memset(strTemp,0,sizeof(strTemp));

  if (GetXMLBuffer(xmlbuffer,fieldname,strTemp,50) == true)
  {
    (*value) = atol(strTemp); return true;
  }

  return false;
}

bool GetXMLBuffer(const char *xmlbuffer,const char *fieldname,unsigned long *value)
{
  if (value==0) return false;    // 判断空指针。

  (*value) = 0;

  char strTemp[51];

  memset(strTemp,0,sizeof(strTemp));

  if (GetXMLBuffer(xmlbuffer,fieldname,strTemp,50) == true)
  {
    (*value) = (unsigned long)atol(strTemp); return true;
  }

  return false;
}

bool GetXMLBuffer(const char *xmlbuffer,const char *fieldname,double *value)
{
  if (value==0) return false;    // 判断空指针。

  (*value) = 0;

  char strTemp[51];

  memset(strTemp,0,sizeof(strTemp));

  if (GetXMLBuffer(xmlbuffer,fieldname,strTemp,50) == true)
  {
    (*value) = atof(strTemp); return true;
  }

  return false;
}

// 把整数表示的时间转换为字符串表示的时间。
// ltime：整数表示的时间。
// stime：字符串表示的时间。
// fmt：输出字符串时间stime的格式，与LocalTime函数的fmt参数相同，如果fmt的格式不正确，stime将为空。
void timetostr(const time_t ltime,char *stime,const char *fmt)
{
  if (stime==0) return;    // 判断空指针。

  strcpy(stime,"");

  struct tm sttm = *localtime ( &ltime );
  // struct tm sttm; localtime_r(&ltime,&sttm); 

  sttm.tm_year=sttm.tm_year+1900;
  sttm.tm_mon++;

  if (fmt==0)
  {
    snprintf(stime,20,"%04u-%02u-%02u %02u:%02u:%02u",sttm.tm_year,
                    sttm.tm_mon,sttm.tm_mday,sttm.tm_hour,
                    sttm.tm_min,sttm.tm_sec);
    return;
  }

  if (strcmp(fmt,"yyyy-mm-dd hh24:mi:ss") == 0)
  {
    snprintf(stime,20,"%04u-%02u-%02u %02u:%02u:%02u",sttm.tm_year,
                    sttm.tm_mon,sttm.tm_mday,sttm.tm_hour,
                    sttm.tm_min,sttm.tm_sec);
    return;
  }

  if (strcmp(fmt,"yyyy-mm-dd hh24:mi") == 0)
  {
    snprintf(stime,17,"%04u-%02u-%02u %02u:%02u",sttm.tm_year,
                    sttm.tm_mon,sttm.tm_mday,sttm.tm_hour,
                    sttm.tm_min);
    return;
  }

  if (strcmp(fmt,"yyyy-mm-dd hh24") == 0)
  {
    snprintf(stime,14,"%04u-%02u-%02u %02u",sttm.tm_year,
                    sttm.tm_mon,sttm.tm_mday,sttm.tm_hour);
    return;
  }

  if (strcmp(fmt,"yyyy-mm-dd") == 0)
  {
    snprintf(stime,11,"%04u-%02u-%02u",sttm.tm_year,sttm.tm_mon,sttm.tm_mday);
    return;
  }

  if (strcmp(fmt,"yyyy-mm") == 0)
  {
    snprintf(stime,8,"%04u-%02u",sttm.tm_year,sttm.tm_mon);
    return;
  }

  if (strcmp(fmt,"yyyymmddhh24miss") == 0)
  {
    snprintf(stime,15,"%04u%02u%02u%02u%02u%02u",sttm.tm_year,
                    sttm.tm_mon,sttm.tm_mday,sttm.tm_hour,
                    sttm.tm_min,sttm.tm_sec);
    return;
  }

  if (strcmp(fmt,"yyyymmddhh24mi") == 0)
  {
    snprintf(stime,13,"%04u%02u%02u%02u%02u",sttm.tm_year,
                    sttm.tm_mon,sttm.tm_mday,sttm.tm_hour,
                    sttm.tm_min);
    return;
  }

  if (strcmp(fmt,"yyyymmddhh24") == 0)
  {
    snprintf(stime,11,"%04u%02u%02u%02u",sttm.tm_year,
                    sttm.tm_mon,sttm.tm_mday,sttm.tm_hour);
    return;
  }

  if (strcmp(fmt,"yyyymmdd") == 0)
  {
    snprintf(stime,9,"%04u%02u%02u",sttm.tm_year,sttm.tm_mon,sttm.tm_mday);
    return;
  }

  if (strcmp(fmt,"hh24miss") == 0)
  {
    snprintf(stime,7,"%02u%02u%02u",sttm.tm_hour,sttm.tm_min,sttm.tm_sec);
    return;
  }

  if (strcmp(fmt,"hh24mi") == 0)
  {
    snprintf(stime,5,"%02u%02u",sttm.tm_hour,sttm.tm_min);
    return;
  }

  if (strcmp(fmt,"hh24") == 0)
  {
    snprintf(stime,3,"%02u",sttm.tm_hour);
    return;
  }

  if (strcmp(fmt,"mi") == 0)
  {
    snprintf(stime,3,"%02u",sttm.tm_min);
    return;
  }
}

void LocalTime(char *stime,const char *fmt,const int timetvl)
{
  if (stime==0) return;    // 判断空指针。

  time_t  timer;

  time( &timer ); timer=timer+timetvl;

  timetostr(timer,stime,fmt);
}


CLogFile::CLogFile(const long MaxLogSize)
{
  m_tracefp = 0;
  memset(m_filename,0,sizeof(m_filename));
  memset(m_openmode,0,sizeof(m_openmode));
  m_bBackup=true;
  m_bEnBuffer=false;
  m_MaxLogSize=MaxLogSize;
  if (m_MaxLogSize<10) m_MaxLogSize=10;

  // pthread_pin_init(&spin,0);  // 初学暂时不要关心这行代码。
}

CLogFile::~CLogFile()
{
  Close();

  // pthread_spin_destroy(&spin);  // 初学暂时不要关心这行代码。
}

void CLogFile::Close()
{
  if (m_tracefp != 0) { fclose(m_tracefp); m_tracefp=0; }

  memset(m_filename,0,sizeof(m_filename));
  memset(m_openmode,0,sizeof(m_openmode));
  m_bBackup=true;
  m_bEnBuffer=false;
}

bool CLogFile::Open(const char *filename,const char *openmode,bool bBackup,bool bEnBuffer)
{
  // 如果文件指针是打开的状态，先关闭它。
  Close();

  STRCPY(m_filename,sizeof(m_filename),filename);
  m_bEnBuffer=bEnBuffer;
  m_bBackup=bBackup;
  if (openmode==0) STRCPY(m_openmode,sizeof(m_openmode),"a+");
  else STRCPY(m_openmode,sizeof(m_openmode),openmode);

  if ((m_tracefp=FOPEN(m_filename,m_openmode)) == 0) return false;

  return true;
}

bool CLogFile::BackupLogFile()
{
  if (m_tracefp == 0) return false;

  // 不备份
  if (m_bBackup == false) return true;

  //fseek(m_tracefp,0,2);

  if (ftell(m_tracefp) > m_MaxLogSize*1024*1024)
  {
    fclose(m_tracefp); m_tracefp=0;

    char strLocalTime[21];
    memset(strLocalTime,0,sizeof(strLocalTime));
    LocalTime(strLocalTime,"yyyymmddhh24miss");

    char bak_filename[301];
    SNPRINTF(bak_filename,sizeof(bak_filename),300,"%s.%s",m_filename,strLocalTime);
    rename(m_filename,bak_filename);

    if ((m_tracefp=FOPEN(m_filename,m_openmode)) == 0) return false;
  }

  return true;
}

bool CLogFile::Write(const char *fmt,...)
{
  if (m_tracefp == 0) return false;

  // pthread_spin_lock(&spin);  // 初学暂时不要关心这行代码。

  if (BackupLogFile() == false) return false;

  char strtime[20]; LocalTime(strtime);
  va_list ap;
  va_start(ap,fmt);
  fprintf(m_tracefp,"%s ",strtime);
  vfprintf(m_tracefp,fmt,ap);
  va_end(ap);

  if (m_bEnBuffer==false) fflush(m_tracefp);

  // pthread_spin_unlock(&spin);  // 初学暂时不要关心这行代码。

  return true;
}

bool CLogFile::WriteEx(const char *fmt,...)
{
  if (m_tracefp == 0) return false;

  // pthread_spin_lock(&spin);  // 初学暂时不要关心这行代码。

  va_list ap;
  va_start(ap,fmt);
  vfprintf(m_tracefp,fmt,ap);
  va_end(ap);

  if (m_bEnBuffer==false) fflush(m_tracefp);

  // pthread_spin_unlock(&spin);  // 初学暂时不要关心这行代码。

  return true;
}

CIniFile::CIniFile()
{

}

bool CIniFile::LoadFile(const char *filename)
{
  m_xmlbuffer.clear();

  CFile File;

  if ( File.Open(filename,"r") == false) return false;

  char strLine[501];

  while (true)
  {
    memset(strLine,0,sizeof(strLine));

    if (File.FFGETS(strLine,500) == false) break;

    m_xmlbuffer=m_xmlbuffer+strLine;
  }

  if (m_xmlbuffer.length() < 10) return false;

  return true;
}

bool CIniFile::GetValue(const char *fieldname,bool   *value)
{
  return GetXMLBuffer(m_xmlbuffer.c_str(),fieldname,value);
}

bool CIniFile::GetValue(const char *fieldname,char *value,int ilen)
{
  return GetXMLBuffer(m_xmlbuffer.c_str(),fieldname,value,ilen);
}

bool CIniFile::GetValue(const char *fieldname,int *value)
{
  return GetXMLBuffer(m_xmlbuffer.c_str(),fieldname,value);
}

bool CIniFile::GetValue(const char *fieldname,unsigned int *value)
{
  return GetXMLBuffer(m_xmlbuffer.c_str(),fieldname,value);
}

bool CIniFile::GetValue(const char *fieldname,long *value)
{
  return GetXMLBuffer(m_xmlbuffer.c_str(),fieldname,value);
}

bool CIniFile::GetValue(const char *fieldname,unsigned long *value)
{
  return GetXMLBuffer(m_xmlbuffer.c_str(),fieldname,value);
}

bool CIniFile::GetValue(const char *fieldname,double *value)
{
  return GetXMLBuffer(m_xmlbuffer.c_str(),fieldname,value);
}

// 关闭全部的信号和输入输出
void CloseIOAndSignal(bool bCloseIO)
{
  int ii=0;

  for (ii=0;ii<64;ii++)
  {
    if (bCloseIO==true) close(ii);

    signal(ii,SIG_IGN);
  }
}

bool MKDIR(const char *filename,bool bisfilename)
{
  // 检查目录是否存在，如果不存在，逐级创建子目录
  char strPathName[301];

  int ilen=strlen(filename);

  for (int ii=1; ii<ilen;ii++)
  {
    if (filename[ii] != '/') continue;

    STRNCPY(strPathName,sizeof(strPathName),filename,ii);

    if (access(strPathName,F_OK) == 0) continue; // 如果目录已存在，continue

    if (mkdir(strPathName,0755) != 0) return false;  // 如果目录不存在，创建它。
  }

  if (bisfilename==false)
  {
    if (access(filename,F_OK) != 0)
    {
      if (mkdir(filename,0755) != 0) return false;
    }
  }

  return true;
}

FILE *FOPEN(const char *filename,const char *mode)
{
  if (MKDIR(filename) == false) return 0;

  return fopen(filename,mode);
}

int FileSize(const char *filename)
{
  struct stat st_filestat;

  if (stat(filename,&st_filestat) < 0) return -1;

  return st_filestat.st_size;
}

bool UTime(const char *filename,const char *mtime)
{
  struct utimbuf stutimbuf;

  stutimbuf.actime=stutimbuf.modtime=strtotime(mtime);

  if (utime(filename,&stutimbuf)!=0) return false;

  return true;
}

time_t strtotime(const char *stime)
{
  char strtime[21],yyyy[5],mm[3],dd[3],hh[3],mi[3],ss[3];
  memset(strtime,0,sizeof(strtime));
  memset(yyyy,0,sizeof(yyyy));
  memset(mm,0,sizeof(mm));
  memset(dd,0,sizeof(dd));
  memset(hh,0,sizeof(hh));
  memset(mi,0,sizeof(mi));
  memset(ss,0,sizeof(ss));

  PickNumber(stime,strtime,false,false);

  if (strlen(strtime) != 14) return -1;

  strncpy(yyyy,strtime,4);
  strncpy(mm,strtime+4,2);
  strncpy(dd,strtime+6,2);
  strncpy(hh,strtime+8,2);
  strncpy(mi,strtime+10,2);
  strncpy(ss,strtime+12,2);

  struct tm time_str;

  time_str.tm_year = atoi(yyyy) - 1900;
  time_str.tm_mon = atoi(mm) - 1;
  time_str.tm_mday = atoi(dd);
  time_str.tm_hour = atoi(hh);
  time_str.tm_min = atoi(mi);
  time_str.tm_sec = atoi(ss);
  time_str.tm_isdst = 0;

  return mktime(&time_str);
}

bool AddTime(const char *in_stime,char *out_stime,const int timetvl,const char *fmt)
{
  if ( (in_stime==0) || (out_stime==0) ) return false;    // 判断空指针。

  time_t  timer;
  if ( (timer=strtotime(in_stime))==-1) { strcpy(out_stime,""); return false; }

  timer=timer+timetvl;

  strcpy(out_stime,"");

  timetostr(timer,out_stime,fmt);

  return true;
}

bool FileMTime(const char *filename,char *mtime,const char *fmt)
{
  // 判断文件是否存在。
  struct stat st_filestat;

  if (stat(filename,&st_filestat) < 0) return false;

  char strfmt[25];
  memset(strfmt,0,sizeof(strfmt));
  if (fmt==0) strcpy(strfmt,"yyyymmddhh24miss");
  else strcpy(strfmt,fmt);

  timetostr(st_filestat.st_mtime,mtime,strfmt);

  return true;
}


CDir::CDir()
{
  m_pos=0;

  STRCPY(m_DateFMT,sizeof(m_DateFMT),"yyyy-mm-dd hh24:mi:ss");

  m_vFileName.clear();

  initdata();
}

void CDir::initdata()
{
  memset(m_DirName,0,sizeof(m_DirName));
  memset(m_FileName,0,sizeof(m_FileName));
  memset(m_FullFileName,0,sizeof(m_FullFileName));
  m_FileSize=0;
  memset(m_CreateTime,0,sizeof(m_CreateTime));
  memset(m_ModifyTime,0,sizeof(m_ModifyTime));
  memset(m_AccessTime,0,sizeof(m_AccessTime));
}

// 设置文件时间的格式，支持"yyyy-mm-dd hh24:mi:ss"和"yyyymmddhh24miss"两种，缺省是前者。
void CDir::SetDateFMT(const char *in_DateFMT)
{
  memset(m_DateFMT,0,sizeof(m_DateFMT));
  STRCPY(m_DateFMT,sizeof(m_DateFMT),in_DateFMT);
}

bool CDir::OpenDir(const char *in_DirName,const char *in_MatchStr,const unsigned int in_MaxCount,const bool bAndChild,bool bSort)
{
  m_pos=0;
  m_vFileName.clear();

  // 如果目录不存在，就创建该目录
  if (MKDIR(in_DirName,false) == false) return false;

  bool bRet=_OpenDir(in_DirName,in_MatchStr,in_MaxCount,bAndChild);

  if (bSort==true)
  {
    sort(m_vFileName.begin(), m_vFileName.end());
  }

  return bRet;
}

// 这是一个递归函数，用于OpenDir()的调用，在CDir类的外部不需要调用它。
bool CDir::_OpenDir(const char *in_DirName,const char *in_MatchStr,const unsigned int in_MaxCount,const bool bAndChild)
{
  DIR *dir;

  if ( (dir=opendir(in_DirName)) == 0 ) return false;

  char strTempFileName[3001];

  struct dirent *st_fileinfo;
  struct stat st_filestat;

  while ((st_fileinfo=readdir(dir)) != 0)
  {
    // 以"."打头的文件不处理
    if (st_fileinfo->d_name[0]=='.') continue;

    SNPRINTF(strTempFileName,sizeof(strTempFileName),300,"%s//%s",in_DirName,st_fileinfo->d_name);

    UpdateStr(strTempFileName,"//","/");

    stat(strTempFileName,&st_filestat);

    // 判断是否是目录，如果是，处理各级子目录。
    if (S_ISDIR(st_filestat.st_mode))
    {
      if (bAndChild == true)
      {
        if (_OpenDir(strTempFileName,in_MatchStr,in_MaxCount,bAndChild) == false)
        {
          closedir(dir); return false;
        }
      }
    }
    else
    {
      // 如果是文件，把能匹配上的文件放入m_vFileName容器中。
      if (MatchStr(st_fileinfo->d_name,in_MatchStr) == false) continue;

      m_vFileName.push_back(strTempFileName);

      if ( m_vFileName.size()>=in_MaxCount ) break;
    }
  }

  closedir(dir);

  return true;
}

/*
st_gid 
  Numeric identifier of group that owns file (UNIX-specific) This field will always be zero on NT systems. A redirected file is classified as an NT file.
st_atime
  Time of last access of file.
st_ctime
  Time of creation of file.
st_dev
  Drive number of the disk containing the file (same as st_rdev).
st_ino
  Number of the information node (the inode) for the file (UNIX-specific). On UNIX file systems, the inode describes the file date and time stamps, permissions, and content. When files are hard-linked to one another, they share the same inode. The inode, and therefore st_ino, has no meaning in the FAT, HPFS, or NTFS file systems.
st_mode
  Bit mask for file-mode information. The _S_IFDIR bit is set if path specifies a directory; the _S_IFREG bit is set if path specifies an ordinary file or a device. User read/write bits are set according to the file’s permission mode; user execute bits are set according to the filename extension.
st_mtime
  Time of last modification of file.
st_nlink
  Always 1 on non-NTFS file systems.
st_rdev
  Drive number of the disk containing the file (same as st_dev).
st_size
  Size of the file in bytes; a 64-bit integer for _stati64 and _wstati64
st_uid
  Numeric identifier of user who owns file (UNIX-specific). This field will always be zero on NT systems. A redirected file is classified as an NT file.
*/

// 从m_vFileName容器中获取一条记录（文件名），同时得到该文件的大小、修改时间等信息。
// 调用OpenDir方法时，m_vFileName容器被清空，m_pos归零，每调用一次ReadDir方法m_pos加1。
// 当m_pos小于m_vFileName.size()，返回true，否则返回false。
bool CDir::ReadDir()
{
  initdata();

  int ivsize=m_vFileName.size();

  // 如果已读完，清空容器
  if (m_pos >= ivsize)
  {
    m_pos=0; m_vFileName.clear(); return false;
  }

  int pos=0;

  pos=m_vFileName[m_pos].find_last_of("/");

  // 目录名
  STRCPY(m_DirName,sizeof(m_DirName),m_vFileName[m_pos].substr(0,pos).c_str());

  // 文件名
  STRCPY(m_FileName,sizeof(m_FileName),m_vFileName[m_pos].substr(pos+1,m_vFileName[m_pos].size()-pos-1).c_str());

  // 文件全名，包括路径
  SNPRINTF(m_FullFileName,sizeof(m_FullFileName),300,"%s",m_vFileName[m_pos].c_str());

  struct stat st_filestat;

  stat(m_FullFileName,&st_filestat);

  m_FileSize=st_filestat.st_size;

  struct tm nowtimer;

  if (strcmp(m_DateFMT,"yyyy-mm-dd hh24:mi:ss") == 0)
  {
    nowtimer = *localtime(&st_filestat.st_mtime);
    // localtime_r(&st_filestat.st_mtime,&nowtimer); 
    nowtimer.tm_mon++;
    snprintf(m_ModifyTime,20,"%04u-%02u-%02u %02u:%02u:%02u",\
             nowtimer.tm_year+1900,nowtimer.tm_mon,nowtimer.tm_mday,\
             nowtimer.tm_hour,nowtimer.tm_min,nowtimer.tm_sec);

    nowtimer = *localtime(&st_filestat.st_ctime);
    // localtime_r(&st_filestat.st_ctime,&nowtimer); 
    nowtimer.tm_mon++;
    snprintf(m_CreateTime,20,"%04u-%02u-%02u %02u:%02u:%02u",\
             nowtimer.tm_year+1900,nowtimer.tm_mon,nowtimer.tm_mday,\
             nowtimer.tm_hour,nowtimer.tm_min,nowtimer.tm_sec);

    nowtimer = *localtime(&st_filestat.st_atime);
    // localtime_r(&st_filestat.st_atime,&nowtimer); 
    nowtimer.tm_mon++;
    snprintf(m_AccessTime,20,"%04u-%02u-%02u %02u:%02u:%02u",\
             nowtimer.tm_year+1900,nowtimer.tm_mon,nowtimer.tm_mday,\
             nowtimer.tm_hour,nowtimer.tm_min,nowtimer.tm_sec);
  }

  if (strcmp(m_DateFMT,"yyyymmddhh24miss") == 0)
  {
    nowtimer = *localtime(&st_filestat.st_mtime);
    // localtime_r(&st_filestat.st_mtime,&nowtimer); 
    nowtimer.tm_mon++;
    snprintf(m_ModifyTime,20,"%04u%02u%02u%02u%02u%02u",\
             nowtimer.tm_year+1900,nowtimer.tm_mon,nowtimer.tm_mday,\
             nowtimer.tm_hour,nowtimer.tm_min,nowtimer.tm_sec);

    nowtimer = *localtime(&st_filestat.st_ctime);
    // localtime_r(&st_filestat.st_ctime,&nowtimer); 
    nowtimer.tm_mon++;
    snprintf(m_CreateTime,20,"%04u%02u%02u%02u%02u%02u",\
             nowtimer.tm_year+1900,nowtimer.tm_mon,nowtimer.tm_mday,\
             nowtimer.tm_hour,nowtimer.tm_min,nowtimer.tm_sec);

    nowtimer = *localtime(&st_filestat.st_atime);
    // localtime_r(&st_filestat.st_atime,&nowtimer); 
    nowtimer.tm_mon++;
    snprintf(m_AccessTime,20,"%04u%02u%02u%02u%02u%02u",\
             nowtimer.tm_year+1900,nowtimer.tm_mon,nowtimer.tm_mday,\
             nowtimer.tm_hour,nowtimer.tm_min,nowtimer.tm_sec);
  }

  m_pos++;

  return true;
}

CDir::~CDir()
{
  m_vFileName.clear();

  // m_vDirName.clear();
}

bool REMOVE(const char *filename,const int times)
{
  // 如果文件不存在，直接返回失败
  if (access(filename,R_OK) != 0) return false;

  for (int ii=0;ii<times;ii++)
  {
    if (remove(filename) == 0) return true;

    usleep(100000);
  }

  return false;
}

bool RENAME(const char *srcfilename,const char *dstfilename,const int times)
{
  // 如果文件不存在，直接返回失败
  if (access(srcfilename,R_OK) != 0) return false;

  if (MKDIR(dstfilename) == false) return false;

  for (int ii=0;ii<times;ii++)
  {
    if (rename(srcfilename,dstfilename) == 0) return true;

    usleep(100000);
  }

  return false;
}


CTcpClient::CTcpClient()
{
  m_conn_fd=-1;
  memset(m_ip,0,sizeof(m_ip));
  m_port=0;
  m_b_timeout=false;
}

bool CTcpClient::ConnectToServer(const char *ip,const int port)
{
  // 如果已连接到服务端，则断开，这种处理方法没有特别的原因，不要纠结。
  if (m_conn_fd!=-1) { close(m_conn_fd); m_conn_fd=-1; }

  // 忽略SIGPIPE信号，防止程序异常退出。
  // 如果send到一个disconnected socket上，内核就会发出SIGPIPE信号。这个信号
  // 的缺省处理方法是终止进程，大多数时候这都不是我们期望的。我们重新定义这
  // 个信号的处理方法，大多数情况是直接屏蔽它。
  signal(SIGPIPE,SIG_IGN);

  STRCPY(m_ip,sizeof(m_ip),ip);
  m_port=port;

  struct hostent* h;
  struct sockaddr_in servaddr;

  if ( (m_conn_fd = socket(AF_INET,SOCK_STREAM,0) ) < 0) return false;

  if ( !(h = gethostbyname(m_ip)) )
  {
    close(m_conn_fd);  m_conn_fd=-1; return false;
  }

  memset(&servaddr,0,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(m_port);  // 指定服务端的通讯端口
  memcpy(&servaddr.sin_addr,h->h_addr,h->h_length);

  if (connect(m_conn_fd, (struct sockaddr *)&servaddr,sizeof(servaddr)) != 0)
  {
    close(m_conn_fd);  m_conn_fd=-1; return false;
  }

  return true;
}

// 接收服务端发送过来的数据。
// buffer：接收数据缓冲区的地址，数据的长度存放在m_buflen成员变量中。
// itimeout：等待数据的超时时间，单位：秒，缺省值是0-无限等待。
// 返回值：true-成功；false-失败，失败有两种情况：1）等待超时，成员变量m_btimeout的值被设置为true；2）socket连接已不可用。
bool CTcpClient::Read(char *buffer,const int itimeout)
{
  if (m_conn_fd==-1) return false;

  // 如果itimeout>0，表示需要等待itimeout秒，如果itimeout秒后还没有数据到达，返回false。
  if (itimeout>0)
  {
    struct pollfd fds;
    fds.fd=m_conn_fd;
    fds.events=POLLIN;
    int iret;
    m_b_timeout=false;
    if ( (iret=poll(&fds,1,itimeout*1000)) <= 0 )
    {
      if (iret==0) m_b_timeout = true;
      return false;
    }
  }

  m_buff_len = 0;
  return (TcpRead(m_conn_fd,buffer,&m_buff_len));
}

bool CTcpClient::Write(const char *buffer,const int ibuflen)
{
  if (m_conn_fd==-1) return false;

  int ilen=ibuflen;

  if (ibuflen==0) ilen=strlen(buffer);

  return(TcpWrite(m_conn_fd,buffer,ilen));
}

void CTcpClient::Close()
{
  if (m_conn_fd > 0) close(m_conn_fd);

  m_conn_fd=-1;
  memset(m_ip,0,sizeof(m_ip));
  m_port=0;
  m_b_timeout=false;
}

CTcpClient::~CTcpClient()
{
  Close();
}

CTcpServer::CTcpServer()
{
  m_listen_fd=-1;
  m_conn_fd=-1;
  m_socklen=0;
  m_b_timeout=false;
}

bool CTcpServer::InitServer(const unsigned int port,const int backlog)
{
  // 如果服务端的socket>0，关掉它，这种处理方法没有特别的原因，不要纠结。
  if (m_listen_fd > 0) { close(m_listen_fd); m_listen_fd=-1; }

  if ( (m_listen_fd = socket(AF_INET,SOCK_STREAM,0))<=0) return false;

  // 忽略SIGPIPE信号，防止程序异常退出。
  signal(SIGPIPE,SIG_IGN);

  // 打开SO_REUSEADDR选项，当服务端连接处于TIME_WAIT状态时可以再次启动服务器，
  // 否则bind()可能会不成功，报：Address already in use。
  //char opt = 1; unsigned int len = sizeof(opt);
  int opt = 1; unsigned int len = sizeof(opt);
  setsockopt(m_listen_fd,SOL_SOCKET,SO_REUSEADDR,&opt,len);

  memset(&m_serv_addr,0,sizeof(m_serv_addr));
  m_serv_addr.sin_family = AF_INET;
  m_serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);   // 任意ip地址。
  m_serv_addr.sin_port = htons(port);
  if (bind(m_listen_fd,(struct sockaddr *)&m_serv_addr,sizeof(m_serv_addr)) != 0 )
  {
    CloseListen(); return false;
  }

  if (listen(m_listen_fd,backlog) != 0 )
  {
    CloseListen(); return false;
  }

  return true;
}

bool CTcpServer::Accept()
{
  if (m_listen_fd==-1) return false;

  m_socklen = sizeof(struct sockaddr_in);

  if ((m_conn_fd=accept(m_listen_fd,(struct sockaddr *)&m_client_addr,(socklen_t*)&m_socklen)) < 0)
      return false;

  return true;
}

char *CTcpServer::GetIP()
{
  return(inet_ntoa(m_client_addr.sin_addr));
}

bool CTcpServer::Read(char *buffer,const int itimeout)
{
  if (m_conn_fd==-1) return false;

  // 如果itimeout>0，表示需要等待itimeout秒，如果itimeout秒后还没有数据到达，返回false。
  if (itimeout>0)
  {
    struct pollfd fds;
    fds.fd=m_conn_fd;
    fds.events=POLLIN;
    m_b_timeout=false;
    int iret;
    if ( (iret=poll(&fds,1,itimeout*1000)) <= 0 )
    {
      if (iret==0) m_b_timeout = true;
      return false;
    }
  }

  m_buff_len = 0;
  return(TcpRead(m_conn_fd,buffer,&m_buff_len));
}

bool CTcpServer::Write(const char *buffer,const int ibuflen)
{
  if (m_conn_fd==-1) return false;

  int ilen = ibuflen;
  if (ilen==0) ilen=strlen(buffer);

  return(TcpWrite(m_conn_fd,buffer,ilen));
}

void CTcpServer::CloseListen()
{
  if (m_listen_fd > 0)
  {
    close(m_listen_fd); m_listen_fd=-1;
  }
}

void CTcpServer::CloseClient()
{
  if (m_conn_fd > 0)
  {
    close(m_conn_fd); m_conn_fd=-1;
  }
}

CTcpServer::~CTcpServer()
{
  CloseListen(); CloseClient();
}

bool TcpRead(const int sockfd,char *buffer,int *ibuflen,const int itimeout)
{
  if (sockfd==-1) return false;

  // 如果itimeout>0，表示需要等待itimeout秒，如果itimeout秒后还没有数据到达，返回false。
  if (itimeout>0)
  {
    struct pollfd fds;
    fds.fd=sockfd;
    fds.events=POLLIN;
    if ( poll(&fds,1,itimeout*1000) <= 0 ) return false;
  }

  // 如果itimeout==-1，表示不等待，立即判断socket的缓冲区中是否有数据，如果没有，返回false。
  if (itimeout==-1)
  {
    struct pollfd fds;
    fds.fd=sockfd;
    fds.events=POLLIN;
    if ( poll(&fds,1,0) <= 0 ) return false;
  }

  (*ibuflen) = 0;  // 报文长度变量初始化为0。

  // 先读取报文长度，4个字节。
  if (Readn(sockfd,(char*)ibuflen,4) == false) return false;

  (*ibuflen)=ntohl(*ibuflen);  // 把报文长度由网络字节序转换为主机字节序。

  // 再读取报文内容。
  if (Readn(sockfd,buffer,(*ibuflen)) == false) return false;

  return true;
}

bool TcpWrite(const int sockfd,const char *buffer,const int ibuflen)
{
  if (sockfd==-1) return false;

  int ilen=0;  // 报文长度。

  // 如果ibuflen==0，就认为需要发送的是字符串，报文长度为字符串的长度。
  if (ibuflen==0) ilen=strlen(buffer);
  else ilen=ibuflen;

  int ilenn=htonl(ilen);    // 把报文长度转换为网络字节序。

  char TBuffer[ilen+4];     // 发送缓冲区。
  memset(TBuffer,0,sizeof(TBuffer));  // 清区发送缓冲区。
  memcpy(TBuffer,&ilenn,4);           // 把报文长度拷贝到缓冲区。
  memcpy(TBuffer+4,buffer,ilen);      // 把报文内容拷贝到缓冲区。

  // 发送缓冲区中的数据。
  if (Writen(sockfd,TBuffer,ilen+4) == false) return false;

  return true;
}

bool Readn(const int sockfd,char *buffer,const size_t n)
{
  int nLeft=n;  // 剩余需要读取的字节数。
  int idx=0;    // 已成功读取的字节数。
  int nread;    // 每次调用recv()函数读到的字节数。

  while(nLeft > 0)
  {
    if ( (nread=recv(sockfd,buffer+idx,nLeft,0)) <= 0) return false;

    idx=idx+nread;
    nLeft=nLeft-nread;
  }

  return true;
}

bool Writen(const int sockfd,const char *buffer,const size_t n)
{
  int nLeft=n;  // 剩余需要写入的字节数。
  int idx=0;    // 已成功写入的字节数。
  int nwritten; // 每次调用send()函数写入的字节数。

  while(nLeft > 0 )
  {
    if ( (nwritten=send(sockfd,buffer+idx,nLeft,0)) <= 0) return false;

    nLeft=nLeft-nwritten;
    idx=idx+nwritten;
  }

  return true;
}

bool COPY(const char *srcfilename,const char *dstfilename)
{
  if (MKDIR(dstfilename) == false) return false;

  char strdstfilenametmp[301];
  SNPRINTF(strdstfilenametmp,sizeof(strdstfilenametmp),300,"%s.tmp",dstfilename);

  int  srcfd,dstfd;

  srcfd=dstfd=-1;

  int iFileSize=FileSize(srcfilename);

  int  bytes=0;
  int  total_bytes=0;
  int  onread=0;
  char buffer[5000];

  if ( (srcfd=open(srcfilename,O_RDONLY)) < 0 ) return false;

  if ( (dstfd=open(strdstfilenametmp,O_WRONLY|O_CREAT|O_TRUNC,S_IWUSR|S_IRUSR|S_IXUSR)) < 0) { close(srcfd); return false; }

  while (true)
  {
    memset(buffer,0,sizeof(buffer));

    if ((iFileSize-total_bytes) > 5000) onread=5000;
    else onread=iFileSize-total_bytes;

    bytes=read(srcfd,buffer,onread);

    if (bytes > 0) write(dstfd,buffer,bytes);

    total_bytes = total_bytes + bytes;

    if (total_bytes == iFileSize) break;
  }

  close(srcfd);

  close(dstfd);

  // 更改文件的修改时间属性
  char strmtime[21];
  memset(strmtime,0,sizeof(strmtime));
  FileMTime(srcfilename,strmtime);
  UTime(strdstfilenametmp,strmtime);

  if (RENAME(strdstfilenametmp,dstfilename) == false) { REMOVE(strdstfilenametmp); return false; }

  return true;
}


CTimer::CTimer()
{
  memset(&m_start,0,sizeof(struct timeval));
  memset(&m_end,0,sizeof(struct timeval));

  // 开始计时
  Start();
}

// 开始计时
void CTimer::Start()
{
  gettimeofday( &m_start, 0 );
}

// 计算已逝去的时间，单位：秒，小数点后面是微秒
// 每调用一次本方法之后，自动调用Start方法重新开始计时。
double CTimer::Elapsed()
{

  gettimeofday( &m_end, 0 );

  double dstart,dend;

  dstart=dend=0;

  char strtemp[51];
  SNPRINTF(strtemp,sizeof(strtemp),30,"%ld.%06ld",m_start.tv_sec,m_start.tv_usec);
  dstart=atof(strtemp);

  SNPRINTF(strtemp,sizeof(strtemp),30,"%ld.%06ld",m_end.tv_sec,m_end.tv_usec);
  dend=atof(strtemp);

  // 重新开始计时
  Start();

  return dend-dstart;
}

CSEM::CSEM()
{
  m_semid=-1;
  m_sem_flg=SEM_UNDO;
}

// 如果信号量已存在，获取信号量；如果信号量不存在，则创建它并初始化为value。
bool CSEM::init(key_t key,unsigned short value,short sem_flg)
{
  if (m_semid!=-1) return false;

  m_sem_flg=sem_flg;

  // 信号量的初始化不能直接用semget(key,1,0666|IPC_CREAT)，因为信号量创建后，初始值是0。

  // 信号量的初始化分三个步骤：
  // 1）获取信号量，如果成功，函数返回。
  // 2）如果失败，则创建信号量。
  // 3) 设置信号量的初始值。

  // 获取信号量。
  if ( (m_semid=semget(key,1,0666)) == -1)
  {
    // 如果信号量不存在，创建它。
    if (errno==2)
    {
      // 用IPC_EXCL标志确保只有一个进程创建并初始化信号量，其它进程只能获取。
      if ( (m_semid=semget(key,1,0666|IPC_CREAT|IPC_EXCL)) == -1)
      {
        if (errno!=EEXIST)
        {
          perror("init 1 semget()"); return false;
        }
        if ( (m_semid=semget(key,1,0666)) == -1)
        { perror("init 2 semget()"); return false; }

        return true;
      }

      // 信号量创建成功后，还需要把它初始化成value。
      union semun sem_union;
      sem_union.val = value;   // 设置信号量的初始值。
      if (semctl(m_semid,0,SETVAL,sem_union) <  0) { perror("init semctl()"); return false; }
    }
    else
    { perror("init 3 semget()"); return false; }
  }

  return true;
}

bool CSEM::P(short sem_op)
{
  if (m_semid==-1) return false;

  struct sembuf sem_b;
  sem_b.sem_num = 0;      // 信号量编号，0代表第一个信号量。
  sem_b.sem_op = sem_op;  // P操作的sem_op必须小于0。
  sem_b.sem_flg = m_sem_flg;
  if (semop(m_semid,&sem_b,1) == -1) { perror("p semop()"); return false; }

  return true;
}

bool CSEM::V(short sem_op)
{
  if (m_semid==-1) return false;

  struct sembuf sem_b;
  sem_b.sem_num = 0;      // 信号量编号，0代表第一个信号量。
  sem_b.sem_op = sem_op;  // V操作的sem_op必须大于0。
  sem_b.sem_flg = m_sem_flg;
  if (semop(m_semid,&sem_b,1) == -1) { perror("V semop()"); return false; }

  return true;
}

// 获取信号量的值，成功返回信号量的值，失败返回-1。
int CSEM::value()
{
  return semctl(m_semid,0,GETVAL);
}

bool CSEM::destroy()
{
  if (m_semid==-1) return false;

  if (semctl(m_semid,0,IPC_RMID) == -1) { perror("destroy semctl()"); return false; }

  return true;
}

CSEM::~CSEM()
{
}

CPActive::CPActive()
{
  m_shmid=0;
  m_pos=-1;
  m_shm=0;
}

// 把当前进程的心跳信息加入共享内存进程组中。
bool CPActive::AddPInfo(const int timeout,const char *pname,CLogFile *logfile)
{
  if (m_pos!=-1) return true;

  if (m_sem.init(SEMKEYP) == false)  // 初始化信号量。
  {
    if (logfile!=0) logfile->Write("创建/获取信号量(%x)失败。\n",SEMKEYP);
    else printf("创建/获取信号量(%x)失败。\n",SEMKEYP);

    return false;
  }

  // 创建/获取共享内存，键值为SHMKEYP，大小为MAXNUMP个st_procinfo结构体的大小。
  if ( (m_shmid = shmget((key_t)SHMKEYP, MAXNUMP*sizeof(struct st_procinfo), 0666|IPC_CREAT)) == -1)
  {
    if (logfile!=0) logfile->Write("创建/获取共享内存(%x)失败。\n",SHMKEYP);
    else printf("创建/获取共享内存(%x)失败。\n",SHMKEYP);

    return false;
  }

  // 将共享内存连接到当前进程的地址空间。
  m_shm=(struct st_procinfo *)shmat(m_shmid, 0, 0);

  struct st_procinfo stprocinfo;    // 当前进程心跳信息的结构体。
  memset(&stprocinfo,0,sizeof(stprocinfo));

  stprocinfo.pid=getpid();            // 当前进程号。
  stprocinfo.timeout=timeout;         // 超时时间。
  stprocinfo.atime=time(0);           // 当前时间。
  STRNCPY(stprocinfo.pname,sizeof(stprocinfo.pname),pname,50); // 进程名。

  // 进程id是循环使用的，如果曾经有一个进程异常退出，没有清理自己的心跳信息，
  // 它的进程信息将残留在共享内存中，不巧的是，当前进程重用了上述进程的id，
  // 这样就会在共享内存中存在两个进程id相同的记录，守护进程检查到残留进程的
  // 心跳时，会向进程id发送退出信号，这个信号将误杀当前进程。

  // 如果共享内存中存在当前进程编号，一定是其它进程残留的数据，当前进程就重用该位置。
  for (int ii=0;ii<MAXNUMP;ii++)
  {
    if ( (m_shm+ii)->pid==stprocinfo.pid ) { m_pos=ii; break; }
  }

  m_sem.P();  // 给共享内存上锁。

  if (m_pos==-1)
  {
    // 如果m_pos==-1，共享内存的进程组中不存在当前进程编号，找一个空位置。
    for (int ii=0;ii<MAXNUMP;ii++)
      if ( (m_shm+ii)->pid==0 ) { m_pos=ii; break; }
  }

  if (m_pos==-1)
  {
    if (logfile!=0) logfile->Write("共享内存空间已用完。\n");
    else printf("共享内存空间已用完。\n");

    m_sem.V();  // 解锁。

    return false;
  }

  // 把当前进程的心跳信息存入共享内存的进程组中。
  memcpy(m_shm+m_pos,&stprocinfo,sizeof(struct st_procinfo));

  m_sem.V();   // 解锁。

  return true;
}

// 更新共享内存进程组中当前进程的心跳时间。
bool CPActive::UptATime()
{
  if (m_pos==-1) return false;

  (m_shm+m_pos)->atime=time(0);

  return true;
}

CPActive::~CPActive()
{
  // 把当前进程从共享内存的进程组中移去。
  if (m_pos!=-1) memset(m_shm+m_pos,0,sizeof(struct st_procinfo));

  // 把共享内存从当前进程中分离。
  if (m_shm!=0) shmdt(m_shm);
}
