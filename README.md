# Intech Studio firmware repository
![img](./diagram.svg)
[Interactive Diagram](https://mango-dune-07a8b7110.1.azurestaticapps.net/?repo=intechstudio%2Fgrid-fw)

## Instructions
This document will walk you through the process of updating the firmware on your IntechGrid midicontroller. Please read through the  entire document beforehand in order to avid making any mistakes.
 
 The process will not require any special tools, you will only need
 - A computer running Windows, Mac or Linux
 - The Grid controller you wish to update
 - A USB Type C data cable
 
This tutorial will walk you through the following steps:
 - Enumerate in bootloader mode
 - Check the bootloader version on the controller
 - Update the bootloader if it is outdated
 - Upload the new application firmware

 
 
## Enumerating the bootloader

Holding down the mapmode button on the side of the controller while plugging in the USB C cable will boot the device in bootloader mode. All of the LEDs on the user interface should display a dim red color. This means that the device booted successfully, but the host computer has not recognized the device yet. You can release the mapmode button. Once the device is configured on the host, all of the LEDs turn green. On the host computer the grid controller now shows up as a removable storage device named "GRID".

## Checking the bootloader version
Once the controller succesfully enumerated as a removable storage device, you can check the version of the bootloader. If you browse the device you will find 3 files on it:
- CURRENT.UF2 is the binary image file of the current firmware
- INDEX.HTM is a readme file that redirects to this instruction page
- INFO_UF2.TXT contains the bootloader version and other basic information about the controller

Open INFO_UF2.TXT and compare the contents to the latest version quoted here:

```
UF2 Bootloader v3.3.0-6-g9262ab2-dirty SFHWRO
Model: Bootloader 20191211
Board-ID: SAMD51N20A-GRID
```

## Updating the bootloader
Please verify the Bootloader version before before uploading a new firmware image to your controller. The bootloader is maintained in a separate repository:
[grid-uf2](https://github.com/intechstudio/grid-uf2/releases/tag/v4.3.3-8)
Download the bootloader image from the repository and unzip the archive. The bootloader binary is contained in a single update-bootloader***.uf2 file. Once the controller is enumerated in bootloader mode, copy the .uf2 image to the removable storage device called GRID. After a successful update, the controller will disconnect from USB turn off. Unplug the usb cable and repeat this step for all of your Grid modules.

## Updating the firmware
Download the firmware image from the repository and unzip the archive. The firmware binary is contained in the  grid_toplevel_release.uf2 file. **You will find two binary files corresponding to the MCU type in the Grid module, if you're unsure which MCU your module uses, choose the D51 version.** Once the bootloader is enumerated copy the new firmware image to the removable storage device called GRID. After a successful update, the controller will reset and boot the newly installed firmware, ready to use. Make sure to repeat this process for all of your Grid modules, to ensure that they are all running the latest firmware.

### The firmware updating in steps...
- Download & unzip the latest release.
- Hold down the utility button on the side of the module while plugging in the module to your computer.
![img](https://intech.studio/assets/image/access_bootloader.gif)
- You should see Grid booting up as a removable device.
- Drag and drop the new firmware on the device.
- Wait a few seconds, you should see the module rebooting.
- Disconnect the module.
- Reconnect the module, it should work fine now.

## Stable Build
Here you can download the latest stable relese. Plese visit the [releases](https://github.com/intechstudio/grid-fw/releases)
page for more information.

[![GitHub version](https://badge.fury.io/gh/intechstudio%2Fgrid-fw.svg)](https://github.com/intechstudio/grid-fw/releases/latest)
[![Only 32 Kb](https://badge-size.herokuapp.com/intechstudio/grid-fw/master/binary/grid_release.uf2)](https://github.com/intechstudio/grid-fw/tree/master/binary/grid_release.uf2)

## Nightly Build
Nightly builds of the Grid firmware are available for those who wish to experiment with the latest features.
Please check the related commit messages to learn more about the current build.

[![Only 32 Kb](https://badge-size.herokuapp.com/intechstudio/grid-fw/master/binary/grid_nightly.uf2)](https://github.com/intechstudio/grid-fw/tree/master/binary/grid_nightly.uf2)


## Recovery Firmware
This version erases all configuration from the Grid. Use this version of the firmware to recover from faulty configuration state or if the module cannot boot properly. 

[![Only 32 Kb](https://badge-size.herokuapp.com/intechstudio/grid-fw/master/binary/grid_nvm_hardware_test.uf2)](https://github.com/intechstudio/grid-fw/tree/master/binary/grid_nvm_hardware_test.uf2)

