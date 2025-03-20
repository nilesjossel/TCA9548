#include <stdio.h>
#include "pico/stdlib.h"

// FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

// IMU
#include "bno08x.h"
#include "utils.h"  // BNO08X driver related

// BME280
extern "C" {    // extern because drivers are .c files
#include "bme280_driver.h"
}

// LoRa
#include "hardware/spi.h"
#include "RadioLib.h"
#include "hal/RPiPico/PicoHal.h"

// WiFi modem
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

// I2C defines
#define I2C_PORT i2c0    // i2c port. shared between BME280 & BNO08X
#define I2C_SDA 8        // gpio pin for i2c SDA
#define I2C_SCL 9        // gpio pin for i2c SCL
#define I2C_SPEED 400000 // i2c bus speed, 400KHz

// SPI defines. only used for radio
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_MOSI 19
#define PIN_SCK 18
#define PIN_CS 17
#define PIN_DIO1 26
#define PIN_RST 21
#define PIN_BUSY 22
#define PIN_ANTSW 20

// LoRa parameters
// see https://unsigned.io/understanding-lora-parameters/
#define LORA_FREQ               915.0
#define LORA_BANDWIDTH          500.0
#define LORA_SPREADINGFACTOR    9
#define LORA_CODINGRATE         7
#define LORA_SYNCWORD           RADIOLIB_SX126X_SYNC_WORD_PRIVATE // 0x12
#define LORA_TX_POWER           -9      // in dBm. DO NOT EXCEED 9 dBm! 
                                        // set to lowest option (-9dBm) to prevent overcurrent on USB power
#define LORA_PREAMBLE_LENGTH    8
#define LORA_TXCO_VOLTAGE       0
#define LORA_USE_LDO            false


// BME280
#define BME280_ADDR 0x77        // i2c address of the BME280

// LoRa
static PicoHal myHal(SPI_PORT, PIN_MISO, PIN_MOSI, PIN_SCK);
static SX1262 radio = SX1262(new Module( & myHal, PIN_CS, PIN_DIO1, PIN_RST, PIN_BUSY));

// mutex to stop sensor tasks from 
// accessing the shared i2c bus at the same time
static SemaphoreHandle_t I2C_MUTEX;


void led_task(void* pvParameters)
{
    while (true) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        printf("LED ON\n");
        vTaskDelay(5000);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        printf("LED OFF\n");
        vTaskDelay(5000);
    }
}

void lora_task(void* pvParameters) {

    // starting LoRa module
    printf("[SX1262] Initializing ...\n");

    int initState = radio.begin(
        LORA_FREQ, 
        LORA_BANDWIDTH, 
        LORA_SPREADINGFACTOR, 
        LORA_CODINGRATE, 
        LORA_SYNCWORD, 
        LORA_TX_POWER, 
        LORA_PREAMBLE_LENGTH, 
        LORA_TXCO_VOLTAGE, 
        LORA_USE_LDO
    );

    if (initState != RADIOLIB_ERR_NONE) {
        printf("[SX1262] Radio init failed, code %d\n", initState);
    }
    else {
        printf("[SX1262] Radio init success\n");
    }

    // setting up RF switch. LR62XE's RF switch setup is identical to the one found
    // in the SX1262DVK1CAS reference design schematic. DIO2 can be used as an RF switch
    // if ANTSW is held high and DIO2 is toggled. DIO2 should be high to TX, low to RX.
    // RadioLib already has function to handle this automatically.

    gpio_pull_up(PIN_ANTSW);    // set antsw high
    if (radio.setDio2AsRfSwitch() != RADIOLIB_ERR_NONE) {
        printf("[SX1262] Failed to set DIO2 as RF switch!");
    }

    while(true) {
        // nothing rn
    } 
}

