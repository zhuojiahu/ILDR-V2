/*;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  Copyright (c) 1994-1998, ADLINK Technology Inc.  All rights reserved.  ;;
;;                                                                         ;;
;;      File Name   :   DLL2.H                                             ;;
;;      Purpose     :   Header File of ACLS-DLL2 for W95 & WNT Libraries   ;;
;;      Date        :   4/22/1998                                          ;;
;;      Version     :   3.1                                                ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/
#ifndef DLL2_NT_H
#define DLL2_NT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef char            I8;
typedef unsigned char   U8;
typedef short           I16;
typedef unsigned short  U16;
typedef long            I32;
typedef unsigned long   U32;
typedef float           F32;
typedef double          F64;
typedef char            Boolean;


#ifndef COMMON_DLL1_DLL2
#define COMMON_DLL1_DLL2
/******** Error code definition for ACL cards ********/
enum {
    ERR_NoError,                        // 0
    ERR_BoardNoInit,                    // 1
    ERR_InvalidBoardNumber,             // 2
    ERR_InitializedBoardNumber,         // 3
    ERR_BaseAddressError,               // 4
    ERR_BaseAddressConflict,            // 5
    ERR_DuplicateBoardSetting,          // 6
    ERR_DuplicateIrqSetting,            // 7
    ERR_PortError,                      // 8
    ERR_ChannelError,                   // 9
    ERR_InvalidADChannel,               //10
    ERR_InvalidDAChannel,               //11
    ERR_InvalidDIChannel,               //12
    ERR_InvalidDOChannel,               //13
    ERR_InvalidDIOChannel,              //14
    ERR_InvalidIRQChannel,              //15
    ERR_InvalidDMAChannel,              //16
    ERR_InvalidChangeValue,             //17
    ERR_InvalidTimerValue,              //18
    ERR_InvalidTimerMode,               //19
    ERR_InvalidCounterValue,            //20
    ERR_InvalidCounterMode,             //21
    ERR_InvalidADMode,                  //22
    ERR_InvalidMode,                    //23
    ERR_NotOutputPort,                  //24
    ERR_NotInputPort,                   //25
    ERR_AD_DMANotSet,                   //26
    ERR_AD_INTNotSet,                   //27
    ERR_AD_AquireTimeOut,               //28
    ERR_AD_InvalidGain,                 //29
    ERR_INTNotSet,                      //30
    ERR_InvalidPortNumber,              //31
    ERR_InvalidTrigSrc,                 //32
    ERR_InvalidINTMode,                 //33
    ERR_TotalErrorCount                 //34
};

/******** possible value of card_number ********/
#define CARD_MaxCardNo  8
#define CARD_1     0
#define CARD_2     1
#define CARD_3     2
#define CARD_4     3
#define CARD_5     4
#define CARD_6     5
#define CARD_7     6
#define CARD_8     7

/******** Mode of Timer #0 (8112PG/DG/HG, 8216, 8316/12 for Dll2) ********/
#define TIMER_MODE0        0    //Timer : Terminal Count
#define TIMER_MODE1        1    //Timer : Programmer One-shot
#define TIMER_MODE2        2    //Timer : Frq.  Generator
#define TIMER_MODE3        3    //Timer : Square Wave Generator
#define TIMER_MODE4        4    //Timer : Counter, Soft Trigger
#define TIMER_MODE5        5    //Timer : Counter, Hard Trigger

#endif //COMMON_DLL1_DLL2

#define         MAX_8316_CARDS  3

/******** for DI and DO ********/
#define DI_LO_BYTE    0    //for port_no of DI (low  byte)
#define DI_HI_BYTE    1    //for port_no of DI (high byte)
#define DO_LO_BYTE    0    //for port_no of DO (low  byte)
#define DO_HI_BYTE    1    //for port_no of DO (high byte)

/******** for DA Interrupt status ********/
#define DA_INT_STOP   0    //Interrupt completed
#define DA_INT_RUN    1    //Interrupt not completed

/******** for AD Interrupt status ********/
#define AD_INT_STOP   0    //Interrupt completed
#define AD_INT_RUN    1    //Interrupt not completed

