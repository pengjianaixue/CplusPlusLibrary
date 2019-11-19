#ifndef _COMDEVICE_H_F6E6E156D_45F6_4A2B_A786_64A674453E1F
#define _COMDEVICE_H_F6E6E156D_45F6_4A2B_A786_64A674453E1F
#if _MSC_VER >1100
#pragma once
#endif
#include <windows.h>
#include <iostream>
#include <process.h>
#include <ioapiset.h>
#include <deque>
#include <cstring>
//C++ 11支持
//#if _MSC_VER >=1700
//#include <thread>
//#include <mutex>
//#include <atomic>
//#endif // 0
#ifdef WIN32
#ifdef _MSC_VER //VC++平台使用安全格式化函数
#define sprintf sprintf_s
#endif // _MSC_VER


//Windows 串口参数结构体定义如下
//typedef struct _DCB {
//	DWORD DCBlength;      // sizeof(DCB)
//	DWORD BaudRate;       /* Baudrate at which running
//	DWORD fBinary : 1;     /* Binary Mode (skip EOF check)
//	DWORD fParity : 1;     /* Enable parity checking
//	DWORD fOutxCtsFlow : 1; /* CTS handshaking on output
//	DWORD fOutxDsrFlow : 1; /* DSR handshaking on output
//	DWORD fDtrControl : 2;  /* DTR Flow control
//	DWORD fDsrSensitivity : 1; /* DSR Sensitivity
//	DWORD fTXContinueOnXoff : 1; /* Continue TX when Xoff sent
//	DWORD fOutX : 1;       /* Enable output X-ON/X-OFF
//	DWORD fInX : 1;        /* Enable input X-ON/X-OFF
//	DWORD fErrorChar : 1;  /* Enable Err Replacement
//	DWORD fNull : 1;       /* Enable Null stripping
//	DWORD fRtsControl : 2;  /* Rts Flow control
//	DWORD fAbortOnError : 1; /* Abort all reads and writes on Error
//	DWORD fDummy2 : 17;     /* Reserved
//	WORD wReserved;       /* Not currently used
//	WORD XonLim;          /* Transmit X-ON threshold
//	WORD XoffLim;         /* Transmit X-OFF threshold
//	BYTE ByteSize;        /* Number of bits/byte, 4-8
//	BYTE Parity;          /* 0-4=None,Odd,Even,Mark,Space
//	BYTE StopBits;        /* 0,1,2 = 1, 1.5, 2
//	char XonChar;         /* Tx and Rx X-ON character
//	char XoffChar;        /* Tx and Rx X-OFF character
//	char ErrorChar;       /* Error replacement char
//	char EofChar;         /* End of Input character
//	char EvtChar;         /* Received Event character
//	WORD wReserved1;      /* Fill for now.
//} DCB
/** 串口通信类
*
*  本类实现了对串口的基本操作
*  本类实现了串口的异步和同步的两种操作方式，在数据量较小，频率较低时建议采用同步模式，数据量大，
*  发送数据很频繁的时候采用异步模式
*  例如监听发到指定串口的数据、发送指定数据到串口
*/
class CSerialPort
{

public:
    enum COMTYPE
    {
        Synchronous_mode = 0, //同步模式
        Asynchronous_mode,    //异步模式
        BackThread_MODE		  //后台线程模式：串口读读取是一个后台线程
    };
    enum COMIOOPTYPE
    {
        WIRTE = 0, //写操作
        READ,    //读操作
    };
    inline CSerialPort(COMTYPE comtype= Synchronous_mode);
    inline ~CSerialPort(void);

    typedef	struct ComOverLapped:public OVERLAPPED
    {
        int(*Completecallbackfunction)(int param, void *arg);
        int CompleteResult;
        int IOOPType;
        int CallBackFuncparam;
        void *CallBackFuncArg;
        ComOverLapped() :Completecallbackfunction(NULL), CompleteResult(-1),
                        IOOPType(-1),CallBackFuncparam(0), CallBackFuncArg(NULL)
        {
            this->hEvent = NULL;
            this->Internal = NULL;
            this->InternalHigh = NULL;
            this->Offset=0;
            this->OffsetHigh = 0;
            this->Pointer = NULL;
        };
    }COMOVERLAPPED,*PCOMOVERLAPPED;

