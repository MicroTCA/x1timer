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

time_t time_dlt;
time_t time_dlt_drv;
time_t time_dlt_even;
time_t time_dlt_drv_even;

struct sig_event  {
       time_t sig_time_s;
       time_t sig_time_u;
       time_t sig_time_old_s;
       time_t sig_time_old_u;
       time_t drv_sig_time_new_s;
       time_t drv_sig_time_new_u;
       time_t drv_sig_time_old_s;
       time_t drv_sig_time_old_u;
       u_int		sig_mask;
       u_int		sig_flag;
};

int main(int argc, char* argv[])
{
    int                i;
	int                n = 0;
	int                k;
	int                odd = 0;
	int                mask_sts = 0;
	int                was_matched = 0;
    int                sig;
    sigset_t           mask;
    sigset_t           w_mask;
    int                code = 0;
    u_int              tmp_mask = 0x1;
    char nod_name[15] = "";
    int			fd;
    t_x1timer_ioctl_data	l_Ctrl;
    t_x1timer_ioctl_event   Event_Ctrl;
    device_rw	            l_Read;

    int                   len = 0;
    int                   itemsize = 0;
	
	struct sig_event      sig_event[4];

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

    printf("ENABLING IRQ\n");
    len = write (fd, &l_Read, sizeof(device_rw));
    if (len != itemsize ){
            printf ("#CAN'T READ FILE \n");
    }

    printf ("INPUT NUMBER OF IRQs (up to 4) -\n");
    scanf ("%i",&n);
    fflush(stdin);

    for(i = 0; i < n; i++){
        printf ("INPUT IRQ MASK (HEX)  -\n");
        scanf ("%x",&(sig_event[i].sig_mask));
        fflush(stdin);
		tmp_mask |= sig_event[i].sig_mask;
	}
	printf("SIG_MASK IS %X\n",tmp_mask);
	for(i = 0; i < n; i++){
        printf ("EVENT_NUM %i EVENT_MASK %x\n", i, sig_event[i].sig_mask);
	}
	printf ("INPUT ANY NUMBER TO START  -\n");
    scanf ("%x",&code);
    fflush(stdin);
    code = 0;
    sigemptyset (&mask);
    sigaddset   (&mask, SIGUSR1);
    
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
		
        code    = ioctl (fd, X1TIMER_EVENT_INFO, &Event_Ctrl);
        if (code){
            printf ("Error in talking to IPTIMER driver - %i\n", fd);
        }
        gettimeofday (&s_time, NULL);
        was_matched = 0;
        for(i = 0; i < n; i++){
            //printf("SIG_EVENT_NUM %i EVENT_MASK %X\n", i, sig_event[i].sig_mask);
            mask_sts = Event_Ctrl.f_Irq_Flag & sig_event[i].sig_mask;
            if(mask_sts){
                was_matched++;
                //printf("SIG_EVENT %i MASK_STS %X ODD %i\n", i, mask_sts, odd);
                sig_event[i].sig_flag                = Event_Ctrl.f_Irq_Flag;
                sig_event[i].sig_time_s            = s_time.tv_sec;
                sig_event[i].sig_time_u            = s_time.tv_usec;
                sig_event[i].drv_sig_time_new_s   =  Event_Ctrl.f_Irq_Sec;
                sig_event[i].drv_sig_time_new_u  = Event_Ctrl.f_Irq_uSec;
                
				
                if(i == 0){
                    printf("**********EVENT FLAG %X\n", Event_Ctrl.f_Irq_Flag);
                    printf("**********EVENT MASK %X\n", Event_Ctrl.f_Irq_Mask);
                    printf("**********EVENT STAUS %X\n", Event_Ctrl.f_Irq_Sts);

                    time_dlt_even   = (sig_event[i].sig_time_s  - sig_event[i].sig_time_old_s) * 1000000L;
                    time_dlt_even += (sig_event[i].sig_time_u  - sig_event[i].sig_time_old_u);
                    time_dlt_drv_even    =  (sig_event[i].drv_sig_time_new_s  - sig_event[i].drv_sig_time_old_s) * 1000000L;
                    time_dlt_drv_even += (sig_event[i].drv_sig_time_new_u  - sig_event[i].drv_sig_time_old_u);

                    printf("**********DELAY     %li  local time %li\n",time_dlt_even, s_time.tv_sec);
                    printf("**********DRV_DELAY %li\n", time_dlt_drv_even);
                    printf("**********TIME      %li:%li\n", s_time.tv_sec, s_time.tv_usec);
                    printf("**********DRV TIME  %i:%i\n", Event_Ctrl.f_Irq_Sec, Event_Ctrl.f_Irq_uSec);
                    printf("**********EVENT INFO GEN_EVENT   %X DELAY %i\n", (u_int)Event_Ctrl.f_gen_event, Event_Ctrl.f_Irq_Delay);
                    printf("====================================================\n");

                    for(k = 1; k < n; k++){
                        printf("SIG_EVENT %i MASK %X FLAG %X ODD %i\n", k, sig_event[k].sig_mask, sig_event[k].sig_flag, odd);
                        time_dlt_even   = (sig_event[i].sig_time_s  - sig_event[k].sig_time_old_s) * 1000000L;
                        time_dlt_even += (sig_event[i].sig_time_u  - sig_event[k].sig_time_old_u);
                        //time_dlt_drv_even    =  (sig_event[i].drv_sig_time_new_s  - sig_event[k].drv_sig_time_old_s) * 1000000L;
                        //time_dlt_drv_even += (sig_event[i].drv_sig_time_new_u  - sig_event[k].drv_sig_time_old_u);
                        printf("DELAY   FROM EVENT %i -  %li \n", time_dlt);
                        //printf("DRV_DELAY FROM EVENT %i -  %li\n", time_dlt_drv);
                        printf("*******************************************************\n");
                    }
                }
                
                sig_event[i].sig_time_old_s            = sig_event[i].sig_time_s;
                sig_event[i].sig_time_old_u            = sig_event[i].sig_time_u;
                sig_event[i].drv_sig_time_old_s     = sig_event[i].drv_sig_time_new_s;
                sig_event[i].drv_sig_time_old_u    = sig_event[i].drv_sig_time_new_u;
                
				
            }
            mask_sts = 0;
	    }
        if(!was_matched){
            printf("&&&&&&&&&&&&&NO MATCHED EVENT&&&&&&&&&&&&&&\n");
		}
        //printf("\n"); 
		if(!odd){
		    odd = 1;
	    }else{
		    odd = 0;
		}  
    } 
    close (fd);
    exit (0);
    return(0);
}

