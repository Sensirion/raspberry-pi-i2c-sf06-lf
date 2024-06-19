/*
 * Copyright (c) 2023, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "sensirion_common.h"
#include "sensirion_i2c.h"
#include "sensirion_i2c_hal.h"
#include "sf06_lf_i2c.h"
#include <stdio.h>  // printf
// pigpio is used to access the GPIO Pins. See README for installation.
#include <pigpio.h>

#define sensirion_hal_sleep_us sensirion_i2c_hal_sleep_usec

// GPIO pin numbers to which the IRQn pin of your sensors are connected
#define IRQN_PIN_SENSOR_A 14
#define IRQN_PIN_SENSOR_B 15

// I2C addresses to use for the sensors
#define I2C_ADDR_SENSOR_A 0x0A
#define I2C_ADDR_SENSOR_B 0x0B

void print_byte_array(uint8_t* array, uint16_t len) {
    uint16_t i = 0;
    printf("0x");
    for (; i < len; i++) {
        printf("%02x", array[i]);
    }
}

/*
 * Perfoms a soft reset for all sensors connected to the I2C Bus.
 */
void i2c_soft_reset() {
    uint8_t reset_command[1] = {0x06};
    sensirion_i2c_write_data(0x00, reset_command, 1);
}

/**
 * @brief Change the I2C address of the sensor.
 *
 * @details If you have more then one SLF3x sensor
 * connected to the same I2C bus you need to configure the sensor addresses as
 * all of them have the same default address.

 * Preconditions to call this method:
 * * Your sensor needs to be resetted (hard or soft reset, no commands sent
 *   after the reset)
 * * IRQn Pin (Pin 1) of the sensor for which you want to change the address
 *   must be connected to a GPIO pin
 * * Note that only sensors with a serial number above 23xxxxxxxx have the IRQn
 *   pin
 *
 * @note The I2C address is not configured permanently, you need to run the
 * procedure after each restart of the sensor
 * (hard or soft reset depending on your sensor model).
 *
 * @param new_address the I2C address that shall be assigned to your sensor
 * @param irqn_pin GPIO pin number to which you have IRQn Pin (Pin nr 1) of
 *                 the sensor connected
 * @return error_code 0 on success, an error code otherwise (-1 if the
 *         confirmation pulse from the sensor could not be read)
 */
int change_sensor_address(uint16_t new_address, uint8_t irqn_pin) {
    // Initialise GPIO and make sure IRQn pin of sensor is pulled to 0
    if (gpioInitialise() < 0) {
        printf("Failed to initialise GPIO");
        return -1;
    }
    gpioSetMode(irqn_pin, PI_INPUT);
    gpioSetPullUpDown(irqn_pin, PI_PUD_DOWN);

    // Send I2C address change command 0x3661 with the new I2C address as a
    // parameter (including CRC for the parameter)
    uint8_t communication_buffer[5] = {0};
    uint16_t local_offset = 0;
    local_offset = sensirion_i2c_add_command16_to_buffer(communication_buffer,
                                                         local_offset, 0x3661);
    local_offset = sensirion_i2c_add_uint16_t_to_buffer(
        communication_buffer, local_offset, new_address);
    printf("%i\n", local_offset);
    printf("%i\n", communication_buffer[local_offset]);
    // Note that the command is sent to the default address 0x08 of the sensor
    int16_t local_error = sensirion_i2c_write_data(
        SLF3C_1300F_I2C_ADDR_08, communication_buffer, local_offset);
    if (local_error != NO_ERROR) {
        printf("error sending address change command: %i\n", local_error);
        printf(
            "As there are multiple sensors attached initially listening on the same I2C address \
        the acknowledge might overlap and cause an error which you can ignore if the subsequent communication is successful.\n");
    }

    // Set IRQN pin of one sensor to high for at least 150μs to confirm address
    // change. Only after this pulse has been sent, the sensor actually accepts
    // the new I2C address sent before.
    gpioSetMode(irqn_pin, PI_OUTPUT);
    gpioWrite(irqn_pin, 1);
    sensirion_hal_sleep_us(500);
    // reset IRQn pin back to low state
    gpioWrite(irqn_pin, 0);

    // switch mode to input and listen to the pulse the sensor sends 1500μs
    // after the address change command to confirm the new I2C address
    gpioSetMode(irqn_pin, PI_INPUT);
    gpioSetPullUpDown(irqn_pin, PI_PUD_DOWN);
    sensirion_hal_sleep_us(500);
    int16_t cnt = 0;
    int16_t success = 0;
    while (success == 0 && cnt < 100) {
        success = gpioRead(irqn_pin);
        sensirion_hal_sleep_us(10);
        cnt++;
    }
    gpioTerminate();

    if (success == 0) {
        // return error as sensor did not acknowledge address change
        return -1;
    }

    printf("Sensor address changed to: %02x\n", new_address);
    return NO_ERROR;
}

