#include <stdint.h>

#ifndef DHT_READ_H
#define DHT_READ_H

typedef struct {
    uint16_t humidity;
    float temperature;
    bool success;
} dht22_data_t;

bool read_dht(uint8_t gpio_pin, dht22_data_t* read);
void dht_init(dht22_data_t *read);

#endif
