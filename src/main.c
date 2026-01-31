#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/dma.h>
#include <zephyr/drivers/timer/system_timer.h>
#include <zephyr/sys/clock.h>
#include <zephyr/sys/time_units.h>

// #include "/../modules/hal/rpi_pico/src/rp2_common/hardware_adc/include/hardware/adc.h"
// #include "/../modules/hal/rpi_pico/src/rp2_common/hardware_dma/include/hardware/dma.h"
// #include "/../modules/hal/rpi_pico/src/rp2_common/hardware_irq/include/hardware/irq.h"
// #include "hardware/irq.h"
#include "hardware/adc.h"
// #include "hardware/dma.h"
// #include "pico/runtime_init.h"
// #include "hardware/timer.h"

#define BUF_SIZE 10

uint16_t buffer[BUF_SIZE];
int channel0;
int flag=0;
// void dma_channel_handler(void);

dma_callback_t dma_callback(const struct device *dev, void *user_data, uint32_t channel, int status){
        printk("DMA callback called\n");
        printk("channel: %d, status: %d\n", channel, status);
        adc_run(false);
        adc_fifo_drain();
        flag=1;
    };
int main(void){

    k_msleep(5000);
    printk("start adc configure\n");
    // IRQ_CONNECT(DMA_IRQ_0, 0, dma_irq_handler, NULL , 0);
    // irq_enable(DMA_IRQ_0);
    
    //configuring ADC 
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);
    adc_set_clkdiv(3000); // 48Mhz/3000 = 16KHz sampling
    adc_fifo_setup(true, true, 1, false, false);
    


    printk("start dma configure\n");
    //configuring DMA
    const struct device *dma_dev = DEVICE_DT_GET(DT_NODELABEL(dma));
    
    if(!device_is_ready(dma_dev)) {
        printk("DMA not ready\n");
        while(1){};
    }

    
    struct dma_block_config blk = {0};
    blk.source_address = (uint32_t)&adc_hw->fifo;
    blk.dest_address   = (uint32_t)buffer;
    blk.block_size     = sizeof(buffer);
    blk.source_addr_adj = 0b10; // no change
    blk.dest_addr_adj   = 0b00; // increment
    
    struct dma_config cfg = {0};
    cfg.channel_direction = PERIPHERAL_TO_MEMORY;
    cfg.complete_callback_en = 0b0;
    cfg.source_data_size = 2;
    cfg.dest_data_size = 2;
    cfg.block_count = 1;
    cfg.head_block = &blk;
    cfg.dma_slot = 36;
    cfg.dma_callback = dma_callback;
    // channel0 = dma_claim_unused_channel(true);
    // printk("DMA channel0: %d\n", channel0);
    // int channel1 = dma_claim_unused_channel(true);
    // printk("DMA channel1: %d\n", channel1);
    // dma_channel_config cfg=dma_channel_get_default_config(channel0);
    
    // channel_config_set_read_increment(&cfg, false);
    // channel_config_set_write_increment(&cfg, true);
    // channel_config_set_dreq(&cfg, DREQ_ADC);
    // channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
    
    // dma_channel_configure(channel0, &cfg, buffer, &adc_hw->fifo, BUF_SIZE, false);
    // dma_channel_set_irq0_enabled(channel0, true);
    // irq_set_exclusive_handler(DMA_IRQ_0, dma_channel_handler);
    // irq_set_enabled(DMA_IRQ_0, true);
    
    // dma_channel_start(channel0);

    
    printk("c1\n");
    dma_config(dma_dev, 0, &cfg);
    printk("c2\n");
    dma_start(dma_dev, 0);
    printk("c3\n");
    adc_run(true);
    printk("c4\n");
    
    while(1){
        printk("in loop\n");
        if(flag){
            for(int i=0; i<BUF_SIZE; i++){
                printk("Sample %d: %d\n", i, buffer[i]);
            }
        }
        flag=0;
        k_msleep(1000);

    }
}

// void dma_irq_handler(void){
//     printk("DMA Transfer Complete\n");
//     adc_run(false);
//     adc_fifo_drain();
//     dma_channel_acknowledge_irq0(channel0);
//     irq_clear(DMA_IRQ_0);
// }