    /*串口的操作接口*/
public:

    /** 初始化串口函数
    *
    *  @param:  const char *comPort 串口编号, 如COM1
    *  @param:  UINT baud   波特率,默认为9600
    *  @param:  char parity 是否进行奇偶校验,'Y'表示需要奇偶校验,'N'表示不需要奇偶校验
    *  @param:  UINT databits 数据位的个数,默认值为8个数据位
    *  @param:  UINT stopsbits 停止位使用格式,默认值为1
    *  @param:  DWORD dwCommEvents 默认为EV_RXCHAR,即只要收发任意一个字符,则产生一个事件
    *  @return: bool  初始化是否成功
    *  @note:   在使用其他本类提供的函数前,请先调用本函数进行串口的初始化
    *　　　　　   /n本函数提供了一些常用的串口参数设置,若需要自行设置详细的DCB参数,可使用重载函数
    *           /n本串口类析构时会自动关闭串口,无需额外执行关闭串口
    *  @see:
    */
    inline bool InitPort(const char *comPort = "", UINT  baud = CBR_9600,  UINT  databits = 8, UINT  stopsbits = 1, UINT parity = 0, DWORD dwCommEvents = EV_RXCHAR);
    /** 串口初始化函数
    *
    *  本函数提供直接根据DCB参数设置串口参数
    *  @param:  UINT portNo
    *  @param:  const LPDCB & plDCB
    *  @return: bool  初始化是否成功
    *  @note:   本函数提供用户自定义地串口初始化参数
    *  @see:
    */
     inline bool InitPort(const char *comPort, const LPDCB& plDCB);

    /** 开启监听线程
    *
    *  本监听线程完成对串口数据的监听,并将接收到的数据打印到屏幕输出
    *  @return: bool  操作是否成功
    *  @note:   当线程已经处于开启状态时,返回flase
    *  @see:
    */
     inline bool OpenListenThread();

    /** 关闭监听线程
    *
    *
    *  @return: bool  操作是否成功
    *  @note:   调用本函数后,监听串口的线程将会被关闭
    *  @see:
    */
     inline bool CloseListenTread();

     /** 开启重叠结构清理线程
     *
     *
     *  @return: bool  操作是否成功
     *  @note:		   调用本函数后,如果串口被设置在异步模式下，本线程
                       会对类中使用的重叠结构进行检测和清理，避免资源耗费过大
     *  @see:
     */
     inline bool OpenClearupTread();

     /** 清理线程函数
     *
     *
     *  @return: void
     *  @note:	 本函数对异步模式下使用的重叠结构进行检测和清理，避免资源耗费过大
     *  @see:
     */
     inline static unsigned  int  WINAPI  Clearupfun(LPVOID lpparam);

    /** 向串口写数据
    *
    *  将缓冲区中的数据写入到串口
    *  @param:  unsigned char * pData 指向需要写入串口的数据缓冲区
    *  @param:  unsigned int length 需要写入的数据长度
    *  @return: bool  操作是否成功
    *  @note:   length不要大于pData所指向缓冲区的大小
    *  @see:
    */
     inline bool WriteData(const char* pData, unsigned int length, bool &nCompleteFlag,unsigned int WirtetimeGap);

    /** 获取串口缓冲区中的字节数
    *
    *
    *  @return: UINT  操作是否成功
    *  @note:   当串口缓冲区中无数据时,返回0
    *  @see:
    */
     inline UINT GetBytesInCOM();

    /** 读取串口接收缓冲区中指定长度数据
    *
    *
    *  @param:  char & cRecved 存放读取数据的字符变量
    *  @return: bool  读取是否成功
    *  @note:
    *  @see:
    */
     inline bool  ReadComPort(char * RecviData, DWORD &recvlength, int(*Completecallbackfun)(int param, void *arg) = NULL, int CallBackfunparam = 0, void *arg = NULL);
     /** 读取串口接收缓冲区中指定长度数据
     *
     *
     *  @param:  char & cRecved 存放读取数据的字符变量
     *  @return: bool  读取是否成功
     *  @note:
     *  @see:
     */
     inline bool SetComTimeOut(unsigned int millsectime);

