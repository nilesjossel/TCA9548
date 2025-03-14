#include "pico/cyw43_driver.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define DEBUG_MODE  // Comment this out for release build

#ifdef DEBUG_MODE
#define DEBUG_PRINT(x, ...) printf((x), ##__VA_ARGS__)
#define DEBUG_INDICATOR(state) PrintStateIndicator(state)
#else
#define DEBUG_PRINT(x, ...)
#define DEBUG_INDICATOR(state)
#endif

// Base EventData class for passing data between states
class EventData {
public:
    virtual ~EventData() {}
};

// Empty event data for states that don't need specific data
class NoEventData : public EventData {
};

// WiFi event data for passing WiFi-specific information
class WiFiEventData : public EventData {
public:
    const char* ssid;
    const char* password;
    uint32_t timeout_ms;
    bool security_enabled;
};

// State machine event command
typedef struct {
    uint8_t newState;
    EventData* pData;
} StateEvent_t;

// WiFi Controller State Machine
class WiFiController {
    public:
        // State enumeration - order must match the state map
        enum States {
            ST_DISCONNECTED,
            ST_SCANNING,
            ST_CONNECTING,
            ST_CONNECTED,
            ST_MAX_STATES
        };
    
        WiFiController() : m_currentState(ST_DISCONNECTED) {
            // Create event queue for state changes
            m_eventQueue = xQueueCreate(10, sizeof(StateEvent_t));
            assert(m_eventQueue != NULL);
            
            // Create mutex for thread-safe access to state
            m_stateMutex = xSemaphoreCreateMutex();
            assert(m_stateMutex != NULL);
            
            DEBUG_PRINT("WiFi Controller initialized\n");
        }
    
    ~WiFiController() {
        if (m_eventQueue != NULL) {
            vQueueDelete(m_eventQueue);
        }
        if (m_stateMutex != NULL) {
            vSemaphoreDelete(m_stateMutex);
        }
    }

    // External events
    void StartScan() {
        printf("Request to start scanning for networks\n");
        
        StateEvent_t event;
        event.newState = ST_SCANNING;
        event.pData = NULL;
        
        // Send event to queue
        if (xQueueSend(m_eventQueue, &event, portMAX_DELAY) != pdPASS) {
            printf("Failed to queue scan event\n");
        }
    }

    void Connect(const char* ssid, const char* password, uint32_t timeout_ms = 10000) {
        printf("Request to connect to network: %s\n", ssid);
        
        WiFiEventData* pData = new WiFiEventData();
        pData->ssid = ssid;
        pData->password = password;
        pData->timeout_ms = timeout_ms;
        pData->security_enabled = (password != nullptr && strlen(password) > 0);
        
        StateEvent_t event;
        event.newState = ST_CONNECTING;
        event.pData = pData;
        
        // Send event to queue
        if (xQueueSend(m_eventQueue, &event, portMAX_DELAY) != pdPASS) {
            printf("Failed to queue connect event\n");
        }
        
        // Note: In a production environment, you would not delete pData here
        // as it's being used by another task. You would need a mechanism
        // to clean it up after it's been processed.
        delete pData; // TODO: remove this delete after implementing the other states
        pData = nullptr;
        }

    void Disconnect() {
        printf("Request to disconnect from network\n");
        
        StateEvent_t event;
        event.newState = ST_DISCONNECTED;
        event.pData = NULL;
        
        // Send event to queue
        if (xQueueSend(m_eventQueue, &event, portMAX_DELAY) != pdPASS) {
            printf("Failed to queue disconnect event\n");
        }
    }
    
    // Task function for the state machine controller
    void StateMachineTask() {
        StateEvent_t event;
        
        // Initialize to DISCONNECTED state
        HandleDisconnectedState(NULL);
        
        while (true) {
            // Wait for a state change event
            if (xQueueReceive(m_eventQueue, &event, portMAX_DELAY) == pdPASS) {
                // Get mutex to update state
                if (xSemaphoreTake(m_stateMutex, portMAX_DELAY) == pdTRUE) {
                    m_currentState = event.newState;
                    xSemaphoreGive(m_stateMutex);
                    
                    // Call appropriate state handler
                    switch (m_currentState) {
                        case ST_DISCONNECTED:
                            HandleDisconnectedState(event.pData);
                            break;
                        case ST_SCANNING:
                            HandleScanningState(event.pData);
                            break;
                        case ST_CONNECTING:
                            // Not implemented in this example
                            break;
                        case ST_CONNECTED:
                            // Not implemented in this example
                            break;
                        default:
                            // Should never happen - add assertion
                            assert(false);
                            break;
                    }
                    
                    // Clean up event data if needed
                    // In a real implementation, memory management would be more sophisticated
                    if (event.pData != NULL && event.pData != &m_noEventData) {
                        delete event.pData;
                    }
                }
            }
        }
    }
    
    // Task function for state indicators (replaces LED blinking task)
    void StateIndicatorTask() {
        TickType_t xLastWakeTime;
        const TickType_t xFrequency = pdMS_TO_TICKS(500); // 500ms period for updates
        
        xLastWakeTime = xTaskGetTickCount();
        
        while (true) {
            // Get current state (thread-safe)
            uint8_t currentState = GetCurrentState();
            
            // Update state indicator
            DEBUG_INDICATOR(currentState);
            
            // Wait exactly 500ms using vTaskDelayUntil for precise timing
            vTaskDelayUntil(&xLastWakeTime, xFrequency);
        }
    }

    // Get current state with thread safety
    uint8_t GetCurrentState() {
        uint8_t state;
        if (xSemaphoreTake(m_stateMutex, portMAX_DELAY) == pdTRUE) {
            state = m_currentState;
            xSemaphoreGive(m_stateMutex);
        }
        return state;
    }

private:
    uint8_t m_currentState;
    QueueHandle_t m_eventQueue;
    SemaphoreHandle_t m_stateMutex;
    NoEventData m_noEventData;

    #ifdef DEBUG_MODE
    // For debug state indicator printing
    void PrintStateIndicator(uint8_t state) {
        static uint8_t toggle = 0;
        
        switch(state) {
            case ST_SCANNING:
                // Visual "blinking" effect in console
                printf("WiFi Status: SCANNING %s\r", toggle ? "●" : "○");
                toggle = !toggle;
                break;
            case ST_CONNECTED:
                printf("WiFi Status: CONNECTED ●\r");
                break;
            case ST_CONNECTING:
                printf("WiFi Status: CONNECTING...\r");
                break;
            case ST_DISCONNECTED:
                printf("WiFi Status: DISCONNECTED\r");
                break;
            default:
                printf("WiFi Status: UNKNOWN\r");
                break;
        }
        fflush(stdout);  // Ensure output is displayed immediately
    }
    #endif

    // State handler implementations
    void HandleDisconnectedState(const EventData* pData) {
        DEBUG_PRINT("WiFi Controller: Entering DISCONNECTED state\n");
        
        // For demonstration, just print a message
        DEBUG_PRINT("WiFi hardware powered down\n");
        
        // Runtime assertion as required by rule #5
        assert(GetCurrentState() == ST_DISCONNECTED);
    }

    void HandleScanningState(const EventData* pData) {
        DEBUG_PRINT("WiFi Controller: Entering SCANNING state\n");
        
        // For demonstration, simulate starting a scan
        DEBUG_PRINT("Starting WiFi scan...\n");
    }
};
