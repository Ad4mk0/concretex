#include "inc/sensors.h"
#include "inc/flash.h"


float temp_sensor() {
    const struct device *i2c_dev = device_get_binding(dev_i2c.bus->name);
    if (!i2c_dev) {
        printk("I2C device not found\n");
    }

	uint8_t config[] = {0xFD};
	int ret = i2c_write_dt(&dev_i2c, config, sizeof(config));
	if(ret != 0){
		printk("Failed to write to I2C device address %x at reg. %x \n\r", dev_i2c.addr,config[0]);
	}

    k_sleep(K_MSEC(200)); // Wait for measurement to complete


	uint8_t rx_data[6] = {0};
    ret = i2c_read(i2c_dev, rx_data, sizeof(rx_data), dev_i2c.addr);
    if (ret != 0) {
        printk("Failed to read from I2C device address %x\n", dev_i2c.addr);
    }


    uint16_t t_ticks = (rx_data[0] * 256) + rx_data[1];
	// printk("Temperature: %d °C\n", t_ticks);
    // uint16_t rh_ticks = (rx_data[3] * 256) + rx_data[4];

    float temperature = -45.0 + (175.0 * t_ticks / 65535.0);
    // float humidity = -6.0 + (125.0 * rh_ticks / 65535.0);

	printk("Temperature: %.8f °C\n", temperature);

	return temperature;
    // printk("Humidity: %.8f %%RH\n", humidity);
}


int on_pwm()
{
	if (pwm_set_dt(&pwm_led0, (uint32_t)PWM_PERIOD_NS, (uint32_t)PWM_PERIOD_NS / 2U)) {
        printk("Error: Failed to set PWM period\n");
        return 0;
    }
	return 1;
}


int off_pwm()
{
	if (pwm_set_dt(&pwm_led0, (uint32_t)PWM_PERIOD_NS, 0)) {
        printk("Error: Failed to set PWM period\n");
        return 0;
    }
	return 1;
}


int measure_adc()
{
	int err;
	uint32_t count = 0;
	int16_t buf;
	struct adc_sequence sequence = {
		.buffer = &buf,
		.buffer_size = sizeof(buf),
	};

	if (!adc_is_ready_dt(&adc_channel)) {
		printk("ADC controller devivce %s not ready\n", adc_channel.dev->name);
		return 0;
	}

	err = adc_channel_setup_dt(&adc_channel);
	if (err < 0) {
		printk("Could not setup channel #%d (%d)\n", 0, err);
		return 0;
	}

	err = adc_sequence_init_dt(&adc_channel, &sequence);
	if (err < 0) {
		printk("Could not initalize sequnce\n");
		return 0;
	}

	int val_mv;
	err = adc_read(adc_channel.dev, &sequence);
	if (err < 0) {
		printk("Could not read (%d) \n", err);
	}

	val_mv = (int)buf;
	printk("ADC reading[%u]: %s, channel %d: Raw: %d \n", count++, adc_channel.dev->name,
		adc_channel.channel_id, val_mv);

	err = adc_raw_to_millivolts_dt(&adc_channel, &val_mv);
	if (err < 0) {
		printk(" (value in mV not available)\n");
	} else {
		printk(" = %d mV \n", val_mv);
	}

	return val_mv;
}


struct measurements measurement_cycle()
{
	struct measurements measurements;

	on_pwm();
	k_sleep(K_MSEC(5));
	measurements.mV = measure_adc();
	off_pwm();
	k_sleep(K_MSEC(5));
	measurements.temperature = temp_sensor();
	return measurements;
}



int init_sensors() {
	if (!pwm_is_ready_dt(&pwm_led0)) {
		printk("Error: PWM device %s is not ready\n", pwm_led0.dev->name);
		return 0;
	}

	if (!adc_is_ready_dt(&adc_channel)) {
		printk("ADC controller devivce %s not ready\n", adc_channel.dev->name);
		return 0;
	}
	return 1;
}



void serialize_measurements(struct measurements data, uint8_t *buffer) {
    uint16_t temp = (uint16_t)(data.temperature * 1000);
    uint16_t mV = (uint16_t)(data.mV);

    // Place the bytes of temp into the buffer
    buffer[0] = (uint8_t)(temp & 0xFF);        // Lower 8 bits of temp
    buffer[1] = (uint8_t)((temp >> 8) & 0xFF); // Upper 8 bits of temp

    // Place the bytes of mV into the buffer
    buffer[2] = (uint8_t)(mV & 0xFF);          // Lower 8 bits of mV
    buffer[3] = (uint8_t)((mV >> 8) & 0xFF);   // Upper 8 bits of mV
}