     /** 关闭串口
     *
     *
     *  @return: void  操作是否成功
     *  @note:
     *  @see:
     */
     inline void ClosePort();
     /** 获取串口打开状态
     *
     *
     *  @return: bool true:打开 false:未打开
     *  @note:
     *  @see:
     */
     inline bool IsOpen();

//串口类用到的功能函数
public:
    /*
    获取本机的处理器(逻辑CPU)个数
    *  @return: int  个数
    */
    int GetNumofProcessors();

private:

    /** 打开串口
    *
    *
    *  @param:  UINT portNo 串口设备号
    *  @return: bool  打开是否成功
    *  @note:
    *  @see:
    */
    inline bool openPort(const char *comPortName);

    /** 串口监听线程
    *
    *  监听来自串口的数据和信息
    *  @param:  void * pParam 线程参数
    *  @return: UINT WINAPI 线程返回值
    *  @note:
    *  @see:
    */
    inline static UINT WINAPI ListenThread(void* pParam);
    /** 串口监听线程
    *
    *  监听来自串口的数据和信息
    *  @param:  void * pParam 线程参数
    *  @return: UINT WINAPI 线程返回值
    *  @note:
    *  @see:
    */
    inline	HANDLE CreateNewCompletionPort(UINT nNumofconcurrentthreads);
    /** 串口监听线程
    *
    *  监听来自串口的数据和信息
    *  @param:  void * pParam 线程参数
    *  @return: UINT WINAPI 线程返回值
    *  @note:
    *  @see:
    */
    inline	bool   BindCompletionPort(HANDLE hBindCompletionPortHandle, HANDLE hOpreationHandle, UINT nCompletionKey);
    /** 串口完全端口事件处理线程
    *
    *  处理IO完全端口完成的事件
    *  @param:  void * pParam 线程参数
    *  @return: UINT WINAPI 线程返回值
    *  @note:
    *  @see:
    */
    inline static UINT WINAPI GetQueuedCompletionStatusThread(LPVOID lpparam);
    inline BOOL OpenGetQueuedCompletionStatusThread();
    inline bool CloseGetQueuedCompletionStatusThread();
	inline bool JudegmentComPortNumIsMoreThanTen(const char *comPortName);

private:

    /** 串口句柄 */
    HANDLE  m_hComm;

    ///** 线程退出标志变量 */
    //static bool s_bExit;

    /** 线程句柄 */
    volatile HANDLE    m_hListenThread;

    /** 同步互斥,临界区保护 */
    CRITICAL_SECTION   m_csCommunicationSync;       //!< 互斥操作串口
    /** 串口模式: Synchronous_mode  同步模式,
                  Asynchronous_mode 异步模式*/
    COMTYPE			   m_thiscomtype;
    //串口超时时间设置结构体
    COMMTIMEOUTS	   m_CommTimeouts;
    //保存overlapped指针用于释放资源
    std::deque<OVERLAPPED*> m_usedoverlappedlist;
    bool					m_ClearupFlags;
    bool					m_bGetQueuedCompletionFlags;
    HANDLE					m_ClearupTread;
    HANDLE					m_CompletionPort;
    HANDLE					m_GetCompletionQueuethread;
    std::deque<HANDLE>		m_GetCompletionQueuethreadHANDLEList;
    std::deque<std::string> m_Recvibuffer;
};

///** 线程退出标志 */
//bool CSerialPort::s_bExit = false;
/** 当串口无数据时,sleep 至下次查询间隔的时间,单位:毫秒 */
const UINT SLEEP_TIME_INTERVAL = 5;
inline CSerialPort::CSerialPort(COMTYPE comtype):m_hListenThread(INVALID_HANDLE_VALUE),m_thiscomtype(Synchronous_mode),
                                                 m_ClearupFlags(false), m_ClearupTread(NULL), m_CompletionPort(NULL),
                                                 m_bGetQueuedCompletionFlags(false), m_GetCompletionQueuethread(NULL)
{
    m_hComm = INVALID_HANDLE_VALUE;
    m_hListenThread = INVALID_HANDLE_VALUE;
    InitializeCriticalSection(&m_csCommunicationSync);
    m_thiscomtype = comtype;



}

