/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for Petit FatFs (C)ChaN, 2014      */
/*-----------------------------------------------------------------------*/

#include <avr/io.h>

#include "diskio.h"

#define CS_HIGH() PORTB |= (1 << PB0)
#define CS_LOW()  PORTB &= ~(1 << PB0)

#define CMD0 0
#define CMD8 8
#define CMD17 17
#define CMD41 41
#define CMD55 55

#define SD_MAX_READ_ATTEMPTS    1563

void debug_bin(uint8_t data) {
	for(uint8_t i = 0; i < 8; i++) {
        while ( !( UCSR0A & (1 << UDRE0)) )
        {
            // wait, until UDR0 is available again
        }
        UDR0 = (((data >> (7 - i)) & 0x01) + 48);
	}
	while ( !( UCSR0A & (1 << UDRE0)) )
        {
            // wait, until UDR0 is available again
        }
    UDR0 = '\n';
}

void init_spi()
{
	// Set SCK and MOSI as output
	DDRB |= (1 << PB1) | (1 << PB2);

	// Set CS as output and set HIGH to set SD card in SPI mode
	// If the CS pin isn't manually set as output, SPI won't do anything
	DDRB |= (1 << PB0);

	// Enable SPI, set Master Mode, set prescaler to 128
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0) | (1 << SPR1);
}

uint8_t spi_transmit(uint8_t data)
{
	SPDR = data;
	// Wait until the transmission is complete
	while(!(SPSR & (1<<SPIF))){}
	return SPDR;
}

uint8_t send_command(uint8_t command, uint32_t argument)
{
	// Select the card
	CS_HIGH();
	spi_transmit(0xFF);
	CS_LOW();
	spi_transmit(0xFF);

	// send the start bits and the command
	spi_transmit(0b01000000 | command);
	
	// send 4 byte argument
	for (uint8_t i = 0; i < 32; i+=8) 
	{
		spi_transmit((argument >> (24 - i)) & 0xFF);
	}

	// send CRC and stop bit
	if     (command == CMD0) spi_transmit(0x95);
	else if(command == CMD8) spi_transmit(0x87);
	else spi_transmit(0x01);

	// Wait for response with a timeout of 80 cycles
	uint8_t n = 10;
	uint8_t r1;
	do {
		r1 = spi_transmit(0xFF);
	} while((r1 & 0x80) && --n);

	CS_LOW();
	spi_transmit(0xFF);
	CS_HIGH();
	spi_transmit(0xFF);

	return r1;
}

uint32_t get_r3()
{
	uint32_t r3;
	for(int i = 0; i < 4; i++) {
		r3 = (r3 << 8) | spi_transmit(0xFF);
	}
	return r3;
}

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize()
{
	uint8_t r1;
	uint32_t r3;

	init_spi();

	CS_HIGH();
	for(uint8_t i = 0; i < 10; i++)
	{
		spi_transmit(0xFF); // 80 clock cycles with CS and MOSI high
	}

	r1 = send_command(CMD0, 0);
	if(r1 != 1)
	{
		return STA_NOINIT;
	}
	r1 = send_command(CMD8, 0x1AA);
	if(r1 == 1) { // SDv2
		r3 = get_r3();
		// Send ACMD41 until card leaves idle state
		do { // Send ACMD41 until card is ready
			send_command(CMD55, 0);
			r1 = send_command(CMD41, 0x40000000);
		} while(r1 != 0);
	} else { // SDv1 or MMCv3
	}

	CS_HIGH();
	spi_transmit(0xFF);

	return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Partial Sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp (
	BYTE* buff,		/* Pointer to the destination object */
	DWORD sector,	/* Sector number (LBA) */
	UINT offset,	/* Offset in the sector */
	UINT count		/* Byte count (bit15:destination) */
)
{
    uint8_t res1; 
	uint8_t read;
    uint16_t readAttempts;

	DRESULT res = RES_ERROR;

	CS_HIGH();
	spi_transmit(0xFF);
	CS_LOW();
	spi_transmit(0xFF);

	res1 = send_command(CMD17, sector * 512 + offset);

 	if(res1 != 0xFF)
    {
		readAttempts = 0;
        while(++readAttempts != SD_MAX_READ_ATTEMPTS)
            if((read = spi_transmit(0xFF)) != 0xFF) break;

        // if response token is 0xFE
        if(read == 0xFE)
        {
            // read 512 byte block
            for(uint16_t i = 0; i < 512; i++)
			{
				*(buff++) = spi_transmit(0xFF);
			}
			
            // read 16-bit CRC
            spi_transmit(0xFF);
            spi_transmit(0xFF);
		}

		res = RES_ERROR;
	}

	CS_LOW();
	spi_transmit(0xFF);
	CS_HIGH();
	spi_transmit(0xFF);

	return res;
}



/*-----------------------------------------------------------------------*/
/* Write Partial Sector                                                  */
/*-----------------------------------------------------------------------*/

DRESULT disk_writep (
	const BYTE* buff,		/* Pointer to the data to be written, NULL:Initiate/Finalize write operation */
	DWORD sc		/* Sector number (LBA) or Number of bytes to send */
)
{
	DRESULT res;


	if (!buff) {
		if (sc) {

			// Initiate write process

		} else {

			// Finalize write process

		}
	} else {

		// Send data to the disk

	}

	return res;
}