/******** for AD DMA status ********/
#define AD_DMA_STOP   0    //DMA completed
#define AD_DMA_RUN    1    //DMA not completed

/******** possible ad_ch_no of AD ********/
#define AD_CH_0      0
#define AD_CH_1      1
#define AD_CH_2      2
#define AD_CH_3      3
#define AD_CH_4      4
#define AD_CH_5      5
#define AD_CH_6      6
#define AD_CH_7      7
#define AD_CH_8      8
#define AD_CH_9      9
#define AD_CH_10    10
#define AD_CH_11    11
#define AD_CH_12    12
#define AD_CH_13    13
#define AD_CH_14    14
#define AD_CH_15    15

/******** possible da_ch_no of DA ********/
#define DA_CH_1      0
#define DA_CH_2      1
#define DA_CH_3      2
#define DA_CH_4      3
#define DA_CH_5      4
#define DA_CH_6      5

/******** for IRQ channel number ********/
#define IRQ3         3
#define IRQ4         4
#define IRQ5         5
#define IRQ6         6
#define IRQ7         7
#define IRQ9         9
#define IRQ10       10
#define IRQ11       11
#define IRQ12       12
#define IRQ14       14
#define IRQ15       15

/******** for DMA channel number ********/
#define DMA_CH_1     1
#define DMA_CH_3     3
#define DMA_CH_5     5
#define DMA_CH_6     6
#define DMA_CH_7     7

/******** AD mode (ACL-8111) ********/
#define A8111_AD_MODE_0    0     //Software Trig, Polling transfer
#define A8111_AD_MODE_1    1     //Software Trig, Interrupt transfer
#define A8111_AD_MODE_2    2     //External Trig, Polling  transfer
#define A8111_AD_MODE_3    3     //External Trig, Interrupt transfer
#define A8111_AD_MODE_4    4     //Timer    Trig, Polling  transfer
#define A8111_AD_MODE_5    6     //Timer    Trig, Interrupt transfer

/******** AD mode (ACL-8112PG/DG/HG) ********/
#define A8112_AD_MODE_0    0    //External Trig, Software Polling
#define A8112_AD_MODE_1    1    //Software Trig, Software Polling
#define A8112_AD_MODE_2    2    //Timer    Trig, DMA Transfer
#define A8112_AD_MODE_3    3    //External Trig, DMA Transfer
#define A8112_AD_MODE_4    4    //External Trig, Soft/Int transfer
#define A8112_AD_MODE_5    5    //Soft     Trig, Soft/Int transfer
#define A8112_AD_MODE_6    6    //Timer    Trig, Soft/Int transfer
#define A8112_AD_MODE_7    7    //Not Used

/******** AD mode (ACL-8216) ********/
#define A8216_AD_MODE_0    0    // External Trig, Software Polling
#define A8216_AD_MODE_1    1    // Software Trig, Software Polling
#define A8216_AD_MODE_2    2    // Timer    Trig, DMA Transfer
#define A8216_AD_MODE_3    3    // External Trig, DMA Transfer
#define A8216_AD_MODE_4    4    // External Trig, Soft/Int transfer
#define A8216_AD_MODE_5    5    // Soft     Trig, Soft/Int transfer
#define A8216_AD_MODE_6    6    // Timer    Trig, Soft/Int transfer
#define A8216_AD_MODE_7    7    // Not Used

/************ AD mode (ACL-8316/12) ****************************************/
/******* AD mode control register value (ACL-8316/12) ********/
#define A8316_AD_DMA          0x08    /* DMA enable                   */
#define A8316_AD_EXT_SRC      0x04    /* external A/D trigger source  */
#define A8316_AD_INT_SRC      0x00    /* internal A/D trigger source  */
#define A8316_AD_TimerTrig    0x02    /* internal timer pacer trigger */
#define A8316_AD_SoftTrig     0x00    /* software trigger             */
#define A8316_AD_AutoScan     0x01    /* auto scan cahnnel            */

