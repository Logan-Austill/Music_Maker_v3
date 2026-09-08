#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "dht_read.h"
#include "pico/multicore.h"

// function for reading temperature and humidity from dht22
bool read_dht(uint8_t gpio_pin, dht22_data_t* read)
{
    uint8_t data[5] = {0};

    // Start signal
    gpio_set_dir(gpio_pin, GPIO_OUT);
    gpio_put(gpio_pin, 0);
    sleep_ms(20);

    gpio_put(gpio_pin, 1);
    sleep_us(40);

    gpio_set_dir(gpio_pin, GPIO_IN);

    // timeout is set to 2ms from current time for safety
    absolute_time_t timeout = make_timeout_time_ms(2);

    // dht has 2ms to pull pin low, then high again
    // mcu currently has the line pulled high, this loop waits for dht to pull line low
    // I have a serious issue here but I can't seem to solve it
    while (gpio_get(gpio_pin))
    {
        if (absolute_time_diff_us(get_absolute_time(), timeout) < 0)
        {
            return false;
        }
        tight_loop_contents();
    }

    // this loop waits for dht to pull line high
    while (!gpio_get(gpio_pin))
    {
        if (absolute_time_diff_us(get_absolute_time(), timeout) < 0)
        {
            return false;
        }
        tight_loop_contents(); 
    }

    // now dht is prepared to send data which starts with a pull low before each bit
    // this loop stalls before the first pull down happens
    while (gpio_get(gpio_pin))
    {
        if (absolute_time_diff_us(get_absolute_time(), timeout) < 0)
        {
            return false;
        }
        tight_loop_contents();
    }

    // Read 40 bits
    for (int i = 0; i < 40; i++)
    {
        // Wait for line to go high for next bit
        while (!gpio_get(gpio_pin));

        // marking time of beginning of next bit
        absolute_time_t start = get_absolute_time();

        // stall while line is high
        while (gpio_get(gpio_pin));

        // line is now low. measure delta between start time and current time
        int pulse_length =
            absolute_time_diff_us(start, get_absolute_time());

        // Shift bit into data
        data[i / 8] <<= 1;

        // sets new bit to 1 if pulse_length is greater than 40
        if (pulse_length > 40)
        {
            /*
                bitwise OR operation
                ex:
                    1100 OR 0001 = 1101
            */
            data[i / 8] |= 1;
        }
    }

    // Verify checksum
    // note: uint8_t will truncate MSB's exceeding the 8th bit
    uint8_t checksum =
        data[0] + data[1] + data[2] + data[3];

    // catching if checksum fails
    if (checksum != data[4])
    {
        read->success = false;
        //multicore_fifo_push_blocking(0xDEADBEEF);
        return false;
    }

    // Convert humidity
    read->humidity = (data[0] << 8) | data[1];

    // Convert temperature
    read->temperature = ((data[2] & 0x7F) << 8) | data[3];

    read->success = true;

    /*
    // Negative temperature check. catching sign bit being set to 1
    if (raw_temp & 0x80)
    {
        raw_temp *= -1;
    }
    */

    //uint32_t data_packet = ((uint32_t)raw_humidity << 16) | raw_temp;
    //multicore_fifo_push_blocking(data_packet);

    return true;
}

void dht_init(dht22_data_t *read)
{
    read->humidity = 0;
    read->temperature = 0;
    read->success = false;
}