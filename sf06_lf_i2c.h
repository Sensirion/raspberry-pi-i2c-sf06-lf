/*
 * THIS FILE IS AUTOMATICALLY GENERATED
 *
 * Generator:    sensirion-driver-generator 0.9.0
 * Product:      sf06_lf
 * Version:      1.0
 */
/*
 * Copyright (c) 2022, Sensirion AG
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

#ifndef SF06_LF_I2C_H
#define SF06_LF_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sensirion_config.h"

// supported i2c addresses of SLF3C-1300F
#define SLF3C_1300F_ADDR_A 0x08

// supported i2c addresses of SLF3S-1300F
#define SLF3S_1300F_ADDR_A 0x08

// supported i2c addresses of SLF3S-0600F
#define SLF3S_0600F_ADDR_A 0x08

// supported i2c addresses of SLF3S-4000B
#define SLF3S_4000B_ADDR_A 0x08

// supported i2c addresses of LD20-0600L
#define LD20_0600L_ADDR_A 0x08

// supported i2c addresses of LD20-2600B
#define LD20_2600B_ADDR_A 0x08
#define SF06_LF_I2C_ADDRESS 0x08

typedef enum {
    START_H2O_CONTINUOUS_MEASUREMENT_CMD_ID = 0x3608,
    START_IPA_CONTINUOUS_MEASUREMENT_CMD_ID = 0x3615,
    STOP_CONTINUOUS_MEASUREMENT_CMD_ID = 0x3ff9,
    START_SINGLE_THERMAL_CONDUCTIVITY_MEASUREMENT_SYNC_CMD_ID = 0x3646,
    START_SINGLE_THERMAL_CONDUCTIVITY_MEASUREMENT_ASYNC_CMD_ID = 0x3646,
    ENTER_SLEEP_CMD_ID = 0x3677,
    EXIT_SLEEP_CMD_ID = 0x0,
    READ_PRODUCT_IDENTIFIER_PREPARE_CMD_ID = 0x367c,
    READ_PRODUCT_IDENTIFIER_CMD_ID = 0xe102,
} cmd_id_t;

typedef enum {
    INV_FLOW_SCALE_FACTORS_SLF3C_1300F = 500,
    INV_FLOW_SCALE_FACTORS_SLF3S_1300F = 500,
    INV_FLOW_SCALE_FACTORS_SLF3S_4000B = 32,
    INV_FLOW_SCALE_FACTORS_SLF3S_0600F = 10,
    INV_FLOW_SCALE_FACTORS_LD20_0600L = 1200,
    INV_FLOW_SCALE_FACTORS_LD20_2600B = 20,
} inv_flow_scale_factors;

/**
 * @brief Initialize i2c address of driver
 *
 * @param[in] i2c_address Used i2c address
 *
 */
void init_driver(uint8_t i2c_address);

/**
 * @brief signal_flow
 *
 * @param[in] raw_flow
 * @param[in] inv_flow_scale_factor inverse scaling factor to convert raw flow
 * value
 *
 * @return As the flow scaling differs for specific sensor types the scaling
 * factor must be passed as an argument. See the enum *inv_flow_scale_factors*
 * for scaling factors of supported sensors. The raw value is converted by: flow
 * = raw_flow / inv_flow_scale_factor Resulting unit depends on your specific
 * sensor type.
 */
float signal_flow(int16_t raw_flow,
                  inv_flow_scale_factors inv_flow_scale_factor);

/**
 * @brief signal_temperature
 *
 * @param[in] raw_temperature
 *
 * @return temperature in degree celsius
 */
float signal_temperature(int16_t raw_temperature);

/**
 * @brief signal_delta_temperature
 *
 * @param[in] raw_delta_temperature
 *
 * @return delta temperature in degree celsius
 */
float signal_delta_temperature(int16_t raw_delta_temperature);

/**
 * @brief signal_thermal_conductivity
 *
 * @param[in] raw_thermal_conductivity
 *
 * @return thermal conductivity in arbitary unit
 */
int16_t signal_thermal_conductivity(int16_t raw_thermal_conductivity);

/**
 * @brief read measured data
 *
 * Reads the raw measurement values and converts them to their physcial units
 * where applicable. For the flow the scaling factor and resulting flow unit
 * depends on the specific sensor. The scaling factor is passed as an argument
 * and the raw flow value is converted by applying: flow = raw_flow /
 * inv_flow_scale_factor The scaling factors for the supported sensor are
 * defined in enum *inv_flow_scale_factors*
 *
 * @param[in] inv_flow_scale_factor used to convert raw flow value
 * @param[out] a_flow As the flow scaling differs for specific sensor types the
 * scaling factor must be passed as an argument. See the enum
 * *inv_flow_scale_factors* for scaling factors of supported sensors. The raw
 * value is converted by: flow = raw_flow / inv_flow_scale_factor Resulting unit
 * depends on your specific sensor type.
 * @param[out] a_temperature temperature in degree celsius
 * @param[out] a_signaling_flags
 *
 * @return error_code 0 on success, an error code otherwise.
 */