/*;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;              >>>>>> mode for A/D conversion by DMA transfer <<<<<<       ;;
;; <1> DMA_MODE_0 : >>>internal timer trigger<<<                            ;;
;;                 DMA transfer and A/D conversion trigger source is using  ;;
;;                 internal timer pacer, using _8316_AD_Timer(c1, c2) to    ;;
;;                 start trigger.                                           ;;
;; <2> DMA_MODE_1 : >>>external timer trigger<<<                            ;;
;;                 DMA transfer and A/D conversion trigger source is using  ;;
;;                 external trigger source.                                 ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/
#define A8316_DMA_MODE_0      (A8316_AD_DMA+A8316_AD_INT_SRC+A8316_AD_TimerTrig)
#define A8316_DMA_MODE_1      (A8316_AD_DMA+A8316_AD_EXT_SRC)

/*;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;        >>>>>> mode for A/D conversion by Interrupt transfer <<<<<<       ;;
;; <1> INT_MODE_0 : >>>internal timer trigger + EOC interrupt<<<            ;;
;;                 INT transfer and A/D conversion trigger source is using  ;;
;;                 internal timer pacer, using _8316_AD_Timer(c1, c2) to    ;;
;;                 start trigger, and get one A/D converted data from I/O   ;;
;;                 port.                                                    ;;
;; <2> INT_MODE_1 : >>>internal timer trigger + FIFO half full interrupt<<< ;;
;;                 INT transfer and A/D conversion trigger source is using  ;;
;;                 internal timer pacer, using _8316_AD_Timer(c1, c2) to    ;;
;;                 start trigger, and converted data is stored in FIFO when ;;
;;                 the FIFO is half full (512 records) then interrupt occurs;;
;;                 it means you can get 512 records of converted A/D data,  ;;
;;                 so the size of ad_buff must be the times of 512.         ;;
;; <3> INT_MODE_2 : >>>external trigger + EOC interrupt<<<                  ;;
;;                 INT transfer and A/D conversion trigger source is using  ;;
;;                 external trigger source, and get one A/D converted data  ;;
;;                 from I/O port.                                           ;;
;; <4> INT_MODE_3 : >>>external trigger + FIFO half full interrupt<<<       ;;
;;                 INT transfer and A/D conversion trigger source is using  ;;
;;                 external trigger source, and converted data is stored in ;;
;;                 FIFO when the FIFO is half full (512 records) then       ;;
;;                 interrupt occurs it means you can get 512 records of     ;;
;;                 converted A/D data, so the size of ad_buff must be the   ;;
;;                 times of 512.                                            ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/
#define A8316_INT_MODE_0      0   /* internal timer trigger + EOC interrupt */
#define A8316_INT_MODE_1      1   /* internal timer trigger + FIFO half full interrupt */
#define A8316_INT_MODE_2      2   /* external trigger + EOC interrupt */
#define A8316_INT_MODE_3      3   /* external trigger + FIFO half full interrupt */

/******* 8316/12 Interrupt Trigger Source Control ********/
/******* for function _8316_AD_Set_INT_Source(source) ********/
#define  A8316_INTSRC_EOC      0x00    /* End Of A/D Conversion     */
#define  A8316_INTSRC_EXTERNAL 0x01    /* external interrupt source */
#define  A8316_INTSRC_INTERNAL 0x02    /* internal pacer interrupt  */
#define  A8316_INTSRC_FIFO_HF  0x03    /* FIFO half full            */

/******** Channel Input Mode ********/
#define SINGLE_ENDED       0
#define DIFFERENTIAL       1

/******** 8112 HG   High Gain Range Table ********/
#define AD_B_5_V           0    //Bipolar +/- 5V
#define AD_B_0_5_V         1    //Bipolar +/- 0.5V
#define AD_B_0_05_V        2    //Bipolar +/- 0.05V
#define AD_B_0_005_V       3    //Bipolar +/- 0.005V
#define AD_U_10_V          4    //Unipolar  0V - 10V
#define AD_U_1_V           5    //Unipolar  0V -  1V
#define AD_U_0_1_V         6    //Unipolar  0V -  0.1V
#define AD_U_0_01_V        7    //Unipolar  0V -  0.01V
#define AD_B_10_V          8    //Bipolar +/- 10V
#define AD_B_1_V           9    //Bipolar +/-  1V
#define AD_B_0_1_V        10    //Bipolar +/-  0.1V
#define AD_B_0_01_V       11    //Bipolar +/-  0.01V

