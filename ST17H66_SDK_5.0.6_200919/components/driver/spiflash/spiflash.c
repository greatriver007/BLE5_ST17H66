/**************************************************************************************************
*******
**************************************************************************************************/

#include "error.h"
#include "spiflash.h"
#include "log.h"
#include "string.h"
#include "dma.h"


hal_spi_t spiflash_spi ={
	.spi_index = SPI0,
};

void spi_cb(spi_evt_t* evt)
{
}

void dma_cb(DMA_CH_t ch)
{   
}


spi_Cfg_t spi_cfg = {
	.sclk_pin = GPIO_P18,
	.ssn_pin = GPIO_P11,
	.MOSI = GPIO_P14,
	.MISO = GPIO_P15,
	
	.baudrate = 2000000,
	.spi_tmod = SPI_TRXD,
	.spi_scmod = SPI_MODE0,
	
	.int_mode = false,
	.force_cs = true,
	.evt_handler = spi_cb,
};

HAL_DMA_t dma_cfg = {
    .dma_channel = DMA_CH_0,
    .evt_handler = dma_cb,
};



#define spiflash_cmd_tx_and_rx(mode,dmatx,dmarx,tx_buf,rx_buf,tx_len,rx_len)   \
		hal_spi_transmit(&spiflash_spi,mode,dmatx,dmarx,tx_buf,rx_buf,tx_len,rx_len)

//gd25q16 driver
uint32_t spiflash_read_identification(void)//check
{
	uint8_t buf_send[1] = {FLASH_RDID};
	uint8_t buf_rece[3] = {0x00,0x00,0x00};
	
	if(PPlus_SUCCESS == spiflash_cmd_tx_and_rx(SPI_EEPROM,0,0,buf_send,buf_rece,1,3))
		return (buf_rece[0] << 16)|(buf_rece[1] << 8) | (buf_rece[2]);
	else
		return 0xFFFFFF;
}

uint16_t spiflash_read_status_register(uint8_t bitsSel)//0~low other~high
{
	uint8_t buf_send[1] = {0x00};
	uint8_t buf_rece[2] = {0x00,0x00};
	
	if(bitsSel == 0)
		buf_send[0] = FLASH_RDSR_LOW;
	else
		buf_send[0] = FLASH_RDSR_HIGH;
	
	if(PPlus_SUCCESS == spiflash_cmd_tx_and_rx(SPI_EEPROM,0,0,buf_send,buf_rece,1,2))
		return (buf_rece[0] << 8) | (buf_rece[1]);
	else
		return 0xFFFF;
}

bool spiflash_bus_busy(void)
{
	return (spiflash_read_status_register(0) & 0x01);
}


void spiflash_program_erase_suspend(void)
{
	uint8_t buf_send[1] = {FLASH_PES};
	
	if(PPlus_SUCCESS == spiflash_cmd_tx_and_rx(SPI_TXD,0,0,buf_send,NULL,1,0))
	{
		;
	}
}

void spiflash_program_erase_resume(void)
{
	uint8_t buf_send[1] = {FLASH_PER};
	
	if(PPlus_SUCCESS == spiflash_cmd_tx_and_rx(SPI_TXD,0,0,buf_send,NULL,1,0))
	{
		;
	}
}

void spiflash_deep_powerdown(void)
{
	uint8_t buf_send[1] = {FLASH_DP};
	
	if(PPlus_SUCCESS == spiflash_cmd_tx_and_rx(SPI_TXD,0,0,buf_send,NULL,1,0))
	{
		;
	}
}

void spiflash_release_from_powerdown(void)
{
	uint8_t buf_send[1] = {FLASH_RDI};
	
	if(PPlus_SUCCESS == spiflash_cmd_tx_and_rx(SPI_TXD,0,0,buf_send,NULL,1,0))
	{
		;
	}
}

