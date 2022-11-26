/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xil_io.h"
#include "xtime_l.h"



//#include <stdio.h>

#include "/home/dejan/y22-g03/dut/vitis/image_array_gen/img_file_1.h"
//#include "image_array_gen/img_file_2.h"
//#include "image_array_gen/img_file_3.h"
//#include "image_array_gen/img_file_4.h"
//#include "image_array_gen/img_file_5.h"
//#include "image_array_gen/img_file_6.h"
//#include "image_array_gen/img_file_7.h"
//#include "image_array_gen/img_file_8.h"
//#include "image_array_gen/img_file_9.h"
//#include "image_array_gen/img_file_10.h"

#include "/home/dejan/y22-g03/dut/vitis/image_array_gen/tmpl_file_1.h"
//#include "image_array_gen/tmpl_file_2.h"
//#include "image_array_gen/tmpl_file_3.h"
//#include "image_array_gen/tmpl_file_4.h"
//#include "image_array_gen/tmpl_file_5.h"
//#include "image_array_gen/tmpl_file_6.h"
//#include "image_array_gen/tmpl_file_7.h"
//#include "image_array_gen/tmpl_file_8.h"
//#include "image_array_gen/tmpl_file_9.h"
//#include "image_array_gen/tmpl_file_10.h"

#include "/home/dejan/y22-g03/dut/vitis/image_array_gen/resp_file_1.h"
//#include "image_array_gen/resp_file_2.h"
//#include "image_array_gen/resp_file_3.h"
//#include "image_array_gen/resp_file_4.h"
//#include "image_array_gen/resp_file_5.h"
//#include "image_array_gen/resp_file_6.h"
//#include "image_array_gen/resp_file_7.h"
//#include "image_array_gen/resp_file_8.h"
//#include "image_array_gen/resp_file_9.h"
//#include "image_array_gen/resp_file_10.h"

#include "/home/dejan/y22-g03/dut/vitis/image_array_gen/sum1_file_1.h"
//#include "image_array_gen/sum1_file_2.h"
//#include "image_array_gen/sum1_file_3.h"
//#include "image_array_gen/sum1_file_4.h"
//#include "image_array_gen/sum1_file_5.h"
//#include "image_array_gen/sum1_file_6.h"
//#include "image_array_gen/sum1_file_7.h"
//#include "image_array_gen/sum1_file_8.h"
//#include "image_array_gen/sum1_file_9.h"
//#include "image_array_gen/sum1_file_10.h"


#include "/home/dejan/y22-g03/dut/vitis/image_array_gen/sum2_file_1.h"
//#include "image_array_gen/sum2_file_2.h"
//#include "image_array_gen/sum2_file_3.h"
//#include "image_array_gen/sum2_file_4.h"
//#include "image_array_gen/sum2_file_5.h"
//#include "image_array_gen/sum2_file_6.h"
//#include "image_array_gen/sum2_file_7.h"
//#include "image_array_gen/sum2_file_8.h"
//#include "image_array_gen/sum2_file_9.h"
//#include "image_array_gen/sum2_file_10.h"

//dimensions for first group, for others see Makefile and folder data
//#include "/home/dejan/y22-g03/dut/vitis/image_array_gen/dimensions_file_0.h"


#define TMPL_COLS 159
#define TMPL_ROWS 143
#define IMG_COLS 951

