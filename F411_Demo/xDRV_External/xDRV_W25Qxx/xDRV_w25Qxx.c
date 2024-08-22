/***************************************************************
 * 文件名: xDRV_w25Qxx.c
 * 版本号: v1.2
 * 更新时间: 2024-08-20
 * 更新人物: Xiang Likang
 * 邮件地址: 3265103350@qq.com
 * 
 * 描述:
 * 该文件是W25Qxx系列Flash存储器的驱动程序。
 * 提供了基本的读写操作、扇区和芯片擦除功能、掉电模式管理等。
 *
 * 
 * 更新内容:
 * 版本号: 
 * v1.0 2023-11-21   
 *   - 优化了W25QXX的初始化过程，确保对不同型号芯片的支持。
 *   - 修复了在连续写入数据时可能导致数据覆盖的问题。
 * v1.2 2024-08-20
 *   - 增加了对自定义字体数据的读取功能，支持16点、24点、32点三种字体尺寸。
 *   - 实现了结构体数据的存储和读取功能，便于系统配置信息的持久化。
 ***************************************************************/


#include "xDRV_W25Qxx/xDRV_w25Qxx.h"
#include "xBSP_Delay/xBSP_delay.h"


#include "spi.h"

uint16_t W25QXX_TYPE=W25Q128;	//默认是W25Q256


extern SPI_HandleTypeDef hspi1;  //SPI1句柄
//SPI速度设置函数
//SPI速度=fAPB1/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_2 256
//fAPB1时钟一般为42Mhz：
void SPI_SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
    __HAL_SPI_DISABLE(&hspi1);            //关闭SPI
    hspi1.Instance->CR1 &= 0XFFC7;          //位3-5清零，用来设置波特率
    hspi1.Instance->CR1 |= SPI_BaudRatePrescaler;//设置SPI速度
    __HAL_SPI_ENABLE(&hspi1);             //使能SPI
    
}

//SPI1 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
uint8_t SPI_ReadWriteByte(uint8_t TxData)
{
    uint8_t Rxdata;
    HAL_SPI_TransmitReceive(&hspi1,&TxData,&Rxdata,1, 1000);       
		return Rxdata;          		    //返回收到的数据		
}

//4Kbytes为一个Sector
//16个扇区为1个Block
//W25Q256
//容量为32M字节,共有512个Block,8192个Sector 
//初始化SPI FLASH的IO口

void W25QXX_Init(void)
{ 
    uint8_t temp;
    
		W25QXX_CS(1);			               	 //SPI FLASH不选中
		MX_SPI1_Init();		   			       		 //初始化SPI
		SPI_SetSpeed(SPI_BAUDRATEPRESCALER_2); //设置为42M时钟,高速模式
		W25QXX_TYPE=W25QXX_ReadID();	        //读取FLASH ID.
		// Uart_printf(&huart6,"W25QXX ID : 0x%X(W25Q128)\r\n",W25QXX_TYPE);	  
    if(W25QXX_TYPE==NM25Q128)                //SPI FLASH为W25Q256
    {
        temp=W25QXX_ReadSR(3);              //读取状态寄存器3，判断地址模式
        if((temp&0X01)==0)			      	   //如果不是4字节地址模式,则进入4字节地址模式
				{
					W25QXX_CS(0); 			       		  //选中
					SPI_ReadWriteByte(W25X_Enable4ByteAddr);//发送进入4字节地址模式指令   
					W25QXX_CS(1);       		        //取消片选   
				}
    }
}  


