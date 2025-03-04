#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "bno08x.h"
#include "utils.h"

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

int main()
{
    stdio_init_all();
    cyw43_arch_init();

    xTaskCreate(led_task, "LED_Task", 256, NULL, 2, NULL);
    xTaskCreate(imu_task, "IMU_Task", 256, NULL, 1, NULL);
    vTaskStartScheduler();

    while(1){};
}