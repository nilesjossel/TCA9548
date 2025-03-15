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

# RocketStation-Flight-Computer-Software

## About
The FemtoSat project is a testbed designed to validate a novel capsule-to-capsule communications scheme designed for the third iteration of the Kentucky Re-Entry Probe Experiment (KREPE3).
The RocketStation is the main receiving station for the RockSat-XG femtosat network. It will receive and uplink all radio transmissions through the rocket's telemetry system.
It will also interface with a Raspberry Pi 5 & a camera module, which will take a picture of a KRUPS capsule as it is deployed by the rocket.

The RocketStation is a Raspberry Pi 5 hat with the following components:
* A Raspberry Pi Pico 2 W (RP2350) acting as the flight computer
* A LoRa modem (LR62XE) for receiving long range intercapsule communications
* A temperature/pressure/humidity sensor (BME280)
* A 9-axis Inertial Measurement Unit (BNO086)
* A two channel RS-232 level shifter that forwards RocketStation data to the rocket's telemetry pins

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
8. Use the debugger to restart the pico by beginning a debugging session.