//读取W25QXX的状态寄存器，W25QXX一共有3个状态寄存器
//状态寄存器1：
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
//状态寄存器2：
//BIT7  6   5   4   3   2   1   0
//SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
//状态寄存器3：
//BIT7      6    5    4   3   2   1   0
//HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
//regno:状态寄存器号，范:1~3
//返回值:状态寄存器值
uint8_t W25QXX_ReadSR(uint8_t regno)   
{  
	uint8_t byte=0,command=0; 
    switch(regno)
    {
        case 1:
            command=W25X_ReadStatusReg1;    //读状态寄存器1指令
            break;
        case 2:
            command=W25X_ReadStatusReg2;    //读状态寄存器2指令
            break;
        case 3:
            command=W25X_ReadStatusReg3;    //读状态寄存器3指令
            break;
        default:
            command=W25X_ReadStatusReg1;    
            break;
    }    
	W25QXX_CS(0);                            //使能器件   
	SPI_ReadWriteByte(command);            //发送读取状态寄存器命令    
	byte=SPI_ReadWriteByte(0Xff);          //读取一个字节  
	W25QXX_CS(1);                            //取消片选     
	return byte;   
}
//写W25QXX状态寄存器
void W25QXX_Write_SR(uint8_t regno,uint8_t sr)   
{   
    uint8_t command=0;
    switch(regno)
    {
        case 1:
            command=W25X_WriteStatusReg1;    //写状态寄存器1指令
            break;
        case 2:
            command=W25X_WriteStatusReg2;    //写状态寄存器2指令
            break;
        case 3:
            command=W25X_WriteStatusReg3;    //写状态寄存器3指令
            break;
        default:
            command=W25X_WriteStatusReg1;    
            break;
    }   
	W25QXX_CS(0);                            //使能器件   
	SPI_ReadWriteByte(command);            //发送写取状态寄存器命令    
	SPI_ReadWriteByte(sr);                 //写入一个字节  
	W25QXX_CS(1);                            //取消片选     	      
}   
//W25QXX写使能	
//将WEL置位   
void W25QXX_Write_Enable(void)   
{
	W25QXX_CS(0);                            //使能器件   
	SPI_ReadWriteByte(W25X_WriteEnable);   //发送写使能  
	W25QXX_CS(1);                            //取消片选     	      
} 
//W25QXX写禁止	
//将WEL清零  
void W25QXX_Write_Disable(void)   
{  
	W25QXX_CS(0);                            //使能器件   
	SPI_ReadWriteByte(W25X_WriteDisable);  //发送写禁止指令    
	W25QXX_CS(1);                            //取消片选     	      
}

//读取芯片ID
//返回值如下:				   
//0XEF13,表示芯片型号为W25Q80  
//0XEF14,表示芯片型号为W25Q16    
//0XEF15,表示芯片型号为W25Q32  
//0XEF16,表示芯片型号为W25Q64 
//0XEF17,表示芯片型号为W25Q128 	  
//0XEF18,表示芯片型号为W25Q256
uint16_t W25QXX_ReadID(void)
{
	uint16_t Temp = 0;	  
	W25QXX_CS(0);				    
	SPI_ReadWriteByte(0x90);//发送读取ID命令	    
	SPI_ReadWriteByte(0x00); 	    
	SPI_ReadWriteByte(0x00); 	    
	SPI_ReadWriteByte(0x00); 	 			   
	Temp|=SPI_ReadWriteByte(0xFF)<<8;  
	Temp|=SPI_ReadWriteByte(0xFF);	 
	W25QXX_CS(1);				    
	return Temp;
}   		    
//读取SPI FLASH  
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
void W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)   
{ 
 	uint16_t i;   										    
	W25QXX_CS(0);                            //使能器件   
    SPI_ReadWriteByte(W25X_ReadData);      //发送读取命令  
    if(W25QXX_TYPE==W25Q256)                //如果是W25Q256的话地址为4字节的，要发送最高8位
    {
        SPI_ReadWriteByte((uint8_t)((ReadAddr)>>24));    
    }
    SPI_ReadWriteByte((uint8_t)((ReadAddr)>>16));   //发送24bit地址    
    SPI_ReadWriteByte((uint8_t)((ReadAddr)>>8));   
    SPI_ReadWriteByte((uint8_t)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
		{ 
        pBuffer[i]=SPI_ReadWriteByte(0XFF);    //循环读数  
    }
	W25QXX_CS(1);  				    	      
}

//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!	 
void W25QXX_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
 	uint16_t i;  
    W25QXX_Write_Enable();                  //SET WEL 
		W25QXX_CS(0);                            //使能器件   
    SPI_ReadWriteByte(W25X_PageProgram);   //发送写页命令   
    if(W25QXX_TYPE==W25Q256)                //如果是W25Q256的话地址为4字节的，要发送最高8位
    {
        SPI_ReadWriteByte((uint8_t)((WriteAddr)>>24)); 
    }
    SPI_ReadWriteByte((uint8_t)((WriteAddr)>>16)); //发送24bit地址    
    SPI_ReadWriteByte((uint8_t)((WriteAddr)>>8));   
    SPI_ReadWriteByte((uint8_t)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)SPI_ReadWriteByte(pBuffer[i]);//循环写数  
	W25QXX_CS(1);                            //取消片选 
	W25QXX_Wait_Busy();					   //等待写入结束
} 
//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void W25QXX_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain;	   
	pageremain=256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
		}
	}
} 

