#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/dma.h>
#include <zephyr/drivers/timer/system_timer.h>
#include <zephyr/sys/clock.h>
#include <zephyr/sys/time_units.h>

// #include "/../modules/hal/rpi_pico/src/rp2_common/hardware_adc/include/hardware/adc.h"
// #include "/../modules/hal/rpi_pico/src/rp2_common/hardware_dma/include/hardware/dma.h"
// #include "/../modules/hal/rpi_pico/src/rp2_common/hardware_irq/include/hardware/irq.h"
#include "hardware/irq.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "pico/runtime_init.h"
// #include "hardware/timer.h"

#define BUF_SIZE 10

uint16_t buffer[BUF_SIZE];
int channel0;
void dma_channel_handler(void);

int main(void){

    k_msleep(5000);
    
    //configuring ADC 
    int flag=0;
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);
    adc_set_clkdiv(3000); // 48Mhz/3000 = 16KHz sampling
    adc_fifo_setup(true, true, 1, false, false);
    
    //configuring DMA
    channel0 = dma_claim_unused_channel(true);
    printk("DMA channel0: %d\n", channel0);
    int channel1 = dma_claim_unused_channel(true);
    printk("DMA channel1: %d\n", channel1);
    dma_channel_config cfg=dma_channel_get_default_config(channel0);
    
    channel_config_set_read_increment(&cfg, false);
    channel_config_set_write_increment(&cfg, true);
    channel_config_set_dreq(&cfg, DREQ_ADC);
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
    
    dma_channel_configure(channel0, &cfg, buffer, &adc_hw->fifo, BUF_SIZE, false);
    dma_channel_set_irq0_enabled(channel0, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_channel_handler);
    irq_set_enabled(DMA_IRQ_0, true);
    
    dma_channel_start(channel0);
    adc_run(true);
    
    while(1){
        if(flag){
            for(int i=0; i<BUF_SIZE; i++){
                printk("Sample %d: %d\n", i, buffer[i]);
            }
        }
        k_msleep(100);
    }
}

void dma_channel_handler(void){
    printk("DMA Transfer Complete\n");
    adc_run(false);
    adc_fifo_drain();
    dma_channel_acknowledge_irq0(channel0);
    irq_clear(DMA_IRQ_0);
}