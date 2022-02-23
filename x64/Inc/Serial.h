// Serial.h: interface for the CSerial class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERIAL_H__4827523F_0799_415F_9A0E_ACF715EBC576__INCLUDED_)
#define AFX_SERIAL_H__4827523F_0799_415F_9A0E_ACF715EBC576__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSerial  
{
public:
	OVERLAPPED m_OverlappedWrite;
	OVERLAPPED m_OverlappedRead;
	HANDLE m_hComDev;
	bool m_bOpened;
	int m_iUsedPort;
	CSerial();
	virtual ~CSerial();
	//BOOL Open( int nPort, int nBaud );
	//加详细串口设置by zhoucs 208-08-19
	BOOL Open( int nPort, int nBaud = 9600 ,int nByteSize = 8, int nParity = 0,int nStopBits = 0);
	//加关闭串口by zhoucs 208-08-19
	void close();

	int InBufferCount( void );
	DWORD ReadData( void *buffer, DWORD dwBytesRead);
	DWORD SendData( const char *buffer, DWORD dwBytesWritten);

};

#endif // !defined(AFX_SERIAL_H__4827523F_0799_415F_9A0E_ACF715EBC576__INCLUDED_)
