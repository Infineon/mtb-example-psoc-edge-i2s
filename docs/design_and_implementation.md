[Click here](../README.md) to view the README.

## Design and implementation

The design of this application is minimalistic to get started with code examples on PSOC&trade; Edge MCU devices. All PSOC&trade; Edge E84 MCU applications have a dual-CPU three-project structure to develop code for the CM33 and CM55 cores. The CM33 core has two separate projects for the secure processing environment (SPE) and non-secure processing environment (NSPE). A project folder consists of various subfolders, each denoting a specific aspect of the project. The three project folders are as follows:

**Table 1. Application projects**

Project | Description
--------|------------------------
*proj_cm33_s* | Project for CM33 secure processing environment (SPE)
*proj_cm33_ns* | Project for CM33 non-secure processing environment (NSPE)
*proj_cm55* | CM55 project

<br>

In this code example, at device reset, the secure boot process starts from the ROM boot with the secure enclave (SE) as the root of trust (RoT). From the secure enclave, the boot flow is passed on to the system CPU subsystem where the secure CM33 application starts. After all necessary secure configurations, the flow is passed on to the non-secure CM33 application. Resource initialization for this example is performed by this CM33 non-secure project. It configures the system clocks, pins, clock to peripheral connections, and other platform resources. It then enables the CM55 core using the `Cy_SysEnableCM55()` function and the CM55 core is subsequently put to DeepSleep mode.

In the CM33 non-secure application, the clocks and system resources are initialized by the BSP initialization function. The retarget-io middleware is configured to use the debug UART. The debug UART prints a message (as shown in [Terminal output on program startup](../images/terminal-i2s.png)) on the terminal emulator, the onboard KitProg3 acts the USB-UART bridge to create the virtual COM port. 

The `KIT_PSE84_EVAL` kit contains the audio codec TLV320DAC3100 and a loudspeaker that allows you to listen to any audio data stream transmitted to the audio codec over the I2S interface.

PSOC&trade; Edge MCU streams the data over I2S, which is implemented by storing a short audio clip in the flash memory and writing it to the Tx FIFO of the I2S hardware block. The *wave.h/c* files contain the audio data represented as a binary array. You can generate such a file by converting a WAV audio file to a C array. You can use the [bin2c](https://sourceforge.net/projects/bin2c/) tool.

The application-level abstraction APIs are used to initialize, enable, and activate I2S. On pressing the **User button 1**, an empty frame is written into the I2S Tx hardware FIFO. On a subsequent I2S interrupt, the audio stream of interest is written into Tx hardware FIFO until the end of the stream.

I2S hardware block of PSOC&trade; Edge MCU can provide MCLK. The MCLK provided by the I2S hardware block is the interface clock of the I2S hardware block. To achieve the desired sampling frequency required by DAC of TLV320DAC3100 codec, multiple dividers are provided internal to the codec. For supported sampling rates, the dividers values are automatically calculated in the initialization call based on the MCLK value. 

The code example contains an I2C controller through which PSOC&trade; MCU configures the audio codec.

The audio codec chip converts a digital audio stream into an analog audio stream. The audio codec is configured by PSOC&trade; Edge MCU over an I2C interface. The code example includes the **audio-codec-tlv320dac3100**, which provides the *mtb_tlv320dac3100.c/h* files that configures the audio codec used in this example.

**Figure 1. Flow diagram for the I2S application**

![](../images/flowchart.svg)

<br>