int16_t
sf06_lf_read_measurement_data(inv_flow_scale_factors inv_flow_scale_factor,
                              float* a_flow, float* a_temperature,
                              uint16_t* a_signaling_flags);

/**
 * @brief sf06_lf_read_thermal_conductivity_measurement_data
 *
 * Reads single thermal conductivity measurement after a measurement has been
 * started with *start_single_thermal_conductivity_measurement*.
 * Supported by products: SLF3C-1300F, SLF3S-4000B
 *
 * @param[out] a_thermal_conductivity thermal conductivity in arbitary unit
 * @param[out] a_temperature temperature in degree celsius
 * @param[out] a_delta_temperature delta temperature in degree celsius
 *
 * @return error_code 0 on success, an error code otherwise.
 */
int16_t sf06_lf_read_thermal_conductivity_measurement_data(
    int16_t* a_thermal_conductivity, float* a_temperature,
    float* a_delta_temperature);

/**
 * @brief sf06_lf_read_product_identifier
 *
 * Read product identifier and the serial number. The command can only be
 * executed from the idle mode, i.e. when the sensor is not performing
 * measurements.
 *
 * @param[out] product_identifier Note that the last 8 bits are the sensor’s
 * revision number and are subject to change in case of an update of the
 * specifications.
 * @param[out] serial_number 64-bit unique serial number
 *
 * @return error_code 0 on success, an error code otherwise.
 */
int16_t sf06_lf_read_product_identifier(uint32_t* product_identifier,
                                        uint8_t* serial_number,
                                        uint16_t serial_number_size);

/**
 * @brief Start continuous measurement mode for H₂O
 *
 * Starts continuous measurement mode using calibration values for H₂O. The
 * sensor measures both the flow rate and the temperature. After the command has
 * been sent, the chip continuously measures and updates the measurement results
 * which can be read with *read_measurement_data_raw*.
 *
 * @note The first measurement result will be available after 12ms. Due to the
 * thermal measurement principle small accuracy deviations (% m.v.) can occur
 * while the sensor warms-up (60ms for SLF3x, 120ms for LD2x including the 12 ms
 * for measurement initialization).
 *
 * @return error_code 0 on success, an error code otherwise.
 */
int16_t sf06_lf_start_h2o_continuous_measurement();

/**
 * @brief Start continuous measurement mode for Isopropyl alcohol
 *
 * Starts continuous measurement mode using calibration values for Isopropyl
 * alcohol. The sensor measures both the flow rate and the temperature. After
 * the command has been sent, the chip continuously measures and updates the
 * measurement results which can be read with *read_measurement_data_raw*.
 * Supported by products: SLF3C-1300F, SLF3S-1300F, SLF3S-0600F, SLF3S-4000B
 *
 * @note The first measurement result will be available after 12ms. Due to the
 * thermal measurement principle small accuracy deviations (% m.v.) can occur
 * while the sensor warms-up (60ms for SLF3x, 120ms for LD2x including the 12 ms
 * for measurement initialization).
 *
 * @return error_code 0 on success, an error code otherwise.
 */
int16_t sf06_lf_start_ipa_continuous_measurement();

/**
 * @brief Read out the flow measurement data from the sensor.
 *
 * After the command *start_xx_continuous_measurement* has been sent, the chip
 * continuously measures and updates the measurement results. New results (flow,
 * temperature, and signaling flags) can be read continuously with this command.
 *
 * @param[out] raw_flow
 * For SLF3C-1300F, SLF3S-1300F convert to ml/min by applying:
 * flow = raw_flow / 500
 * For SLF3S-4000B convert to ml/min by applying:
 * flow = raw_flow / 32
 * For SLF3S-0600F convert to μl/min by applying:
 * flow = raw_flow / 10
 * For LD20-0600L convert to ml/h by applying:
 * flow = raw_flow / 1200
 * For LD20-2600B convert to ml/h by applying:
 * flow = raw_flow / 20
 * @param[out] raw_temperature Convert to degrees celsius by
 * temperature = raw_temperature / 200
 * @param[out] signaling_flags Gives additional information about the
 * measurement status. Refer to the sensor data sheet for detailed information.
 * Following flags are defined: Air-in-Line flag (Bit 0), High Flow flag (Bit
 * 1), Exponential smoothing active (Bit 5)
 *
 * @note The first measurement result will be available after 12ms after
 * starting the measurement. Small accuracy deviations (% m.v.) can occur during
 * the first 60ms for SLF3x, 120ms for LD2x (including the 12ms initialization)
 *
 * @return error_code 0 on success, an error code otherwise.
 */
int16_t sf06_lf_read_measurement_data_raw(int16_t* raw_flow,
                                          int16_t* raw_temperature,
                                          uint16_t* signaling_flags);