inline CSerialPort::~CSerialPort(void)
{
    CloseListenTread();
    ClosePort();
    m_ClearupFlags = false;
    m_bGetQueuedCompletionFlags = false;
    WaitForSingleObject(m_ClearupTread,INFINITE);
    for (size_t i = 0; i < m_GetCompletionQueuethreadHANDLEList.size(); ++i)
    {
        WaitForSingleObject(m_GetCompletionQueuethreadHANDLEList[i],INFINITE);
    }
    DeleteCriticalSection(&m_csCommunicationSync);
}
bool CSerialPort::InitPort(const char *comPort /*= 1*/, UINT baud /*= CBR_9600*/,
    UINT databits /*= 8*/, UINT stopsbits /*= 1*/, UINT parity /*= 0*/, DWORD dwCommEvents /*= EV_RXCHAR*/)
{

    /** 临时变量,将制定参数转化为字符串形式,以构造DCB结构 */
    char szDCBparam[50] = {0};
    sprintf_s(szDCBparam, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, stopsbits);
    //baud = 1200 parity = N data = 8 stop = 1
    /** 打开指定串口,该函数内部已经有临界区保护,上面请不要加保护 */
    if (!openPort(comPort))
    {
        return false;
    }

    /** 进入临界段 */
    EnterCriticalSection(&m_csCommunicationSync);
    /** 是否有错误发生 */
    BOOL bIsSuccess = TRUE;
    /** 在此可以设置输入输出的缓冲区大小,如果不设置,则系统会设置默认值.
    *  自己设置缓冲区大小时,要注意设置稍大一些,避免缓冲区溢出
    */
    //if (bIsSuccess )
    //{
    //bIsSuccess = SetupComm(m_hComm,1024,1024);
    //}
    /** 设置串口的超时时间,均设为0,表示不使用超时限制 */
    m_CommTimeouts.ReadIntervalTimeout = 100;
    m_CommTimeouts.ReadTotalTimeoutMultiplier = 0;
    m_CommTimeouts.ReadTotalTimeoutConstant = 1000;
    m_CommTimeouts.WriteTotalTimeoutMultiplier = 0;
    m_CommTimeouts.WriteTotalTimeoutConstant = 0;
    if (bIsSuccess)
    {
        bIsSuccess = SetCommTimeouts(m_hComm, &m_CommTimeouts);
    }
    DCB  dcb;
    bIsSuccess = GetCommState(m_hComm, &dcb);
    if (bIsSuccess)
    {
        dcb.BaudRate = baud;
        dcb.ByteSize = databits;
        dcb.StopBits = stopsbits-1;
        dcb.Parity = parity;
    }
    //if (bIsSuccess)
    //{
    //	// 将ANSI字符串转换为UNICODE字符串
    //	DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, NULL, 0);
    //	wchar_t *pwText = new wchar_t[dwNum];
    ///*	memset(pwText, 0, dwNum);*/
    //	if (MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, pwText, dwNum)>0)
    //	{
    //		bIsSuccess = TRUE;
    //	}
    //	/** 获取当前串口配置参数,并且构造串口DCB参数 */
    //	bIsSuccess = GetCommState(m_hComm, &dcb) && BuildCommDCB(pwText, &dcb);

    //	if (bIsSuccess!= TRUE)
    //	{
    //		int errorcode = GetLastError();
    //		bIsSuccess = FALSE;
    //	}
    //	/** 开启RTS flow控制 */
    //	dcb.fRtsControl = RTS_CONTROL_ENABLE;
    //	/** 释放内存空间 */
    //	delete[] pwText;
    //}
    if (bIsSuccess)
    {
        /** 使用DCB参数配置串口状态 */
        bIsSuccess = SetCommState(m_hComm, &dcb);
        if (!bIsSuccess)
        {
            int errorcode = GetLastError();
            bIsSuccess = FALSE;
        }


    }
    /**  清空串口缓冲区 */
    PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

    /** 离开临界段 */
    LeaveCriticalSection(&m_csCommunicationSync);
    if (!bIsSuccess)
    {
        CloseGetQueuedCompletionStatusThread();
        CloseHandle(m_hComm);
    }
    return bIsSuccess == TRUE;
}

