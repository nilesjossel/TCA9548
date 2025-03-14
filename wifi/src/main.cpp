#include <stdio.h>
#include "wrappers.cpp"

int main() {
    // Initialize standard Pico SDK functions
    stdio_init_all();
    // cyw43_driver_init();
    // lwip_nosys_deinit();
    
    printf("WiFi Protocol State Machine with FreeRTOS\n");
    printf("=============================================\n\n");
    
    // Create a WiFi controller
    static WiFiController wifiController;
    
    // Create the tasks
    TaskHandle_t stateMachineTaskHandle = NULL;
    TaskHandle_t stateIndicatorTaskHandle = NULL;
    
    // Create state machine controller task with higher priority
    xTaskCreate(
        vWiFiStateMachineTask,       // Function that implements the task
        "WiFiStateMachine",          // Text name for the task
        256,                         // Stack size in words, not bytes
        &wifiController,             // Parameter passed into the task
        2,                           // Priority at which the task is created
        &stateMachineTaskHandle      // Used to pass out the created task's handle
    );
    
    // DEBUG: Create state indicator with lower priority
    xTaskCreate(
        vStateIndicatorTask,               // Function that implements the task
        "StateIndicator",                  // Text name for the task
        128,                         // Stack size in words, not bytes
        &wifiController,             // Parameter passed into the task
        1,                           // Priority at which the task is created
        &stateIndicatorTaskHandle          // Used to pass out the created task's handle
    );
    
    // Create a task for demonstrating state changes
    xTaskCreate(
        [](void* pvParameters) {
            WiFiController* controller = static_cast<WiFiController*>(pvParameters);
            
            // Wait for system to initialize
            vTaskDelay(pdMS_TO_TICKS(2000));
            
            printf("\nStarting network scan...\n");
            controller->StartScan();
            
            // Allow time to observe the scanning state
            vTaskDelay(pdMS_TO_TICKS(5000));
            
            printf("\nDisconnecting...\n");
            controller->Disconnect();
            
            vTaskDelay(pdMS_TO_TICKS(1000));
            
            printf("\nDemo complete, but tasks continue running!\n");
            
            // Delete this task as it's no longer needed
            vTaskDelete(NULL);
        },
        "DemoTask",                 // Text name for the task
        256,                        // Stack size in words, not bytes
        &wifiController,            // Parameter passed into the task
        1,                          // Priority at which the task is created
        NULL                        // No need to track this task's handle
    );
    
    // Start the FreeRTOS scheduler
    printf("Starting FreeRTOS scheduler\n");
    vTaskStartScheduler();
    
    // If the scheduler exits (which it shouldn't), handle the error
    printf("Scheduler exited! This should never happen.\n");
    
    // Loop forever in case of scheduler failure
    while (true) {
        // We should never get here as the scheduler should never exit
    }
    
    return 0;
}
