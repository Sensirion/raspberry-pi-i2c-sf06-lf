# Sensirion Raspberry Pi I²C SF06-LF Driver

The repository provides a driver for setting up a sensor of the SF06-LF family to run on a Raspberry Pi over I²C.

<img src="images/sensor_SLF3C_1300F.png" width="300px">

Click [here](https://sensirion.com/products/product-categories/liquid-flow/) to learn more about the Sensirion SF06-LF sensor family.


Not all sensors of this driver family support all measurements.
In case a measurement is not supported by all sensors, the products that
support it are listed in the API description.



## Supported sensor types

| Sensor name   | I²C Addresses  |
| ------------- | -------------- |
|[SLF3C-1300F](https://sensirion.com/products/catalog/SLF3C-1300F/)| **0x08**|
|[SLF3S-1300F](https://sensirion.com/products/catalog/SLF3S-1300F/)| **0x08**|
|[SLF3S-0600F](https://sensirion.com/products/catalog/SLF3S-0600F/)| **0x08**|
|[SLF3S-4000B](https://sensirion.com/products/catalog/SLF3S-4000B/)| **0x08**|
|[LD20-0600L](https://sensirion.com/products/catalog/LD20-0600L/)| **0x08**|
|[LD20-2600B](https://sensirion.com/products/catalog/LD20-2600B/)| **0x08**|

The following instructions and examples use a *SLF3C-1300F*.



## Connect the sensor

Your sensor has 4 different connectors: SDA, VDD, GND, SCL.
Use the following pins to connect your SF06-LF:

| *SF06-LF* | *Cable Color*  |   *Raspberry Pi*   |
| :----------------: | -------------- | ------------------ |
| SDA | green | Pin 3
| VDD | red | Pin 1
| GND | black | Pin 6
| SCL | yellow | Pin 5


<img src="images/raspi-i2c-pinout-3.3V.png" width="400px">


### Detailed sensor pinout

<img src="images/SLF3x_Pinout.png" width="300px">

| *Pin* | *Cable Color* | *Name* | *Description*  | *Comments* |
|-------|---------------|:------:|----------------|------------|
| 1 |  | NC | Do not connect | 
| 2 | green | SDA | I2C: Serial data input / output | 
| 3 | red | VDD | Supply Voltage | 3.2V to 3.8V
| 4 | black | GND | Ground | 
| 5 | yellow | SCL | I2C: Serial clock input | 
| 6 |  | NC | Do not connect | 



## Quick start example

- [Install the Raspberry Pi OS on to your Raspberry Pi](https://projects.raspberrypi.org/en/projects/raspberry-pi-setting-up)
- [Enable the I²C interface in the raspi-config](https://www.raspberrypi.org/documentation/configuration/raspi-config.md)
- Download the SF06-LF driver from [Github](https://github.com/Sensirion/raspberry-pi-i2c-sf06-lf) and extract the `.zip` on your Raspberry Pi
- Connect the SF06-LF sensor as explained in the [section above](#connect-the-sensor)
- The provided example is working with a SLF3C-1300F, I²C address 0x08.
  In order to use the code with another product or I²C address you need to change it in the call sf06_lf_init(ADDRESS) in
  `sf06_lf_i2c_example_usage.c`. The list of supported I²C-addresses is found in the header 
  `sf06_lf_i2c.h`.

- Compile the driver
    1. Open a [terminal](https://projects.raspberrypi.org/en/projects/raspberry-pi-using/8)
    2. Navigate to the driver directory. E.g. `cd ~/raspberry-pi-i2c-sf06_lf`
    3. Navigate to the subdirectory example-usage.
    4. Run the `make` command to compile the driver

       Output:
       ```
       rm -f sf06_lf_i2c_example_usage
       cc -Os -Wall -fstrict-aliasing -Wstrict-aliasing=1 -Wsign-conversion -fPIC -I. -o sf06_lf_i2c_example_usage  sf06_lf_i2c.h sf06_lf_i2c.c sensirion_i2c_hal.h sensirion_i2c.h sensirion_i2c.c \
           sensirion_i2c_hal.c sensirion_config.h sensirion_common.h sensirion_common.c sf06_lf_i2c_example_usage.c
       ```
- Test your connected sensor
    - Run `./sf06_lf_i2c_example_usage` in the same directory you used to
      compile the driver. You should see the measurement values in the console.

## Troubleshooting

### Building driver failed

If the execution of `make` in the compilation step 3 fails with something like

```bash
 make: command not found
```

your RaspberryPi likely does not have the build tools installed. Proceed as follows:

```
$ sudo apt-get update
$ sudo apt-get upgrade
$ sudo apt-get install build-essential
```


### Initialization failed

If you run `./sf06_lf_i2c_example_usage` but do not get sensor readings but something like this instead

```
Error executing stop_continuous_measurement(): -1
Error executing read_product_identifier(): -1
Error executing start_h2o_continuous_measurement(): -1
...
```
then go through the below troubleshooting steps.


-   Ensure that you connected the sensor correctly: All cables are fully
    plugged in and connected to the correct pin.
-   Ensure that I²C is enabled on the Raspberry Pi. For this redo the steps on
    "Enable the I²C interface in the raspi-config" in the guide above.
-   Ensure that your user account has read and write access to the I²C device.
    If it only works with user root (`sudo ./sf06_lf_i2c_example_usage`), it's
    typically due to wrong permission settings. See the next chapter how to solve this.

### Missing I²C permissions

If your user is missing access to the I²C interface you should first verfiy
the user belongs to the `i2c` group.

```
$ groups
users input some other groups etc
```
If `i2c` is missing in the list add the user and restart the Raspberry Pi.

```
$ sudo adduser your-user i2c
Adding user `your-user' to group `i2c' ...
Adding user your-user to group i2c
Done.
$ sudo reboot
```

If that did not help you can make globally accessible hardware interfaces
with a udev rule. Only do this if everything else failed and you are
reasoably confident you are the only one having access to your Pi.

Go into the `/etc/udev/rules.d` folder and add a new file named
`local.rules`.
```
$ cd /etc/udev/rules.d/
$ sudo touch local.rules
```
Then add a single line `ACTION=="add", KERNEL=="i2c-[0-1]*", MODE="0666"`
to the file with your favorite editor.
```
$ sudo vi local.rules
```

## Contributing

**Contributions are welcome!**

We develop and test this driver using our company internal tools (version
control, continuous integration, code review etc.) and automatically
synchronize the master branch with GitHub. But this doesn't mean that we don't
respond to issues or don't accept pull requests on GitHub. In fact, you're very
welcome to open issues or create pull requests :)

This Sensirion library uses
[`clang-format`](https://releases.llvm.org/download.html) to standardize the
formatting of all our `.c` and `.h` files. Make sure your contributions are
formatted accordingly:

The `-i` flag will apply the format changes to the files listed.

```bash
clang-format -i *.c *.h
```

Note that differences from this formatting will result in a failed build until
they are fixed.


## License

See [LICENSE](LICENSE).