//写SPI FLASH  
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)						
//NumByteToWrite:要写入的字节数(最大65535)   
uint8_t W25QXX_BUFFER[4096];		 
void W25QXX_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	uint8_t * W25QXX_BUF;	  
	W25QXX_BUF=W25QXX_BUFFER;	     
 	secpos=WriteAddr/4096;//扇区地址  
	secoff=WriteAddr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//测试用
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
	while(1) 
	{	
		W25QXX_Read(W25QXX_BUF,secpos*4096,4096);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			W25QXX_Erase_Sector(secpos);//擦除这个扇区
			for(i=0;i<secremain;i++)	   //复制
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//写入整个扇区  

		}else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumByteToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	pBuffer+=secremain;  //指针偏移
			WriteAddr+=secremain;//写地址偏移	   
		   	NumByteToWrite-=secremain;				//字节数递减
			if(NumByteToWrite>4096)secremain=4096;	//下一个扇区还是写不完
			else secremain=NumByteToWrite;			//下一个扇区可以写完了
		}	 
	};	 
}
//擦除整个芯片		  
//等待时间超长...
void W25QXX_Erase_Chip(void)   
{                                   
    W25QXX_Write_Enable();                  //SET WEL 
    W25QXX_Wait_Busy();   
  	W25QXX_CS(0);                            //使能器件   
    SPI_ReadWriteByte(W25X_ChipErase);        //发送片擦除命令  
		W25QXX_CS(1);                            //取消片选     	      
		W25QXX_Wait_Busy();   				   //等待芯片擦除结束
}   
//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个扇区的最少时间:150ms
void W25QXX_Erase_Sector(uint32_t Dst_Addr)   
{  
	//监视falsh擦除情况,测试用   
 	//printf("fe:%x\r\n",Dst_Addr);	  
		Dst_Addr*=4096;
    W25QXX_Write_Enable();                  //SET WEL 	 
    W25QXX_Wait_Busy();   
  	W25QXX_CS(0);                            //使能器件   
    SPI_ReadWriteByte(W25X_SectorErase);   //发送扇区擦除指令 
    if(W25QXX_TYPE==W25Q256)                //如果是W25Q256的话地址为4字节的，要发送最高8位
    {
        SPI_ReadWriteByte((uint8_t)((Dst_Addr)>>24)); 
    }
    SPI_ReadWriteByte((uint8_t)((Dst_Addr)>>16));  //发送24bit地址    
    SPI_ReadWriteByte((uint8_t)((Dst_Addr)>>8));   
    SPI_ReadWriteByte((uint8_t)Dst_Addr);  
	W25QXX_CS(1);                            //取消片选     	      
    W25QXX_Wait_Busy();   				    //等待擦除完成
}  
//等待空闲
void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR(1)&0x01)==0x01);   // 等待BUSY位清空
}  
//进入掉电模式
void W25QXX_PowerDown(void)   
{ 
  	W25QXX_CS(0);                            //使能器件   
    SPI_ReadWriteByte(W25X_PowerDown);     //发送掉电命令  
		W25QXX_CS(1);                            //取消片选     	      
    delay_us(3);                            //等待TPD  
}   
//唤醒
void W25QXX_WAKEUP(void)   
{  
  	W25QXX_CS(0);                                //使能器件   
    SPI_ReadWriteByte(W25X_ReleasePowerDown);  //  send W25X_PowerDown command 0xAB    
		W25QXX_CS(1);                                //取消片选     	      
    delay_us(3);                                //等待TRES1
}


//      后半空间开始存储自定义信息
#define WS_W25Qxx_FONT_Start_Sector  (4096/2)
#define WS_W25Qxx_FONT16_Start_Sector (0  )  //  0开始 ，   69  结束  ， 用70扇区
#define WS_W25Qxx_FONT24_Start_Sector (70 )  //  70开始 ，  249 结束  ， 用150扇区
#define WS_W25Qxx_FONT32_Start_Sector (250)  //  250开始 ， 509 结束  ， 用260扇区
#define WS_W25Qxx_Sector_Size 4096  

