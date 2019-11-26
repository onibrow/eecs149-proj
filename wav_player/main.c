#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "app_error.h"
#include "app_util_platform.h"
#include "boards.h"
#include "nrf.h"
#include "nrf_sdh.h"
#include "nrf_delay.h"
#include "nrf_drv_i2s.h"
#include "nrfx_gpiote.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_serial.h"
#include "buckler.h"
#include "simple_logger.h"

#define I2S_DATA_BLOCK_WORDS    64
// static uint32_t m_buffer_rx[2][I2S_DATA_BLOCK_WORDS];
static uint32_t m_buffer_tx[2][I2S_DATA_BLOCK_WORDS];

nrf_drv_i2s_buffers_t const buffer_zero = {
    .p_rx_buffer = NULL,
    .p_tx_buffer = m_buffer_tx[0],
};
nrf_drv_i2s_buffers_t const buffer_one = {
    .p_rx_buffer = NULL,
    .p_tx_buffer = m_buffer_tx[1],
};

uint8_t  next_i2s_buff = 0;
bool     file_done = false;

static uint8_t volatile m_blocks_transferred     = 0;

static uint32_t       * volatile mp_block_to_fill  = NULL;
static uint32_t const * volatile mp_block_to_check = NULL;

static void prepare_tx_data(uint32_t * p_block)
{
    // These variables will be both zero only at the very beginning of each
    // transfer, so we use them as the indication that the re-initialization
    // should be performed.
    /*
    if (m_blocks_transferred == 0 && m_zero_samples_to_ignore == 0)
    {
        // Number of initial samples (actually pairs of L/R samples) with zero
        // values that should be ignored - see the comment in 'check_samples'.
        m_zero_samples_to_ignore = 2;
        m_sample_value_to_send   = 0xCAFE;
        m_sample_value_expected  = 0xCAFE;
        m_error_encountered      = false;
    }*/
    file_done = simple_logger_read((uint8_t *) p_block , I2S_DATA_BLOCK_WORDS) != 0;
    uint8_t i = 0;
    uint32_t temp = 0;
    while (i < I2S_DATA_BLOCK_WORDS) {
        temp = p_block[i];
        p_block[i] = ((temp & 0xFF) << 24) + 
                     ((temp & 0xFF00) << 8) + 
                     ((temp & 0xFF0000) >> 8) + 
                     ((temp & 0xFF000000) >> 24);
        i += 1;
    }
    // each data word contains two 16-bit samples
    /*
    uint16_t i;
    for (i = 0; i < I2S_DATA_BLOCK_WORDS; ++i)
    {
    	uint16_t sample_l, sample_r;
    	if (i < I2S_DATA_BLOCK_WORDS / 2) {
    		sample_l = 0x7FFF;
    		sample_r = 0x7FFF;
    	} else {
    		sample_l = 0x0000;
    		sample_r = 0x0000;
    	}

        uint32_t * p_word = &p_block[i];
        ((uint16_t *)p_word)[0] = sample_l;
        ((uint16_t *)p_word)[1] = sample_r;
    }
    */
}

static void data_handler(nrf_drv_i2s_buffers_t const * p_released,
                         uint32_t                      status)
{
    // 'nrf_drv_i2s_next_buffers_set' is called directly from the handler
    // each time next buffers are requested, so data corruption is not
    // expected.
    ASSERT(p_released);

    // When the handler is called after the transfer has been stopped
    // (no next buffers are needed, only the used buffers are to be
    // released), there is nothing to do.
    if (!(status & NRFX_I2S_STATUS_NEXT_BUFFERS_NEEDED))
    {
        return;
    }

    // First call of this handler occurs right after the transfer is started.
    // No data has been transferred yet at this point, so there is nothing to
    // check. Only the buffers for the next part of the transfer should be
    // provided.
    if (next_i2s_buff == 0) {
        APP_ERROR_CHECK(nrf_drv_i2s_next_buffers_set(&buffer_one));
        next_i2s_buff = 1;
        prepare_tx_data(buffer_zero.p_tx_buffer);

        // printf("\nbuffer_zero: \n", buffer_zero);
        // for (int i = 0; i < I2S_DATA_BLOCK_WORDS; i++) {
        //     printf("%x ", buffer_zero.p_tx_buffer[i]);
        // }
        // printf("\nbuff_zero done\n");
    } else if (next_i2s_buff == 1) {
        APP_ERROR_CHECK(nrf_drv_i2s_next_buffers_set(&buffer_zero));
        next_i2s_buff = 0;        
        prepare_tx_data(buffer_one.p_tx_buffer);

        // printf("\nbuffer_one: \n", buffer_one);   
        // for (int i = 0; i < I2S_DATA_BLOCK_WORDS; i++) {
        //     printf("%x ", buffer_one.p_tx_buffer[i]);
        // }
        // printf("\nbuff_one done\n");     
    }
        /*
    if (!p_released->p_rx_buffer)
    {
        nrf_drv_i2s_buffers_t const next_buffers = {
            .p_rx_buffer = m_buffer_rx[1],
            .p_tx_buffer = m_buffer_tx[1],
        };
        APP_ERROR_CHECK(nrf_drv_i2s_next_buffers_set(&next_buffers));

        mp_block_to_fill = m_buffer_tx[1];
    }
    else
    {
        mp_block_to_check = p_released->p_rx_buffer;
        // The driver has just finished accessing the buffers pointed by
        // 'p_released'. They can be used for the next part of the transfer
        // that will be scheduled now.
        APP_ERROR_CHECK(nrf_drv_i2s_next_buffers_set(p_released));

        // The pointer needs to be typecasted here, so that it is possible to
        // modify the content it is pointing to (it is marked in the structure
        // as pointing to constant data because the driver is not supposed to
        // modify the provided data).
        mp_block_to_fill = (uint32_t *)p_released->p_tx_buffer;
    }
    */
}

