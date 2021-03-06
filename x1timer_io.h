/**
*Copyright 2016-  DESY (Deutsches Elektronen-Synchrotron, www.desy.de)
*
*This file is part of X1TIMER driver.
*
*X1TIMER is free software: you can redistribute it and/or modify
*it under the terms of the GNU General Public License as published by
*the Free Software Foundation, either version 3 of the License, or
*(at your option) any later version.
*
*X1TIMER is distributed in the hope that it will be useful,
*but WITHOUT ANY WARRANTY; without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*GNU General Public License for more details.
*
*You should have received a copy of the GNU General Public License
*along with X1TIMER.  If not, see <http://www.gnu.org/licenses/>.
**/


/*
*	Author: Ludwig Petrosyan (Email: ludwig.petrosyan@desy.de)
*/

#ifndef _X1TIMER_IO_H
#define _X1TIMER_IO_H

#include <linux/ioctl.h> /* needed for the _IOW etc stuff used later */

#define X1TIMER_IOC                'k'
#define X1TIMER_PHYSICAL_SLOT     _IOWR(X1TIMER_IOC, 50, int)
#define X1TIMER_DRIVER_VERSION    _IOWR(X1TIMER_IOC, 51, int)
#define X1TIMER_FIRMWARE_VERSION  _IOWR(X1TIMER_IOC, 52, int)
#define X1TIMER_ADD_SERVER_SIGNAL _IOWR(X1TIMER_IOC, 53, int)
#define X1TIMER_DEL_SERVER_SIGNAL _IOWR(X1TIMER_IOC, 54, int)
#define X1TIMER_GET_STATUS        _IOWR(X1TIMER_IOC, 55, int)
#define X1TIMER_CHECK_IRQ         _IOWR(X1TIMER_IOC, 56, int)
#define X1TIMER_EVENT_INFO        _IOWR(X1TIMER_IOC, 57, int)
#define X1TIMER_READ_IRQ          _IOWR(X1TIMER_IOC, 58, int)
#define X1TIMER_EVENT_MASK        _IOWR(X1TIMER_IOC, 59, int)

#define X1TIMER_SET_TIMER_OFFSET  _IOWR(X1TIMER_IOC, 60, int)
#define X1TIMER_SET_TIMER         _IOWR(X1TIMER_IOC, 61, int)
#define X1TIMER_EVENT_INFO_EXT    _IOWR(X1TIMER_IOC, 62, int)

#define X1TIMER_READ_DMA     _IOWR(X1TIMER_IOC, 63, int)
#define X1TIMER_WRITE_DMA    _IOWR(X1TIMER_IOC, 64, int)
#define X1TIMER_DMA_TIME     _IOWR(X1TIMER_IOC, 65, int)

#define X1TIMER_BRD_STATUS        _IOWR(X1TIMER_IOC, 66, int)
#define X1TIMER_SET_HP_SIG         _IOWR(X1TIMER_IOC, 67, int)
#define X1TIMER_VECTOR_RW         _IOWR(X1TIMER_IOC, 68, int)

#define X1TIMER_IOC_MINNR 50
#define X1TIMER_IOC_MAXNR 69

#define X1TIMER_IRQ_MAIN       	0x00000001
#define X1TIMER_IRQ_FRNT1       0x00000100
#define X1TIMER_IRQ_FRNT2       0x00000200
#define X1TIMER_IRQ_FRNT3       0x00000400
#define X1TIMER_IRQ_FRNT4       0x00000800
#define X1TIMER_IRQ_FRNT5       0x00001000
#define X1TIMER_IRQ_FRNT6       0x00002000
#define X1TIMER_IRQ_BACK1       0x00010000
#define X1TIMER_IRQ_BACK2       0x00020000
#define X1TIMER_IRQ_BACK3       0x00040000
#define X1TIMER_IRQ_BACK4       0x00080000
#define X1TIMER_IRQ_BACK5       0x00100000
#define X1TIMER_IRQ_BACK6       0x00200000
#define X1TIMER_IRQ_BACK7       0x00400000
#define X1TIMER_IRQ_BACK8       0x00800000
#define X1TIMER_IRQ_RTM1        0x01000000
#define X1TIMER_IRQ_RTM2        0x02000000
#define X1TIMER_IRQ_RTM3        0x04000000
#define X1TIMER_IRQ_RTM4        0x08000000
#define X1TIMER_IRQ_RTM5        0x10000000
#define X1TIMER_IRQ_RTM6        0x20000000
#define X1TIMER_IRQ_RTM7        0x40000000
#define X1TIMER_IRQ_RTM8        0x80000000
#define X1TIMER_IRQ_RTM0        0x00000020
#define SIG1        0
#define SIG2        1
#define URG         2
#define MSK_WANT    3
#define CUR_SIG     4

#include "pciedev_io.h"

#define X1TIMER_DMA_HOST_ADDR     76
#define X1TIMER_DMA_BOARD_ADDR  80
#define X1TIMER_DMA_CTRL                84
#define X1TIMER_DMA_SIZE                 88

#define X1TIMER_DMA_CTRL_IRQ        0x10
#define X1TIMER_DMA_IRQ_ENBL        0x4
#define X1TIMER_DMA_IRQ_MASK        0x4
#define X1TIMER_DMA_RD_START        0x11
#define X1TIMER_DMA_WR_START       0x14

//standart register set will be moved to pciedev_io.h
//#include <linux/list.h>
#define ASCII_BOARD_MAGIC_NUM     0x424F5244
#define ASCII_PROJ_MAGIC_NUM        0x50524F4A

// end of standart register sets

struct x1timer_ioctl_data{
       u_int		f_Irq_Mask;
       u_int		f_Irq_Sig;
       u_int		f_Status;

};
typedef struct x1timer_ioctl_data t_x1timer_ioctl_data;

struct x1timer_ioctl_event  {
       int              f_Irq_Sec;
       int              f_Irq_uSec;
       int              f_Irq_Delay;
       u_int            f_Irq_Mask;
       u_int            f_Irq_Flag;
       u_int            f_Irq_Sts;
       long             f_gen_event;
};
typedef struct x1timer_ioctl_event t_x1timer_ioctl_event;

struct x1timer_ioctl_event_ht {
       int              f_Irq_Sec;
       int              f_Irq_uSec;
       int              f_Irq_Delay;
       u_int            f_Irq_Mask;
       u_int            f_Irq_Flag;
       u_int            f_Irq_Sts;
       long             f_gen_event;
       u_int            f_board_Sec;
       u_int            f_board_uSec;
       u_int            f_global_Sec;
       u_int            f_global_uSec;
       u_int            f_master_Sec;
       u_int            f_master_uSec;
};
typedef struct x1timer_ioctl_event_ht t_x1timer_ioctl_event_ht;

struct x1timer_ioctl_rw{
       u_short		io_chn;
       u_short		io_data;
       u_short		io_rw;
};
typedef struct x1timer_ioctl_rw t_x1timer_ioctl_rw;
    
#endif
