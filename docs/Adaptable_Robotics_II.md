



# **AMD Kria:tm: adaptable Robotics II – Unifying the communication protocol.**







## **Description**

In this example we will build and run Micro-ROS on two(2) of the MicroBlaze softcore CPUs. Micro-ROS will publish a message to an Agent running on a separate host, as seen in Figure #1 below. In this example we will continue to target the Kria KR260 board.


### **Platform Overview**

This tutorial builds on top of the other tutorial  [AMD Kria:tm: adaptable Robotics I – The right engine for the right task.](docs/Adaptable_Robotics_I.md).  
However, instead of printing to the PMOD, we will be running Micro-ROS client.

 
Following this tutorial, you should be able to put together a system containing the following -

* Ubuntu running on the A53 cluster

* MicroBlaze soft CPU's, running on demand in the FPGA fabric

* MicroBlaze controlled from Ubuntu via RemoteProc (start/stop/load)

* Micro-ROS running on MicroBlaze 

* R5 free to use



![Alt text](../docs/images/MicroROSOverview.png)



*Figure # 1 - A notional picture of the KR260 communicating to an external host. In this case, Micro-ROS running on MicroBlaze and communicating to an external host.*
<br>
<br>




This is what the final result should look like when you have gone through this tutorial.

<video width="640" height="480" controls autoplay loop muted>
  <source src="../docs/images/MicroROSonMicroBlaze.mp4" type="video/mp4">
</video>






https://github.com/user-attachments/assets/ae074b0e-7779-4779-8552-804b7e13d3c7





## **Building static Micro-ROS**



### **Description**:

The following information describes how to build static Micro-ROS images for the soft MicroBlaze CPU. The same concept can be done with the ARM Cortex R5 as well.

These instructions are based on eProsima's documentation, located here:

[micro-ROS for AMD Vitis](https://github.com/micro-ROS/micro_ros_vitis_component)

Follow the instructions in the link above to build the static Micro-ROS images for the soft MicroBlaze CPU.


### **Import Micro-ROS application for MicroBlaze**

The instructions above should result in static Micro-ROS libraries. Copy the static Micro-ROS files to the build system located here: *ros-dds-microblaze/KR260/examples/MB/lib/microroslibs/microros*

```
$ dir ros-dds-microblaze/KR260/examples/MB/lib/microroslibs/microros
drwxr-xr-x 45 xde xde 5 14:45 include
-rw-r--r--  1 xde xde 5 14:45 libmicroros.a
```

From Tutorial  [AMD Kria:tm: adaptable Robotics I – The right engine for the right task.](../docs/Adaptable_Robotics_I.md) the following should already be there:

```
$ dir ros-dds-microblaze/KR260/platforms/
drwxr-xr-x 4 tomast tomast 4096 Jan 16 09:56 xilinx_kr260_4mb_4pmod_202210_1
```
```
 dir ros-dds-microblaze/KR260/BSP/
total 12
drwxr-xr-x 3 tomast tomast 4096 Oct 17 11:56 embeddedsw
-rw-r--r-- 1 tomast tomast  836 Oct 17 09:50 Makefile
drwxr-xr-x 2 tomast tomast 4096 Oct 17 09:50 patch
```


Build the Micro-ROS applications. (Make sure you have sourced Vitis version 2022.1)

```
cd ros-dds-microblaze/KR260/examples/MB/Adaptable_Robotics_II
make example
```
*NOTE - The above examples are built with a fixed IP address. If you want to change the IP address, you can do so by modifying the files in the src directory. 
The following is fixed:
MB0_MicroROS is set to "192.168.2.170", and MB1_MicroROS is set to "192.168.2.160, and they both communicate to the MicroROS agent at "192.168.2.114" and they both use the "192.168.2.1" for the gateway IP. Please modify these IP addresses to reflect your own network configuration.*


After building the applications, the result are two projects, one for MicroBlaze #0 and one for MicroBlaze #1. As can be seen in the picture below. 

![Alt text](../docs/images/vitis_explorer.png)

Copy the two elf files to Ubuntu running on the KR260 board. I.e.

```
scp MicroROS_example_MB0.elf MicroROS_example_MB1.elf ubuntu@192.168.2.137:/tmp
```

On the target board (KR260) do the following:
```
cp /tmp/MicroROS_example_MB0.elf /lib/firmware
cp /tmp/MicroROS_example_MB1.elf /lib/firmware
```

To start Vitis on the workspace created, type the following:
```
vitis --workspace ./vitis_kr260_MB_ws
```

#### **Run the Micro-ROS applications**

Finally we want to load and run the MicroBlaze code on the softcores. To do that follow these instructions.

The MicroROS applications need a MicroROS server to connect to, please follow these instructions to run the MicroROS agent, and start the agent on a different x86 machine. https://hub.docker.com/r/microros/micro-ros-agent

```
docker run -it --rm --net=host microros/micro-ros-agent:humble udp4 -p 8888
```

Connect Ethernet PHY GEM2 and GEM3 and make sure they are connected to your local network. As mentioned earlier, this design is configured to use static IP addresses, so make sure you change them to your local network subnet and gateway.


Copy the below files needed to the development board, if not already there.
To get these files, the user will need to follow the tutorial - [AMD Kria:tm: adaptable Robotics I – The right engine for the right task.](../docs/Adaptable_Robotics_I.md)

* FPGA binary file - kr260_4mb_4pmod.bin
* Device tree overlay - kr260_4mb_4pmod.dtbo
* XMUTIL metadata - shell.json
* MicroBlaze kernel driver *.ko file. (should be on the target already)
* MicroBlaze executable

On the board, do the following to load the remoteproc feature. (replace with your filenames should they be different)

```
xmutil unloadapp
xmutil loadapp kr260_4mb_4pmod
insmod mb_remoteproc.ko
```

Load the MicroBlaze application #0

```
cp MicroROS_example_MB0.elf /lib/firmware
echo MicroROS_example_MB0.elf > /sys/class/remoteproc/remoteproc0/firmware
```

Start and stop the MicroBlaze application

```
echo start > /sys/class/remoteproc/remoteproc0/state
echo stop > /sys/class/remoteproc/remoteproc0/state
```

Load the MicroBlaze application #1

```
cp MicroROS_example_MB1.elf /lib/firmware
echo MicroROS_example_MB1.elf > /sys/class/remoteproc/remoteproc1/firmware
```

Start and stop the MicroBlaze application

```
echo start > /sys/class/remoteproc/remoteproc1/state
echo stop > /sys/class/remoteproc/remoteproc1/state
```



In the end, this is what it should look like. Hope you made it this far! Thanks for trying this out, I hope it will help in your future endeavors.

<video width="640" height="480" controls autoplay loop muted>
  <source src="../docs/images/MicroROSonMicroBlaze2.mp4" type="video/mp4">
</video>





https://github.com/user-attachments/assets/88853f34-03a0-4558-b22e-47ddeffa13a3





<br>
<br>
<br>

### **Physical hardware information**





### PMOD 1 - 4 address map

![](../docs/images/pmods.png)



### IO SWITCH

![](../docs/images/io_switch.png)



### GEM's SLOTS

![](../docs/images/gem_map.png)



### GEM's PHY SLOTS

![](../docs/images/gemsPHY.png)



### UART Connections example

![](../docs/images/uart.jpg)

### USB TO TTL example hardware

Connect RX and TX to the oposite on the PMOD.

```
USBTTL TX -> PMOD RX
USBTTL RX -> PMOD TX
USBTTL ground -> PMOD ground
USBTTL power 3.3v -> PMOD power 3.3v
```


![](../docs/images/usbToTTL.png)