bool CSerialPort::InitPort(const char *comPort, const LPDCB& plDCB)
{
    /** 打开指定串口,该函数内部已经有临界区保护,上面请不要加保护 */
    if (!openPort(comPort))
    {
        return false;
    }
    /** 进入临界段 */
    EnterCriticalSection(&m_csCommunicationSync);

    /** 配置串口参数 */
    if (!SetCommState(m_hComm, plDCB))
    {
        return false;

    }

    /**  清空串口缓冲区 */
    PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

    /** 离开临界段 */
    LeaveCriticalSection(&m_csCommunicationSync);

    return true;
}

void CSerialPort::ClosePort()
{
    /** 如果有串口被打开，关闭它 */
    CloseGetQueuedCompletionStatusThread();
    if (m_hComm != INVALID_HANDLE_VALUE)
    {
        //CloseGetQueuedCompletionStatusThread();
        CloseHandle(m_CompletionPort);
        CloseHandle(m_hComm);
        m_hComm = INVALID_HANDLE_VALUE;
    }
}

inline bool CSerialPort::IsOpen()
{
    if (m_hComm!= INVALID_HANDLE_VALUE)
    {
        return true;
    }
    return false;
}

bool CSerialPort::openPort(const char *comPortName)
{
    /** 进入临界段 */
	char OpenComPortName[64] = { "\\\\.\\" };
    EnterCriticalSection(&m_csCommunicationSync);
	if (JudegmentComPortNumIsMoreThanTen(comPortName))
	{
		strcat_s(OpenComPortName, 64, comPortName);
	}
	else
	{
		strcpy_s(OpenComPortName, 64, comPortName);
	}
    if (Asynchronous_mode == m_thiscomtype)
    {
        /** 打开指定的串口 */
        m_hComm = CreateFileA(OpenComPortName,     /** 设备名,COM1,COM2等 */
            GENERIC_READ | GENERIC_WRITE, /** 访问模式,可同时读写 */
            0,                            /** 共享模式,0表示不共享 */
            NULL,
            /** 安全性设置,一般使用NULL */
            OPEN_EXISTING,                /** 该参数表示设备必须存在,否则创建失败 */
            FILE_FLAG_OVERLAPPED,
            0);
        if (m_hComm!= INVALID_HANDLE_VALUE)
        {
            m_CompletionPort = CreateNewCompletionPort(GetNumofProcessors());
            if (m_CompletionPort != NULL)
            {
                if (!BindCompletionPort(m_CompletionPort, m_hComm, 0))
                    return false;
            }
            else
            {
                return false;
            }
            if (CSerialPort::COMTYPE::Asynchronous_mode == m_thiscomtype)
            {
                m_bGetQueuedCompletionFlags = true;
                OpenGetQueuedCompletionStatusThread();

            }
        }
    }
    else if (Synchronous_mode == m_thiscomtype)
    {
        /** 打开指定的串口 */
        m_hComm = CreateFileA(OpenComPortName,	  /** 设备名,COM1,COM2等 */
            GENERIC_READ | GENERIC_WRITE, /** 访问模式,可同时读写 */
            0,                            /** 共享模式,0表示不共享 */
            NULL,                         /** 安全性设置,一般使用NULL */
            OPEN_EXISTING,                /** 该参数表示设备必须存在,否则创建失败 */
            0,
            0);
    }
    else
    {
        return false;
    }
    /** 如果打开失败，释放资源并返回 */
    if (m_hComm == INVALID_HANDLE_VALUE)
    {
        LeaveCriticalSection(&m_csCommunicationSync);
        return false;
    }

    /** 退出临界区 */
    LeaveCriticalSection(&m_csCommunicationSync);

    return true;
}

