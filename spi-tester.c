//
//  spi-tester.c
//  
//
//  Created by Kristoffer Kjærnes on 21.02.2017.
//  Based on the SPI example by Mike McCauley, author of the bcm2835 library
//
// Example program for bcm2835 library
/* 
Test program which works if you have looped back the MISO pin to the MOSI pin.
Useful for initial testing of the SPI, logic level converters etc. Reports back 
the frequencies that gave correct transfer.
 */
//
// After installing bcm2835, you can build this
// with something like:
// gcc -o spi-tester spi-tester.c -l bcm2835
// sudo ./spi

#include <bcm2835.h>
#include <stdio.h>
int main(int argc, char **argv)
{
    // If you call this, it will not actually access the GPIO
    // Use for testing
    // bcm2835_set_debug(1);
    
    // First initialize the SPI and start an SPI session:
    if (!bcm2835_init())
    {
        printf("bcm2835_init failed. Are you running as root??\n");
        return 1;
    }
    if (!bcm2835_spi_begin())
    {
        printf("bcm2835_spi_begin failedg. Are you running as root??\n");
        return 1;
    }
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // The default
    //bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_65536); // The default
    //bcm2835_spi_setClockDivider(12);                              // Max speed for IO-lines
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_32768); // The default
    bcm2835_spi_chipSelect(BCM2835_SPI_CS1);                      // MCP3008
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);      // the default
    
    // Send a byte to the slave and simultaneously read a byte back from the slave
    // If you tie MISO to MOSI, you should read back what was sent
    int i = 32768; // Start with small clock freq! 250 MHz/32768 = 7.63 kHz
    uint8_t send_data;
    uint8_t read_data;
    double spiClock;
    double divisor;
    while (i){
        bcm2835_spi_setClockDivider(i);
        send_data = 0x23;
        read_data = bcm2835_spi_transfer(send_data);
        if (send_data != read_data){    // Transfer failed.
            divisor = i+2;
            spiClock = 250000/divisor;
            printf("Maximum SPI clock frequency for successful transfer is %f kHz!\n",spiClock);
            divisor = i;
            spiClock = 250000/divisor;
            i = 0;
            printf("Sent to SPI: 0x%02X. Read back from SPI: 0x%02X.\n", send_data, read_data);
            printf("SPI transfer broke down at SPI clock frequency %f kHz\n",spiClock);
            if (spiClock < 500){
                printf("Max clock frequency achieved was lower than 500 kHz.\n");
                printf("Do you have the loopback from MOSI to MISO connected?\n");
            }
        }
        else if (i == 32768 || i == 25000 || i == 10000 || i == 1000 || i == 100 || i == 70 || i == 50){
            divisor = i;
            spiClock = 250000/divisor;
            printf("SPI transfer OK at SPI clock frequency %f kHz. Continuing...\n",spiClock);
            i = i - 2;  // increase SPI frequency.
        }
        else{
            i = i - 2;  // increase SPI frequency.
        }
    }
    bcm2835_spi_end();
    bcm2835_close();
    return 0;
}
