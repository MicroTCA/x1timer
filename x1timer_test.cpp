**
*Copyright 2016-  DESY (Deutsches Elektronen-Synchrotron, www.desy.de)
*
*This file is part of X1TIMER driver.
*
*Foobar is free software: you can redistribute it and/or modify
*it under the terms of the GNU General Public License as published by
*the Free Software Foundation, either version 3 of the License, or
*(at your option) any later version.
*
*Foobar is distributed in the hope that it will be useful,
*but WITHOUT ANY WARRANTY; without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*GNU General Public License for more details.
*
*You should have received a copy of the GNU General Public License
*along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
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

/* useconds from struct timeval */
/* useconds from struct timeval */
#define	MIKRS(tv) (((double)(tv).tv_usec ) + ((double)(tv).tv_sec * 1000000.0)) 
#define	MILLS(tv) (((double)(tv).tv_usec/1000 )  + ((double)(tv).tv_sec * 1000.0)) 

struct timeval   start_time;
struct timeval   end_time;

int	         fd, fd1, fd2, fd3;
int main(int argc, char* argv[])
{
    int	                  ch_in        = 0;
    char                  nod_name[15] = "";
    device_rw	          l_Read;
    device_ioctrl_data    io_RW;
    t_x1timer_ioctl_data          irq_RW;
    device_ioctrl_dma     DMA_RW;
    device_ioctrl_time    DMA_TIME;
    int			  tmp_mode;
    u_int	          tmp_offset;
    int      	          tmp_data;
    int      	          tmp_print = 0;
    int                   len = 0;
    int                   itemsize = 0;
    float                 tmp_fdata;
    int*                 tmp_dma_buf;
    int*                 tmp_write_buf;
      u_int	          tmp_size;
    int                code = 0;
    double          time_tmp = 0;
    double          time_dlt;
    int      	  tmp_print_start = 0;
    int      	  tmp_print_stop  = 0;
    int            k = 0;
    int                   i = 0;
    int                   ii = 0;
    int                   rpt = 0;
     u_int	          tmp_pattern;
     u_int              tmp_mask = 0x1;
     u_int              tmp_sig    = 0x1;
     int                  loop_count = 0;
	 
	 
     //vector RW
	device_ioc_rw            ioc_ACCESS;
	device_vector_rw       ioc_VREAD; 
	u_int        tmp_dataPtr;	// In the case of more than one register access
	u_int        tmp_maskPtr;
	u_int*                        tmp_vrw_buf[6];
	device_ioc_rw            vrw_ioc_ACCESS[6];
	int                             vrw_count;
	u_int*              tmp_rw_buf;
	char*              ctmp_rw_buf;
    //
    
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

    
    
    fd1 = open (nod_name, O_RDWR);
    if (fd < 0) {
        printf ("#CAN'T OPEN FILE \n");
        exit (1);
    }
    sleep(1);
    fd2= open (nod_name, O_RDWR);
    if (fd < 0) {
        printf ("#CAN'T OPEN FILE \n");
        exit (1);
    }
    sleep(1);
    fd3 = open (nod_name, O_RDWR);
    if (fd < 0) {
        printf ("#CAN'T OPEN FILE \n");
        exit (1);
    }
    sleep(1);
    close(fd1);
    sleep(1);
    close(fd2);
    sleep(1);
    close(fd3);
    
    
    
    
    while (ch_in != 11){
        printf("\n READ (1) or WRITE (0) or END (11) ?-");
        printf("GET DRIVER VERSION (2) or GET FIRMWARE VERSION (3)?-\n");
        printf("GET SLOT NUM (4) GET_DRV_INFO (5) ?-\n");
        printf("SET GEN_EVENT MASK (6) GET GEN_EVENT MASK (7) ?-\n");
        printf("ADD SIGNAL (8) DEL SIGNAL (9) ?-\n");
        printf("VECTOR READ (10) VECTOR WRITE (11) ?-\n");
        printf("VECTOR READ X1TIMER IO(12) ?-\n");
        printf("CTRL_DMA READ (30) CTRL_DMA WRITE (31) ?-\n ");
        printf("CTRL_DMA READ /WRITE  IN LOOP (32) ?-\n ");
        printf("CTRL_DMA READ   IN LOOP (33)  WRITE  IN LOOP (34) ?-\n ");

        scanf("%d",&ch_in);
        fflush(stdin);
        l_Read.offset_rw   = 0;
        l_Read.data_rw     = 0;
        l_Read.mode_rw     = 0;
        l_Read.barx_rw     = 0;
        l_Read.size_rw     = 0;
	switch (ch_in){
            case 0 :
                printf ("\n INPUT  MODE  (0-D8,1-D16,2-D32)  -");
                scanf ("%x",&tmp_mode);
                fflush(stdin);

                printf ("\n INPUT  REG_NUM   -");
                scanf ("%i",&tmp_offset);
                fflush(stdin);
                //tmp_offset = tmp_offset*4;

                printf ("\n INPUT DATA -");
                scanf ("%i",&tmp_data);
                fflush(stdin);

                l_Read.data_rw   = tmp_data;
                l_Read.offset_rw = tmp_offset;
                l_Read.mode_rw   = tmp_mode;
                l_Read.barx_rw   = 0;
                l_Read.size_rw   = 0;

                printf ("MODE - %X , OFFSET - %X, DATA - %i\n",  
                     l_Read.mode_rw, l_Read.offset_rw, l_Read.data_rw);

                len = write (fd, &l_Read, sizeof(device_rw));
                if (len != itemsize ){
                        printf ("#CAN'T READ FILE \n");
                }

                break;
	    case 1 :
                printf ("\n INPUT  MODE  (0-D8,1-D16,2-D32)  -");
                scanf ("%x",&tmp_mode);
                fflush(stdin);
                printf ("\n INPUT REG_NUM  -");
                scanf ("%i",&tmp_offset);
                //tmp_offset = tmp_offset*4;
                fflush(stdin);		
                l_Read.data_rw   = 0;
                l_Read.offset_rw = tmp_offset;
                l_Read.mode_rw   = tmp_mode;
                l_Read.barx_rw   = 0;
                l_Read.size_rw   = 0;
                printf ("MODE - %X , OFFSET - %X, DATA - %i\n", 
                        l_Read.mode_rw, l_Read.offset_rw, l_Read.data_rw);                
                len = read (fd, &l_Read, sizeof(device_rw));
                if (len != itemsize ){
		   printf ("#CAN'T READ FILE \n");
		}
                
                printf ("READED : MODE - %X , OFFSET - %X, DATA - %i, RETURN %i\n",
			l_Read.mode_rw, l_Read.offset_rw, l_Read.data_rw, len);
		break;
           case 2 :
                len = ioctl(fd, X1TIMER_DRIVER_VERSION, &io_RW);
                tmp_fdata = (float)((float)io_RW.offset/10.0);
                tmp_fdata += (float)io_RW.data;
                printf ("DRIVER VERSION IS %f, RETURN %i\n", tmp_fdata, len);
                break;
	   case 3 :
                len = ioctl(fd, X1TIMER_FIRMWARE_VERSION, &io_RW);
                printf ("FIRMWARE VERSION IS - %X, RETURN %i\n", io_RW.data, len);
		break;
           case 4 :
                len = ioctl(fd, X1TIMER_PHYSICAL_SLOT, &io_RW);
                printf ("SLOT NUM IS - %X, RETURN %i\n", io_RW.data, len);
		break;
           case 5 :
                fflush(stdin);
                l_Read.data_rw   = 0;
                l_Read.offset_rw = 0;
                l_Read.mode_rw   = RW_INFO;
                l_Read.barx_rw   = 0;
                l_Read.size_rw   = 0;
                len = read (fd, &l_Read, sizeof(device_rw));
                if (len != itemsize ){
		   printf ("#CAN'T READ FILE \n");
		}
                printf ("READED : SLOT             - %X \n", l_Read.barx_rw);
                printf ("READED : FIRMWARE VERSION - %X \n", l_Read.mode_rw);
                tmp_fdata = (float)((float)l_Read.offset_rw/10.0);
                tmp_fdata += (float)l_Read.data_rw;
                printf ("DRIVER VERSION IS %f\n", tmp_fdata);
               break;
          case 6 :
                fflush(stdin);
                irq_RW.f_Irq_Mask = 0;
                irq_RW.f_Irq_Sig = 0;
                irq_RW.f_Status = 0;
                printf ("\n INPUT MASK (HEX)  -");
                scanf ("%x",&tmp_offset);
                fflush(stdin);
                irq_RW.f_Irq_Mask = tmp_offset;
                len = ioctl(fd, X1TIMER_EVENT_MASK, &irq_RW);
                break;
       case 7 :
                fflush(stdin);
                irq_RW.f_Irq_Mask = 0;
                irq_RW.f_Irq_Sig = 0;
                irq_RW.f_Status = 1;
                len = ioctl(fd, X1TIMER_EVENT_MASK, &irq_RW);
                printf ("GEN_EVENT MASK - %X \n", irq_RW.f_Irq_Mask);
                break;
                
      case 8 :          
                printf ("\n INPUT IRQ MASK (HEX)  -");
                scanf ("%x",&tmp_mask);
                fflush(stdin);
                
                printf ("\n INPUT SIGNAL (0-3) -");
                scanf ("%x",&tmp_sig);
                fflush(stdin);
                
                irq_RW.f_Irq_Mask	= (tmp_mask & 0XFFFFFFFF);
                irq_RW.f_Irq_Sig	= tmp_sig;
                irq_RW.f_Status     = 0;
                code = ioctl (fd, X1TIMER_ADD_SERVER_SIGNAL, &irq_RW);
                if (code) {
                    printf ("Error X1IMER_ADD_SERVER_SIGNAL - %i\n", fd);
                    printf ("#2 %d\n", code);
                }
                break;
       case 9 :          
                printf ("\n INPUT IRQ MASK (HEX)  -");
                scanf ("%x",&tmp_mask);
                fflush(stdin);
                
                irq_RW.f_Irq_Mask	= (tmp_mask & 0XFFFFFFFF);
                irq_RW.f_Irq_Sig	= 0;
                irq_RW.f_Status     = 0;
                code = ioctl (fd, X1TIMER_DEL_SERVER_SIGNAL, &irq_RW);
                if (code) {
                    printf ("Error X1IMER_ADD_SERVER_SIGNAL - %i\n", fd);
                    printf ("#2 %d\n", code);
                }
                break;         
				
       case 10:
		ioc_VREAD.number_of_rw      = 0;
		ioc_VREAD.device_ioc_rw_ptr = 0;
		vrw_count = 0;
		printf ("\n NUMBER OF READS (max 6) ? -");
		scanf ("%i",&vrw_count);
		fflush(stdin);
		ioc_VREAD.number_of_rw      = vrw_count;
		ctmp_rw_buf  = 0;
		ctmp_rw_buf                 = new char[sizeof(device_ioc_rw)*vrw_count];
		ioc_VREAD.device_ioc_rw_ptr = (pointer_type)ctmp_rw_buf;
		for(i = 0; i< vrw_count; ++i){
			vrw_ioc_ACCESS[i].rw_access_mode = 0; /* mode of rw (MTCA_SIMLE_READ,...) */
			tmp_mode = 0;
			vrw_ioc_ACCESS[i].barx_rw = 0;
			vrw_ioc_ACCESS[i].register_size = 2;
			printf ("\n OFFSET (HEX) ? -");
			scanf ("%X",&tmp_mode);
			fflush(stdin);
			vrw_ioc_ACCESS[i].offset_rw = tmp_mode;
			printf ("\n COUNT ? -");
			scanf ("%i",&tmp_mode);
			fflush(stdin);
			vrw_ioc_ACCESS[i].count_rw = tmp_mode;
			tmp_vrw_buf[i] = 0;
			tmp_vrw_buf[i] = new u_int[tmp_mode];
			vrw_ioc_ACCESS[i].dataPtr =(pointer_type)tmp_vrw_buf[i];
			memcpy((ctmp_rw_buf + i* sizeof (device_ioc_rw)), &vrw_ioc_ACCESS[i], 
																					sizeof (device_ioc_rw));
		}
		for(i = 0; i< vrw_count; ++i){
			printf ("#####READ NUM %i\n", i);
			printf ("REG_SIZE %i BAR %i OFFSET %X COUNT %i\n", 
				*(int*)((char*)ctmp_rw_buf + i* sizeof (device_ioc_rw)),
				*(int*)((char*)ctmp_rw_buf + i* sizeof (device_ioc_rw) +4),	
				*(int*)((char*)ctmp_rw_buf + i* sizeof (device_ioc_rw) +8),	
				*(int*)((char*)ctmp_rw_buf + i* sizeof (device_ioc_rw) +12)
					);
		}
		printf ("\n REPEAT)  -");
		scanf ("%x",&rpt);
		fflush(stdin);
		if(!rpt) rpt = 1;

		gettimeofday(&start_time, 0);
		for(i = 0; i < rpt; i++){
			len = ioctl(fd, PCIEDEV_VECTOR_RW, &ioc_VREAD);
			if (len < 0 ){
				printf ("#CAN'T READ FILE return %i\n", len);
			}
		}
		gettimeofday(&end_time, 0);
		printf ("===========READED  CODE %i\n", len);
		time_tmp    =  (MIKRS(end_time) - MIKRS(start_time))/rpt;
		time_dlt       =  (MILLS(end_time) - MILLS(start_time))/rpt;
		printf("STOP READING TIME %fms : %fmks  SIZE %lu\n",
							 time_dlt, time_tmp,(sizeof(int)*tmp_size));
		printf("STOP READING KBytes/Sec %f\n",((sizeof(int)*tmp_size*1000)/time_tmp));

		tmp_print = 0;
		printf ("PRINT (0 NO, 1 YES)  -\n");
		scanf ("%d",&tmp_print);
		fflush(stdin);
		while (tmp_print){
			printf ("START POS  -\n");
			scanf ("%d",&tmp_print_start);
			fflush(stdin);
			printf ("STOP POS  -\n");
			scanf ("%d",&tmp_print_stop);
			fflush(stdin);
			for(ii = 0; ii< vrw_count; ++ii){
				printf ("#####READ NUM %i\n", ii);
				k = tmp_print_start*4;
				for(int i = tmp_print_start; i < tmp_print_stop; i++){
					printf ("VREAD :NM %i, BAR %i,  MODE - %X , OFFSET - %X,  ", 
						i, vrw_ioc_ACCESS[ii].barx_rw, vrw_ioc_ACCESS[ii].register_size ,k);
					printf (" DATA - %X\n", *((u_int*)vrw_ioc_ACCESS[ii].dataPtr + i));
					k += 4;
				}
			}
			printf ("PRINT (0 NO, 1 YES)  -\n");
			scanf ("%d",&tmp_print);
			fflush(stdin);
		}
				
		for(i = 0; i< vrw_count; ++i){
			if(tmp_vrw_buf[i]) delete tmp_vrw_buf[i];
			tmp_vrw_buf[i] = 0;
		}
		if(ctmp_rw_buf) delete ctmp_rw_buf;
		ctmp_rw_buf  = 0;
		break;
		
       case 12:
		ioc_VREAD.number_of_rw      = 0;
		ioc_VREAD.device_ioc_rw_ptr = 0;
		vrw_count = 0;
		printf ("\n NUMBER OF READS (max 6) ? -");
		scanf ("%i",&vrw_count);
		fflush(stdin);
		ioc_VREAD.number_of_rw      = vrw_count;
		ctmp_rw_buf  = 0;
		ctmp_rw_buf                 = new char[sizeof(device_ioc_rw)*vrw_count];
		ioc_VREAD.device_ioc_rw_ptr = (pointer_type)ctmp_rw_buf;
		for(i = 0; i< vrw_count; ++i){
			vrw_ioc_ACCESS[i].rw_access_mode = 0; /* mode of rw (MTCA_SIMLE_READ,...) */
			tmp_mode = 0;
			vrw_ioc_ACCESS[i].barx_rw = 0;
			vrw_ioc_ACCESS[i].register_size = 2;
			printf ("\n OFFSET (HEX) ? -");
			scanf ("%X",&tmp_mode);
			fflush(stdin);
			vrw_ioc_ACCESS[i].offset_rw = tmp_mode;
			printf ("\n COUNT ? -");
			scanf ("%i",&tmp_mode);
			fflush(stdin);
			vrw_ioc_ACCESS[i].count_rw = tmp_mode;
			tmp_vrw_buf[i] = 0;
			tmp_vrw_buf[i] = new u_int[tmp_mode];
			vrw_ioc_ACCESS[i].dataPtr =(pointer_type)tmp_vrw_buf[i];
			memcpy((ctmp_rw_buf + i* sizeof (device_ioc_rw)), &vrw_ioc_ACCESS[i], 
																					sizeof (device_ioc_rw));
		}
		for(i = 0; i< vrw_count; ++i){
			printf ("#####READ NUM %i\n", i);
			printf ("REG_SIZE %i BAR %i OFFSET %X COUNT %i\n", 
				*(int*)((char*)ctmp_rw_buf + i* sizeof (device_ioc_rw)),
				*(int*)((char*)ctmp_rw_buf + i* sizeof (device_ioc_rw) +4),	
				*(int*)((char*)ctmp_rw_buf + i* sizeof (device_ioc_rw) +8),	
				*(int*)((char*)ctmp_rw_buf + i* sizeof (device_ioc_rw) +12)
					);
		}
		printf ("\n REPEAT)  -");
		scanf ("%x",&rpt);
		fflush(stdin);
		if(!rpt) rpt = 1;

		gettimeofday(&start_time, 0);
		for(i = 0; i < rpt; i++){
			len = ioctl(fd, X1TIMER_VECTOR_RW, &ioc_VREAD);
			if (len < 0 ){
				printf ("#CAN'T READ FILE return %i\n", len);
			}
		}
		gettimeofday(&end_time, 0);
		printf ("===========READED  CODE %i\n", len);
		time_tmp    =  (MIKRS(end_time) - MIKRS(start_time))/rpt;
		time_dlt       =  (MILLS(end_time) - MILLS(start_time))/rpt;
		printf("STOP READING TIME %fms : %fmks  SIZE %lu\n",
							 time_dlt, time_tmp,(sizeof(int)*tmp_size));
		printf("STOP READING KBytes/Sec %f\n",((sizeof(int)*tmp_size*1000)/time_tmp));

		tmp_print = 0;
		printf ("PRINT (0 NO, 1 YES)  -\n");
		scanf ("%d",&tmp_print);
		fflush(stdin);
		while (tmp_print){
			printf ("START POS  -\n");
			scanf ("%d",&tmp_print_start);
			fflush(stdin);
			printf ("STOP POS  -\n");
			scanf ("%d",&tmp_print_stop);
			fflush(stdin);
			for(ii = 0; ii< vrw_count; ++ii){
				printf ("#####READ NUM %i\n", ii);
				k = tmp_print_start*4;
				for(int i = tmp_print_start; i < tmp_print_stop; i++){
					printf ("VREAD :NM %i, BAR %i,  MODE - %X , OFFSET - %X,  ", 
						i, vrw_ioc_ACCESS[ii].barx_rw, vrw_ioc_ACCESS[ii].register_size ,k);
					printf (" DATA - %X\n", *((u_int*)vrw_ioc_ACCESS[ii].dataPtr + i));
					k += 4;
				}
			}
			printf ("PRINT (0 NO, 1 YES)  -\n");
			scanf ("%d",&tmp_print);
			fflush(stdin);
		}
				
		for(i = 0; i< vrw_count; ++i){
			if(tmp_vrw_buf[i]) delete tmp_vrw_buf[i];
			tmp_vrw_buf[i] = 0;
		}
		if(ctmp_rw_buf) delete ctmp_rw_buf;
		ctmp_rw_buf  = 0;
		break;
			
       case 30 :
                DMA_RW.dma_offset  = 0;
                DMA_RW.dma_size    = 0;
                DMA_RW.dma_cmd     = 0;
                DMA_RW.dma_pattern = 0; 
                printf ("\n INPUT  DMA_SIZE (num of sumples (int))  -");
                scanf ("%d",&tmp_size);
                fflush(stdin);
                DMA_RW.dma_size    = sizeof(int)*tmp_size;
                printf ("\n INPUT OFFSET (int)  -");
                scanf ("%d",&tmp_offset);
                fflush(stdin);
                DMA_RW.dma_offset = tmp_offset;
                
                printf ("DMA_OFFSET - %X, DMA_SIZE - %X\n", DMA_RW.dma_offset, DMA_RW.dma_size);
                printf ("MAX_MEM- %X, DMA_MEM - %X:%X\n", 536870912,  (DMA_RW.dma_offset + DMA_RW.dma_size),
                                                                                              (DMA_RW.dma_offset + DMA_RW.dma_size*4));
                
                
                tmp_dma_buf     = new int[tmp_size + DMA_DATA_OFFSET];
                memcpy(tmp_dma_buf, &DMA_RW, sizeof (device_ioctrl_dma));
               
                gettimeofday(&start_time, 0);
                code = ioctl (fd, X1TIMER_READ_DMA, tmp_dma_buf);
                gettimeofday(&end_time, 0);
                printf ("===========READED  CODE %i\n", code);
                time_tmp    =  MIKRS(end_time) - MIKRS(start_time);
                time_dlt       =  MILLS(end_time) - MILLS(start_time);
                printf("STOP READING TIME %fms : %fmks  SIZE %lu\n", time_dlt, time_tmp,(sizeof(int)*tmp_size));
                printf("STOP READING KBytes/Sec %f\n",((sizeof(int)*tmp_size*1000)/time_tmp));
                code = ioctl (fd, X1TIMER_DMA_TIME, &DMA_TIME);
                if (code) {
                    printf ("######ERROR GET TIME %d\n", code);
                }
                printf ("===========DRIVER TIME \n");
                time_tmp = MIKRS(DMA_TIME.stop_time) - MIKRS(DMA_TIME.start_time);
                time_dlt    = MILLS(DMA_TIME.stop_time) - MILLS(DMA_TIME.start_time);
                printf("STOP DRIVER TIME START %li:%li STOP %li:%li\n",
                                                            DMA_TIME.start_time.tv_sec, DMA_TIME.start_time.tv_usec, 
                                                            DMA_TIME.stop_time.tv_sec, DMA_TIME.stop_time.tv_usec);
                printf("STOP DRIVER READING TIME %fms : %fmks  SIZE %lu\n", time_dlt, time_tmp,(sizeof(int)*tmp_size));
                printf("STOP DRIVER READING KBytes/Sec %f\n",((sizeof(int)*tmp_size*1000)/time_tmp));
                printf ("PRINT (0 NO, 1 YES)  -\n");
                scanf ("%d",&tmp_print);
                fflush(stdin);
                while (tmp_print){
                    printf ("START POS  -\n");
                    scanf ("%d",&tmp_print_start);
                    fflush(stdin);
                    printf ("STOP POS  -\n");
                    scanf ("%d",&tmp_print_stop);
                    fflush(stdin);
                    k = tmp_print_start*4;
                    for(int i = tmp_print_start; i < tmp_print_stop; i++){
                            printf("NUM %i OFFSET %X : DATA %X\n", i,k, (u_int)(tmp_dma_buf[i] & 0xFFFFFFFF));
                            k += 4;
                    }
                    printf ("PRINT (0 NO, 1 YES)  -\n");
		    scanf ("%d",&tmp_print);
		    fflush(stdin);
                }
                if(tmp_dma_buf) delete tmp_dma_buf;
                break;
       case 31 :
                DMA_RW.dma_offset  = 0;
                DMA_RW.dma_size    = 0;
                DMA_RW.dma_cmd     = 0;
                DMA_RW.dma_pattern = 0; 
                printf ("\n INPUT  DMA_SIZE (num of sumples (int))  -");
                scanf ("%d",&tmp_size);
                fflush(stdin);
                DMA_RW.dma_size = sizeof(int)*tmp_size;
                printf ("\n INPUT OFFSET (int)  -");
                scanf ("%d",&tmp_offset);
                fflush(stdin);
                DMA_RW.dma_offset = tmp_offset;

                printf ("\n INPUT PATTERN (HEX)  -");
                scanf ("%x",&tmp_pattern);
                fflush(stdin);

                tmp_write_buf     = new int[tmp_size + DMA_DATA_OFFSET];
                memcpy(tmp_write_buf, &DMA_RW, sizeof (device_ioctrl_dma));

                for(int ii = 0; ii < tmp_size; ++ii){
                    tmp_write_buf[ii + DMA_DATA_OFFSET] = tmp_pattern + ii;
                }
                k= 0;
                for(int i = 0; i < 10; i++){
                    printf("OFFSET %d : DATA %X\n", k, (u_int)(tmp_write_buf[i] & 0xFFFFFFFF));
                    k++;
                }

                gettimeofday(&start_time, 0);
                code = ioctl (fd, X1TIMER_WRITE_DMA, tmp_write_buf);
                gettimeofday(&end_time, 0);
                if (code) {
                    printf ("######ERROR DMA  %d\n", code);
                }
                printf ("===========WRITE  CODE %i\n", code);
                time_tmp    =  MIKRS(end_time) - MIKRS(start_time);
                time_dlt       =  MILLS(end_time) - MILLS(start_time);
                printf("STOP WRITE TIME %fms : %fmks  SIZE %li\n", time_dlt, time_tmp,(sizeof(int)*tmp_size));
                printf("STOP WRITE KBytes/Sec %f\n",((sizeof(int)*tmp_size*1000)/time_tmp));
                code = ioctl (fd, X1TIMER_DMA_TIME, &DMA_TIME);
                if (code) {
                    printf ("######ERROR GET TIME %d\n", code);
                }
                printf ("===========DRIVER TIME \n");
                time_tmp = MIKRS(DMA_TIME.stop_time) - MIKRS(DMA_TIME.start_time);
                time_dlt    = MILLS(DMA_TIME.stop_time) - MILLS(DMA_TIME.start_time);
                printf("STOP DRIVER WRITING TIME %fms : %fmks  SIZE %li\n", time_dlt, time_tmp,(sizeof(int)*tmp_size));
                printf("STOP DRIVER WRITING KBytes/Sec %f\n",((sizeof(int)*tmp_size*1000)/time_tmp));
                if(tmp_write_buf) delete tmp_write_buf;
                break;
       case 32 :
                DMA_RW.dma_offset  = 0;
                DMA_RW.dma_size    = 0;
                DMA_RW.dma_cmd     = 0;
                DMA_RW.dma_pattern = 0; 
                printf ("\n INPUT  DMA_SIZE (num of sumples (int))  -");
                scanf ("%d",&tmp_size);
                fflush(stdin);
                printf ("\n INPUT OFFSET (int)  -");
                scanf ("%d",&tmp_offset);
                fflush(stdin);
                printf ("\n INPUT PATTERN (HEX)  -");
                scanf ("%x",&tmp_pattern);
                fflush(stdin);
                printf ("DMA_OFFSET - %X, DMA_SIZE - %X\n", DMA_RW.dma_offset, DMA_RW.dma_size);
                printf ("MAX_MEM- %X, DMA_MEM - %X:%X\n", 536870912,  (DMA_RW.dma_offset + DMA_RW.dma_size),
                                                                                              (DMA_RW.dma_offset + DMA_RW.dma_size*4));
                
                loop_count = 0;
                while (1){
                    tmp_dma_buf      = new int[tmp_size + DMA_DATA_OFFSET];
                    tmp_write_buf     = new int[tmp_size + DMA_DATA_OFFSET];

                    DMA_RW.dma_size    = sizeof(int)*tmp_size;
                    DMA_RW.dma_offset = tmp_offset;
                    memcpy(tmp_dma_buf, &DMA_RW, sizeof (device_ioctrl_dma));
                    code = ioctl (fd, X1TIMER_READ_DMA, tmp_dma_buf);
                    if (code) {
                        printf ("###################ERROR DMA  %d\n", code);
                    }
                    k = 0;
                    for(int i = 0; i < 10; i++){
                            printf("NUM %i OFFSET %X : DATA %X\n", i,k, (u_int)(tmp_dma_buf[i] & 0xFFFFFFFF));
                            k += 4;
                    }
                    
                    usleep(20000);
                    
                    l_Read.data_rw   = 0;
                    l_Read.offset_rw  = 0;
                    l_Read.mode_rw   = 2;
                    l_Read.barx_rw   = 0;
                    l_Read.size_rw   = 0;
                    len = read (fd, &l_Read, sizeof(device_rw));
                    if (len != itemsize ){
                       printf ("#CAN'T READ FILE \n");
                    }
                    printf ("AFTER DMA_READ READED 0x0 : MODE - %X , OFFSET - %X, DATA - %X, RETURN %i \n",
                                               l_Read.mode_rw, l_Read.offset_rw, l_Read.data_rw, len);
                    
                    l_Read.data_rw   = 0;
                    l_Read.offset_rw  = 4;
                    l_Read.mode_rw   = 2;
                    l_Read.barx_rw   = 0;
                    l_Read.size_rw   = 0;
                    len = read (fd, &l_Read, sizeof(device_rw));
                    if (len != itemsize ){
                       printf ("#CAN'T READ FILE \n");
                    }
                    printf ("AFTER DMA_READ READED 0x4 : MODE - %X , OFFSET - %X, DATA - %X, RETURN %i\n",
                                               l_Read.mode_rw, l_Read.offset_rw, l_Read.data_rw, len);
                    
                    l_Read.data_rw   = 0;
                    l_Read.offset_rw  = 8;
                    l_Read.mode_rw   = 2;
                    l_Read.barx_rw   = 0;
                    l_Read.size_rw   = 0;
                    len = read (fd, &l_Read, sizeof(device_rw));
                    if (len != itemsize ){
                       printf ("#CAN'T READ FILE \n");
                    }
                    printf ("AFTER DMA_READ READED 0x8 : MODE - %X , OFFSET - %X, DATA - %X, RETURN %i\n",
                                               l_Read.mode_rw, l_Read.offset_rw, l_Read.data_rw, len);
                    
                    DMA_RW.dma_size    = sizeof(int)*tmp_size;
                    DMA_RW.dma_offset = tmp_offset;
                    memcpy(tmp_write_buf, &DMA_RW, sizeof (device_ioctrl_dma));
                    for(int ii = 0; ii < tmp_size; ++ii){
                        tmp_write_buf[ii + DMA_DATA_OFFSET] = tmp_pattern + ii + loop_count;
                    }
                    code = ioctl (fd, X1TIMER_WRITE_DMA, tmp_write_buf);
                    if (code) {
                        printf ("###################ERROR DMA  %d\n", code);
                    }
                    
                    usleep(20000);
                    
                    if(tmp_dma_buf) delete tmp_dma_buf;
                    if(tmp_write_buf) delete tmp_write_buf;
                    
                    
                    
                    l_Read.data_rw   = 0;
                    l_Read.offset_rw  = 0;
                    l_Read.mode_rw   = 2;
                    l_Read.barx_rw   = 0;
                    l_Read.size_rw   = 0;
                    len = read (fd, &l_Read, sizeof(device_rw));
                    if (len != itemsize ){
                       printf ("#CAN'T READ FILE \n");
                    }
                    printf ("AFTER DMA_WRITE READED 0x0 : MODE - %X , OFFSET - %X, DATA - %X, RETURN %i \n",
                                               l_Read.mode_rw, l_Read.offset_rw, l_Read.data_rw, len);
                    
                    l_Read.data_rw   = 0;
                    l_Read.offset_rw  = 4;
                    l_Read.mode_rw   = 2;
                    l_Read.barx_rw   = 0;
                    l_Read.size_rw   = 0;
                    len = read (fd, &l_Read, sizeof(device_rw));
                    if (len != itemsize ){
                       printf ("#CAN'T READ FILE \n");
                    }
                    printf ("AFTER DMA_WRITE READED 0x4 : MODE - %X , OFFSET - %X, DATA - %X, RETURN %i\n",
                                               l_Read.mode_rw, l_Read.offset_rw, l_Read.data_rw, len);
                    
                    l_Read.data_rw   = 0;
                    l_Read.offset_rw  = 8;
                    l_Read.mode_rw   = 2;
                    l_Read.barx_rw   = 0;
                    l_Read.size_rw   = 0;
                    len = read (fd, &l_Read, sizeof(device_rw));
                    if (len != itemsize ){
                       printf ("#CAN'T READ FILE \n");
                    }
                    printf ("AFTER DMA_WRITE READED 0x8 : MODE - %X , OFFSET - %X, DATA - %X, RETURN %i\n",
                                               l_Read.mode_rw, l_Read.offset_rw, l_Read.data_rw, len);
                    
                    usleep(10000);
                    if(loop_count > 0x8000) loop_count = 0;
                    loop_count++;
                }
                
                break;
       case 33 :
                DMA_RW.dma_offset  = 0;
                DMA_RW.dma_size    = 0;
                DMA_RW.dma_cmd     = 0;
                DMA_RW.dma_pattern = 0; 
                printf ("\n INPUT  DMA_SIZE (num of sumples (int))  -");
                scanf ("%d",&tmp_size);
                fflush(stdin);
                printf ("\n INPUT OFFSET (int)  -");
                scanf ("%d",&tmp_offset);
                fflush(stdin);
                printf ("\n INPUT PATTERN (HEX)  -");
                scanf ("%x",&tmp_pattern);
                fflush(stdin);
                printf ("DMA_OFFSET - %X, DMA_SIZE - %X\n", DMA_RW.dma_offset, DMA_RW.dma_size);
                printf ("MAX_MEM- %X, DMA_MEM - %X:%X\n", 536870912,  (DMA_RW.dma_offset + DMA_RW.dma_size),
                                                                                              (DMA_RW.dma_offset + DMA_RW.dma_size*4));
                
                loop_count = 0;
                while (1){
                    tmp_dma_buf      = new int[tmp_size + DMA_DATA_OFFSET];

                    DMA_RW.dma_size    = sizeof(int)*tmp_size;
                    DMA_RW.dma_offset = tmp_offset;
                    memcpy(tmp_dma_buf, &DMA_RW, sizeof (device_ioctrl_dma));
                    code = ioctl (fd, X1TIMER_READ_DMA, tmp_dma_buf);
                    if (code) {
                        printf ("###################ERROR DMA  %d\n", code);
                    }
                    k = 0;
                    for(int i = 0; i < 10; i++){
                            printf("NUM %i OFFSET %X : DATA %X\n", i,k, (u_int)(tmp_dma_buf[i] & 0xFFFFFFFF));
                            k += 4;
                    }
                    
                    if(tmp_dma_buf) delete tmp_dma_buf;
                    
                    l_Read.data_rw   = 0;
                    l_Read.offset_rw  = 0;
                    l_Read.mode_rw   = 2;
                    l_Read.barx_rw   = 0;
                    l_Read.size_rw   = 0;
                    len = read (fd, &l_Read, sizeof(device_rw));
                    if (len != itemsize ){
                       printf ("#CAN'T READ FILE \n");
                    }
                    printf ("AFTER DMA_READ READED 0x0 : MODE - %X , OFFSET - %X, DATA - %X, RETURN %i \n",
                                               l_Read.mode_rw, l_Read.offset_rw, l_Read.data_rw, len);
                    
                    l_Read.data_rw   = 0;
                    l_Read.offset_rw  = 4;
                    l_Read.mode_rw   = 2;
                    l_Read.barx_rw   = 0;
                    l_Read.size_rw   = 0;
                    len = read (fd, &l_Read, sizeof(device_rw));
                    if (len != itemsize ){
                       printf ("#CAN'T READ FILE \n");
                    }
                    printf ("AFTER DMA_READ READED 0x4 : MODE - %X , OFFSET - %X, DATA - %X, RETURN %i\n",
                                               l_Read.mode_rw, l_Read.offset_rw, l_Read.data_rw, len);
                    
                    l_Read.data_rw   = 0;
                    l_Read.offset_rw  = 8;
                    l_Read.mode_rw   = 2;
                    l_Read.barx_rw   = 0;
                    l_Read.size_rw   = 0;
                    len = read (fd, &l_Read, sizeof(device_rw));
                    if (len != itemsize ){
                       printf ("#CAN'T READ FILE \n");
                    }
                    printf ("AFTER DMA_READ READED 0x8 : MODE - %X , OFFSET - %X, DATA - %X, RETURN %i\n",
                                               l_Read.mode_rw, l_Read.offset_rw, l_Read.data_rw, len);
                    
                    usleep(10000);
                    if(loop_count > 0x8000) loop_count = 0;
                    loop_count++;
                }
                
                break;
       case 34 :
                DMA_RW.dma_offset  = 0;
                DMA_RW.dma_size    = 0;
                DMA_RW.dma_cmd     = 0;
                DMA_RW.dma_pattern = 0; 
                printf ("\n INPUT  DMA_SIZE (num of sumples (int))  -");
                scanf ("%d",&tmp_size);
                fflush(stdin);
                printf ("\n INPUT OFFSET (int)  -");
                scanf ("%d",&tmp_offset);
                fflush(stdin);
                printf ("\n INPUT PATTERN (HEX)  -");
                scanf ("%x",&tmp_pattern);
                fflush(stdin);
                printf ("DMA_OFFSET - %X, DMA_SIZE - %X\n", DMA_RW.dma_offset, DMA_RW.dma_size);
                printf ("MAX_MEM- %X, DMA_MEM - %X:%X\n", 536870912,  (DMA_RW.dma_offset + DMA_RW.dma_size),
                                                                                              (DMA_RW.dma_offset + DMA_RW.dma_size*4));
                
                loop_count = 0;
                while (1){
                    tmp_write_buf     = new int[tmp_size + DMA_DATA_OFFSET];

                    
                    DMA_RW.dma_size    = sizeof(int)*tmp_size;
                    DMA_RW.dma_offset = tmp_offset;
                    memcpy(tmp_write_buf, &DMA_RW, sizeof (device_ioctrl_dma));
                    for(int ii = 0; ii < tmp_size; ++ii){
                        tmp_write_buf[ii + DMA_DATA_OFFSET] = tmp_pattern + ii + loop_count;
                    }
                    code = ioctl (fd, X1TIMER_WRITE_DMA, tmp_write_buf);
                    if (code) {
                        printf ("###################ERROR DMA  %d\n", code);
                    }
                    if(tmp_write_buf) delete tmp_write_buf;
                    
                    
                    
                    l_Read.data_rw   = 0;
                    l_Read.offset_rw  = 0;
                    l_Read.mode_rw   = 2;
                    l_Read.barx_rw   = 0;
                    l_Read.size_rw   = 0;
                    len = read (fd, &l_Read, sizeof(device_rw));
                    if (len != itemsize ){
                       printf ("#CAN'T READ FILE \n");
                    }
                    printf ("AFTER DMA_WRITE READED 0x0 : MODE - %X , OFFSET - %X, DATA - %X, RETURN %i \n",
                                               l_Read.mode_rw, l_Read.offset_rw, l_Read.data_rw, len);
                    
                    l_Read.data_rw   = 0;
                    l_Read.offset_rw  = 4;
                    l_Read.mode_rw   = 2;
                    l_Read.barx_rw   = 0;
                    l_Read.size_rw   = 0;
                    len = read (fd, &l_Read, sizeof(device_rw));
                    if (len != itemsize ){
                       printf ("#CAN'T READ FILE \n");
                    }
                    printf ("AFTER DMA_WRITE READED 0x4 : MODE - %X , OFFSET - %X, DATA - %X, RETURN %i\n",
                                               l_Read.mode_rw, l_Read.offset_rw, l_Read.data_rw, len);
                    
                    l_Read.data_rw   = 0;
                    l_Read.offset_rw  = 8;
                    l_Read.mode_rw   = 2;
                    l_Read.barx_rw   = 0;
                    l_Read.size_rw   = 0;
                    len = read (fd, &l_Read, sizeof(device_rw));
                    if (len != itemsize ){
                       printf ("#CAN'T READ FILE \n");
                    }
                    printf ("AFTER DMA_WRITE READED 0x8 : MODE - %X , OFFSET - %X, DATA - %X, RETURN %i\n",
                                               l_Read.mode_rw, l_Read.offset_rw, l_Read.data_rw, len);
                    
                    usleep(10000);
                    if(loop_count > 0x8000) loop_count = 0;
                    loop_count++;
                }
                
                break;
       default:
               break;
    }
    }

    close(fd);
    return 0;
}