void spiflash_write_enable(void)
{
	uint8_t buf_send[1] = {FLASH_WREN};
	
	while(spiflash_bus_busy());
	if(PPlus_SUCCESS == spiflash_cmd_tx_and_rx(SPI_TXD,0,0,buf_send,NULL,1,0))
	{
		;
	}
}

void spiflash_write_disable(void)
{
	uint8_t buf_send[1] = {FLASH_WRDIS};
	
	//while(spiflash_bus_busy());
	if(PPlus_SUCCESS == spiflash_cmd_tx_and_rx(SPI_TXD,0,0,buf_send,NULL,1,0))
	{
		;
	}
}

void spiflash_chip_erase(void)
{	
	uint8_t buf_send[1] = {FLASH_CE};
	
	buf_send[0] = FLASH_CE;
	spiflash_write_enable();
	if(PPlus_SUCCESS == spiflash_cmd_tx_and_rx(SPI_TXD,0,0,buf_send,NULL,1,0))
	{
		;
	}
}

void spiflash_sector_erase(uint32_t addr)
{	
	uint8_t buf_send[4] = {FLASH_SE,0x00,0x00,0x00};
	
	buf_send[1] = (addr>>16) & 0xff;
	buf_send[2] = (addr>>8) & 0xff;
	buf_send[3] = addr & 0xff;

	spiflash_write_enable();
	if(PPlus_SUCCESS == spiflash_cmd_tx_and_rx(SPI_TXD,0,0,buf_send,NULL,4,0))
	{
		;
	}
}

void spiflash_block_erase_32KB(uint32_t addr)
{
	uint8_t buf_send[4] = {FLASH_BE_32KB,0x00,0x00,0x00};
	
	buf_send[1] = (addr>>16) & 0xff;
	buf_send[2] = (addr>>8) & 0xff;
	buf_send[3] = addr & 0xff;

	spiflash_write_enable();
	if(PPlus_SUCCESS == spiflash_cmd_tx_and_rx(SPI_TXD,0,0,buf_send,NULL,4,0))
	{
		;
	}
}

void spiflash_block_erase_64KB(uint32_t addr)
{
	uint8_t buf_send[4] = {FLASH_BE_64KB,0x00,0x00,0x00};
	
	buf_send[1] = (addr>>16) & 0xff;
	buf_send[2] = (addr>>8) & 0xff;
	buf_send[3] = addr & 0xff;

	spiflash_write_enable();
	if(PPlus_SUCCESS == spiflash_cmd_tx_and_rx(SPI_TXD,0,0,buf_send,NULL,4,0))
	{
		;
	}
}


void spiflash_write_status_register(uint8_t data)
{	
	uint8_t buf_send[2] = {FLASH_WRSR,0x00};
	
	buf_send[1] = data;
	while(spiflash_bus_busy());
	spiflash_write_enable();	
	if(PPlus_SUCCESS == spiflash_cmd_tx_and_rx(SPI_TXD,0,0,buf_send,NULL,2,0))
	{
		;
	}
}

