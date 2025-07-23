#ifndef ApiRsLibH
#define ApiRsLibH

#include <QtGlobal>
#include <QString>
#include <windef.h>
#include <winbase.h>
#include <winuser.h>

// ---------------------------------------------------------------------------
/*#define   ESC (char) 0x1b
#define CR  (char) 0x0d
#define LF  (char) 0x0a
#define FS  (char) 0x1c
#define DC1 (char) 0x11
#define DC2 (char) 0x12
#define DC3 (char) 0x13
#define DC4 (char) 0x14
#define STX (char) 0x02
#define ETX (char) 0x03
#define EOT (char) 0x04
#define ENQ (char) 0x05
#define ACK (char) 0x06
#define NAK (char) 0x15
#define SO  (char) 0x0e
#define SI  (char) 0x0f*/

#define MY_STX (char) 0x02
#define MY_ETX (char) 0x03
#define MY_EOT (char) 0x04
#define MY_CR   (char) 0x0d
#define MY_LF   (char) 0x0a

#define RecvBufSize 10240
#define SendBufSize 10240            //

class RS
{
    HANDLE hFile; //
    DWORD numRead, numWrite; //
    bool open; //
    DCB dcb; //
    COMMTIMEOUTS timeouts; //
    char Bcc; //

public:
    // ---------------------------------------------------------------------
    bool OpenPort(int PortNo, DWORD bps, BYTE data, BYTE parity, BYTE stop, int rTimeout, int wTimeout);

    bool SetTimeOut(int Timeout);
    bool SetTimeOut(int rTimeout, int wTimeout);
    bool ClosePort();

    bool PortStatus();

    int rdchr();
    int rdchr(int Timeout);
    bool wrchr(char c);
    bool Send(const char *c);
    bool Send(char *c, DWORD length);
    bool Receive(char *c, DWORD length);
    bool Receive(char *c, DWORD length, int Timeout);
    bool LinePurge();
    bool LinePurge(DWORD action);
    bool TransChar(char c);
    bool Flush();
    RS();
    ~RS();
    int rs_loc();
};
#endif
