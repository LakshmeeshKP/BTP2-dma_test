#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/dma.h>
#include <zephyr/drivers/timer/system_timer.h>
#include <zephyr/sys/clock.h>
#include <zephyr/sys/time_units.h>




// static int16_t adc_buf[1024];



#define BUF_SIZE 10

__aligned(32) uint32_t src[BUF_SIZE];
__aligned(32) uint32_t dst[BUF_SIZE];

int main(void){
    // const struct device *adc_dev = DEVICE_DT_GET(DT_NODELABEL(adc));
    
    // struct adc_channel_cfg ch_cfg = {
    //     .gain = ADC_GAIN_1,
    //     .reference = ADC_REF_INTERNAL,
    //     .acquisition_time = ADC_ACQ_TIME_DEFAULT,
    //     .channel_id = 0,
    // };
    
    // adc_channel_setup(adc_dev, &ch_cfg);
    
    // struct adc_sequence seq = {
    //     .channels = BIT(0),
    //     .buffer = adc_buf,
    //     .buffer_size = sizeof(adc_buf),
    //     .resolution = 12,
    // };
    
    // adc_read(adc_dev, &seq);


    for (int i = 0; i < BUF_SIZE; i++) {
        src[i] = 0xABCDABCD;
    }
    
    const struct device *dma_dev = DEVICE_DT_GET(DT_NODELABEL(dma));
    
    if(!device_is_ready(dma_dev)) {
        printk("DMA not ready\n");
        while(1){};
    }
    
    struct dma_block_config blk = {0};
    blk.source_address = (uint32_t)src;
    blk.dest_address   = (uint32_t)dst;
    blk.block_size     = sizeof(src);
    
    struct dma_config cfg = {0};
    cfg.channel_direction = MEMORY_TO_MEMORY;
    cfg.source_data_size = sizeof(uint32_t);
    cfg.dest_data_size = sizeof(uint32_t);
    cfg.block_count = 1;
    cfg.head_block = &blk;



    dma_config(dma_dev, 0, &cfg);
    dma_start(dma_dev, 0);

    // dma_stop(dma_dev, channel);

    k_msleep(10);

    for (int i = 0; i < BUF_SIZE; i++) {
        printk("dst[%d] = 0x%X\n", i, dst[i]);
    }
}