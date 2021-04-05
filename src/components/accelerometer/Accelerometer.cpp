#include "Accelerometer.h"
#include <FreeRTOS.h>
#include <legacy/nrf_drv_gpiote.h>
#include <libraries/delay/nrf_delay.h>
#include <task.h>
#include "board_config.h"

using namespace Pinetime::Controllers;


/* Earth's gravity in m/s^2 */
#define GRAVITY_EARTH       (9.80665f)


int8_t user_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr) {
    Pinetime::Drivers::TwiMaster *twiMaster = (Pinetime::Drivers::TwiMaster *)intf_ptr;
    twiMaster->Read(BMA421_TWI_ADDR, reg_addr, reg_data, length);
    return 0;
}


int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr) {
    Pinetime::Drivers::TwiMaster *twiMaster = (Pinetime::Drivers::TwiMaster *)intf_ptr;
    twiMaster->Write(BMA421_TWI_ADDR, reg_addr, reg_data, length);
    return 0;
}

void user_delay(uint32_t period_us, void *intf_ptr) {
    nrf_delay_us(period_us);    
}


Accelerometer::Accelerometer(Pinetime::Drivers::TwiMaster &twiMaster) : twiMaster{twiMaster} {}


void Accelerometer::Init() {

    bma.intf = BMA4_I2C_INTF;
    //dev_addr = BMA4_I2C_ADDR_PRIMARY;
    bma.bus_read = user_i2c_read;
    bma.bus_write = user_i2c_write;
    bma.variant = BMA42X_VARIANT;
    bma.intf_ptr = &twiMaster;
    bma.delay_us = user_delay;
    bma.read_write_len = 8;

    /* Sensor initialization */
    int8_t rslt;

    rslt = bma421_init(&bma);

    //nrf_delay_us(50);
    vTaskDelay(50);

    /* Soft reset */
    bma4_soft_reset(&bma);    
    //nrf_delay_us(100);
    vTaskDelay(50);
    //bma4_set_advance_power_save(BMA4_DISABLE, &bma);    

    deviceReady = true;
    /* Upload the configuration file to enable the features of the sensor. */
    rslt = bma421_write_config_file(&bma);
    if ( rslt != BMA4_OK ) {
      nrf_delay_us(100);
      deviceReady = false;
    }
    //nrf_delay_us(100);
    vTaskDelay(50);

    
}

void Accelerometer::Config() {
    struct bma4_int_pin_config pinConfig;
    if ( !deviceReady ) return;
    /* Enable the accelerometer */
    bma4_set_accel_enable(BMA4_ENABLE, &bma);
    vTaskDelay(50);

    /* Accelerometer Configuration Setting */
    accel_conf.odr = BMA4_OUTPUT_DATA_RATE_100HZ;
    accel_conf.range = BMA4_ACCEL_RANGE_2G;
    accel_conf.bandwidth = BMA4_ACCEL_NORMAL_AVG4;
    accel_conf.perf_mode = BMA4_CIC_AVG_MODE;

    /* Set the accel configurations */    
    bma4_set_accel_config(&accel_conf, &bma);

    bma421_step_detector_enable(BMA4_ENABLE, &bma);

    /* Enable step counter */
    bma421_feature_enable(BMA421_STEP_CNTR , BMA4_ENABLE, &bma);    

    bma421_reset_step_counter(&bma);

    /* Sets the electrical behaviour of interrupt */
    pinConfig.edge_ctrl = BMA4_LEVEL_TRIGGER;
    pinConfig.lvl = BMA4_ACTIVE_LOW;
    pinConfig.od = BMA4_PUSH_PULL;
    pinConfig.output_en = BMA4_OUTPUT_ENABLE;
    pinConfig.input_en = BMA4_INPUT_DISABLE;
    bma4_set_int_pin_config(&pinConfig, BMA4_INTR1_MAP, &bma);

    /* Interrupt Mapping */    
    bma421_map_interrupt(BMA4_INTR1_MAP, BMA421_STEP_CNTR_INT , BMA4_ENABLE, &bma);
   
}

void Accelerometer::Wakeup() {
    if ( !deviceReady ) return;
    /* Sets the advance power save mode */
    bma4_set_advance_power_save(BMA4_DISABLE, &bma);    
}

void Accelerometer::Sleep() {
    if ( !deviceReady ) return;
    /* Sets the advance power save mode */
    bma4_set_advance_power_save(0x03, &bma);
}

void Accelerometer::ResetSteps() {
    if ( !deviceReady ) return;
    bma421_reset_step_counter(&bma);    
}

void Accelerometer::ReadIRQStatus() {
    if ( !deviceReady ) return;
    bma421_read_int_status(&irqStatus, &bma);
}

bool Accelerometer::isStepIRQ() {
    if ( !deviceReady ) return false;
    return (bool)(BMA421_STEP_CNTR_INT & irqStatus);
}



void Accelerometer::UpdateAccel() {
    if ( !deviceReady ) return;
    struct bma4_accel sens_data;
    
    bma4_read_accel_xyz(&sens_data, &bma);
    accelData.x = (sens_data.x / 0x10);
    accelData.y = (sens_data.y / 0x10);
    accelData.z = (sens_data.z / 0x10);
}

void Accelerometer::Update() {
    if ( !deviceReady ) return;
    uint32_t step_out = 0;

    int32_t get_temp_C = 0;

    /* Get temperature in degree C */
    bma4_get_temperature(&get_temp_C, BMA4_DEG, &bma);    

    tempC = (float)get_temp_C / (float)BMA4_SCALE_TEMP;

    bma421_step_counter_output(&step_out, &bma);
    
    step_count = step_out;

}
