#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "bno08x.h"
#include "utils.h"
#include "TCA9548.h"
#include "Honeywell_SSC.h"


#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

void led_task(void* pvParameters)
{
    while (true) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        printf("LED ON\n");
        vTaskDelay(500);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        printf("LED OFF\n");
        vTaskDelay(500);
    }
}

void imu_task(void* pvParameters) {
    i2c_inst_t* i2c_port0;
    initI2C(i2c_port0, false);
    BNO08x IMU;

    //set up IMU
    while (IMU.begin(CONFIG::BNO08X_ADDR, i2c_port0)==false) {
        printf("BNO08x not detected at default I2C address. Check wiring. Freezing\n");
        scan_i2c_bus();
        sleep_ms(1000);
    }
    IMU.enableRotationVector();

    float roll = 0.0f;
    float pitch = 0.0f;
    float yaw = 0.0f;

    while (true) {

        if (IMU.getSensorEvent() == true) {
            if (IMU.getSensorEventID() == SENSOR_REPORTID_ROTATION_VECTOR) {                
                roll = IMU.getRoll();
                pitch = IMU.getPitch();
                yaw = IMU.getYaw();
            }
        }

        printf("R: %.2f P: %.2f Y: %.2f\n", roll, pitch, yaw);
        vTaskDelay(10);

    }
}

void pressuresensor_task(void* pvParameters {
    i2c_inst_t* i2c_port1;
    initI2C(i2c_port1, false);
    TCA9548 tca(i2c_port1, 0x70);         //TCA i2c address [0x70, 0x77] as output port number (0 to 7)
    // Honeywell_SSC pressureSensors[5] = {
    //     //Re-assign i2c address for SSC sensors according the address pin configuration
    //     Honeywell_SSC(i2c_port1, 0x28, 0.0, 15.0, "psi"), 
    //     Honeywell_SSC(i2c_port1, 0x28, 0.0, 15.0, "psi"),
    //     Honeywell_SSC(i2c_port1, 0x28, 0.0, 15.0, "psi"),
    //     Honeywell_SSC(i2c_port1, 0x28, 0.0, 15.0, "psi"),
    //     Honeywell_SSC(i2c_port1, 0x28, 0.0, 1.6, "bar")  // SSCSRNN1.6BA7A3 sensor
    // };

    // float pressures[5] = {0.0f};

    Honeywell_SSC pressureSensors = (i2c_port1, 0x28, 0.0, 15.0, "psi");
    float pressures = 0.0f;    

    // while (true) {
    //     for (int channel = 0; channel < 5; channel++) {
    //         tca.selectChannel(channel);
    //         pressureSensor[channel].update();
    //         pressures[channel] = pressureSensor[channel].pressure();        
    //         printf("Channel %d Pressure: %.2f %s\n", channel, pressures[channel], pressureSensors[channel].unit());
    //     }

    //     vTaskDelay(50);
    // }

    printf("Pressure: %.2f %s\n", pressures, pressureSensors.unit())

    tca.selectChannel(0);

})


// void rpi5_task(void* pvParameters) {
//     uart_inst_t* uart_port1;
//     initUART(uart_port1, 1000000, 6, 7);
    
    
// }

int main()
{
    stdio_init_all();
    cyw43_arch_init();

    xTaskCreate(led_task, "LED_Task", 256, NULL, 2, NULL);
    xTaskCreate(imu_task, "IMU_Task", 256, NULL, 1, NULL);
    xTaskCreate(pressuresensor_task, "PressureSensor_Task", 256, NULL, 1, NULL);
    vTaskStartScheduler();

    while(1){};
}