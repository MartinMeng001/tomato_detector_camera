#include <stdio.h>
#include <string.h>
#include "ApiRsLib.h"

bool RS::OpenPort(int PortNo, DWORD bps, BYTE data, BYTE parity, BYTE stop, int rTimeout, int wTimeout)
{
    wchar_t tmp[256];
    if (open)
        return false;
    wsprintf(tmp, L"\\\\.\\COM%d\0", PortNo); // \\\\.\\ ----
    hFile = CreateFile(tmp, GENERIC_READ | GENERIC_WRITE,
                       0,
                       NULL, OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile)
        return false;

    if (!SetupComm(hFile, RecvBufSize, SendBufSize))
        return false;

    dcb.DCBlength = sizeof(dcb); //
    dcb.BaudRate = bps; //
    dcb.fBinary = true; //
    dcb.fParity = (0 != parity); //

    // dcb.fOutxCtsFlow = true;                // CTS
    // dcb.fOutxDsrFlow = true;                // RTS
    // dcb.fDsrSensitivity = true;             // DSR
    // dcb.fTXContinueOnXoff = true;           // Xon/off

    dcb.fOutxCtsFlow = false; // CTS
    dcb.fOutxDsrFlow = false; // RTS
    dcb.fDsrSensitivity = false; // DSR
    dcb.fTXContinueOnXoff = false; // Xon/off

    dcb.fOutX = false; // Xon/off
    dcb.fInX = false; // Xon/off

    dcb.fErrorChar = false; //
    dcb.ErrorChar = 0; //

    dcb.fNull = false; //
    dcb.fDtrControl = DTR_CONTROL_DISABLE; //
    dcb.fRtsControl = RTS_CONTROL_DISABLE; //
    dcb.fAbortOnError = false; //
    dcb.fDummy2 = 0; //
    dcb.wReserved = 0; //
    dcb.XonLim = 2048; //
    dcb.XoffLim = 512; //
    dcb.ByteSize = data; //

    {
        BYTE tmp = 0;
        switch (parity) {
        case 0:
            tmp = NOPARITY;
            break;
        case 1:
            tmp = ODDPARITY;
            break;
        case 2:
            tmp = EVENPARITY;
            break;
        case 3:
            tmp = MARKPARITY;
            break;
        case 4:
            tmp = parity; // space
            break;
        }

        dcb.Parity = tmp; //
    }

    {
        BYTE tmp = 0;

        switch (stop) {
        case 0:
            tmp = ONESTOPBIT;
            break;
        case 1:
            tmp = ONE5STOPBITS;
            break;
        case 2:
            tmp = TWOSTOPBITS;
            break;
        }
        dcb.StopBits = tmp; //
    }

    dcb.XonChar = 17; //
    dcb.XoffChar = 19; //
    dcb.EofChar = 0; //
    dcb.EvtChar = 0; //
    dcb.wReserved1 = 0; //

    if (!SetCommState(hFile, &dcb)) {
        ClosePort();
        return false;
    }

    EscapeCommFunction(hFile, SETDTR);
    SetTimeOut(rTimeout, wTimeout);
    open = true;
    return true;
}

bool RS::SetTimeOut(int Timeout)
{
    timeouts.ReadIntervalTimeout = Timeout;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = Timeout;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 10000; //
    timeouts.WriteTotalTimeoutConstant = Timeout;
    return SetCommTimeouts(hFile, &timeouts);
}

bool RS::PortStatus()
{
    return open;
}

bool RS::ClosePort()
{
    if (open)
        CloseHandle(hFile); //
    open = false;
    return true;
}

int RS::rdchr()
{
    unsigned char c;
    bool ret;

    ret = ReadFile(hFile, &c, 1, &numRead, 0);
    if (false == ret)
        return -2;
    if (1 == numRead)
        return int(c);
    return -1;
}

int RS::rdchr(int Timeout)
{
    unsigned char c;
    bool ret;

    SetTimeOut(Timeout);
    ret = ReadFile(hFile, &c, 1, &numRead, 0);
    if (false == ret)
        return -2;
    if (1 == numRead)
        return int(c);
    return -1;
}

bool RS::wrchr(char c)
{
    return WriteFile(hFile, &c, 1, &numWrite, 0);
}

bool RS::Send(const char *c)
{
    return WriteFile(hFile, c, strlen(c), &numWrite, 0);
}

bool RS::Send(char *c, DWORD length)
{
    return WriteFile(hFile, c, length, &numWrite, 0);
}

bool RS::Receive(char *c, DWORD length)
{
    bool ret;

    ret = ReadFile(hFile, c, length, &numRead, 0);
    if (false == ret)
        return false;
    if (length != numRead)
        return false;
    return true;
}

bool RS::Receive(char *c, DWORD length, int Timeout)
{
    bool ret;

    SetTimeOut(Timeout);

    ret = ReadFile(hFile, c, length, &numRead, 0);
    if (false == ret)
        return false;
    if (length != numRead)
        return false;
    return true;
}

bool RS::LinePurge()
{
    return PurgeComm(hFile, PURGE_RXCLEAR);
}

bool RS::LinePurge(DWORD action)
{
    return PurgeComm(hFile, action);
}

bool RS::TransChar(char c)
{
    return TransmitCommChar(hFile, c);
}

RS::RS()
{
    open = false;
}

RS::~RS()
{
    if (true == open)
        ClosePort();
}

int RS::rs_loc()
{
    DWORD dwErrors;
    COMSTAT cst;

    ::ZeroMemory(&cst, sizeof(cst));
    ::ClearCommError(hFile, &dwErrors, &cst);

    return ((int)cst.cbInQue);
}

bool RS::SetTimeOut(int rTimeout, int wTimeout) //
{
    timeouts.ReadIntervalTimeout = rTimeout;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = rTimeout;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = wTimeout;
    return SetCommTimeouts(hFile, &timeouts);
}