int main(void)
{
    uint32_t err_code = NRF_SUCCESS;

    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
    nrf_sdh_enable_request();

    printf("\n\n=============================");
    printf("\nI2S wav file example started.\n");
    printf("=============================\n\n");

// SD Card Initialization {{{
    // Initialize GPIO driver
    if (!nrfx_gpiote_is_init()) {
        err_code = nrfx_gpiote_init();
    }
    APP_ERROR_CHECK(err_code);
    // Configure SD Card GPIOs
    nrf_gpio_cfg_output(BUCKLER_SD_ENABLE);
    nrf_gpio_cfg_output(BUCKLER_SD_CS);
    nrf_gpio_cfg_output(BUCKLER_SD_MOSI);
    nrf_gpio_cfg_output(BUCKLER_SD_SCLK);
    nrf_gpio_cfg_input(BUCKLER_SD_MISO, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_pin_set(BUCKLER_SD_ENABLE);
    nrf_gpio_pin_set(BUCKLER_SD_CS);

    const char filename[] = "blah.txt";
    const char permissions[] = "a,r"; // w = write, a = append, r = read (?)
    // uint8_t BUF_SIZE = I2S_DATA_BLOCK_WORDS;
    // char read_buff [BUF_SIZE + 1];
    // read_buff[BUF_SIZE] = '\0';

    APP_ERROR_CHECK(simple_logger_init(filename, permissions));

    simple_logger_reset_fp();
// }}} SD CARD INIT END

// CONFIGURE AND START I2S {{{    
    nrf_drv_i2s_config_t config = NRF_DRV_I2S_DEFAULT_CONFIG;

    config.channels  = NRF_I2S_CHANNELS_STEREO;
    config.mck_setup = NRF_I2S_MCK_32MDIV21;
    config.ratio = NRF_I2S_RATIO_96X;
    err_code = nrf_drv_i2s_init(&config, data_handler);
    APP_ERROR_CHECK(err_code);

    printf("SDIN Pin  %d\n", config.sdin_pin);
    printf("SDOUT Pin %d\n", config.sdout_pin);
    printf("BCLK Pin  %d\n", config.sck_pin);
    printf("LRCLK Pin %d\n", config.lrck_pin);
    printf("IQRQ:     %d\n", config.irq_priority);
    printf("Mode:     %d\n", config.mode);
    printf("Fortmat:  %d\n", config.format);
    printf("Align:    %d\n", config.alignment);
    printf("Width:    %d\n", config.sample_width);
    printf("Channels: %d\n", config.channels);
    printf("MCK Setup %d\n", config.mck_setup);
    printf("Ratio     %d\n", config.ratio);
    printf("\n\n===========================\n\n");
    printf("Buffer Zero Addr: %x\n", &buffer_zero);
    printf("Buffer One Addr:  %x\n\n", &buffer_one);
    m_blocks_transferred = 0;
    mp_block_to_fill  = NULL;
    mp_block_to_check = NULL;

    prepare_tx_data(buffer_zero.p_tx_buffer);
    prepare_tx_data(buffer_one.p_tx_buffer);

    // nrf_drv_i2s_buffers_t const initial_buffers = {
    //     .p_tx_buffer = m_buffer_tx[0],
    //     .p_rx_buffer = m_buffer_rx[0],
    // };
    APP_ERROR_CHECK(nrf_drv_i2s_start(&buffer_zero, I2S_DATA_BLOCK_WORDS, 0));

// }}} END I2S INIT

// MAIN LOOP {{{
    // printf("File Contents: \n\n");

    while (!file_done) {
        // if (mp_block_to_fill)
        // {
        //     file_done = simple_logger_read((uint8_t *) m_buffer_tx, I2S_DATA_BLOCK_WORDS) != 0;
        //     // prepare_tx_data(mp_block_to_fill);
        //     mp_block_to_fill = NULL;
        // }
        // file_done = simple_logger_read((uint8_t *) read_buff, BUF_SIZE) != 0;
        // if (next_i2s_buff == 
        // printf("Buffer one: ");
        // for (int i = 0; i < I2S_DATA_BLOCK_WORDS; i++) {
        //     printf("%x ", buffer_one.p_tx_buffer[i]);
        // }
        // printf("Buffer one: ");
        // for (int i = 0; i < I2S_DATA_BLOCK_WORDS; i++) {
        //     printf("%x ", buffer_one.p_tx_buffer[i]);
        // }
        // printf("%s", read_buff);
        nrf_delay_us(100);
    }
    printf("\n\nDone reading.\n");
    nrf_drv_i2s_stop(); 
// MAIN LOOP END }}}

    // Signal that lines were written
    nrf_gpio_cfg_output(BUCKLER_LED0);
    nrf_gpio_pin_clear(BUCKLER_LED0);
}

/** @} */
