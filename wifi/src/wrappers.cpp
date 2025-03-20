#include "fsm.cpp"

// Static wrapper functions for FreeRTOS tasks
static void vWiFiStateMachineTask(void* pvParameters) {
    WiFiController* controller = static_cast<WiFiController*>(pvParameters);
    controller->StateMachineTask();
    
    // Tasks should never return, but if they do:
    vTaskDelete(NULL);
}

static void vStateIndicatorTask(void* pvParameters) {
    WiFiController* controller = static_cast<WiFiController*>(pvParameters);
    controller->StateIndicatorTask();
    
    // Tasks should never return, but if they do:
    vTaskDelete(NULL);
}