#define START_REG 0
#define TMPL_COLS_REG 4
#define TMPL_ROWS_REG 8
#define IMG_COLS_REG 12
#define READY_REG 16
#define SUM2_REG 20
#define RST_REG 24
/*//used for sum1 and resp
#define AXI_BRAM_CTRL_0 0x40000000
#define AXI_BRAM_CTRL_0_PORTA ..
#define AXI_BRAM_CTRL_0_PORTB ..

//used for tmpl
#define AXI_BRAM_CTRL_1 0x42000000
//used for img
#define AXI_BRAM_CTRL_2 0x44000000
//used for registers
#define CROSS_CORRELATION_IP 0x43C00000
*/
int main()
{

	init_platform();

	print("Platform initialized\n");

	int test_time;
	XTime gblTimeBP;
	XTime *P_gblTimeBT = &gblTimeBP;

	XTime gblTimeBR;
	XTime *P_gblTimeBR = &gblTimeBR;

	XTime gblTimeAfter;
	XTime *P_gblTimeAfter = &gblTimeAfter;

	XTime_GetTime(P_gblTimeBT);

	//-----------------------Write dimensions---------------------------------------
	Xil_Out8(XPAR_CROSS_CORRELATION_0_BASEADDR + TMPL_COLS_REG, TMPL_COLS);

	Xil_Out8(XPAR_CROSS_CORRELATION_0_BASEADDR + TMPL_ROWS_REG, TMPL_ROWS);

	Xil_Out16(XPAR_CROSS_CORRELATION_0_BASEADDR + IMG_COLS_REG, IMG_COLS);

	XTime_GetTime(P_gblTimeBR);

	//------------------------Write template in BRAM---------------------------------
	for (int i = 0; i < TMPL_ROWS * TMPL_COLS ; i++)
	{
		Xil_Out32(XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR +  4*i, tmpl_stripe[i]);
	}

	//------------------------Loop counting number of stripes------------------------
	for(int k = 0; k < 3 * 754; k++)
	{
		//--------------------Write image in BRAM------------------------------------

		for (int i = 0; i <  IMG_COLS * TMPL_ROWS; i++)
		{
			Xil_Out32(XPAR_AXI_BRAM_CTRL_0_S_AXI_BASEADDR + 4*i,  image_stripe[i]);
		}

		Xil_Out32(XPAR_AXI_BRAM_CTRL_0_S_AXI_BASEADDR + 639996,  (int)0);

		Xil_Out32(XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR +  203996,  (int)0);

		//---------------------Sending start bit---------------------------------------

		Xil_Out8(XPAR_CROSS_CORRELATION_0_BASEADDR + START_REG, 1);

		while (Xil_In32(XPAR_CROSS_CORRELATION_0_BASEADDR + READY_REG));
			Xil_Out8(XPAR_CROSS_CORRELATION_0_BASEADDR + START_REG, 0);

		//---------------------Waiting for ready----------------------------------------
		while (!Xil_In32(XPAR_CROSS_CORRELATION_0_BASEADDR + READY_REG));

		long int resp_tmp;
		long int sum1_tmp;
		long int sum2_tmp;

		//----------------------Reading sum2 -------------------------------------------
		sum2_tmp = (long int)Xil_In32(XPAR_CROSS_CORRELATION_0_BASEADDR + SUM2_REG);

		if (sum2_tmp != sum2[0])
			printf("Missmatch sum2, %d\n", (long int)Xil_In32(XPAR_CROSS_CORRELATION_0_BASEADDR + SUM2_REG));
		else
			printf("Result matching for sum2 \n");

		 //---------------------Reading resp from BRAM-----------------------------------
		for (int i = 0; i < IMG_COLS - TMPL_COLS ; i++)
		{
			resp_tmp = (long int)Xil_In32(XPAR_AXI_BRAM_CTRL_2_S_AXI_BASEADDR +  4 * i);
			if (resp_tmp != resp[i])
				printf("Missmatch, %d resp correct value is = %d on location =  %d \n",(long int)resp_tmp, resp[i], i);
			else
				printf("Result matching for resp on location %d \n", i);
		}

		//------------------------Reading sum1 from BRAM ----------------------------------
		for (int i = 0; i < IMG_COLS - TMPL_COLS ; i++)
		{
			sum1_tmp =(long int) Xil_In32(XPAR_AXI_BRAM_CTRL_3_S_AXI_BASEADDR + 4 * i);
			if (sum1_tmp != sum1[i])
				printf("Missmatch sum1 %d correct value is = %d on location =  %d \n", sum1_tmp, sum1[i], i);
			else
				printf("Result matching sum1 on location %d \n", i);
		}


		// ------------------------ Time needed for all picture -----------------------------
		XTime_GetTime(P_gblTimeAfter);

		test_time = (int) gblTimeBR - (int) gblTimeBP;

		printf("Time needed to write template to bram and axi is %d  \n", test_time);

		test_time= (int) gblTimeAfter - (int) gblTimeBR;

		printf("Time needed to write stripe to bram and finish cal and read is %d \n", test_time);
	}

	cleanup_platform();
	return 0;
}

