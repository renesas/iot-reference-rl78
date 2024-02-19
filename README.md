# FreeRTOS RL78 Family IoT Reference

## Introduction

This product provides the reference of IoT solution with using [RL78 family](https://www.renesas.com/products/microcontrollers-microprocessors/rl78-low-power-8-16-bit-mcus), [AWS](https://aws.amazon.com), and [FreeRTOS](https://www.freertos.org/RTOS.html). The RL78 family is a kind of MCUs provided by the [Renesas](https://www.renesas.com).
You can easily try to run AWS IoT demos using RL78 family while working with our various other products.
Refer to the [Getting Started Guide](#user_documents) for step by step instructions on setting up your development environment and running demos.

## Product Specifications

This reference is consist of demo applications, FreeRTOS kernel, middleware provided by AWS and 3rd party, middleware and drivers for RL78 family provided as the Software Integration System by the Renesas, files to collaborate Renesas tools such as the IDE [e2 studio](https://www.renesas.com/software-tool/e-studio), and etc.
Summary of specifications explains in the following chapters.

### Using AWS Services

* [AWS IoT core](https://aws.amazon.com/iot-core/)

### Demos

This reference consits the following demo projects:

* Demo project (PubSub)
  * Perform simple data upload via MQTT communication.
* Demo project (OTA)
  * Perform firmware update via OTA.

The preceding demos use the following technical elements of the AWS IoT:

* [AWS IoT Jobs](https://docs.aws.amazon.com/iot/latest/developerguide/iot-jobs.html)
* [MQTT File Delivery](https://docs.aws.amazon.com/iot/latest/developerguide/mqtt-based-file-delivery.html)

### Supported Boards

This reference supports the following boards:

* [RL78/G23-128p Fast Prototyping Board(RL78/G23-128p FPB)](https://www.renesas.com/products/microcontrollers-microprocessors/rl78-low-power-8-16-bit-mcus/rtk7rlg230csn000bj-rl78g23-128p-fast-prototyping-board-rl78g23-128p-fpb) with:
  * Cellular(CAT M1)
    * [RYZ024A PMOD Expansion Board](https://www.renesas.com/products/wireless-connectivity/cellular-iot-modules/rtkyz024a0b00000be-pmod-expansion-board-ryz024a)

### User Documents

The documents for this reference are followings:

* Getting Started Guide
  * For RL78/G23-128p Fast Prototyping Board
    * **WIP**[Getting Started Guide for Connecting Amazon Web Services in LTE Communication: RL78/G23-128p Fast Prototyping Board + FreeRTOS]()

### Open Source Software (OSS) Components

The following table indicates name and version of OSS which are used in this reference. The column *LTS Until* indicates the support period as LTS. The column *Use* indicates whether the OSS is performed in demo projects or not.

| Library                     | Version             | LTS Until  | LTS Repo URL                                                                | Use |
|-------------------------    |---------------------|------------|---------------------------------------------------------------------------|-----|
| FreeRTOS Cellular Interface              | 1.3.0              | 10/31/2024 | <https://github.com/FreeRTOS/FreeRTOS-Cellular-Interface>                        | Yes |
| FreeRTOS Kernel             | 10.5.1              | 10/31/2024 | <https://github.com/FreeRTOS/FreeRTOS-Kernel>                        | Yes |
| FreeRTOS-Plus-TCP             | 3.1.0              | 10/31/2024 | <https://github.com/FreeRTOS/FreeRTOS-Kernel>                        | No  |
| backoffAlgorithm             | 1.3.0              | 10/31/2024 | <https://github.com/FreeRTOS/backoffAlgorithm>                        | Yes |
| coreHTTP Client             | 3.0.0              | 10/31/2024 | <https://github.com/FreeRTOS/coreHTTP>                        | No  |
| coreJSON             | 3.2.0              | 10/31/2024 | <https://github.com/FreeRTOS/coreJSON>                        | Yes |
| coreMQTT Client             | 2.1.1              | 10/31/2024 | <https://github.com/FreeRTOS/coreMQTT>                        | Yes |
| coreMQTT Agent             | 1.2.0              | 10/31/2024 | <https://github.com/FreeRTOS/coreMQTT-Agent>                        | Yes |
| corePKCS11             | 3.5.0              | 10/31/2024 | <https://github.com/FreeRTOS/corePKCS11>                        | No  |
| coreSNTP             | 1.2.0              | 10/31/2024 | <https://github.com/FreeRTOS/coreSNTP>                        | No  |
| AWS IoT Device Defender             | 1.3.0              | 10/31/2024 | <https://github.com/aws/Device-Defender-for-AWS-IoT-embedded-sdk>                        | No  |
| AWS IoT Device Shadow             | 1.3.0              | 10/31/2024 | <https://github.com/aws/Device-Shadow-for-AWS-IoT-embedded-sdk>                        | No  |
| AWS IoT Fleet Provisioning             | 1.1.0              | 10/31/2024 | <https://github.com/aws/Fleet-Provisioning-for-AWS-IoT-embedded-sdk>                        | No  |
| AWS IoT Jobs              | 1.3.0              | 10/31/2024 | <https://github.com/aws/Jobs-for-AWS-IoT-embedded-sdk>                        | No  |
| AWS SigV4             | 1.2.0              | 10/31/2024 | <https://github.com/aws/SigV4-for-AWS-IoT-embedded-sdk>                        | No  |
| AWS IoT Over-the-air             | 3.4.0              | 10/31/2024 | <https://github.com/aws/ota-for-aws-iot-embedded-sdk>                        | Yes |
| tinycbor             | 0.5.2              | -- | <https://github.com/intel/tinycbor>                        | Yes |
| FreeRTOS-Plus network_transport             | --              | -- | <https://www.freertos.org/network-interface.html>                        | Yes |
| Logging Interface             | 1.1.3              | -- | <https://github.com/aws/amazon-freertos/tree/main/libraries/logging>                        | Yes |
| tinycrypt             | 0.2.8              | -- | <https://github.com/intel/tinycrypt>                        | Yes |

### Software Integration System (SIS) and CG (Code Generation) Modules

The following table indicates name and version.

| SIS or CG Modules | Revision | URL |
|------------|---------|---------|
|r_bsp|1.60|[Board Support Package Module Using Software Integration System](https://www.renesas.com/document/apn/rl78-family-board-support-package-module-using-software-integration-system)|
|Config_UART|1.6.0|-|
|Config_PORT|1.4.0|-|

### Other Drivers

The following table indicates name and version.

| Drivers | Revision | URL |
|------------|---------|---------|
|r_fwup|2.01|[RL78/G22, RL78/G23, RL78/G24 Firmware Update Module](https://www.renesas.com/document/apn/rl78g22-rl78g23-rl78g24-firmware-update-module-rev201)|
|Renesas Flash Driver RL78 Type01|1.00|[Renesas Flash Driver RL78 Type01](https://www.renesas.com/document/man/renesas-flash-driver-rl78-type-01-users-manual-rl78g23)|
|r_byteq||[BYTEQ Module Using Firmware Integration Technology](https://www.renesas.com/document/apn/byteq-module-using-firmware-integration-technology)|
|r_sci||[SCI Module Using Firmware Integration Technology](https://www.renesas.com/document/apn/sci-module-using-firmware-integration-technology)|

## Limitation

* Demo project (OTA) will not work properly unless `otaconfigMAX_NUM_BLOCKS_REQUEST` is set to "1".  
  If `otaconfigMAX_NUM_BLOCKS_REQUEST` is set to anything other than 1, a build error will occur.  
* Notes on bootloader to application transition.  
  When transitioning from the bootloader sample program to the application, the settings of the bootloader's peripheral functions are taken over by the application.  
  For more information, check chapter 7 of the following document.  
  [RL78/G22, RL78/G23, RL78/G24 Firmware Update Module](https://www.renesas.com/document/apn/rl78g22-rl78g23-rl78g24-firmware-update-module-rev201)
* In version LR8.2.0.2-59200 or later, which is one of RYZ024A's firmware, the `AT+SQNSNVW` command has limitation that indexes 0 to 4 and 7 to 10 of non-volatile memory in RYZ024A are reserved so they cann't be used for writing / deleting.  
  This reference avoid to use these indexes, but pay attention if you use this command directly in your application.
  For more information about this command, refer to the [RYZ024 Modules AT Command User's Manual](https://www.renesas.com/document/mah/ryz024-modules-command-users-manual?r=1636901).

### Configuration values changed from the default in the FIT Modules

* The configuration values of the FIT modules that have been changed from the default values are listed in the table below.
* However, each projects is evaluated only with preset values, including configuration values that have not been changed from the default values.
* If changed, the program may not work properly.

#### RL78/G23-128p FPB Cellular-RYZ024A Projects

  | FIT module | Config name | Default Value | Project value | Reason for change |
  |------------|-------------|---------------|---------------|-------------------|
  | r_bsp      | BSP_CFG_HEAP_BYTES | 0x400 | 0x1000 | Because LittleFS and fleet provisioning demo uses malloc which is not an OS feature.<br>Also, because the default value cannot secure enough heap memory. |
  |            | BSP_CFG_CODE_FLASH_BANK_MODE | 1 | 0 | This project uses the Dual bank function. |
  |            | BSP_CFG_RTOS_USED | 0 | 1 | This project uses FreeRTOS. |
  |            | BSP_CFG_SCI_UART_TERMINAL_ENABLE | 0 | 1 | This project uses SCI UART terminals. |
  |            | BSP_CFG_SCI_UART_TERMINAL_CHANNEL | 8 | 5 | This project uses SCI CH5 as the SCI UART terminal. |
  | r_flash_rx | FLASH_CFG_CODE_FLASH_ENABLE | 0 | 1 | OTA library rewrites code flash. |
  |            | FLASH_CFG_DATA_FLASH_BGO | 0 | 1 | LittleFS is implemented to rewrite data flash using BGO functionality. |
  |            | FLASH_CFG_CODE_FLASH_BGO | 0 | 1 | OTA library is implemented to rewrite code flash using BGO functionality. |
  |            | FLASH_CFG_CODE_FLASH_RUN_FROM_ROM | 0 | 1 | OTA library is implemented to execute code that rewrites the code flash from another bank. |
  | r_sci_rx   | SCI_CFG_CH5_INCLUDED | 0 | 1 | SCI CH5 is used as the SCI UART terminal. |
  |            | SCI_CFG_CH6_INCLUDED | 0 | 1 | SCI CH6 is used to communicate with the RYZ014A module. |
  |            | SCI_CFG_CH6_TX_BUFSIZ | 80 | 2180 | The TX buffer size needs to be increased to communicate with RYZ014A. |
  |            | SCI_CFG_CH6_RX_BUFSIZ | 80 | 8192 | The RX buffer size needs to be increased to communicate with RYZ014A. |
  |            | SCI_CFG_TEI_INCLUDED | 0 | 1 | Transmit end interrupt is used. |
  | r_fwup     | FWUP_CFG_UPDATE_MODE | 1 | 0 | This project uses Dual bank function. |
  |            | FWUP_CFG_FUNCTION_MODE | 0 | 1 | This project is user program. |
  |            | FWUP_CFG_MAIN_AREA_ADDR_L | 0xFFE00000U | 0xFFF00000U | This value is set according to the RX65N ROM 2MB product. |
  |            | FWUP_CFG_BUF_AREA_ADDR_L | 0xFFEF8000U | 0xFFE00000U | This value is set according to the RX65N ROM 2MB product. |
  |            | FWUP_CFG_AREA_SIZE | 0xF8000U | 0xF0000U | This value is set according to the RX65N ROM 2MB product. |

#### RL78/G23-128p FPB Bootloader Projects

  | FIT module | Config name | Default Value | Project value | Reason for change |
  |------------|-------------|---------------|---------------|-------------------|
  | r_bsp      | BSP_CFG_USER_CHARPUT_ENABLED | 0 | 1 | Use with log output function. |
  |            | BSP_CFG_CODE_FLASH_BANK_MODE | 1 | 0 | This project uses the Dual bank function. |
  | r_flash_rx | FLASH_CFG_CODE_FLASH_ENABLE | 0 | 1 | Bootloader rewrites the code flash. |
  |            | FLASH_CFG_CODE_FLASH_RUN_FROM_ROM | 0 | 1 | Bootloader is implemented to execute code that rewrites the code flash from another bank. |
  | r_sci_rx   | SCI_CFG_CH5_INCLUDED | 0 | 1 | SCI CH5 is used to write firmware and output log information. |
  | r_fwup     | FWUP_CFG_UPDATE_MODE | 1 | 0 | This project uses the Dual bank function. |
  |            | FWUP_CFG_MAIN_AREA_ADDR_L | 0xFFE00000U | 0xFFF00000U | This value is set according to the RX65N ROM 2MB product. |
  |            | FWUP_CFG_BUF_AREA_ADDR_L | 0xFFEF8000U | 0xFFE00000U | This value is set according to the RX65N ROM 2MB product. |
  |            | FWUP_CFG_AREA_SIZE | 0xF8000U | 0xF0000U | This value is set according to the RX65N ROM 2MB product. |

## Contribution

See [CONTRIBUTING](CONTRIBUTING.md) for more information.

## License

* Source code located in the *Projects*, *Common*, *Middleware/AWS*, and *Middleware/FreeRTOS* directories are available under the terms of the MIT License. See the LICENSE file for more details.
* Other libraries located in the *Middleware* directories are available under the terms specified in each source file.
* Each Renesas SIS, FIT, and other modules located in the *Projects/xxx/projects/xxx/src/smc_gen* and *Projects/xxx/modules* are available under the terms of the basically MIT License. See the doc/license of following URL for more details.
  * [https://github.com/renesas/rx-driver-package](https://github.com/renesas/rx-driver-package)

## Support

Visit the following official webpage if having any technical questions.

* [English window](https://en-support.renesas.com/dashboard)
* [Japanese window](https://ja-support.renesas.com/dashboard)
* [Chinese window](https://zh-support.renesas.com/dashboard)
