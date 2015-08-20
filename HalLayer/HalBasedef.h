/*---------------------------------------------------------------------------*/
//	CopyRight: BJ-MEMBLAZE
//	2015-08-01
//
//-----------------------------------------------------------------------------
#ifndef _HAL_BASEDEF_H
#define _HAL_BASEDEF_H


typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;
typedef unsigned long long	reg64;
typedef size_t				usys;

typedef signed char			s8;
typedef signed short		s16;
typedef signed int			s32;
typedef signed long long	s64;




typedef s32 (*ManagerInit)(u32 managerID);
typedef s32 (*ManagerPoll)(u32 managerID);



#endif
