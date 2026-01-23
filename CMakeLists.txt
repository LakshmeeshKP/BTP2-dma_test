# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(dma_test)

target_include_directories(app PRIVATE
    ${ZEPHYR_BASE}/../modules/hal/rpi_pico/src/rp2_common/hardware_adc/include
    ${ZEPHYR_BASE}/../modules/hal/rpi_pico/src/rp2_common/hardware_dma/include
    ${ZEPHYR_BASE}/../modules/hal/rpi_pico/src/rp2_common/hardware_irq/include
    ${ZEPHYR_BASE}/../modules/hal/rpi_pico/src/rp2_common/pico_runtime_init/include
    # ${ZEPHYR_BASE}/../modules/hal/rpi_pico/src/rp2_common/hardware_regs/include
)

file(GLOB app_sources src/*)
target_sources(app PRIVATE 
    ${app_sources}
    ${ZEPHYR_BASE}/../modules/hal/rpi_pico/src/rp2_common/hardware_dma/dma.c
    ${ZEPHYR_BASE}/../modules/hal/rpi_pico/src/rp2_common/hardware_adc/adc.c
    ${ZEPHYR_BASE}/../modules/hal/rpi_pico/src/rp2_common/hardware_irq/irq.c
    ${ZEPHYR_BASE}/../modules/hal/rpi_pico/src/rp2_common/pico_runtime_init/runtime_init.c
    )