void readAndPrintSerial() {
    uint32_t product_identifier = 0;
    uint8_t serial_number[8] = {0};
    int16_t local_error =
        sf06_lf_read_product_identifier(&product_identifier, serial_number, 8);
    if (local_error != NO_ERROR) {
        printf("error executing read_product_identifier(): %i\n", local_error);
    }
    printf("product_identifier: %u ", product_identifier);
    printf("serial_number: ");
    print_byte_array(serial_number, 8);
    printf("\n");
}

void readAndPrintMeasurement() {
    float a_flow = 0.0;
    float a_temperature = 0.0;
    uint16_t a_signaling_flags = 0u;

    int16_t local_error = sf06_lf_read_measurement_data(
        INV_FLOW_SCALE_FACTORS_SLF3C_1300F, &a_flow, &a_temperature,
        &a_signaling_flags);
    if (local_error != NO_ERROR) {
        printf("error executing read_measurement_data(): %i\n", local_error);
        return;
    }
    printf("a_flow: %.2f ", a_flow);
    printf("a_temperature: %.2f ", a_temperature);
    printf("a_signaling_flags: %02x\n", a_signaling_flags);
}

int main(void) {
    int16_t error = NO_ERROR;
    sensirion_i2c_hal_init();

    // Make sure sensor is reset before changing the address, as no other
    // commands should be sent to the sensor prior to the address change
    // sequence.
    i2c_soft_reset();
    sensirion_hal_sleep_us(25000);

    // Change the address of the first sensor
    // Set IRQN_PIN_SENSOR_A to the GPIO pin number where you connected Pin 1 of
    // your first sensor.
    error = change_sensor_address(I2C_ADDR_SENSOR_A, IRQN_PIN_SENSOR_A);
    if (error != NO_ERROR) {
        printf("error executing change_sensor_address(): %i\n", error);
        return error;
    }

    // Change the address of the first sensor
    // Set IRQN_PIN_SENSOR_B to the GPIO pin number where you connected Pin 1 of
    // your second sensor.
    error = change_sensor_address(I2C_ADDR_SENSOR_B, IRQN_PIN_SENSOR_B);
    if (error != NO_ERROR) {
        printf("error executing change_sensor_address(): %i\n", error);
        return error;
    }

    // initialize the first sensor
    // tell the driver which i2c address to use before sending commands
    sf06_lf_init(I2C_ADDR_SENSOR_A);
    readAndPrintSerial();
    error = sf06_lf_start_h2o_continuous_measurement();
    if (error != NO_ERROR) {
        printf("error executing start_h2o_continuous_measurement() for sensor "
               "A: %i\n",
               error);
        return error;
    }

    // initialize the second sensor
    // tell the driver which i2c address to use before sending commands
    sf06_lf_init(I2C_ADDR_SENSOR_B);
    readAndPrintSerial();
    error = sf06_lf_start_h2o_continuous_measurement();
    if (error != NO_ERROR) {
        printf("error executing start_h2o_continuous_measurement() for sensor "
               "B: %i\n",
               error);
        return error;
    }

    // read measurements
    uint16_t repetition = 0;
    for (repetition = 0; repetition < 500; repetition++) {
        sensirion_hal_sleep_us(20000);
        sf06_lf_init(I2C_ADDR_SENSOR_A);
        readAndPrintMeasurement();
        sf06_lf_init(I2C_ADDR_SENSOR_B);
        readAndPrintMeasurement();
    }

    // stop measurement mode for first sensor
    sf06_lf_init(I2C_ADDR_SENSOR_A);
    error = sf06_lf_stop_continuous_measurement();
    if (error != NO_ERROR) {
        return error;
    }

    // stop measurement mode for second sensor
    sf06_lf_init(I2C_ADDR_SENSOR_B);
    error = sf06_lf_stop_continuous_measurement();
    if (error != NO_ERROR) {
        return error;
    }

    return NO_ERROR;
}
