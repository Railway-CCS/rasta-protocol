# RaSTA - Getting started
In this tutorial, you will learn how to deploy and use the RaSTA C library.  
This tutorial is written for Linux. If you are using another operating system, the commands should be similar.  

## 1. Getting the library

Clone the repository.


To compile the library you can use the Gradle task `build`
```
gradle build
```

The library will be compiled and the unit tests executed. You should see
```
...
BUILD SUCCESSFUL 
```
after some time, which means the library and all examples have been compiled and are ready to use.  
The shared and static library files are located at `build/libs/rasta`

You can also compile the binaries with

```
mkdir -p build
cd build
cmake ..
make
```

## 2. Using the examples
The RaSTA C library comes with some simple example to show the use of the library.
The following examples are included

- **redundancy_test:** a simple forwarding example using only the redundancy layer. A client sends a message to a server which forwards the received message to another client.
- **rasta_example_new:** a simple forwarding example using the complete RaSTA protocol. A client sends a message to a server which forwards the received message to another client.
- **rasta_example:** an example for communication between a client and a server. (This example uses the old version of the RaSTA implementation)
- as well as some miscellaneous examples to test specific modules that are used in the RaSTA library. 

See [docker.md](docker.md) for instructions on the examples.

## 3. Using the library
You learned to compile the sources and run the example programs, now it's time to write your own program!
In this section the various functions and some other things are listed and explained.

#### Functions
The complete functionality of the library can be used by including `rasta_new.h`.

| Name                      | Description                                                                                                                                                                                                                            |
| ------------------------- | ---------------------------------------------------------                                                                                                                                                                              |
| `sr_init_handle`          | initializes the RaSTA handler, starts threads, loads configuration from config file, etc.                                                                                                                                              |
| `sr_init_handle_manually` | initializest the RaSTA handler without a configuration file. The needed parameters have to be passed to the function instead of being read from a file.                                                                                |
| `sr_connect`              | connects to another RaSTA entity. You have to pass the ID of the remote entity and the transport channel as well as an initialized handler as parameters                                                                               |
| `sr_send`                 | sends a message to a connected entity (connect with `sr_connect`) with the passed ID                                                                                                                                                   |
| `sr_get_received_data`    | gets the first message (i.e. the application message that arrived first in regard to time and order in the RaSTA PDU) from the receive buffer. If the buffer is empty, this call will block until an application message is available. |
| `sr_disconnect`           | sends a disconnection request to the connected entity with the passed ID and closes the RaSTA connection.                                                                                                                              |
| `sr_cleanup`              | cleans up allocated ressources, stops the threads, etc. Call this at the end of you program to avoid memory leak and some other problems (see *Further Information*)                                                                   |

#### Notifications
The notifications are an easy way to react to events that occur during the protocol flow. Notifications are basically function pointers which you can set. The functions will be called when the respective event occurs. The notification functions have to be assigned in an initialized handle (`handle.notifications`).

This is a list of all available notifications.

| Name                                    | Event                                                                                                               |
| --------------------------------------- | ------------------------------------------------------------------------------------------------------------------- |
| `on_receive`                            | an application message has been received an is available in the buffer                                              |
| `on_disconnection_request_received`     | a disconnection request has been received and the connection is closed. You can access the reason and details here! |
| `on_connection_state_change`            | the state of the connection has changed                                                                             |
| `on_diagnostic_notification`            | diagnose data of the send-/retransmission layer is available                                                        |
| `on_redundancy_diagnostic_notification` | diagnose data of the redundancy layer is available                                                                  |

#### Configuration
In general the configuration can be specified in a configuration file. In the configuration file the RaSTA protocol paramters as well as some miscellaneous options like logging. Every option is documented in the example config files and their meaning should be easy understandable. The only one that is a bit more tricky is *RASTA_REDUNDANCY_CONNECTIONS*.  
This option is used to specify the network interfaces and ports where the RaSTA entity will listen on. The format is an array of Strings with format `IP:Port` where the IP corresponds to the IP address a network interface is bound to. If you, for whatever reason, want to listen on any interface, use `0.0.0.0` as the IP.  
Note that the send-behaviour in this case might not work as you expect (which interface sends the PDUs)!

## 4. Further Information
#### Buffer sizes greater than 10 
The library uses POSIX mqueues as FIFO buffers for receiving, sending, etc.  
By OS default the maximum number of messages in a mqueue is 10. The size of the receive buffer according to the SCI protocols has to be 20, so if you try to initialize a RaSTA entity with receive buffer size 20, it won't work out of the box. You need to set the maximum message count of a mqueue to a higher value in the file `/proc/sys/fs/mqueue/msg_max`  
To set the size to i.e. 20 you can use the following command

```
echo "20" > /proc/sys/fs/mqueue/msg_max
```

#### Problem: could not create mqueue
Due to OS limitations, only a rather small amount of mqueues can be created. This might lead to a problem, where a program exits with the error message  *"Could not create mqueue"*. If you tested your program a few times without calling `sr_cleanup` (which frees the allocated mqueue) the solution is to restart your computer / VM. After the reboot it should work again.  
If Another way to solve the issue is to increase the maximum amount of mqueues that are allowed on the system. In order to do change the amout edit the file `/proc/sys/fs/mqueue/queues_max` 

#### Network interface IP by interface name
If you want to get a network interfaces associated IP address by its name (e.g. `eth0`), for example because the IP is assigned dynamically with DHCP, have a look at the system function `getifaddrs` from `ifaddrs.h`. See the [Manpage](http://man7.org/linux/man-pages/man3/getifaddrs.3.html)  for more information.  
However, you can't use the configuration file in this case. Use the manual configuration instead.