bool CSerialPort::OpenListenThread()
{
    /** 检测线程是否已经开启了 */
    if (m_hListenThread != INVALID_HANDLE_VALUE)
    {
        /** 线程已经开启 */
        return false;
    }
    //s_bExit = false;
    /** 线程ID */
    UINT threadId;
    /** 开启串口数据监听线程 */
    m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, ListenThread, this, 0, &threadId);
    if (!m_hListenThread)
    {
        return false;
    }
    /** 设置线程的优先级,高于普通线程 */
    if (!SetThreadPriority(m_hListenThread, THREAD_PRIORITY_ABOVE_NORMAL))
    {
        return false;
    }

    return true;
}

bool CSerialPort::CloseListenTread()
{
    if (m_hListenThread != INVALID_HANDLE_VALUE)
    {
        /** 通知线程退出 */
        /*s_bExit = true;*/
        /** 等待线程退出 */
        Sleep(10);
        /** 置线程句柄无效 */
        CloseHandle(m_hListenThread);
        m_hListenThread = INVALID_HANDLE_VALUE;
    }
    return true;
}

inline bool CSerialPort::OpenClearupTread()
{
    m_ClearupTread = (HANDLE)_beginthreadex(NULL, 0,Clearupfun, this, 0, NULL);
    if (m_ClearupTread == NULL)
    {
        return false;
    }
    return true;
}

inline unsigned int CSerialPort::Clearupfun(LPVOID lpparam)
{
    CSerialPort *thisclass =   static_cast<CSerialPort*>(lpparam);
    while (thisclass->m_ClearupFlags)
    {
        EnterCriticalSection(&thisclass->m_csCommunicationSync);
        if (!thisclass->m_usedoverlappedlist.empty())
        {
            OVERLAPPED *clearget =  thisclass->m_usedoverlappedlist.front();
            if (clearget!= NULL)
            {
                WaitForSingleObject(clearget->hEvent, 1000);
                CloseHandle(clearget->hEvent);
                delete clearget;
            }
            thisclass->m_usedoverlappedlist.pop_front();
        }
        LeaveCriticalSection(&thisclass->m_csCommunicationSync);
    }
    return 0;

}

UINT CSerialPort::GetBytesInCOM()
{
    DWORD dwError = 0;  /** error code  */
    COMSTAT  comstat;   /** COMSTAT结构体,记录通信设备的状态信息 */
    memset(&comstat, 0, sizeof(COMSTAT));
    UINT BytesInQue = 0;
    /** 在调用ReadFile和WriteFile之前,通过本函数清除以前遗留的错误标志 */
    if (ClearCommError(m_hComm, &dwError, &comstat))
    {
        BytesInQue = comstat.cbInQue; /** 获取在输入缓冲区中的字节数 */
    }
    return BytesInQue;
}

UINT WINAPI CSerialPort::ListenThread(void* pParam)
{
    /** 得到本类的指针 */
    CSerialPort *pSerialPort = reinterpret_cast<CSerialPort*>(pParam);

    //// 线程循环,轮询方式读取串口数据
    //while (!pSerialPort->s_bExit)
    //{
    //	UINT BytesInQue = pSerialPort->GetBytesInCOM();
    //	/** 如果串口输入缓冲区中无数据,则休息一会再查询 */
    //	if (BytesInQue == 0)
    //	{
    //		Sleep(SLEEP_TIME_INTERVAL);
    //		continue;
    //	}

    //	/** 读取输入缓冲区中的数据并输出显示 */
    //	//cRecvestring = new char [BytesInQue + 1];
    //	char cRecved = 0x00;
    //	int Count=0;
    //	do
    //	{
    //		cRecved = 0x00;
    //		//if (pSerialPort->ReadComPort() == true)
    //		//{
    //		//	/*cRecvestring[Count] = cRecved;
    //		//	cout << cRecved;
    //		//	Count++;*/
    //		//	continue;
    //		//}
    //	} while (--BytesInQue);
    //	//delete[] cRecvestring;
    //}
    return 0;
}

