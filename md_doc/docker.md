# HowTo - Docker
Instead of using Gradle to test and compile the RaSTA library and examples, Docker can be used instead.

There are, however, some limitations compared to using Gradle as the Docker image uses CMake as the build tool
- CUnit Test are not executed
- RaSTA Wrapper for JNI is not compiled

### Dockerfile
The Dockerfile in the root of the repository is used builds the library and the SCI-LS, SCI-P, and RaSTA example.
Each of them with configuration for running on localhost and on different network nodes.

For the configuration the can be executed on systems in a network, the IPs for the server and client(s) can be specified
to the Docker build command using `--build-arg`. The following build args are supported. All of them are of type String.

| Build Arg Name | Description                                                                                      | 
|----------------|--------------------------------------------------------------------------------------------------|
| SERVER_CH1     | IP address of the the first redundancy channel of the server                                     |
| SERVER_CH2     | IP address of the the second redundancy channel of the server                                    |
| CLIENT1_CH1    | IP address of the the first redundancy channel of client 1 (the client used in SCI examples)     |
| CLIENT1_CH2    | IP address of the the second redundancy channel of client 1 (the client used in SCI examples)    |
| CLIENT2_CH1    | IP address of the the first redundancy channel of client 2 (only used in RaSTA example)          |
| CLIENT2_CH2    | IP address of the the second redundancy channel of client 2 (only used in RaSTA example)         |

In the container the binaries are located in `/opt/rasta-c/build/bin/exe/examples`  
In the container the configs are located in `/opt/rasta-c/build/`

### Running the Examples
The Docker image contains the RaSTA library as well as all examples including their configs.
They can be executed manually once a container is created, e.g.

```shell script
>>> docker run -it [IMAGE NAME] /bin/sh
/ # cd /opt/rasta-c/build/bin/exe/examples
/ # ./rasta_example_localhost r
``` 

#### Helper Script
Instead of building the image and running the examples manually , the `rasta-docker.sh` script can be used.
It allows to build the images and run the examples using simple commands.

Build image and create containers for server, client1, client2
```shell script
>>> ./rasta-docker.sh build
```
This will also setup a network between the containers and configure the IPs for the examples so they can communicate.
Note that running this command will delete the containers and recreate them if they already exist.


Run examples by
```shell script
>>> ./rasta-docker.sh CONTAINER [EXAMPLE]
```
where `CONTAINER` can be either `server`, `client1`, or `client2`.
`EXAMPLE` specifies the example run. Possible options are `scils`, `scip`, or `rasta`.
Note that `client2` does only support `rasta` as `client1` is using in the SCI examples.

When `EXAMPLE` is not specified, a Shell is opened in the specified container.

All three containers can be stopped using
```shell script
>>> ./rasta-docker.sh stop
```
The containers do not have to be started manually, the script will take care of it automatically.


Example: Running the SCI-LS example
```shell script
# if not already executed once
>>> ./rasta-docker.sh build

# run server
>>> ./rasta-docker.sh server scils

# run client in another terminal
>>> ./rasta-docker.sh client1 scils
```