/**
 * @brief Stop the continuous measurement and puts the sensor in idle mode.
 *
 * This command stops the continuous measurement and puts the sensor in idle
 * mode. After it receives the stop command, the sensor needs up to 0.5ms to
 * power down the heater, enter idle mode and be receptive for a new command.
 *
 * @return error_code 0 on success, an error code otherwise.
 */
int16_t sf06_lf_stop_continuous_measurement();

/**
 * @brief Starts a single thermal conductivity measurement, blocking mode.
 *
 * This command starts one thermal conductivity measurement and blocks for 2.3
 * seconds until the measurement results are ready. After completion of the
 * measurement, the heater is switched off and the sensor enters idle mode and
 * the results can be read anytime with
 * *read_thermal_conductivity_measurement_data*. The sensor measures the thermal
 * conductivity, the sensor temperature, and the delta-temperature (a measure
 * for the temperature difference between the liquid and the sensor). Supported
 * by products: SLF3C-1300F, SLF3S-4000B
 *
 * @return error_code 0 on success, an error code otherwise.
 */
int16_t sf06_lf_start_single_thermal_conductivity_measurement_sync();

/**
 * @brief Starts a single thermal conductivity measurement, non-blocking.
 *
 * This command starts one thermal conductivity measurement and returns
 * immediately. Note that the sensor does not accept any other commands while
 * the measurement is running, which takes approximately 2.3 seconds. After
 * completion of the measurement, the heater is switched off and the sensor
 * enters idle mode and the results can be read anytime with
 * *read_thermal_conductivity_measurement_data*. The sensor measures the thermal
 * conductivity, the sensor temperature, and the delta-temperature (a measure
 * for the temperature difference between the liquid and the sensor). Supported
 * by products: SLF3C-1300F, SLF3S-4000B
 *
 * @return error_code 0 on success, an error code otherwise.
 */
int16_t sf06_lf_start_single_thermal_conductivity_measurement_async();

/**
 * @brief Read out the data of a thermal conductivity measurement from the
 * sensor.
 *
 * Reads single thermal conductivity measurement after a measurement has been
 * started with *start_single_thermal_conductivity_measurement_sync* or
 * *start_single_thermal_conductivity_measurement_async*. Supported by products:
 * SLF3C-1300F, SLF3S-4000B
 *
 * @param[out] thermal_conductivity Thermal conductivity signal read from the
 * sensor in arbitary units.
 * @param[out] raw_temperature Calibrated raw temperature signal read from the
 * sensor.
 * @param[out] raw_delta_temperature Calibrated raw delta temperature signal
 * read from the sensor.
 *
 * @return error_code 0 on success, an error code otherwise.
 */
int16_t ll_sf06_lf_read_thermal_conductivity_measurement_data(
    int16_t* thermal_conductivity, int16_t* raw_temperature,
    int16_t* raw_delta_temperature);

/**
 * @brief Enters sleep mode
 *
 * In sleep mode the sensor uses a minimum amount of power. The mode can only be
 * entered from idle mode, i.e. when the sensor is not measuring. This mode is
 * particularly useful for battery operated devices. To minimize the current in
 * this mode, the complexity of the sleep mode circuit has been reduced as much
 * as possible, which is mainly reflected by the way the sensor exits the sleep
 * mode. In sleep mode the sensor cannot be soft reset. Supported by products:
 * LD20-0600L, LD20-2600B
 *
 * @return error_code 0 on success, an error code otherwise.
 */
int16_t sf06_lf_enter_sleep();

/**
 * @brief Exits from sleep mode
 *
 * The sensor exits the sleep mode and enters the idle mode when it receives the
 * valid I2C address and a write bit (‘0’). Note that the I2C address is not
 * acknowledged. It is necessary to poll the sensor to see whether the sensor
 * has received the address and has woken up. This should take typically 25ms.
 * Supported by products: LD20-0600L, LD20-2600B
 *
 * @return error_code 0 on success, an error code otherwise.
 */
int16_t sf06_lf_exit_sleep();

/**
 * @brief Prepare for reading product identifier and the serial number.
 *
 * Prepare to read the product identifier and the serial number. The command can
 * only be executed from the idle mode, i.e. when the sensor is not performing
 * measurements.
 *
 * @return error_code 0 on success, an error code otherwise.
 */
int16_t sf06_lf_read_product_identifier_prepare();

/**
 * @brief Read product identifier and the serial number.
 *
 * This command allows to read product identifier and the serial number. The
 * command can only be executed from the idle mode, i.e. when the sensor is not
 * performing measurements and *read_product_identifier_prepare* is called
 * before.
 *
 * @param[out] product_identifier Note that the last 8 bits are the sensor’s
 * revision number and are subject to change in case of an update of the
 * specifications.
 * @param[out] serial_number 64-bit unique serial number
 *
 * @return error_code 0 on success, an error code otherwise.
 */
int16_t ll_sf06_lf_read_product_identifier(uint32_t* product_identifier,
                                           uint8_t* serial_number,
                                           uint16_t serial_number_size);

#ifdef __cplusplus
}
#endif
#endif  // SF06_LF_I2C_H