/******** 8112 DG    Low Gain Range Table ********/
//#define AD_B_10_V         8   //Bipolar +/- 10V     defined above
//#define AD_B_5_V          0   //Bipolar +/-  5V     defined above
#define AD_B_2_5_V         1    //Bipolar +/- 2.5V
#define AD_B_1_25_V        2    //Bipolar +/- 1.25V
#define AD_B_0_625_V       3    //Bipolar +/- 0.625V
//#define AD_U_10_V         4   //Unipolar  0V - 10V  defined above
#define AD_U_5_V           5    //Unipolar  0V -  5V
#define AD_U_2_5_V         6    //Unipolar  0V -  2.5V
#define AD_U_1_25_V        7    //Unipolar  0V -  1.25V

/******** 8111/8112PG/8113/8216 AD Gain Level ********/
#define         AD_GAIN_1       0
#define         AD_GAIN_2       1
#define         AD_GAIN_4       2
#define         AD_GAIN_8       3
#define         AD_GAIN_16      4

/******** 8316/12 Range Table ********/
//#define AD_B_10_V         8   //Bipolar +/- 10V     defined above
//#define AD_B_5_V          0   //Bipolar +/-  5V     defined above
//#define AD_B_2_5_V        1    //Bipolar +/- 2.5V   defined above
//#define AD_B_1_25_V       2    //Bipolar +/- 1.25V  defined above

/******** 8316/12 DA output mode *********/
#define A8316_DA_TP           0x00    /* DA output is transparence mode */
#define A8316_DA_DB           0x01    /* DA output is double buffer mode */
#define A8316_DA_2S           0x00    /* DA format is 2's complement mode */
#define A8316_DA_BINARY       0x02    /* DA format is binary mode */

#define A8316_DA_MODE_0   (DA_TP+DA_BINARY)   /* transparence,  binary          */
#define A8316_DA_MODE_1   (DA_TP+DA_2S)       /* transparence,  2's complement  */
#define A8316_DA_MODE_2   (DA_DB+DA_BINARY)   /* double buffer, binary          */
#define A8316_DA_MODE_3   (DA_DB+DA_BINARY)   /* double buffer, 2's complement  */


/******** for 8113 only ********/
#define MAX_CHANNEL       32

/******** Function define for old library ********/
#define W_726_Initial               W_6126_Initial
#define W_726_Switch_Card_No        W_6126_Switch_Card_No
#define W_726_DI                    W_6126_DI
#define W_726_DI_Channel            W_6126_DI_Channel
#define W_726_DO                    W_6126_DO
#define W_726_DA                    W_6126_DA

#define W_711_Initial           W_8111_Initial
#define W_711_Switch_Card_No    W_8111_Switch_Card_No
#define W_711_DI                W_8111_DI
#define W_711_DI_Channel        W_8111_DI_Channel
#define W_711_DO                W_8111_DO
#define W_711_DA                W_8111_DA
#define W_711_AD_Set_Channel    W_8111_AD_Set_Channel
#define W_711_AD_Set_Gain       W_8111_AD_Set_Gain
#define W_711_AD_Set_Mode       W_8111_AD_Set_Mode
#define W_711_AD_Soft_Trig      W_8111_AD_Soft_Trig
#define W_711_AD_Aquire         W_8111_AD_Aquire
#define W_711_CLR_IRQ           W_8111_CLR_IRQ
#define W_711_AD_INT_Start      W_8111_AD_INT_Start
#define W_711_AD_INT_Status     W_8111_AD_INT_Status
#define W_711_AD_INT_Stop       W_8111_AD_INT_Stop
#define W_711_AD_Timer          W_8111_AD_Timer

#define W_813_Initial                W_8113_Initial
#define W_813_ActCard_Set            W_8113_ActCard_Set
#define W_813_Gain_Select            W_8113_Gain_Select
#define W_813_Channel_Select         W_8113_Channel_Select
#define W_813_Channel_Deselect       W_8113_Channel_Deselect
#define W_813_Channel_Clear          W_8113_Channel_Clear
#define W_813_ChannelNo_Get          W_8113_ChannelNo_Get
#define W_813_AD_Aquire              W_8113_AD_Aquire
#define W_813_MAD_Aquire             W_8113_MAD_Aquire

