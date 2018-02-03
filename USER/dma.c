#include "dma.h"
#include "string.h"
#include "stdio.h"
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
    DMA_Init(DMA1_Channel2, (uint16_t)usart1_recv_data.RecvData, (uint16_t)0x5231,  
             USART1_DMA_BUFFER_SIZE, DMA_DIR_PeripheralToMemory, DMA_Mode_Normal,  
             DMA_MemoryIncMode_Inc, DMA_Priority_High, DMA_MemoryDataSize_Byte);  
 
    /* DMA channel Tx of USART Configuration */  
    //该函数主要配置发送数组，以及USART的数据寄存器地址，数组大小，以及DMA模式  
    DMA_Init(DMA1_Channel1, (uint16_t)TxBuffer, (uint16_t)0x5231,  
            USART1_DMA_BUFFER_SIZE, DMA_DIR_MemoryToPeripheral, DMA_Mode_Normal,   
             DMA_MemoryIncMode_Inc, DMA_Priority_Low, DMA_MemoryDataSize_Byte);  
      
    /* Enable the USART Tx/Rx DMA requests */  
   // USART_DMACmd(USART1, USART_DMAReq_TX, ENABLE);
    //DMA_GlobalCmd(DISABLE);   
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
    if((usart1_recv_data.RecvData[*pBegin] ==0xfe) && (usart1_recv_data.RecvData[*pBegin+1] ==0xfe) && (usart1_recv_data.RecvData[*pBegin+2]==FRM_HEAD)  )
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
  uint8_t rxbuf_payload_len = 0;
  uint8_t rxbuf_len = 0;
  rxbuf_payload_len= usart1_recv_data.RecvData[*beginIndex+12];
  rxbuf_len = rxbuf_payload_len + 15 ;
  if(rxbuf_payload_len > MAX_DATA_LEN)
  {
    rc = APP_USART_LEN_ERR;
   }
  else if(usart1_recv_data.RecvData[*beginIndex+rxbuf_len-1] == FRM_END)
  {
    
    if(!check_crc(&usart1_recv_data.RecvData[*beginIndex],rxbuf_len-1))
    {
      rc = APP_USART_CRC_ERR;
    } 
    else
    {    
      rc = APP_USART_FRM_END_SUCCESS;   
    }
  }
  return rc;
 }

void bui_app_frm(uint8_t *pIn_msg, app_frm_t *pOut_pkg)
{
  uint8_t data_len;
  memcpy(pOut_pkg->frm_head, &pIn_msg[0],3); 
  pOut_pkg->type=pIn_msg[3];
  memcpy(pOut_pkg->addr, &pIn_msg[4], 7);   
  pOut_pkg->ctr_code   = pIn_msg[11];  //Control code
  pOut_pkg->data_len =  pIn_msg[12];  //Data Length
  data_len = pOut_pkg->data_len;            //Data Length
  memcpy(pOut_pkg->msg, &pIn_msg[13], data_len);  //Data
  pOut_pkg->cs = pIn_msg[13+data_len];          //Check sum
  pOut_pkg->frm_end = pIn_msg[14+data_len];   //Frame tail 
  
}

void uart1_frm_proc(uint8_t *str,uint16_t len)
{
 app_frm_t in_pkg;
 uint8_t control_code;
 bui_app_frm(str,&in_pkg); 
 control_code=in_pkg.ctr_code;
// switch(control_code)
// {
//  case 0x01: data_collection();          break;//数据采集函数
//  
//  
//  case 0x04: switch_and_time_proc();      break;//开关阀操作和设置时间的函数
//  default: break;
// 
// 
// }


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
          usart1_rbuf->rxbuf_payload_len = usart1_recv_data.RecvData[dmaIndex+12];
          usart1_rbuf->rxbuf_len = usart1_rbuf->rxbuf_payload_len + 15; 
          memcpy(usart1_rbuf->rxbuf, &usart1_recv_data.RecvData[dmaIndex], usart1_rbuf->rxbuf_len); 
          dmaIndex += usart1_rbuf->rxbuf_len;
          
          printf("retrive data\n");
          
          uart1_frm_proc(usart1_rbuf->rxbuf,usart1_rbuf->rxbuf_len);
          memset(usart1_rbuf->rxbuf, 0x0, sizeof(usart1_rbuf->rxbuf)); 
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

/***************************************************************************
 * @fn          check_crc
 *     
 * @brief       检查数据帧的CRC
 *     
 * @data        2018年2月1日
 *     
 * @param       frm - 数据
 *              len - 数据长度
 *     
 * @return      1 , CRC正确
 *              0 , CRC错误
 ***************************************************************************
 */ 
uint8_t check_crc(uint8_t *frm, uint16_t len)
{
  uint8_t crc_sum=0;
  uint16_t i;
  for( i =0; i < len-1; i++)
  {
    crc_sum = *frm + crc_sum;
    frm++;
  }
  if(crc_sum == *frm)    //?D??ê?2?ê?D￡?é??
    return 1;      //ê?
  else
    return 0;          //2?ê?
}