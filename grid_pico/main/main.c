/**
 *
 * On-Board LED Blinky
 */


#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pio.h"

#include "hardware/irq.h"
#include "hardware/dma.h"
#include "hardware/spi.h"

#include "uart_tx.pio.h"
#include "uart_rx.pio.h"

#include "hardware/uart.h"

#include <string.h>

#define TEST_SIZE 512

uint dma_tx;
uint dma_rx; 



static uint8_t txbuf[TEST_SIZE];
static uint8_t rxbuf[TEST_SIZE];

const uint CS_PIN = 17; // was 13

uint8_t ready_flags = 255;


// PIO SETUP CONSTANTS    
const uint SERIAL_BAUD = 2000000UL;

const PIO GRID_TX_PIO = pio0;
const PIO GRID_RX_PIO = pio1;


// GRID UART PIN CONNECTIONS
const uint GRID_NORTH_TX_PIN = 23;
const uint GRID_NORTH_RX_PIN = 6;

const uint GRID_EAST_TX_PIN = 26;
const uint GRID_EAST_RX_PIN = 24;

const uint GRID_SOUTH_TX_PIN = 2;
const uint GRID_SOUTH_RX_PIN = 27;

const uint GRID_WEST_TX_PIN = 5;
const uint GRID_WEST_RX_PIN = 3;

// GRID SYNC PIN CONNECTIONS
const uint GRID_SYNC_1_PIN = 25;
const uint GRID_SYNC_2_PIN = 4;

volatile uint8_t spi_dma_done = true;

void dma_handler() {

    spi_dma_done = true;
    gpio_put(CS_PIN, 1);
    dma_hw->ints0 = 1u << dma_rx;

    //printf("FINISH %d\n", txbuf[499]);

}

enum grid_bucket_status_t{
    
    GRID_BUCKET_STATUS_EMPTY,
    GRID_BUCKET_STATUS_RECEIVING,
    GRID_BUCKET_STATUS_FULL,
    GRID_BUCKET_STATUS_TRANSMITTING,
    GRID_BUCKET_STATUS_DONE,

};

struct grid_bucket{

    enum grid_bucket_status_t status;
    uint8_t index;
    uint8_t source_port_index;
    uint8_t buffer[512];
    uint16_t buffer_index;

};

#define BUCKET_BUFFER_LENGTH 500

struct grid_port{

    uint8_t port_index;
    uint8_t tx_buffer[512];
    uint8_t tx_is_busy;
    uint16_t tx_index;

    struct grid_bucket* active_bucket;

};



#define BUCKET_ARRAY_LENGTH 10

uint8_t bucket_array_length = BUCKET_ARRAY_LENGTH;
struct grid_bucket bucket_array[BUCKET_ARRAY_LENGTH];



struct grid_port port_array[4]; 

struct grid_port* NORTH = &port_array[0];
struct grid_port* EAST  = &port_array[1];
struct grid_port* SOUTH = &port_array[2];
struct grid_port* WEST  = &port_array[3];


void grid_port_init(struct grid_port* port, uint8_t index){

    port->port_index = index;

    for (uint16_t i=0; i<512; i++){
        port->tx_buffer[i] = 0;   
    }

    port->tx_index = 0;
    port->tx_is_busy = false;

    port->active_bucket = NULL;

}

void grid_bucket_init(struct grid_bucket* bucket, uint8_t index){

    bucket->status = GRID_BUCKET_STATUS_EMPTY;
    bucket->index = index;

    bucket->source_port_index = 255;

    //for (uint16_t i=0; i<512; i++){
    //    bucket->buffer[i] = 0;
    //}

    bucket->buffer_index = 0;

}

// find next bucket, so UART can safely receive data into it
struct grid_bucket* grid_bucket_find_next_match(struct grid_bucket* previous_bucket, enum grid_bucket_status_t expected_status){

    // if no previous bucket was given then start from the end of the array (+1 will make this the beginning of the array later)
    if (previous_bucket == NULL){
        previous_bucket = &bucket_array[bucket_array_length-1];
    } 

    for (uint8_t i=0; i<bucket_array_length; i++){

        uint8_t next_index = (previous_bucket->index+i+1)%bucket_array_length;

        if (bucket_array[next_index].status == expected_status){

            printf("Bucket 4 u : %d\r\n", next_index);
            return &bucket_array[next_index];
        }

    }
    