void imu_task(void* pvParameters) {

    BNO08x IMU;
    vTaskDelay(50);  // sleep the thread so that the BME280 can get initialized first
    // set up IMU once the mutex is free
    if (xSemaphoreTake(I2C_MUTEX, portMAX_DELAY) == pdTRUE){

        // will block until imu is initialized successfully
        if (IMU.begin(CONFIG::BNO08X_ADDR, I2C_PORT)==false) {
            printf("BNO08x not detected at default I2C address. Check wiring.\n");
        }

        IMU.enableRotationVector();
        xSemaphoreGive(I2C_MUTEX);  // release mutex
    }

    float roll = 0.0f;
    float pitch = 0.0f;
    float yaw = 0.0f;

    while (true) {

        // once mutex is free, check it out and do a sensor read
        if (xSemaphoreTake(I2C_MUTEX, portMAX_DELAY) == pdTRUE){

            if (IMU.getSensorEvent() == true) {
                if (IMU.getSensorEventID() == SENSOR_REPORTID_ROTATION_VECTOR) {                
                    roll = IMU.getRoll();
                    pitch = IMU.getPitch();
                    yaw = IMU.getYaw();
                }
            }
            xSemaphoreGive(I2C_MUTEX);  // release mutex
        }

        printf("Roll: %.2f Pitch: %.2f Yaw: %.2f\n", roll, pitch, yaw);

        // wait 1000ms before reading again
        vTaskDelay(1000);
    }
}

void bme280_task(void* pvParameters)
{
    /*/ Initialize the sensor instance in forced mode

     Forced mode takes a single measurement before putting 
     the sensor back in sleep mode. For our use-case, this
     is probably the best mode of operation.

     See datasheet chapter 3.3 for more info.
    */
    struct bme280_inst bme280;
    int8_t res; // result (i think)

    // once mutex is free, check it out and initialize the sensor 
    if (xSemaphoreTake(I2C_MUTEX, portMAX_DELAY) == pdTRUE){
        res = bme280_init(i2c0,
            BME280_ADDR,
            &bme280,
            BME280_FORCED_MODE,
            BME280_FILTER_OFF,
            BME280_T_OVERSAMPLE_1,
            BME280_H_OVERSAMPLE_1,
            BME280_P_OVERSAMPLE_1
        );
    
        if (res != BME280_OK)
        {
            printf("Error: failed to initialise BME280 with error code %s...\n", bme280_strerr(res));
        }
        else {
            printf("Success! Performing forced read...\n");
        }

        xSemaphoreGive(I2C_MUTEX);  // release mutex
    }

    // for storing formatted readings
    char temp[BME280_T_STRLEN], humid[BME280_H_STRLEN], press[BME280_P_STRLEN];

    while (true) {
        
        if (xSemaphoreTake(I2C_MUTEX, portMAX_DELAY) == pdTRUE){
            
            // perform a sensor read, output to console
            res = bme280_forced_read(&bme280);
            if (res != BME280_OK)
            {
                printf("Error: failed to perform forced read with error code %s...\n", bme280_strerr(res));
            }
            bme280_fmt_temp(&bme280, temp);
            bme280_fmt_humid(&bme280, humid);
            bme280_fmt_press(&bme280, press);

            xSemaphoreGive(I2C_MUTEX);  // release mutex
        }

        printf("Result of forced read: Temp: %s - Hum: %s - Pres: %s\n", temp, humid, press);

        // wait 1000ms before reading again
        vTaskDelay(1000);
    }
}

int main()
{
    stdio_init_all();
    cyw43_arch_init();

    // delay to let you set up your serial monitor
    sleep_ms(5000);

    // I2C Initialization
    i2c_init(I2C_PORT, I2C_SPEED);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // make i2c mutex
    I2C_MUTEX = xSemaphoreCreateMutex();

    // task creation
    xTaskCreate(led_task, "LED_Task", 256, NULL, 3, NULL);
    xTaskCreate(imu_task, "IMU_Task", 256, NULL, 1, NULL);
    xTaskCreate(bme280_task, "BME280_Task", 256, NULL, 2, NULL);
    xTaskCreate(lora_task, "LoRa_Task", 256, NULL, 4, NULL);
    vTaskStartScheduler();

    while(1){};
}