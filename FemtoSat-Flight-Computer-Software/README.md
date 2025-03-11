<div id="toc">
  <ul align="center" style="list-style: none">
    <summary>
      <h1>
        University of Kentucky
      </h1>
     <br>
     <h2>Space Systems Laboratory</h2>
    </summary>
  </ul>
</div>

----

# FemtoSat-Flight-Computer-Software

## About
The FemtoSat project is a testbed designed to validate a novel capsule-to-capsule communications scheme designed for the third iteration of the Kentucky Re-Entry Probe Experiment (KREPE3).

The FemtoSats are scaled down versions of the Kentucky Re-entry Universal Payload System (KRUPS) capsules.  They consist of the following:
* A Raspberry Pi Pico 2 W (RP2350) acting as the flight computer
* A LoRa modem (LR62XE) for long range intercapsule communications
* A temperature/pressure/humidity sensor (BME280)
* A 9-axis Inertial Measurement Unit (BNO086)

The hardware repository is available [here](https://github.com/krups/RockSatX-GHOST-Hardware).

## Getting Started
Import this folder as a project into the VS Code Raspberry Pi Pico extension:
1. Clone this repo and its submodules using your preferred method.
- If you're new to git, I'd recommend [Github Desktop](https://github.com/apps/desktop).
- If you're downloading a zip archive from Github, make sure that you download each linked submodule (external repositories linked in the lib folder) manually as they will not be included.
2. Click "Import Project" under the General tab from within the extension.
3. Select the location of this folder
4. Change the debugger from DebugProbe to SWD (If you're using a pico to debug.  If you're using the debug probe, leave it as-is.)
5. Click "Import"
6. Click "Configuare CMake" under the Project tab from within the extension.
7. Click Flash Project
8. Use the debugger to restart the pico by beginning a debugging session. Alternatively, unplug it and plug it back in.
9. Use a terminal program such as PuTTY to view serial outputs from the pico. See [this](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf) guide; Appendix A, section _Use the UART_.
