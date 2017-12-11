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

//#include "x1timer_io.h"
#include "pciedev_io.h"

/* useconds from struct timeval */
#define	MILLS(tv) (((double)(tv).tv_usec ) + ((double)(tv).tv_sec * 1000000.0))

int	         fd;
int main(int argc, char* argv[])
{
    int	         tmp_cnt          = 0;
    int	         tmp_bar          = 0;
    int	         base_address   = 0;
    int	         base_offset      = 0;
    char                nod_name[15] = "";
    device_rw        l_Read;
    u_int	         tmp_offset;
    u_int	         r_datat;
    u_int	         w_data;
    int                   len = 0;
    int                   itemsize = 0;
    int                   i = 0;
    int                   rtest = 0;
    int                   rd_data[100];

    itemsize = sizeof(device_rw);

    if(argc ==1){
        printf("Input \"prog /dev/x1timer0\" \n");
        return 0;
    }
    strncpy(nod_name,argv[1],sizeof(nod_name));
    fd = open (nod_name, O_RDWR);
    if (fd < 0) {
        printf ("#CAN'T OPEN FILE \n");
        exit (1);
    }
    
     printf ("\n INPUT  BAR NUM  -");
    scanf ("%x",&tmp_bar);
    fflush(stdin);
    
    printf ("\n INPUT  ADDRESS (hex)  -");
    scanf ("%x",&base_address);
    fflush(stdin);
    
    printf ("\n INPUT  START POINT  -");
    scanf ("%i",&base_offset);
    fflush(stdin);
    
    printf ("\n INPUT  COUNT  -");
    scanf ("%i",&tmp_cnt);
    fflush(stdin);
    
    printf ("\n ONLY READ TEST  -");
    scanf ("%i",&rtest);
    fflush(stdin);

    if(rtest == 1){
	for (i = 0; i < tmp_cnt ; i++) {
	    tmp_offset = i*4 + base_offset +base_address ;
	    l_Read.data_rw     = 0;
	    l_Read.offset_rw   = tmp_offset;
	    l_Read.mode_rw   = 2;
	    l_Read.barx_rw     = tmp_bar;
	    l_Read.size_rw      = 0;
	    len = read (fd, &l_Read, sizeof(device_rw));
	    if (len != itemsize ){
		printf ("#CAN'T WRITE \n");
	    }
	    rd_data[i] = l_Read.data_rw;
	}
    }
    for( ; ; ){
	if(!rtest){
	    for (i = 0; i < tmp_cnt ; i++) {
		tmp_offset = i*4 + base_offset +base_address ;
		w_data       = base_offset + i*4;

		l_Read.data_rw     = w_data;
		l_Read.offset_rw   = tmp_offset;
		l_Read.mode_rw   = 2;
		l_Read.barx_rw     = tmp_bar;
		l_Read.size_rw      = 0;
		//printf ("WRITE OFFSET - %X, DATA - %X\n",  l_Read.offset_rw, l_Read.data_rw);
		len = write (fd, &l_Read, sizeof(device_rw));
		if (len != itemsize ){
			printf ("#CAN'T WRITE \n");
		}

		l_Read.data_rw    = 0;
		l_Read.offset_rw  = tmp_offset;
		l_Read.mode_rw  = 2;
		l_Read.barx_rw    = tmp_bar;
		l_Read.size_rw     = 0;
		len = read (fd, &l_Read, sizeof(device_rw));
		if (len != itemsize ){
		    printf ("#CAN'T READ \n");
		}
		//printf ("READED : OFFSET - %X, DATA - %X \n", l_Read.offset_rw, l_Read.data_rw);
		r_datat = l_Read.data_rw;

		if (r_datat != w_data) printf ("########[%d] %x %x\n", i, w_data, r_datat);
		//usleep(50);
	    }
	}else{
	    if(rtest == 1){
		 for (i = 0; i < tmp_cnt ; i++) {
		    tmp_offset = i*4 + base_offset +base_address ;
		    l_Read.data_rw    = 0;
		    l_Read.offset_rw  = tmp_offset;
		    l_Read.mode_rw  = 2;
		    l_Read.barx_rw    = tmp_bar;
		    l_Read.size_rw     = 0;
		    len = read (fd, &l_Read, sizeof(device_rw));
		    if (len != itemsize ){
			printf ("#CAN'T READ \n");
		    }
		    //printf ("READED : OFFSET - %X, DATA - %X \n", l_Read.offset_rw, l_Read.data_rw);
		    r_datat = l_Read.data_rw;

		    if (r_datat != rd_data[i]) printf ("########[%d] %x %x\n", i, rd_data[i], r_datat);
		    //usleep(50);
		}
	    }else{
		for (i = 0; i < tmp_cnt ; i++) {
		    tmp_offset = i*4 + base_offset +base_address ;
		    w_data       = base_offset + i*4;

		    l_Read.data_rw     = w_data;
		    l_Read.offset_rw   = tmp_offset;
		    l_Read.mode_rw   = 2;
		    l_Read.barx_rw     = tmp_bar;
		    l_Read.size_rw      = 0;
		    //printf ("WRITE OFFSET - %X, DATA - %X\n",  l_Read.offset_rw, l_Read.data_rw);
		    len = write (fd, &l_Read, sizeof(device_rw));
		    if (len != itemsize ){
			    printf ("#CAN'T WRITE \n");
		    }
		    //usleep(50);
		}
	    }
	}
     }
        

    close(fd);
    return 0;
}


