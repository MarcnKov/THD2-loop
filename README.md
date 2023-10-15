# THD2 loop

THD2 loop is a project conducted during M2 OSAE by Marcin Kovalevskij and David Picard within the SP3 module.

The main goal of this project is to develop a closed loop system that corrects a beam pointing error, that is caused by the vibrations on the order of 2 kHz, on the THD2 bench. 

You can find a detailed description of this project in the **Other/Documentation** section.

In summary, the correction loop consists of four elements, camera, processing unit (computer), digital-to-analog converter and a tip-tilt correcting mirror. During the timeframe of this project we mainly developed the central control unit that allows to visualize the acquired PSF and to control the camera and loop parameters. 

## Set up the hardware

The camera Ethernet port should be configured first. The instructions to configure the Ethernet port are found in **Other/Documentation/Mako_Ethernet_configuration_linux.pdf** file. 

## Set up the software

The software consists of acquisition and visualziation programs.

Firtsly, you have to set up the acquisition program which is written in the C programming language.

Navigate to the project root folder, open the Linux terminal and execute the following :

```shell
chmod +x setup_vimba.sh
./setup_vimba.sh
```

The  **setup_vimba.sh** script configures the USB and GigETL settings so that the camera can work directly from the USB port.

> **Warning** <br>
> After a predefined OS timeout the OS automatically resets the USB and GigETL parameters. When this happens, you won't be able to start the camera and you'll get the message : "Could not open camera. Error code ". To solve this you will have to repeat the vimba set up procedure.

Secondly, you have to set up the visualization program which is written in the Python3 programming language.

Navigate to the project root folder, open the Linux terminal and execute the following :

```shell
chmod +x setup_visualization.sh
./setup_visualization.sh
```

The  **setup_visualization.sh** script downloads and installs necessary  Python3  dependencies.

> **Note** <br>
>
> Having configured your computer by following the steps above and executing the *./run.sh* command below if you get the following error :
>
> *qt.qpa.plugin: Could not load the Qt platform plugin ...* 
>
> You can solve the problem by installing the following package: `sudo apt install libxcb-xinerama0 `

## Run the software

Connect the Mako camera to the USB port, navigate to the project root folder, open the Linux terminal and execute the following :

```shell
chmod +x run.sh
./run.sh
```

The  **run.sh** script starts acquisition and visualization programs.

## TO DO

1. The default frame size is 40x40.  GUI set up frame width and height buttons are developed and connected in visualization.py, also this functionality is <u>almost fully</u> developed in SynchronousGrab.c program. If one needs to develop a dynamic change of a frame width and height during the acquisition process, one should uncomment the lines : 348, 362, 366, 374, 423 in the SynchronousGrab.c program and comment the line 422, uncomment the line 31 and comment the line 30 in the msg_queue.h file.

## Vimba error messages

| ERROR_CODE | ERROR_MESSAGE                                                |
| ---------- | ------------------------------------------------------------ |
| 0          | No error                                                     |
| -1         | Unexpected fault in VimbaC or driver                         |
| -2         | VmbStartup() was not called before the current command       |
| -3         | The designated instance (camera, feature etc.) cannot be found |
| -4         | The given handle is not valid                                |
| -5         | Device was not opened for usage                              |
| -6         | Operation is invalid with the current access mode            |
| -7         | One of the parameters is invalid (usually an illegal pointer) |
| -8         | The given struct size is not valid for this version of the API |
| -9         | More data available in a string/list than space is provided  |
| -10        | Wrong feature type for this access function                  |
| -11        | The value is not valid; either out of bounds or not an increment of the minimum |
| -12        | Timeout during wait                                          |
| -13        | Other error                                                  |
| -14        | Resources not available (e.g. memory)                        |
| -15        | Call is invalid in the current context (e.g. callback)       |
| -16        | No transport layers are found                                |
| -17        | API feature is not implemented                               |
| -18        | API feature is not supported                                 |
| -19        | The current operation was not completed (e.g. a multiple registers read or write) |
| -20        | Low level IO error in transport layer                        |



## Explication sur le travail de COG :

1) Plusieurs images de tests sont dans PSF_images  
2) Images converties en datastream en utilisant image_to_stream.py en attendant le vrai datastream  
3) main.c contient le calcul COG, Gain, Delta => alerte si delta trop grand, la boucle doit se stopper  