/*****************************************************************************/
/*                         Function Declarations                             */
/*****************************************************************************/

/************************ 6126 Function Declarations ************************/
int __stdcall W_6126_Initial( int card_number , int base_address , int irq);
int __stdcall W_6126_Switch_Card_No( int card_number );
int __stdcall W_6126_DI( int port_number, unsigned char  *di_data );
int __stdcall W_6126_DI_Channel( int di_ch_no , unsigned int  *di_data );
int __stdcall W_6126_DO( int port_number , unsigned char do_data );
int __stdcall W_6126_DA( int da_ch_no, unsigned int da_data );
int __stdcall W_6126_INTOP_Start(int count);
int __stdcall W_6126_INTOP_Status(int  *status, int  *count);
int __stdcall W_6126_INTOP_Stop(int  *count);
int __stdcall Set_INT_Op(int da_ch, unsigned int  *buf, int size);
int __stdcall W_6126_INT_Enable(HANDLE *phEvent);
int __stdcall W_6126_INT_Disable();
int __stdcall Reset_INT_Op();

/************************ 6128 Function Declarations ************************/
int __stdcall W_6128_Initial( int card_number , int base_address);
int __stdcall W_6128_Switch_Card_No( int card_number );
int __stdcall W_6128_DA( int da_ch_no, unsigned int da_data );

/************************ 8111 Function Declarations ************************/
int __stdcall W_8111_Initial( int card_number , int base_address );
int __stdcall W_8111_Switch_Card_No( int card_number );
int __stdcall W_8111_DI( int port_number, unsigned char  *di_data );
int __stdcall W_8111_DI_Channel( int di_ch_no, unsigned int  *di_data );
int __stdcall W_8111_DO( int port_number, unsigned char do_data );
int __stdcall W_8111_DA( unsigned int da_data );
int __stdcall W_8111_AD_Set_Channel( int ad_ch_no );
int __stdcall W_8111_AD_Set_Gain   ( int ad_gain  );
int __stdcall W_8111_AD_Set_Mode   ( int irq_no ,int ad_mode );
int __stdcall W_8111_AD_Soft_Trig  ( void );
int __stdcall W_8111_AD_Aquire     ( int  *ad_data );
int __stdcall W_8111_CLR_IRQ       ( void );
int __stdcall W_8111_AD_INT_Start( int ad_ch_no, int ad_gain, int irq_ch_no,
                    int count, unsigned short  *ad_buffer, unsigned int c1, unsigned int c2 );
int __stdcall W_8111_AD_INT_Status( int  *status, int  *count  );
int __stdcall W_8111_AD_INT_Stop( int  *count );
int __stdcall W_8111_AD_Timer( unsigned int c1 , unsigned int c2 );
int __stdcall W_8111_AD_SCANINT_Start( int ad_ch_no, int ad_gain , int irq_no, int count ,
                       unsigned short  *ad_buffer , unsigned int c1 , unsigned int c2);
int __stdcall W_8111_AD_ContINT_Start( int ad_ch_no, Boolean autoscan, int ad_gain , int irq_no, int count ,
                       unsigned short  *ad_buffer , unsigned int c1 , unsigned int c2);
int __stdcall W_8111_AD_DblBufferHalfReady ( BOOLEAN *bHalfReady );
int __stdcall W_8111_AD_DblBufferTransfer ( USHORT *pwBuffer );

/************************ 8112 Function Declarations ************************/
int __stdcall W_8112_Initial( int card_number , int base_address );
int __stdcall W_8112_Switch_Card_No( int card_number );
int __stdcall W_8112_DI( int port_number, unsigned char  *di_data );
int __stdcall W_8112_DI_Channel( int di_ch_no , unsigned int  *di_data );
int __stdcall W_8112_DO( int port_number , unsigned char do_data );
int __stdcall W_8112_DA( int da_ch_no, unsigned int da_data );
int __stdcall W_8112_AD_Set_Channel( int ad_ch_no );
int __stdcall W_8112_AD_Set_Range      ( int range  );
int __stdcall W_8112_AD_Set_Mode   ( int ad_mode  );
int __stdcall W_8112_AD_Soft_Trig  ( void );
int __stdcall W_8112_AD_Aquire    ( int  *ad_data );
int __stdcall W_8112_CLR_IRQ       ( void );
int __stdcall W_8112_AD_DMA_Start( int ad_ch_no, int ad_range , int dma_ch_no ,
            int irq_ch_no, int count , unsigned short  *ad_buffer , unsigned int c1 , unsigned int c2 );