    //printf("No bucket 4 u :(\r\n");
    return NULL;
}

struct grid_bucket* spi_active_bucket = NULL;


void grid_bucket_put_character(struct grid_bucket* bucket, char next_char){

    if (bucket == NULL){
        
        printf("PUTC failed: no bucket specified!\n");
        return; 
    }

    if (bucket->buffer_index < BUCKET_BUFFER_LENGTH){

        bucket->buffer[bucket->buffer_index] = next_char;
        bucket->buffer_index++;

    }
    else{
        printf("PUTC: no more space");
    }



}

void spi_start_transfer(uint tx_channel, uint rx_channel, uint8_t* tx_buffer, uint8_t* rx_buffer, irq_handler_t callback){

    spi_dma_done = false;
    gpio_put(CS_PIN, 0);

    dma_channel_config c = dma_channel_get_default_config(tx_channel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_dreq(&c, spi_get_dreq(spi_default, true));
    dma_channel_configure(tx_channel, &c,
                        &spi_get_hw(spi_default)->dr, // write address
                        tx_buffer, // read address
                        TEST_SIZE, // element count (each element is of size transfer_data_size)
                        false); // don't start yet


    c = dma_channel_get_default_config(rx_channel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_dreq(&c, spi_get_dreq(spi_default, false));
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);
    dma_channel_configure(rx_channel, &c,
                        rx_buffer, // write address
                        &spi_get_hw(spi_default)->dr, // read address
                        TEST_SIZE, // element count (each element is of size transfer_data_size)
                        false); // don't start yet


    if (callback != NULL){
        dma_channel_set_irq0_enabled(rx_channel, true);
        irq_set_exclusive_handler(DMA_IRQ_0, callback);
        irq_set_enabled(DMA_IRQ_0, true);
    }



    dma_start_channel_mask((1u << tx_channel) | (1u << rx_channel));


}


void spi_interface_init(){
    // SPI INIT

    // Setup COMMON stuff
    gpio_init(CS_PIN);
    gpio_set_dir(CS_PIN, GPIO_OUT);
    gpio_put(CS_PIN, 1);

    spi_init(spi_default, 31250 * 1000);

    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);

    // Force loopback for testing (I don't have an SPI device handy)
    //hw_set_bits(&spi_get_hw(spi_default)->cr1, SPI_SSPCR1_LBM_BITS);



    dma_tx = dma_claim_unused_channel(true);
    dma_rx = dma_claim_unused_channel(true);

}


void grid_port_attach_bucket(struct grid_port* port){


    port->active_bucket = grid_bucket_find_next_match(port->active_bucket, GRID_BUCKET_STATUS_EMPTY);

    if (port->active_bucket != NULL){

        port->active_bucket->status = GRID_BUCKET_STATUS_RECEIVING;

    }
    else{

        printf("NULL BUCKET\r\n");
    }


}

