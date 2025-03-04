# rp2350-freertos-template
This is a template project for developing FreeRTOS based applications on Raspberry Pi RP2350 based boards. This template uses the "official" RP2350 port from the Raspberry Pi Foundation.

Unmodified, this project will spawn a single task to rapidly blink the LED on and off.

# Getting Started
Import this project into the VS Code raspberry pi pico extension:
1. Click "Import Project" under the General tab from within the extension.
2. Select the location of this project
3. Change the debugger from DebugProbe to SWD (If you're using a pico to debug.  If you're using the debug probe, leave it as-is.)
4. Click "Import"
5. Click "Configuare CMake" under the Project tab from within the extension.
6. Click Flash Project
7. Unplug & plug your board back in. (You should see a flashing light on the pico you're programming.)
