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

#ifndef _X1TIMER_FNC_H_
#define _X1TIMER_FNC_H_
//#include <linux/version.h>
//#include <linux/pci.h>
#include <linux/list.h>
#include "x1timer_io.h"
#ifndef X1TIMER_NR_DEVS
#define X1TIMER_NR_DEVS 15  /* x1timer0 through x1timer11 */
#endif

#define DEVNAME "x1timer"	        /* name of device */
#define X1TIMER_VENDOR_ID 0x10EE	/* XILINX vendor ID */
#define X1TIMER_DEVICE_ID 0x0020	/* X1TIMER dev board device ID */

#define X1TIMER_IRQ_MODE_OFF	 	0
#define X1TIMER_IRQ_MODE_ON	 	1
#define X1TIMER_IRQ_MODE_DEFAULT  	X1TIMER_IRQ_MODE_OFF
#define X1TIMER_MAX_SERVER	 	0x25
#define X1TIMER_MAX_DMA_SIZE	0x80000

		
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,0,1)
static inline void do_gettimeofday(struct timeval *tv)
{
	struct timespec64 now;

	ktime_get_real_ts64(&now);
	tv->tv_sec = now.tv_sec;
	tv->tv_usec = now.tv_nsec/1000;
}
#endif

struct x1timer_file_list {
    struct list_head node_file_list;
    struct file *filp;
    int      file_cnt;
};
typedef struct x1timer_file_list x1timer_file_list;

struct x1timer_server_signal{
   pid_t         f_ServerPref;
   u32           f_IrqProc[5];
   u_short     f_IrqFlag;
   u_short     f_CurSig ;
   u_short     f_HPSig ;
};
typedef struct x1timer_server_signal t_x1timer_ServerSignal;

struct x1timer_dev {
    int                       dev_num;
    int                       dev_minor;
    int                       brd_num;       /* board num dev index*/
    int                       dev_sts;
    int                       binded;          /* is binded to device*/
    int                       null_dev;        /* this is NULL Device returns ERROR to any Driver Device File Operation */
    int                       f_cntl;
    int                       slot_num;
    struct cdev          cdev;          /* Char device structure      */
    struct mutex       dev_mut;    /* mutual exclusion semaphore */
     struct mutex      dev_dma_mut;    /* mutual exclusion semaphore */
    struct pci_dev    *x1timer_pci_dev;
    struct x1timer_file_list dev_file_list;

    struct work_struct             x1timerp_work;
    int                                     soft_run;
    t_x1timer_ServerSignal     server_signal_stack [X1TIMER_MAX_SERVER];
    u16                irq_mode;
    u8                 irq_line;
    u8                 irq_pin;
    u8                 msi;
    u32                pci_dev_irq;
    spinlock_t         irq_lock;
    spinlock_t         soft_lock;
    int                irq_on;
    int                irq_lev;
    int                irq_vec;
    int                server_signals;
    u32                irq_status;
    u32                irq_mask;
    u32                irq_flag;
    u32                irq_sec;
    u32                irq_usec;
    u32                irq_sec_hi;
    u32                irq_usec_hi;
    u32                irq_status_hi;
    u32                irq_delay;
    u32                irq_delay_mask;
    u32                irq_flag_hi;
    u32                irq_mask_hi;
    u16                irq_vect_hi;
    u32                event_mask;
    u32                event_value;
    u16                irq_jitter_test;
    u32                softint;
    u32                count;
    long               gen_event;
    u32                timer_offset;
    u32                board_sec;
    u32                board_usec;
    u32                global_sec;
    u32                global_usec;
    int                  global_time;
	
    u32                master_sec;
    u32                master_usec;
    int                  master_time;
    
    int                           waitFlag;
    struct timeval          dma_start_time;
    struct timeval          dma_stop_time;
    u32                          dev_dma_size;
    u32                          dma_page_num;
    int                           dma_offset;
    int                           dma_order;
    wait_queue_head_t  waitDMA;

    u16    vendor_id;
    u16    device_id;
    u16    subvendor_id;
    u16    subdevice_id;
    u16    class_code;
    u8     revision;
    u32    devfn;
    u32    busNumber;
    u32    devNumber;
    u32    funcNumber;
    int    bus_func;
    u32    mem_base0;
    u32    mem_base0_end;
    u32    mem_base0_flag;
    void __iomem    *memmory_base0;
    loff_t          rw_off;
    
    int             x1timer_all_mems ;
    int             dev_payload_size;
    int             dev_err_detected;
    
    u_int X1TIMER_IRQ_ENABLE;
    u_int X1TIMER_IRQ_MASK;
    u_int X1TIMER_IRQ_FLAGS;
    u_int X1TIMER_IRQ_CLR_FLAGSE;
    u_int X1TIMER_MACRO_PULSE_NUM;
    u_int X1TIMER_MACRO_PULSE_NUM_H;
    u_int X1TIMER_GLOBAL_SEC;
    u_int X1TIMER_GLOBAL_USEC;
	
    u_int X1TIMER_MASTER_SEC;
    u_int X1TIMER_MASTER_USEC;
    
    u_int X1TIMER_BOARD_ID;
    u_int X1TIMER_BOARD_VERSION;
    u_int X1TIMER_BOARD_DATE;
    u_int X1TIMER_BOARD_HW_VERSION;
    u_int X1TIMER_PROJ_1_ID;
    u_int X1TIMER_PROJ_1_VERSION;
    u_int X1TIMER_PROJ_1_DATE;
    u_int X1TIMER_PROJ_1_RESERVED;
    u_int X1TIMER_PROJ_2_ID;
    u_int X1TIMER_PROJ_2_VERSION;
    u_int X1TIMER_PROJ_2_DATE;
    u_int X1TIMER_PROJ_2_RESERVED;
    u_int X1TIMER_PROJ_3_ID;
    u_int X1TIMER_PROJ_3_VERSION;
    u_int X1TIMER_PROJ_3_DATE;
    u_int X1TIMER_PROJ_3_RESERVED;
    
    u_int X1TIMER_NEW;
    u_int X1TIMER_XN;
};

#endif /* _X1TIMER_FNC_H_ */
