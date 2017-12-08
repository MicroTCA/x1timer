**
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

#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <setjmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

#include "x1timer_io.h"

struct timeval          s_time;
time_t time_new_s;
time_t time_new_u;
time_t drv_time_new_s;
time_t drv_time_new_u;
time_t time_old_s =0;
time_t time_old_u =0;
time_t drv_time_old_s =0;
time_t drv_time_old_u =0;
time_t time_dlt;
time_t time_dlt_drv;


int main(int argc, char* argv[])
{
    int                sig;
    sigset_t           mask;
    sigset_t           w_mask;
    int                code = 0;
    int                n    = 0;
    u_int              tmp_mask = 0x1;
    char nod_name[15] = "";
    int			fd;
    t_x1timer_ioctl_data	l_Ctrl;
    t_x1timer_ioctl_event       Event_Ctrl;
    x1timer_ioctl_event_ht     Event_Ctrl_ht;
    device_rw	                l_Read;

    int                   len = 0;
    int                   itemsize = 0;

    itemsize = sizeof(device_rw);

    l_Ctrl.f_Irq_Mask  = 0;
    l_Ctrl.f_Irq_Sig   = 0;
    l_Ctrl.f_Status    = 0;

    if(argc ==1){
        printf("Input \"prog /dev/x1timerX\" \n");
        return 0;
    }
    strncpy(nod_name,argv[1],sizeof(nod_name));
    fd = open (nod_name, O_RDWR);
    if (fd < 0) {
        printf ("#CAN'T OPEN FILE \n");
        exit (1);
    }
    l_Ctrl.f_Irq_Mask	= 0;
    l_Ctrl.f_Irq_Sig	= 0;
    l_Ctrl.f_Status     = 0;

    //enable IRQ on the board
    l_Read.data_rw   = 0x1;
    l_Read.offset_rw = 0xF0;
    l_Read.mode_rw   = 2;
    l_Read.barx_rw   = 0;
    l_Read.size_rw   = 0;

    len = write (fd, &l_Read, sizeof(device_rw));
    if (len != itemsize ){
            printf ("#CAN'T READ FILE \n");
    }



    printf ("\n INPUT IRQ MASK (HEX)  -");
    scanf ("%x",&tmp_mask);
    fflush(stdin);
    sigemptyset (&mask);
    sigaddset   (&mask, SIGUSR1);
    //sigaddset   (&mask, SIGUSR2);
    //sigaddset   (&mask, SIGURG);
    //sigaddset   (&mask, SIGALRM);
    sigprocmask (SIG_BLOCK, &mask, NULL);
    
    l_Ctrl.f_Irq_Mask	= (tmp_mask & 0XFFFFFFFF);
    l_Ctrl.f_Irq_Sig	= SIG1;
    l_Ctrl.f_Status     = 0;
    code = ioctl (fd, X1TIMER_ADD_SERVER_SIGNAL, &l_Ctrl);
    if (code) {
        printf ("Error X1IMER_ADD_SERVER_SIGNAL - %i\n", fd);
            printf ("#2 %d\n", code);
    }
    sigemptyset (&w_mask);
    sigaddset   (&w_mask, SIGUSR1);
    sigaddset   (&w_mask, SIGUSR2);
    while (1) {
        sigwait(&w_mask, &sig);
        printf("GET EVENT INFO FOR SIG %i\n", sig);
        code    = ioctl (fd, X1TIMER_EVENT_INFO_EXT, &Event_Ctrl_ht);
        if (code){
            printf ("Error in talking to IPTIMER driver - %i\n", fd);
        }
        gettimeofday (&s_time, NULL);
        printf("SIG IS %i\n", sig);
        printf("EVENT FLAG %X\n", Event_Ctrl_ht.f_Irq_Flag);
        printf("EVENT MASK %X\n", Event_Ctrl_ht.f_Irq_Mask);
        printf("EVENT STAUS %X\n", Event_Ctrl_ht.f_Irq_Sts);
        printf("====================================================\n");
        switch(sig){
            case(10):
                /*****CHECK STATUS*******/
                l_Ctrl.f_Irq_Mask  = 0;
                l_Ctrl.f_Irq_Sig   = 0;
                l_Ctrl.f_Status    = 0;
                time_new_s = s_time.tv_sec;
                time_new_u = s_time.tv_usec;
                time_dlt  = (time_new_s  - time_old_s) * 1000000L;
                time_dlt += (time_new_u  - time_old_u);
                drv_time_new_s = Event_Ctrl_ht.f_Irq_Sec;
                drv_time_new_u = Event_Ctrl_ht.f_Irq_uSec;
                time_dlt_drv =  (drv_time_new_s  - drv_time_old_s) * 1000000L;
                time_dlt_drv += (drv_time_new_u  - drv_time_old_u);
                printf("%i :USR1\n", n);
                printf("   DELAY     %li  local time %li\n", time_dlt, s_time.tv_sec);
                printf("   DRV_DELAY %li\n", time_dlt_drv);
                printf("   TIME      %li:%li\n", s_time.tv_sec, s_time.tv_usec);
                printf("   DRV TIME  %i:%i\n", Event_Ctrl_ht.f_Irq_Sec, Event_Ctrl_ht.f_Irq_uSec);
                printf("   EVENT INFO GEN_EVENT   %X DELAY %i\n", (u_int)Event_Ctrl_ht.f_gen_event, Event_Ctrl_ht.f_Irq_Delay);
                printf("________________________________________________\n");
                printf("   #######EVENT INFO GLOBAL_TIME %i:%i\n", (u_int)Event_Ctrl_ht.f_global_Sec, Event_Ctrl_ht.f_global_uSec);
                printf("________________________________________________\n");
                printf("   #######EVENT INFO BOARD_TIME %i:%i\n", (u_int)Event_Ctrl_ht.f_board_Sec, Event_Ctrl_ht.f_board_uSec);
                printf("________________________________________________\n");
                time_old_s     = time_new_s;
                time_old_u     = time_new_u;
                drv_time_old_s = drv_time_new_s;
                drv_time_old_u = drv_time_new_u;
                n = n+1;
                break;
            case(12):
                time_new_s = s_time.tv_sec;
                time_new_u = s_time.tv_usec;
                time_dlt  = (time_new_s  - time_old_s) * 1000000L;
                time_dlt += (time_new_u  - time_old_u);
                printf("$$$$$$$$ %i : USR2\n", n);
                printf("$$$$$$$$ DELAY     %li  local time %li\n", time_dlt, s_time.tv_sec);
                printf("$$$$$$$$ TIME      %li:%li\n", s_time.tv_sec, s_time.tv_usec);
                printf("$$$$$$$$ DRV TIME  %i:%i\n", Event_Ctrl.f_Irq_Sec, Event_Ctrl.f_Irq_uSec);
                printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
                time_old_s = time_new_s;
                time_old_u = time_new_u;
                n = 0;
                break;
            default:
                break;
	  }
    } 
    close (fd);
    exit (0);
    return(0);
}