inline HANDLE CSerialPort::CreateNewCompletionPort(UINT nNumofconcurrentthreads)
{
    return CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, nNumofconcurrentthreads);
}

inline bool CSerialPort::BindCompletionPort(HANDLE hBindCompletionPortHandle, HANDLE hOpreationHandle, UINT nCompletionKey)
{
    HANDLE  h = CreateIoCompletionPort(hOpreationHandle, hBindCompletionPortHandle, nCompletionKey, 0);
    return (h== hBindCompletionPortHandle);
}
inline UINT CSerialPort::GetQueuedCompletionStatusThread(LPVOID lpparam)
{
    CSerialPort *thisclass = static_cast<CSerialPort*> (lpparam);
    if (NULL == thisclass  )
    {
        return 1;
    }
    DWORD dwTransferdBytes = 0;
	ULONG_PTR ulCompletionKey;
    LPOVERLAPPED operationverlapped = NULL;
    PCOMOVERLAPPED Comoperationverlapped = NULL;
    operationverlapped = Comoperationverlapped;
    while (thisclass->m_bGetQueuedCompletionFlags)
    {
        BOOL bresult = GetQueuedCompletionStatus(thisclass->m_CompletionPort, &dwTransferdBytes, &ulCompletionKey, &operationverlapped, INFINITE);
        if (bresult)
        {

            delete operationverlapped;
        }
        else
        {
            if (operationverlapped!=NULL)
            {
                if (Comoperationverlapped->Completecallbackfunction!=NULL)
                {
                    Comoperationverlapped->Completecallbackfunction(Comoperationverlapped->CallBackFuncparam, Comoperationverlapped->CallBackFuncArg);
                }
                delete operationverlapped;
            }
            else
            {

                DWORD error = GetLastError();
                if (error==WAIT_TIMEOUT)
                {
                    continue;
                }
            }
        }
    }
    return 0;
}

inline BOOL CSerialPort::OpenGetQueuedCompletionStatusThread()
{
    UINT numofprocessor = GetNumofProcessors();
    HANDLE *threadhandlelist = new HANDLE[numofprocessor];
    for (size_t i = 0; i < numofprocessor*2; i++)
    {
        threadhandlelist[i] = (HANDLE)_beginthreadex(NULL, 0, GetQueuedCompletionStatusThread, this, 0, NULL);
        if (threadhandlelist[i]!=NULL)
        {
            m_GetCompletionQueuethreadHANDLEList.push_back(threadhandlelist[i]);

        }
        else
        {
#ifdef _DEBUG
            std::cout << "创建线程"<<"GetQueuedCompletionStatusThread:"<<i<<"失败"<< std::endl;
#endif // DEBUG

        }
    }
    return TRUE;
}
inline bool CSerialPort::CloseGetQueuedCompletionStatusThread()
{
    if (Asynchronous_mode == m_thiscomtype)
    {
        m_bGetQueuedCompletionFlags = false;
        for (size_t i = 0; i < m_GetCompletionQueuethreadHANDLEList.size(); ++i)
        {
            if (m_GetCompletionQueuethreadHANDLEList[i] != INVALID_HANDLE_VALUE)
            {
                WaitForSingleObject(m_GetCompletionQueuethreadHANDLEList[i], INFINITE);
            }
        }
    }
    CloseHandle(m_CompletionPort);
    return true;
}
inline bool CSerialPort::JudegmentComPortNumIsMoreThanTen(const char *comPortName)
{
	int numcounter = 0;
	for (size_t i = 0; i<strlen(comPortName); i++)
	{
		if (comPortName[i] <='9'&& comPortName[i] >='0')
		{
			++numcounter;
			if (numcounter>1)
			{
				return true;
			}
		}
	}
	return false;
}
bool CSerialPort::ReadComPort(char *RecviData, DWORD &recvlength, int(*Completecallbackfun)(int param, void *arg),int CallBackfunparam, void *arg)
{
    BOOL  bResult = TRUE;
    DWORD BytesRead = 0;
    if (m_hComm == INVALID_HANDLE_VALUE)
    {
        return false;
    }
    ////对接受缓冲区进行初始化
    //UINT BytesInQue = GetBytesInCOM();
    ///** 如果串口输入缓冲区中无数据,则休息一会再查询 */
    //while (BytesInQue == 0)
    //{
    //	Sleep(SLEEP_TIME_INTERVAL);
    //	break;
    //}
    if (m_thiscomtype == CSerialPort::COMTYPE::Asynchronous_mode)
    {
        COMOVERLAPPED *Readover = new COMOVERLAPPED;
        Readover->hEvent = CreateEvent(NULL, FALSE, FALSE, LPCWSTR(""));
        Readover->IOOPType = COMIOOPTYPE::READ;
        Readover->CallBackFuncparam = CallBackfunparam;
        Readover->CallBackFuncArg = arg;
        Readover->Completecallbackfunction = Completecallbackfun;
        bResult = bResult = ReadFile(m_hComm, RecviData, recvlength, &BytesRead, Readover);
        if (bResult != TRUE)
        {
            DWORD errorcode = GetLastError();
            if (errorcode != ERROR_IO_PENDING)
            {
                delete Readover;
                return false;
            }
        }
        else
        {
            delete Readover;
            return false;
        }
    }
    else
    {
        /** 从缓冲区读取数据 */
        bResult = ReadFile(m_hComm, RecviData, recvlength, &BytesRead, NULL);
        if ((!bResult))
        {
            /** 获取错误码,可以根据该错误码查出错误原因 */
            DWORD dwError = GetLastError();
            /** 清空串口缓冲区 */
            PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
            return false;
        }

    }
    if (m_thiscomtype == CSerialPort::COMTYPE::Asynchronous_mode)
    {
        return true;
    }
    else
    {
		recvlength = BytesRead;
        return (recvlength !=0);
    }


}