int __stdcall W_8112_AD_DMA_Status( int  *status , int  *count );
int __stdcall W_8112_AD_DMA_Stop( int  *count );
int __stdcall W_8112_AD_INT_Start( int ad_ch_no, int ad_gain , int irq_ch_no,
                   int count, unsigned short  *ad_buffer, unsigned int c1, unsigned int c2 );
int __stdcall W_8112_AD_INT_Status( int  *status, int  *count  );
int __stdcall W_8112_AD_INT_Stop( int  *count );
int __stdcall W_8112_AD_Timer( unsigned int c1 , unsigned int c2 );
int __stdcall W_8112_TIMER_Start( int timer_mode, unsigned int c0 );
int __stdcall W_8112_TIMER_Read( unsigned int  *counter_value );
int __stdcall W_8112_TIMER_Stop( unsigned int  *counter_value );
int __stdcall W_8112_AD_Input_Mode   ( int channel_mode  );
int __stdcall W_8112_DMA_InitialMemoryAllocated(int *MemSize);
int __stdcall W_8112_AD_SCANINT_Start( int ad_ch_no, int ad_gain , int irq_no, int count ,
                       unsigned short  *ad_buffer , unsigned int c1 , unsigned int c2);
int __stdcall W_8112_AD_DblBufferTransfer ( USHORT *pwBuffer);
int __stdcall W_8112_AD_ContDMA_Start( int ad_ch_no, int ad_gain , int dma_ch_no , int irq_no,
                       int dma_count , unsigned short  *ad_buffer ,unsigned int c1, unsigned int c2);
int __stdcall W_8112_AD_DblBufferHalfReady ( BOOLEAN *bHalfReady );
int __stdcall W_8112_AD_ContINT_Start( int ad_ch_no, Boolean autoscan, int ad_gain , int irq_no, int count ,
                       unsigned short  *ad_buffer , unsigned int c1 , unsigned int c2);
/************************ 8113 Function Declarations ************************/
int __stdcall W_8113_Initial( int card_number , int base_address );
int __stdcall W_8113_ActCard_Set( int card_number );
int __stdcall W_8113_Gain_Select (int gain);
int __stdcall W_8113_Channel_Select ( int ch);
int __stdcall W_8113_Channel_Deselect ( int ch);
int __stdcall W_8113_Channel_Clear( void );
int __stdcall W_8113_ChannelNo_Get( int *no );
int __stdcall W_8113_AD_Aquire     ( int ch, unsigned int *ad_data );
int __stdcall W_8113_MAD_Aquire    ( void);
int __stdcall W_8113_Get_MAD_Data(unsigned int *ad_data_array);
/************************ 8112PG Function Declarations ************************/
int __stdcall W_812_Initial( int card_number , int base_address );
int __stdcall W_812_Switch_Card_No( int card_number );
int __stdcall W_812_DI( int port_number, unsigned char  *di_data );
int __stdcall W_812_DI_Channel( int di_ch_no , unsigned int  *di_data );
int __stdcall W_812_DO( int port_number , unsigned char do_data );
int __stdcall W_812_DA( int da_ch_no, unsigned int da_data );
int __stdcall W_812_AD_Set_Channel( int ad_ch_no );
int __stdcall W_812_AD_Set_Gain   ( int ad_gain  );
int __stdcall W_812_AD_Set_Mode   ( int ad_mode  );
int __stdcall W_812_AD_Soft_Trig  ( void );
int __stdcall W_812_AD_Aquire     ( int  *ad_data );
int __stdcall W_812_CLR_IRQ       ( void );
int __stdcall W_812_AD_DMA_Start( int ad_ch_no, int ad_gain , int dma_ch_no ,
            int irq_ch_no, int count , unsigned short  *ad_buffer , unsigned int c1 , unsigned int c2 );