int main() 
{

    stdio_init_all();

    uint offset_tx = pio_add_program(GRID_TX_PIO, &uart_tx_program);
    uint offset_rx = pio_add_program(GRID_RX_PIO, &uart_rx_program);


    grid_port_init(NORTH,  0);
    grid_port_init(EAST,   1);
    grid_port_init(SOUTH,  2);
    grid_port_init(WEST,   3);

    for (uint8_t i=0; i<bucket_array_length; i++){
        grid_bucket_init(&bucket_array[i], i);
    }




    for (uint8_t i=0; i<4; i++){

        struct grid_port* port = &port_array[i];

        grid_port_attach_bucket(port);

    }


    for (uint8_t i=0; i<4; i++){

        struct grid_port* port = &port_array[i];

        grid_bucket_put_character(port->active_bucket, 'X');

    }

    uart_tx_program_init(GRID_TX_PIO, 0, offset_tx, GRID_NORTH_TX_PIN, SERIAL_BAUD);
    uart_tx_program_init(GRID_TX_PIO, 1, offset_tx, GRID_EAST_TX_PIN, SERIAL_BAUD);
    uart_tx_program_init(GRID_TX_PIO, 2, offset_tx, GRID_SOUTH_TX_PIN, SERIAL_BAUD);
    uart_tx_program_init(GRID_TX_PIO, 3, offset_tx, GRID_WEST_TX_PIN, SERIAL_BAUD);

    uart_rx_program_init(GRID_RX_PIO, 0, offset_rx, GRID_NORTH_RX_PIN, SERIAL_BAUD);
    uart_rx_program_init(GRID_RX_PIO, 1, offset_rx, GRID_EAST_RX_PIN, SERIAL_BAUD);
    uart_rx_program_init(GRID_RX_PIO, 2, offset_rx, GRID_SOUTH_RX_PIN, SERIAL_BAUD);
    uart_rx_program_init(GRID_RX_PIO, 3, offset_rx, GRID_WEST_RX_PIN, SERIAL_BAUD);
    




    // Setup COMMON stuff
    const uint LED_PIN = 15; // was 25 =  PICO_DEFAULT_LED_PIN
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    spi_interface_init();

    for (uint i = 0; i < TEST_SIZE; ++i) {
        txbuf[i] = 0;
    }


    uint32_t loopcouter = 0;
    uint8_t spi_counter = 0;


    while (1) 
    {

        loopcouter++;

        if (loopcouter > 5000){
            gpio_put(LED_PIN, 1);

        }
        if (loopcouter > 10000){
            loopcouter = 0;
            gpio_put(LED_PIN, 0);

            uart_tx_program_putc(GRID_TX_PIO, SOUTH->port_index, '2');

            if (dma_channel_is_busy(dma_rx)){

            }
            else{

                if (spi_dma_done){

                    uint8_t destination_flags = rxbuf[499];

                    // iterate through all the ports
                    for (uint8_t i = 0; i<4; i++){
 
                        struct grid_port* port = &port_array[i]; 

                        // copy message to the addressed port and set it to busy!
                        if ((destination_flags&(1<<port->port_index))){

                            port->tx_is_busy = 1;
                            port->tx_index = 0;
                            strcpy(port->tx_buffer, rxbuf);

                            ready_flags &= ~(1<<port->port_index); // clear ready
                        }

                    }

                    if (spi_active_bucket != NULL){
                        // clear the bucket after use
                        grid_bucket_init(spi_active_bucket, spi_active_bucket->index);
                        spi_active_bucket = NULL;

                    }


                    //printf("START %d\n", txbuf[499]);

                    // try to send bucket content through SPI

                    if (spi_active_bucket == NULL){


                        spi_active_bucket = grid_bucket_find_next_match(spi_active_bucket, GRID_BUCKET_STATUS_FULL);        

                        // found full bucket, send it through SPI
                        if (spi_active_bucket != NULL) {

                            printf("SPI: %s\r\n", spi_active_bucket->buffer);

                            spi_active_bucket->buffer[499] = ready_flags;

                            spi_start_transfer(dma_tx, dma_rx, spi_active_bucket->buffer, rxbuf, dma_handler);
                            

                        }
                        else{
                            // send empty packet with status flags

                            txbuf[0] = 0;
                            sprintf(txbuf, "DUMMY");
                            txbuf[499] = ready_flags;
    
                            spi_start_transfer(dma_tx, dma_rx, txbuf, rxbuf, dma_handler);
                                    
                        }


                    }

                }

            }

        }

        /* ==================================  UART TRANSMIT  =================================*/

        // iterate through all the ports
        for (uint8_t i = 0; i<4; i++){

            struct grid_port* port = &port_array[i]; 

            // if trasmission is in progress then send the next character
            if (port->tx_is_busy){

                char c = port->tx_buffer[port->tx_index];
                port->tx_index++;

                uart_tx_program_putc(GRID_TX_PIO, port->port_index, c);

                if (c == '\n'){

                    port->tx_is_busy = 0;

                    ready_flags |= (1<<port->port_index);
                }

            }

        }

        /* ==================================  UART RECEIVE  =================================*/

        // iterate through all the ports
        for (uint8_t i = 0; i<4; i++){

            struct grid_port* port = &port_array[i]; 

            if (uart_rx_program_is_available(GRID_RX_PIO, port->port_index)){
                char c = uart_rx_program_getc(GRID_RX_PIO, port->port_index);

                grid_bucket_put_character(port->active_bucket, c);

                if (c=='\n'){

                    // end of message, put termination zero character
                    grid_bucket_put_character(port->active_bucket, '\0');


                    printf("BUCKET READY %s\r\n", port->active_bucket->buffer);
                    port->active_bucket->status = GRID_BUCKET_STATUS_FULL;
                    port->active_bucket->buffer_index = 0;
                    // clear bucket

                    grid_port_attach_bucket(port);

                }
                
            }


        }
        
        
    }
}