inline bool CSerialPort::SetComTimeOut(unsigned int millsectime)
{
    int ret = 0;
    m_CommTimeouts.ReadIntervalTimeout = 0;
    m_CommTimeouts.ReadTotalTimeoutConstant = millsectime;
    m_CommTimeouts.ReadTotalTimeoutMultiplier = 1;
    m_CommTimeouts.WriteTotalTimeoutConstant = millsectime;
    m_CommTimeouts.WriteTotalTimeoutMultiplier=1;
    return 1==(ret=SetCommTimeouts(m_hComm,&m_CommTimeouts));
}

inline int CSerialPort::GetNumofProcessors()
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwNumberOfProcessors;
}

bool CSerialPort::WriteData (const char* pData, unsigned int length,bool &nCompleteFlag, unsigned int WirtetimeGap)
{
    BOOL   bResult = TRUE;
    DWORD  BytesToSend = 0;
    if (m_hComm == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    if (CSerialPort::COMTYPE::Synchronous_mode == m_thiscomtype)
    {
        /** 向缓冲区写入指定量的数据 */
        Sleep(WirtetimeGap);
        bResult = WriteFile(m_hComm, pData, length, &BytesToSend, NULL);
        if (!bResult)
        {
            DWORD dwError = GetLastError();
            /** 清空串口缓冲区 */
            PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
            //LeaveCriticalSection(&m_csCommunicationSync);
            return false;
        }
    }
    else if (CSerialPort::COMTYPE::Asynchronous_mode == m_thiscomtype)
    {

        OVERLAPPED *wirteover = new OVERLAPPED;
        *wirteover = { 0 };
        wirteover->Offset = 0;
        wirteover->hEvent = CreateEvent(NULL,FALSE,FALSE,LPCWSTR(""));
        bResult = WriteFile(m_hComm, pData, length, &BytesToSend, wirteover);
        if (bResult!=TRUE)
        {

            DWORD errorcode = GetLastError();
            if (errorcode!= ERROR_IO_PENDING)
            {
                delete wirteover;
                return false;
            }
        }
        else
        {
            delete wirteover ;

            return false;
        }
    }

    return true;
}
#endif // WIN32
#endif