int __stdcall W_812_AD_DMA_Status( int  *status , int  *count );
int __stdcall W_812_AD_DMA_Stop( int  *count );
int __stdcall W_812_AD_INT_Start( int ad_ch_no, int ad_gain , int irq_ch_no,
                   int count, unsigned short  *ad_buffer, unsigned int c1, unsigned int c2 );
int __stdcall W_812_AD_SCANINT_Start( int ad_ch_no, int ad_gain , int irq_ch_no,
                   int count, unsigned short  *ad_buffer, unsigned int c1, unsigned int c2 );
int __stdcall W_812_AD_INT_Status( int  *status, int  *count  );
int __stdcall W_812_AD_INT_Stop( int  *count );
int __stdcall W_812_AD_Timer( unsigned int c1 , unsigned int c2 );
int __stdcall W_812_TIMER_Start( int timer_mode, unsigned int c0 );
int __stdcall W_812_TIMER_Read( unsigned int  *counter_value );
int __stdcall W_812_TIMER_Stop( unsigned int  *counter_value );
int __stdcall W_812_DMA_InitialMemoryAllocated(int *MemSize);
int __stdcall W_812_AD_DblBufferHalfReady ( BOOLEAN *bHalfReady );
int __stdcall W_812_AD_DblBufferTransfer ( USHORT *pwBuffer );
int __stdcall W_812_AD_ContDMA_Start( int ad_ch_no, int ad_gain , int dma_ch_no , int irq_no,
                       int dma_count , unsigned short  *ad_buffer ,unsigned int c1, unsigned int c2);
int __stdcall W_812_AD_ContINT_Start( int ad_ch_no, Boolean autoscan, int ad_gain , int irq_no, int count ,
                       unsigned short  *ad_buffer , unsigned int c1 , unsigned int c2);
/************************ 8216 Function Declarations ************************/
int __stdcall W_8216_Initial( int card_number ,int base_address );
int __stdcall W_8216_Switch_Card_No( int card_number );
int __stdcall W_8216_DI( int port_number, unsigned char  *di_data );
int __stdcall W_8216_DI_Channel( int di_ch_no , unsigned int  *di_data );
int __stdcall W_8216_DO( int port_number , unsigned char do_data );
int __stdcall W_8216_DA( int da_ch_no, unsigned int da_data );
int __stdcall W_8216_AD_Set_Channel( int ad_ch_no );
int __stdcall W_8216_AD_Set_Range      ( int range  );
int __stdcall W_8216_AD_Set_Mode   ( int ad_mode  );
int __stdcall W_8216_AD_Soft_Trig  ( void );
int __stdcall W_8216_AD_Aquire    ( int  *ad_data );
int __stdcall W_8216_CLR_IRQ       ( void );
int __stdcall W_8216_AD_DMA_Start( int ad_ch_no, int ad_gain , int dma_ch_no ,
            int irq_ch_no, int count , short  *ad_buffer , unsigned int c1 , unsigned int c2 );
int __stdcall W_8216_AD_DMA_Status( int  *status , int  *count );
int __stdcall W_8216_AD_DMA_Stop( int  *count );
int __stdcall W_8216_AD_INT_Start( int ad_ch_no, int ad_gain , int irq_ch_no,
                   int count, short  *ad_buffer, unsigned int c1, unsigned int c2 );
int __stdcall W_8216_AD_INT_Status( int  *status, int  *count  );
int __stdcall W_8216_AD_INT_Stop( int  *count );
int __stdcall W_8216_AD_Timer( unsigned int c1 , unsigned int c2 );
int __stdcall W_8216_TIMER_Start( int timer_mode, unsigned int c0 );
int __stdcall W_8216_TIMER_Read( unsigned int  *counter_value );
int __stdcall W_8216_TIMER_Stop( unsigned int  *counter_value );
int __stdcall W_8216_AD_Input_Mode   ( int mode  );
int __stdcall W_8216_DMA_InitialMemoryAllocated(int *MemSize);
int __stdcall W_8216_AD_SCANINT_Start( int ad_ch_no, int ad_gain , int irq_no, int count ,
                       short *ad_buffer , unsigned int c1 , unsigned int c2);
