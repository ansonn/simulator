// HighResElapsedTimer.h: interface for the HighResElapsedTimer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(HIGHRESELAPSEDTIMER_H_INCLUDED_)
#define HIGHRESELAPSEDTIMER_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Windows.h>

//	File:		HighResElapsedTimer.h
//	Author:		Chen Venkataraman (venkatar@sig.com)
//	Date:		Wed Dec 6, 2001
//
//	Purpose: 
//		For determing elapsed time details during execution of code blocks 
//		(initially for debug builds only)
//	
//		Sample usage:
//
//		{
//			#ifdef _DEBUG
//				HighResElapsedTimer		elapsedTime("block id");
//			#endif
//
//			... rest of the code block
//		}
//
//		At the end of the above block, trace msg of the form
//		"block id : Elapsed time - xxxx.xxx microseconds" is spit out

class HighResElapsedTimer  
{
public:
	HighResElapsedTimer(const std::string& strName) : m_strName(strName)
	{
		if (m_llFrequency == 0)			// frequency value not yet set
		{
			LARGE_INTEGER	liFrequency;

			QueryPerformanceFrequency(&liFrequency);
			m_llFrequency = liFrequency.QuadPart;
		}

		QueryPerformanceCounter(&m_llCounter);
	}

	virtual ~HighResElapsedTimer()
	{
		LARGE_INTEGER liNow;
		QueryPerformanceCounter(&liNow);
		double duration = (double)
			(liNow.QuadPart - m_llCounter.QuadPart)
				/
			m_llFrequency;
			
		std::cout << m_strName.c_str() <<
			" : Elapsed time = " << duration << " microseconds" << std::endl;
	}

private:
	std::string		m_strName;
	LARGE_INTEGER	m_llCounter;

private:
	static LONGLONG	m_llFrequency;		// Frequency setting is based hardware clock which doesn't
										// does change - so we want to set this only once

private:
	HighResElapsedTimer(const HighResElapsedTimer&);
	HighResElapsedTimer& operator=(const HighResElapsedTimer&);
};

#endif	// !defined(HIGHRESELAPSEDTIMER_H_INCLUDED_)