//  获取16点 字库数据 ， gbk内无数据或者指定个数汉字读取完毕结束函数
//  在SD卡实现 读取数据
//  buffer:  数据缓存地址
//  gbk   :  汉字内码数据
//  size  :  读取汉字个数，汉字内码数组无数据，或者达到读取个数时，函数结束
//  font: 字体，1->16点 ，2->24点，3->32点
//  返回值： 读取到的数据个数
uint32_t  WS_W25Qxx_Font_Read_Date(uint8_t *buffer ,uint8_t *gbk, uint16_t size,uint16_t font)
{
	  uint32_t  readSize  = 0;
	  uint32_t addr;
    if(buffer == NULL || gbk == 0 || size == 0 )  return 0;
	  // 根据字体修改每个汉字点阵数据量
	  if(font == 1) 
		{
			 font = 32  ; 
       //  字库首扇区 + 偏移扇区		
			 addr = (WS_W25Qxx_FONT16_Start_Sector+WS_W25Qxx_FONT_Start_Sector) * WS_W25Qxx_Sector_Size + 4;
		}
	  if(font == 2) 
		{
			 font = 72  ; 	
			 addr = (WS_W25Qxx_FONT24_Start_Sector+WS_W25Qxx_FONT_Start_Sector) * WS_W25Qxx_Sector_Size + 4;
		}
		if(font == 3) 
		{
			 font = 128  ; 	
			 addr = (WS_W25Qxx_FONT32_Start_Sector+WS_W25Qxx_FONT_Start_Sector) * WS_W25Qxx_Sector_Size + 4;
		}
	  
	  while(*gbk > 0x80 && size > 0)  //  连续读取汉字字库数据，遇到字符停止
		{
		  uint32_t 	n =  (94*(gbk[0]-0xa1)  +  (gbk[1] - 0xa1) ) * font ;
			W25QXX_Read(buffer, addr + n , font );
			//  准备读取下个汉字
			readSize += font  ;     
			buffer   += font  ;     //   缓存地址偏移
			gbk += 2      ;         //   下个汉字 
			size --       ;  			
		}
    return readSize ;		
}

//  实现虚函数 获取16点 字库数据 ， gbk内无数据或者指定个数汉字读取完毕结束函数
//  在SD卡实现 读取数据
//  buffer:  数据缓存地址
//  gbk   :  汉字内码数据
//  size  :  读取汉字个数，汉字内码数组无数据，或者达到读取个数时，函数结束
//  返回值： 读取到的数据个数
uint32_t  WS_Font16_Read_Date(uint8_t *buffer , uint8_t * gbk , uint32_t size)
{
//	 if(W25QxxInfo.font16Flag == 0) return 0;
   return   WS_W25Qxx_Font_Read_Date(buffer,gbk,size,1);
}


//  实现虚函数 获取24点 字库数据 ， gbk内无数据或者指定个数汉字读取完毕结束函数
//  在SD卡实现 读取数据
//  buffer:  数据缓存地址
//  gbk   :  汉字内码数据
//  size  :  读取汉字个数，汉字内码数组无数据，或者达到读取个数时，函数结束
//  返回值： 读取到的数据个数
uint32_t  WS_Font24_Read_Date(uint8_t *buffer , uint8_t * gbk , uint32_t size)
{
//	 if(W25QxxInfo.font24Flag == 0) return 0;
   return   WS_W25Qxx_Font_Read_Date(buffer,gbk,size,2);
}

//  实现虚函数 获取24点 字库数据 ， gbk内无数据或者指定个数汉字读取完毕结束函数
//  在SD卡实现 读取数据
//  buffer:  数据缓存地址
//  gbk   :  汉字内码数据
//  size  :  读取汉字个数，汉字内码数组无数据，或者达到读取个数时，函数结束
//  返回值： 读取到的数据个数
uint32_t  WS_Font32_Read_Date(uint8_t *buffer , uint8_t * gbk , uint32_t size)
{
//	 if(W25QxxInfo.font32Flag == 0) return 0;
   return   WS_W25Qxx_Font_Read_Date(buffer,gbk,size,3);
}


//      读取系统配置信息函数
void  WS_Config_Read_Struct_Callback(uint8_t *  date  ,  uint32_t len)
{
        W25QXX_Read(date,(4096 - 1) * 4096 , len);
}

//      写入系统配置信息函数
void  WS_Config_Write_Struct_Callback(uint8_t *  date  ,  uint32_t len)
{
        W25QXX_Write(date,(4096 - 1) * 4096 , len); 
}

