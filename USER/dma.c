#include "dma.h"
#include "string.h"
uint8_t RxBuffer[50];
uint8_t TxBuffer[50];


DMA_RecvData_t usart1_recv_data = {0};
usart_buffer_t  uart1_frm_rbuf = {0};

void usart_dma_init(void)  
{         
    CLK_PeripheralClockConfig(CLK_Peripheral_DMA1, ENABLE);   //打开时钟，很重要  
    /* Deinitialize DMA channels */  
    DMA_GlobalDeInit();  
   
    DMA_DeInit(DMA1_Channel1);  
    DMA_DeInit(DMA1_Channel2);  
      
    /* DMA channel Rx of USART Configuration */  
    //该函数主要要配置好接受的数组，以及USART的数据寄存器地址，数组大小，以及DMA模式  
    DMA_Init(DMA1_Channel2, (uint16_t)RxBuffer, (uint16_t)0x5231,  
             USART1_DMA_BUFFER_SIZE, DMA_DIR_PeripheralToMemory, DMA_Mode_Normal,  
             DMA_MemoryIncMode_Inc, DMA_Priority_Low, DMA_MemoryDataSize_Byte);  
 
    /* DMA channel Tx of USART Configuration */  
    //该函数主要配置发送数组，以及USART的数据寄存器地址，数组大小，以及DMA模式  
    DMA_Init(DMA1_Channel1, (uint16_t)TxBuffer, (uint16_t)0x5231,  
            USART1_DMA_BUFFER_SIZE, DMA_DIR_MemoryToPeripheral, DMA_Mode_Normal,   
             DMA_MemoryIncMode_Inc, DMA_Priority_High, DMA_MemoryDataSize_Byte);  
      
    /* Enable the USART Tx/Rx DMA requests */  
    USART_DMACmd(USART1, USART_DMAReq_TX, ENABLE);  
    USART_DMACmd(USART1, USART_DMAReq_RX, ENABLE);  
   
    /* Global DMA Enable */  
    DMA_GlobalCmd(ENABLE);  
   
    /* Enable the USART Tx DMA channel */  
   // DMA_Cmd(DMA1_Channel1, ENABLE);  
    /* Enable the USART Rx DMA channel */  
    DMA_Cmd(DMA1_Channel2, ENABLE);   
    
} 

app_usart_proc_t check_usart1_frame_head(uint8_t * pBegin, uint8_t * pEnd )
{
  app_usart_proc_t rc = APP_USART_FRAME_NOT_COMPLETE;
  
  while(*pBegin < *pEnd)
  {
    if(usart1_recv_data.RecvData[*pBegin] == FRM_HEAD)
    {
      rc = APP_USART_FRM_HEAD_SUCCESS; 
      break;
    }
    else
    {
      (*pBegin)++;
      rc = APP_USART_FRM_HEAD_ERR;
    }
  }
  return rc;
}


 app_usart_proc_t check_usart1_frame_end_and_crc( uint8_t *beginIndex)
 {
  app_usart_proc_t rc = APP_USART_FRAME_NOT_COMPLETE;
 //还未实现
  rc = APP_USART_FRM_END_SUCCESS;
  return rc;
 }



void uart1_frm_proc(uint8_t *str,uint16_t len)
{
//帧处理函数


}
void read_Usart1_DMA_FIFO(usart_buffer_t* usart1_rbuf)
{
    uint8_t dmaIndex = 0; 
    app_usart_proc_t rc = APP_USART_FRAME_NOT_COMPLETE;
    memset(usart1_rbuf->rxbuf, 0x0, sizeof(usart1_rbuf->rxbuf));
    usart1_recv_data.curcnt = USART1_DMA_BUFFER_SIZE-DMA_GetCurrDataCounter( DMA1_Channel2);
    while(dmaIndex < usart1_recv_data.curcnt)
    {
  loop_check_head :   
       rc = check_usart1_frame_head(&dmaIndex, &usart1_recv_data.curcnt);//检查帧头是否正确
        if(rc == APP_USART_FRM_HEAD_SUCCESS)//如果帧头正确
      { 
        rc =  check_usart1_frame_end_and_crc( &dmaIndex );//检查帧尾和校验位
        
        if(rc == APP_USART_FRM_END_SUCCESS)
        {
          usart1_rbuf->rxbuf_payload_len = ((usart1_recv_data.RecvData[dmaIndex+22]&0xffff)<<8) | 
                                             usart1_recv_data.RecvData[dmaIndex+23];
          usart1_rbuf->rxbuf_len = usart1_rbuf->rxbuf_payload_len + 26; 
          memcpy(usart1_rbuf, &usart1_recv_data.RecvData[dmaIndex], usart1_rbuf->rxbuf_len); 
          dmaIndex += usart1_rbuf->rxbuf_len;
          uart1_frm_proc(usart1_rbuf->rxbuf,usart1_rbuf->rxbuf_len);
          if((dmaIndex >= usart1_recv_data.curcnt) &&
           ((dmaIndex-usart1_rbuf->rxbuf_len) <= usart1_recv_data.curcnt))
           {
              usart1_recv_data.precnt = usart1_recv_data.curcnt;
              usart1_recv_data.curcnt = USART1_DMA_BUFFER_SIZE-DMA_GetCurrDataCounter( DMA1_Channel2);
              if(usart1_recv_data.precnt == usart1_recv_data.curcnt)
              {
                break;          
              }
              else
              { 
                goto loop_check_head;        
              }
           }
        }
            
      }  
      
  
   }
}

void Iterates_usart1_buffer(void)
{    
  while(usart1_recv_data.recvfrmcnt)
  {
	
    usart1_recv_data.recvFlag = 0;   
    usart1_recv_data.recvfrmcnt--;        
    usart1_recv_data.curcnt = USART1_DMA_BUFFER_SIZE-DMA_GetCurrDataCounter( DMA1_Channel2);
    
    read_Usart1_DMA_FIFO(&uart1_frm_rbuf);
		  
    usart_dma_init() ;
    usart1_recv_data.precnt = 0;
    usart1_recv_data.curcnt = 0; 
    memset(&usart1_recv_data.RecvData[0],0x0,sizeof(usart1_recv_data.RecvData));      
  }  
}