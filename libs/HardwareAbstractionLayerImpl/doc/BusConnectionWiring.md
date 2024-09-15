# Bus Connection Wiring

## Universal Sub-D 9 Pinout

![Sub-D Plug](images/SubDPlug9.png)

The ribbon cable coloring is as follows.

| Pin Number     | Cable Color     |
|----------------|-----------|
| 1              | <p style="background:black;color:white">black</p> |
| 2              | <p style="background:grey;color:white">grey</p> |
| 3              | <p style="background:blue;color:white">blue</p> |
| 4              | <p style="background:yellow;color:black">yellow</p> |
| 5              | <p style="background:red">red</p> |
| 6              | <p style="background:white;color:black">white</p> |
| 7              | <p style="background:violet;color:black">violet</p> |
| 8              | <p style="background:lightgreen;color:black">green</p> |
| 9              | <p style="background:orange;color:black">orange</p> |

## Motor Control Connection

For motor control use a sub-d plug with ribbon cable is used to transport the signals.

| Pin                                            | Signal      | Description             | GPIO pin (Raspberry Pi)  |
|------------------------------------------------|-------------|-------------------------|--------------------------|
| <p style="background:black;color:white">1</p>  | GND         | Ground                  | 39                       |
| <p style="background:grey;color:white">2</p>   | SDA         | I2C SDA Signal (3.3V)   | 03                       |
| <p style="background:blue;color:white">3</p>   | ERR         | Error In Signal (3.3V)  | 33                       |
| <p style="background:yellow;color:black">4</p> | RST         | Reset Signal (3.3V)     | 35                       |
| <p style="background:red;color:black">5</p>    | POWER_3_3V  | power supply (+3.3V)    | 01                       |
| <p style="background:white;color:black">6</p>  | SDL         | I2C SDL Signal (3.3V)   | 05                       |
| 7    | -           | -                  | -                        |
| 8    | -           | -                  | -                        |
| 9    | -           | -                  | -                        |