static void spiflash_write_unit(uint32_t addr,uint8_t* tx_buf,uint8_t tx_len)//tx_len in [1,4]
{
	uint8_t buf_send[8] = {FLASH_PP,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

	buf_send[1] = (addr>>16)&0xff;
	buf_send[2] = (addr>>8)&0xff;
	buf_send[3] = addr & 0xff;
	switch(tx_len)
	{
		case 1:
			buf_send[4] = *tx_buf;
		break;
		case 2:
			buf_send[4] = *tx_buf;
			buf_send[5] = *(tx_buf+1);
		break;
		case 3:
			buf_send[4] = *tx_buf;
			buf_send[5] = *(tx_buf+1);
			buf_send[6] = *(tx_buf+2);
		break;
		case 4:
			buf_send[4] = *tx_buf;
			buf_send[5] = *(tx_buf+1);
			buf_send[6] = *(tx_buf+2);
			buf_send[7] = *(tx_buf+3);
		break;
		default:
		break;
	}
	spiflash_write_enable();
	if(PPlus_SUCCESS == spiflash_cmd_tx_and_rx(SPI_TXD,0,0,buf_send,NULL,(tx_len + 4),0))
	{
		;
	}
}

void spiflash_write(uint32_t addr,uint8_t* tx_buf,uint16_t tx_len)
{
	uint16_t offset = 0,ret16;
	
	ret16 = spiflash_read_status_register(0);
	if(ret16 != 0)
	{
		spiflash_write_status_register(0x00);
		while(spiflash_bus_busy() == TRUE);
	}
					
	while(tx_len > 0)
	{
		if(tx_len >= 4)
		{
			spiflash_write_unit((addr + offset),(tx_buf + offset),4);
			offset += 4;
			tx_len -= 4;
		}
		else
		{
			spiflash_write_unit((addr + offset),(tx_buf + offset),tx_len);
			tx_len = 0;
		}
        while(spiflash_bus_busy() == TRUE);
	}

//you can process the protect with your requirenment	
//	if(ret16 != 0)
//	{
//		spiflash_write_status_register(ret16);
//	}
}

void spiflash_write_eeprom(uint32_t addr,uint8_t* tx_buf,uint16_t tx_len)
{
	uint8_t buf_send[256+4];
	uint16_t ret16;

	buf_send[0] = FLASH_PP;
	buf_send[1] = (addr>>16)&0xff;
	buf_send[2] = (addr>>8)&0xff;
	buf_send[3] = addr & 0xff;
	memcpy(&buf_send[4],tx_buf,tx_len);
	
	ret16 = spiflash_read_status_register(0);
	if(ret16 != 0)
	{
		spiflash_write_status_register(0x00);
		while(spiflash_bus_busy() == TRUE);
	}

	spiflash_write_enable();

//	hal_spi_dfs_set(&spiflash_spi,SPI_TXD);
	if(PPlus_SUCCESS == spiflash_cmd_tx_and_rx(SPI_TXD,1,0,buf_send,NULL,(tx_len + 4),0))
	{
		;
	}
	while(spiflash_bus_busy() == TRUE);
}


static void spiflash_read_unit(uint32_t addr,uint8_t* rx_buf,uint8_t rx_len)//rx_len in [1,4]
{
	uint8_t buf_send[4] = {FLASH_READ,0x00,0x00,0x00};
	uint8_t buf_rece[4] = {0x00,0x00,0x00,0x00};

	buf_send[1] = (addr>>16)&0xff;
	buf_send[2] = (addr>>8)&0xff;
	buf_send[3] = addr & 0xff;
	
	if(PPlus_SUCCESS == spiflash_cmd_tx_and_rx(SPI_EEPROM,0,0,buf_send,buf_rece,4,rx_len))
	{
		switch(rx_len)
		{
			case 1:
			*rx_buf = buf_rece[0];
			break;
			case 2:
			*rx_buf = buf_rece[0];
			*(rx_buf+1) = buf_rece[1];
			break;
			case 3:
			*rx_buf = buf_rece[0];
			*(rx_buf+1) = buf_rece[1];
			*(rx_buf+2) = buf_rece[2];
			break;
			case 4:
			*rx_buf = buf_rece[0];
			*(rx_buf+1) = buf_rece[1];
			*(rx_buf+2) = buf_rece[2];
			*(rx_buf+3) = buf_rece[3];
			break;
			default:
			break;
		}
	}
}

static void spiflash_read_eeprom(uint32_t addr,uint8_t* rx_buf,uint16_t rx_len)//rx_len in [1,4]
{
	uint8_t buf_send[4] = {FLASH_READ,0x00,0x00,0x00};
//	uint8_t buf_rece[4] = {0x00,0x00,0x00,0x00};

	buf_send[1] = (addr>>16)&0xff;
	buf_send[2] = (addr>>8)&0xff;
	buf_send[3] = addr & 0xff;
	
	if(PPlus_SUCCESS == spiflash_cmd_tx_and_rx(SPI_EEPROM,0,1,buf_send,rx_buf,4,rx_len))
	{
		;
	}
}

void spiflash_read(uint32_t addr,uint8_t* rx_buf,uint16_t rx_len)
{
//polling mode=polling
//force_cs=0
	uint16_t offset = 0;
	
	while(rx_len > 0)
	{
		if(rx_len >= 4)
		{
			spiflash_read_unit((addr + offset),(rx_buf + offset),4);
			offset += 4;
			rx_len -= 4;
		}
		else
		{
			spiflash_read_unit((addr + offset),(rx_buf + offset),rx_len);
			rx_len = 0;
		}
	}
}

int spiflash_init(void)
{
    uint8_t retval = PPlus_SUCCESS;

    retval = hal_spi_bus_init(&spiflash_spi,spi_cfg);    
    if(retval != PPlus_SUCCESS)
    {
        LOG("spi init err!please check it!\n");
        return retval;
    }

    retval = hal_dma_init_channel(dma_cfg);
    if(retval != PPlus_SUCCESS)
    {
        LOG("dma init err!please check it!\n");
        return retval;
    }

    return retval;
}


//gd25q16
int GD25_init(void)
{
	//if(hal_spi_bus_init(&spi,cfg) == PPlus_SUCCESS)//config and init spi first
	//	LOG("spi init success!\n");
    uint32_t dev = spiflash_read_identification();

	if((!dev) || (dev==0xFFFFFF)){
		LOG("read flash id error %X\n",dev);
		return false;
	}
	LOG("flash id:0x%x\n",dev);
	return true;
}

int GD25_read(uint32_t addr,uint8_t *data,uint16_t len)
{
	if((addr < 0x200000) && (data != NULL) && (len > 0)){
		spiflash_read_eeprom(addr,data,len);
		return true;
	}
	return false;
}

int GD25_erase(uint32_t addr,uint32_t len)
{
	uint8_t lockinfo = 0;
	uint32_t remainder = 0;

	if((addr >= 0x200000) || (len == 0))
		return false;
	
	lockinfo = spiflash_read_status_register(0);
	spiflash_write_status_register(0x00);
	
	if((addr == 0) && (len == 0x200000))
		spiflash_chip_erase();
	else
	{
		remainder = addr%0x1000;//4KB
		if(remainder){
            addr -= remainder;
			len += remainder;
		}
		remainder = len%0x1000;//4KB
		if(remainder){
			len = len + 0x1000 - remainder;
		}
		
		addr = addr/0x1000;
		len = len/0x1000;
		
		while(len > 0){
			//LOG("addr:%d len:%d\n",addr,len);
			if(((addr %16) == 0) && (len >= 16)){
                while(spiflash_bus_busy());
			    spiflash_block_erase_64KB(addr*0x1000);
			    addr += 16;
			    len -= 16;
			    continue;
		    }
			
			if(((addr %8) == 0) && (len >= 8)){
		        while(spiflash_bus_busy());
			    spiflash_block_erase_32KB(addr*0x1000);
			    addr += 8;
			    len -= 8;
			    continue;
			}
				
			if(len >= 1){
		        while(spiflash_bus_busy());
			    spiflash_sector_erase(addr*0x1000);
			    addr += 1;
			    len -= 1;
			    continue;
			}
		}
	}
	
	spiflash_write_status_register(lockinfo);
	while(spiflash_bus_busy());

	return true;
}

int GD25_write(uint32_t addr,const uint8_t *data,uint16_t len)
{
	if((addr < 0x200000) && (data != NULL) && (len > 0)){
		spiflash_write_eeprom(addr,(uint8_t *)data,len);
		return true;
	}
	return false;
}
