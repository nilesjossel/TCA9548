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

<br /><br /><br />

# Custom WiFi protocol for data transfer between Raspberry Pi Pico 2W

## Getting Started

**ENTRYPOINT**: `krepe_wifi_protocol.c`

```sh
git clone --recurse-submodules https://github.com/krups/krepe_wifi_protocol.git
```

## Notes

<!---
In `kernel/include/FreeRTOS.h` the following change has been made.

```h
// #include "portmacro.h"
#include "../portable/ThirdParty/Community-Supported-Ports/GCC/RP2350_ARM_NTZ/non_secure/portmacro.h"
```

This ensures that port specific definitions are included in the FreeRTOS header file; however, this requires all submodules to be recursively updated and added.
-->

```sh
git submodules update --init --recursive
```

The above command can be used to get these subdirectories if necessary.
