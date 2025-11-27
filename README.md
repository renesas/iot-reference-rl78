# FreeRTOS RL78 Family IoT Reference

## Introduction

This product provides the reference of IoT solution with using [RL78 family](https://www.renesas.com/products/microcontrollers-microprocessors/rl78-low-power-8-16-bit-mcus), [AWS](https://aws.amazon.com), and [FreeRTOS](https://www.freertos.org/RTOS.html). The RL78 family is a kind of MCUs provided by [Renesas](https://www.renesas.com).
You can easily try to run AWS IoT demos using RL78 family while working with our various other products.
Refer to the [Getting Started Guide](#user-documents) for step by step instructions on setting up your development environment and running demos.

## Product Specifications

This reference is consist of demo applications, FreeRTOS kernel, middleware provided by AWS and 3rd party, middleware and drivers for RL78 family provided as the Software Integration System by Renesas, files to collaborate Renesas tools such as the IDE [e2 studio](https://www.renesas.com/software-tool/e-studio), and etc.
Summary of specifications explains in the following chapters.

### Using AWS Services

* [AWS IoT core](https://aws.amazon.com/iot-core/)

### Demos

This reference consits the following demo projects:

* Demo project (PubSub)
  * Perform simple data upload via MQTT communication.
* Demo project (OTA)
  * Perform firmware update using AWS

The preceding demos use the following technical elements of the AWS IoT:

* [AWS IoT Jobs](https://docs.aws.amazon.com/iot/latest/developerguide/iot-jobs.html)
* [MQTT File Delivery](https://docs.aws.amazon.com/iot/latest/developerguide/mqtt-based-file-delivery.html)

### Supported Boards

This reference supports the following boards:

* [RL78/G23-128p Fast Prototyping Board(RL78/G23-128p FPB)](https://www.renesas.com/products/microcontrollers-microprocessors/rl78-low-power-8-16-bit-mcus/rtk7rlg230csn000bj-rl78g23-128p-fast-prototyping-board-rl78g23-128p-fpb) with:
  * Wi-Fi
    * [DA16600MOD Ultra-Low-Power Wi-Fi + Bluetooth Low Energy Combo Pmod Board](https://www.renesas.com/en/products/wireless-connectivity/wi-fi/low-power-wi-fi/us159-da16600evz-ultra-low-power-wi-fi-bluetooth-low-energy-combo-pmod-board#overview)
### User Documents

The documents for this reference are followings:

* Getting Started Guide
  * For RL78/G23-128p Fast Prototyping Board with Wi-Fi (DA16600)
      * [Getting Started Guide for Connecting Amazon Web Services (202406-LTS Version) in Wi-Fi Communication: RL78/G23-128p FPB + FreeRTOS](https://www.renesas.com/en/document/apn/rl78g23-getting-started-guide-connecting-amazon-web-services-202406-lts-version-wi-fi-communication?r=1616891)

### Open Source Software (OSS) Components

The following table indicates name and version of OSS which are used in this reference. The column *LTS Until* indicates the support period as LTS. The column *Use* indicates whether the OSS is performed in demo projects or not.

| Library                     | Version             | LTS Until  | LTS Repo URL                                                                | Use |
|-------------------------    |---------------------|------------|---------------------------------------------------------------------------|-----|
| FreeRTOS Cellular Interface              | 1.4.0              | 06/30/2026 | <https://github.com/FreeRTOS/FreeRTOS-Cellular-Interface>                        | No |
| FreeRTOS Kernel             | 11.1.0              | 06/30/2026 | <https://github.com/FreeRTOS/FreeRTOS-Kernel>                        | Yes |
| FreeRTOS-Plus-TCP             | 4.2.5              | 06/30/2026 | <https://github.com/FreeRTOS/FreeRTOS-Kernel>                        | No  |
| backoffAlgorithm             | 1.4.1              | 06/30/2026 | <https://github.com/FreeRTOS/backoffAlgorithm>                        | Yes |
| coreHTTP Client             | 3.1.1              | 06/30/2026 | <https://github.com/FreeRTOS/coreHTTP>                        | No  |
| coreJSON             | 3.3.0              | 06/30/2026 | <https://github.com/FreeRTOS/coreJSON>                        | Yes |
| coreMQTT Client             | 2.3.1              | 06/30/2026 | <https://github.com/FreeRTOS/coreMQTT>                        | Yes |
| coreMQTT Agent             | 1.3.1              | 06/30/2026 | <https://github.com/FreeRTOS/coreMQTT-Agent>                        | Yes |
| corePKCS11             | 3.6.3              | 06/30/2026 | <https://github.com/FreeRTOS/corePKCS11>                        | No  |
| coreSNTP             | 1.3.1              | 06/30/2026 | <https://github.com/FreeRTOS/coreSNTP>                        | No  |
| AWS IoT Device Defender             | 1.4.0              | 06/30/2026 | <https://github.com/aws/Device-Defender-for-AWS-IoT-embedded-sdk>                        | No  |
| AWS IoT Device Shadow             | 1.4.1              | 06/30/2026 | <https://github.com/aws/Device-Shadow-for-AWS-IoT-embedded-sdk>                        | No  |
| AWS IoT Fleet Provisioning             | 1.2.1              | 06/30/2026 | <https://github.com/aws/Fleet-Provisioning-for-AWS-IoT-embedded-sdk>                        | No  |
| AWS IoT Jobs              | 1.5.1              | 06/30/2026 | <https://github.com/aws/Jobs-for-AWS-IoT-embedded-sdk>                        | Yes  |
| AWS SigV4             | 1.3.0              | 06/30/2026 | <https://github.com/aws/SigV4-for-AWS-IoT-embedded-sdk>                        | No  |
| AWS IoT MQTT File Streams             | 1.1.0              | 06/30/2026 | <https://github.com/aws/ota-for-aws-iot-embedded-sdk>                        | Yes |
| tinycbor             | 0.5.2              | -- | <https://github.com/intel/tinycbor>                        | Yes |
| FreeRTOS-Plus network_transport             | --              | -- | <https://www.freertos.org/network-interface.html>                        | Yes |
| Logging Interface             | 1.1.3              | -- | <https://github.com/aws/amazon-freertos/tree/main/libraries/logging>                        | Yes |
| tinycrypt             | 0.2.8              | -- | <https://github.com/intel/tinycrypt>                        | Yes |

### Renesas Driver Components

The following table indicates drivers provided by Renesas, that are Software Integration System (SIS), Firmware Integration Technology (FIT) , and CG (Code Generation) Modules.

| Component | Revision | Component Type | URL |
|------------|---------|---------|---------|
|r_bsp|1.90|SIS module|[Board Support Package Module Using Software Integration System](https://www.renesas.com/document/apn/rl78-family-board-support-package-module-using-software-integration-system)|
|r_byteq|2.10|FIT module|[BYTEQ Module Using Firmware Integration Technology](https://www.renesas.com/document/apn/byteq-module-using-firmware-integration-technology)|
|Config_UART|1.9.0|CG module|-|
|Config_PORT|1.7.0|CG module|-|
|r_fwup|2.04|-|[RL78/G22, RL78/G23, RL78/G24, RL78/L23 Firmware Update Module](https://www.renesas.com/en/document/apn/rl78g22-rl78g23-rl78g24-rl78l23-firmware-update-module?language=en&queryID=6a78c5ea3b7d3ab06d57579c48418669)|
|Renesas Flash Driver RL78 Type01|1.20|-|[Renesas Flash Driver RL78 Type01](https://www.renesas.com/document/man/renesas-flash-driver-rl78-type-01-users-manual-rl78g23)|

### SubModule Patches

The following submodules have been modified by Renesas.
These changes will be saved in the `\Common\patches` folder and will be incorporated into the project until they are merged into the FreeRTOS LTS.

* Jobs-for-AWS-IoT-embedded-sdk
  * It will be stored in the following folder.
    `\Common\patches\Jobs-for-AWS-IoT-embedded-sdk`
  * The AWS IoT Jobs packaged in LTS_v202406.04 lacks "custom version string" support. So as a temporary workaround, we have modified the Jobs_UpdateMsg API to allow the custom version to be used.

* aws-iot-core-mqtt-file-streams-embedded-c
  * It will be stored in the fowllowing folder.
    `\Common\patches\aws-iot-core-mqtt-file-streams-embedded-c`
  * The aws-iot-core-mqtt-file-streams-embedded-c packaged in LTS_v202406.04 lacks support for 16-bit MCUs. So as a temporary workaround, we have modified the mqttDownloader_createGetDataBlockRequest and handleJsonMessage APIs to ensure they function correctly.

### Configuration values changed from the default in the SIS and FIT Modules

* The configuration values of the SIS and FIT modules that have been changed from the default values are listed in the table below.
* However, each projects is evaluated only with preset values, including configuration values that have not been changed from the default values.
* If changed, the program may not work properly.

<details>
<summary>RL78/G23-128p FPB Wi-Fi - DA16600 Projects</summary>

  | Component | Config name | Default Value | Project value | Reason for change |
  |-----------|-------------|---------------|---------------|-------------------|
  | r_bsp     | BSP_CFG_MCU_PART_ROM_SIZE | 0x3 | 0x6 | This value is set according to the RL78/G23-128p product. |
  |           | BSP_CFG_CONFIGURATOR_SELECT | 0 | 1 | Because enable initialization of peripheral functions by Code Generator/Smart Configurator. |
  |           | BSP_CFG_SOFTWARE_DELAY_API_FUNCTIONS_DISABLE | 1 | 0 | To reduce using ROM/RAM. |
  |           | BSP_CFG_HISYSCLK_SOURCE | 1 | 0 | Clock setting. |
  |           | BSP_CFG_HISYSCLK_OPERATION | 0 | 1 | Clock setting. |
  |           | BSP_CFG_MOCO_SOURCE | 1 | 0 | Clock setting. |
  |           | BSP_CFG_ALLOW_FSUB_IN_STOPHALT | 1 | 0 | Clock setting. |
  |           | BSP_CFG_RTC_OUT_CLK_SOURCE | 1 | 0 | Clock setting. |
  |           | BSP_CFG_WAKEUP_MODE | 1 | 0 | Clock setting. |
  |           | BSP_CFG_RTOS_USED | 0 | 1 | Because using FreeRTOS. |
  | r_byteq   | BYTEQ_CFG_PARAM_CHECKING_ENABLE | BSP_CFG_PARAM_CHECKING_ENABLE | 0 | To reduce using ROM/RAM. |
  |           | BYTEQ_CFG_MAX_CTRL_BLKS | 32 | 8 | To reduce using RAM. |
  | r_fwup    | FWUP_CFG_FUNCTION_MODE | 0 | 1 | This project is user program. |
  |           | FWUP_CFG_MAIN_AREA_ADDR_L | 0xFFFC0000U | 0x01000U | This value is set according to the RL78/G23-128p product. |
  |           | FWUP_CFG_BUF_AREA_ADDR_L | 0xFFF80000U | 0x59000U | This value is set according to the RL78/G23-128p product. |
  |           | FWUP_CFG_AREA_SIZE | 0x34000U | 0x58000U | This value is set according to the RL78/G23-128p product. |
  |           | FWUP_CFG_CF_BLK_SIZE | 0x4000U | 2048U | This value is set according to the RL78/G23-128p product. |
  |           | FWUP_CFG_DF_ADDR_L | 0x00100000 | 0xF1000U | This value is set according to the RL78/G23-128p product. |
  |           | FWUP_CFG_DF_BLK_SIZE | 64U | 256U | This value is set according to the RL78/G23-128p product. |
  |           | FWUP_CFG_DF_NUM_BLKS | 256U | 32U | This value is set according to the RL78/G23-128p product. |
</details>

<details>
<summary>RL78/G23-128p FPB Bootloader Projects</summary>

  | Component | Config name | Default Value | Project value | Reason for change |
  |-----------|-------------|---------------|---------------|-------------------|
  | r_bsp     | BSP_CFG_MCU_PART_ROM_SIZE | 0x3 | 0x6 | This value is set according to the RL78/G23-128p product. |
  |           | BSP_CFG_CONFIGURATOR_SELECT | 0 | 1 | Because enable initialization of peripheral functions by Code Generator/Smart Configurator. |
  |           | BSP_CFG_HISYSCLK_SOURCE | 1 | 0 | Clock setting. |
  |           | BSP_CFG_HISYSCLK_OPERATION | 0 | 1 | Clock setting. |
  |           | BSP_CFG_MOCO_SOURCE | 1 | 0 | Clock setting. |
  |           | BSP_CFG_ALLOW_FSUB_IN_STOPHALT | 1 | 0 | Clock setting. |
  |           | BSP_CFG_RTC_OUT_CLK_SOURCE | 1 | 0 | Clock setting. |
  |           | BSP_CFG_WAKEUP_MODE | 1 | 0 | Clock setting. |
  | r_fwup    | FWUP_CFG_FUNCTION_MODE | 0 | 0 | This project is bootloader. |
  |           | FWUP_CFG_MAIN_AREA_ADDR_L | 0xFFFC0000U | 0x01000U | This value is set according to the RL78/G23-128p product. |
  |           | FWUP_CFG_BUF_AREA_ADDR_L | 0xFFF80000U | 0x59000U | This value is set according to the RL78/G23-128p product. |
  |           | FWUP_CFG_AREA_SIZE | 0x34000U | 0x58000U | This value is set according to the RL78/G23-128p product. |
  |           | FWUP_CFG_CF_BLK_SIZE | 0x4000U | 2048U | This value is set according to the RL78/G23-128p product. |
  |           | FWUP_CFG_DF_ADDR_L | 0x00100000 | 0xF1000U | This value is set according to the RL78/G23-128p product. |
  |           | FWUP_CFG_DF_BLK_SIZE | 64U | 256U | This value is set according to the RL78/G23-128p product. |
  |           | FWUP_CFG_DF_NUM_BLKS | 256U | 32U | This value is set according to the RL78/G23-128p product. |

</details>

## Limitation

<details>
<summary>Click here to check Limitation</summary>

* Notes on bootloader to application transition. When transitioning from the bootloader sample program to the application, the settings of the bootloader's peripheral functions are taken over by the application. For more information, check chapter 7 of the following document. [RL78/G22, RL78/G23, RL78/G24, RL78/L23 Firmware Update Module](https://www.renesas.com/en/document/apn/rl78g22-rl78g23-rl78g24-rl78l23-firmware-update-module)
* The OTA demo in the current implementation does not support rollback mechanisms because it lacks support for the dual‑bank method. If `otaconfigAllowDowngrade` is set to any value other than 1, a build error will occur.
* OTA demo will not work properly unless `mqttFileDownloader_MAX_NUM_BLOCKS_REQUEST` is set to "1". If `mqttFileDownloader_MAX_NUM_BLOCKS_REQUEST` is set to anything other than 1, a build error will occur.
* The current implementation of "custom standard printf function" is not thread-safe, it may cause the log output to look messy or jumbled. Please disable and avoid using the stdlib `printf` function in the application code. Please use the FreeRTOS printf function `configPRINTF` instead.

</details>

## Contribution

See [CONTRIBUTING](CONTRIBUTING.md) for more information.

## License

* Source code located in the *Projects*, *Common*, *Middleware/AWS*, and *Middleware/FreeRTOS* directories are available under the terms of the MIT License. See the LICENSE file for more details.
* Other libraries located in the *Middleware* directories are available under the terms specified in each source file.
* Each Renesas SIS, FIT, and other modules located in the *Projects/xxx/projects/xxx/src/smc_gen* and *Projects/xxx/modules* are available under the disclaimer written in source or header files.
  * License of FIT modules: [https://github.com/renesas/rx-driver-package](https://github.com/renesas/rx-driver-package/blob/master/README.md#license)

## Support

Visit the following official webpage if having any technical questions.

* [English window](https://en-support.renesas.com/dashboard)
* [Japanese window](https://ja-support.renesas.com/dashboard)
* [Chinese window](https://zh-support.renesas.com/dashboard)