int __stdcall W_8216_AD_DblBufferTransfer ( USHORT *pwBuffer );
int __stdcall W_8216_AD_ContDMA_Start( int ad_ch_no, int ad_gain , int dma_ch_no , int irq_no,
                       int dma_count , unsigned short  *ad_buffer ,unsigned int c1, unsigned int c2);
int __stdcall W_8216_AD_DblBufferHalfReady ( BOOLEAN *bHalfReady );
int __stdcall W_8216_AD_ContINT_Start( int ad_ch_no, Boolean autoscan, int ad_gain , int irq_no, int count ,
                       short *ad_buffer , unsigned int c1 , unsigned int c2);

/********************** 8316/12 Function Declarations **********************/
I16 __stdcall W_8316_Initial(U8 card_number, U16 base_address);
I16 __stdcall W_8316_Switch_Card_No(U8 card_number);
I16 __stdcall W_8316_DI(U16 *di_data );
I16 __stdcall W_8316_DI_Channel(U8 di_ch_no , Boolean *di_data );
I16 __stdcall W_8316_DO(U16 data );
I16 __stdcall W_8316_AD_Set_Channel(U8 ad_ch_no );
I16 __stdcall W_8316_AD_Set_Range(U8 range );
I16 __stdcall W_8316_AD_Set_Mode(U8 ad_mode ); /* set AD mode control register */
I16 __stdcall W_8316_AD_Soft_Trig( void );
I16 __stdcall W_8316_AD_Set_Autoscan(Boolean flag);
I16 __stdcall W_8316_AD_Set_FIFO(Boolean flag);
I16 __stdcall W_8316_AD_Set_INT_Source(U8 source);
I16 __stdcall W_8316_CLR_IRQ(void);
I16 __stdcall W_8316_AD_Read_FIFO(I16 *data);
I16 __stdcall W_8316_AD_Aquire(I16 *ad_data);
I16 __stdcall W_8316_AD_DMA_Start(U8 trig_src, Boolean auto_scan,U8 ad_ch_no,
                               U8 ad_range , U8 dma_ch_no , U8 irq_no,
                               int dma_count , I16 *ad_buffer);
I16 __stdcall W_8316_AD_DMA_Status(U8 *status , int *count );
I16 __stdcall W_8316_AD_DMA_Stop(int *count );
I16 __stdcall W_8316_AD_INT_Start(U8 ad_mode, Boolean auto_scan, U8 ad_ch_no,
                        U8 ad_range, U8 irq_no, int int_count, I16 *ad_buffer);
I16 __stdcall W_8316_AD_INT_Status(U8 *status, int *count  );
I16 __stdcall W_8316_AD_INT_Stop(int *count );
I16 __stdcall W_8316_DA_Set_Mode(U8 mode );
I16 __stdcall W_8316_DA(U8 ch_no, I16 data );
I16 __stdcall W_8316_AD_Timer(U16 c1 , U16 c2 );
I16 __stdcall W_8316_TIMER_Start(U8 timer_mode, U16 c0 );
I16 __stdcall W_8316_TIMER_Read(U16 *counter_value );
I16 __stdcall W_8316_TIMER_Stop(U16 *counter_value );
I16 __stdcall W_8316_DMA_InitialMemoryAllocated(int *MemSize);
I16 __stdcall W_8316_AD_DblBufferHalfReady ( BOOLEAN *bHalfReady );
I16 __stdcall W_8316_AD_DblBufferTransfer ( USHORT *pwBuffer );
I16 __stdcall W_8316_AD_ContDMA_Start(U8 trig_src, Boolean autoscan,U8 ad_ch_no, U8 ad_range,
                       U8 dma_ch_no , U8 irq_no, int dma_count , I16 *ad_buffer);
I16 __stdcall W_8316_AD_ContINT_Start(U8 ad_mode, Boolean autoscan, U8 ad_ch_no, U8 ad_range,
                       U8 irq_no, int count, I16 *ad_buffer);
#ifdef __cplusplus
}
#endif

#endif // DLL2_NT_H