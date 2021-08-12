#! /bin/bash

if [ "$1" == "build" ]; then
  # create network for the rasta containers
  echo "Creating network for RaSTA containers..."
  docker network create --driver=bridge --ip-range=10.10.10.0/24 --subnet=10.10.10.0/24 rastanet

  # build the image
  echo "Building container image..."
  docker build \
       --build-arg SERVER_CH1="10.10.10.100" \
       --build-arg SERVER_CH2="10.10.10.100" \
       --build-arg CLIENT1_CH1="10.10.10.110" \
       --build-arg CLIENT1_CH2="10.10.10.110" \
       --build-arg CLIENT2_CH1="10.10.10.120" \
       --build-arg CLIENT2_CH2="10.10.10.120" \
       -t rastac .


  echo "Creating RaSTA containers..."
  # remove containers if they already exist
  docker stop rasta-server
  docker stop rasta-client1
  docker stop rasta-client2
  docker rm rasta-server
  docker rm rasta-client1
  docker rm rasta-client2

  # create containers for server and 2 clients
  docker run -di --network rastanet --ip 10.10.10.100 --name rasta-server rastac
  docker run -di --network rastanet --ip 10.10.10.110 --name rasta-client1 rastac
  docker run -di --network rastanet --ip 10.10.10.120 --name rasta-client2 rastac
elif [ "$1" == "server" ]; then
  # start container if not already running
  docker start rasta-server

  if [ "$2" == "scils" ]; then
    # run scils example
    echo "Executing SCI-LS example server..."
    docker exec -it rasta-server /bin/sh -c "cd /opt/rasta-c/build/bin/exe/examples && ./scils_example s"
  elif [ "$2" == "scip" ]; then
    # run scip example
    echo "Executing SCI-P example server..."
    docker exec -it rasta-server /bin/sh -c "cd /opt/rasta-c/build/bin/exe/examples && ./scip_example s"
  elif [ "$3" == "rasta" ]; then
    # run rasta example
    echo "Executing RaSTA example receiver..."
    docker exec -it rasta-server /bin/sh -c "cd /opt/rasta-c/build/bin/exe/examples && ./rasta_example r"
  else
    # open shell
    docker exec -it rasta-server /bin/sh
  fi
elif [ "$1" == "client1" ]; then
  # start container if not already running
  docker start rasta-client1

  if [ "$2" == "scils" ]; then
    # run scils example
    echo "Executing SCI-LS example server..."
    docker exec -it rasta-client1 /bin/sh -c "cd /opt/rasta-c/build/bin/exe/examples && ./scils_example c"
  elif [ "$2" == "scip" ]; then
    # run scip example
    echo "Executing SCI-P example server..."
    docker exec -it rasta-client1 /bin/sh -c "cd /opt/rasta-c/build/bin/exe/examples && ./scip_example c"
  elif [ "$3" == "rasta" ]; then
    # run rasta example
    echo "Executing RaSTA example receiver..."
    docker exec -it rasta-client1 /bin/sh -c "cd /opt/rasta-c/build/bin/exe/examples && ./rasta_example s1"
  else
    # open shell
    docker exec -it rasta-client1 /bin/sh
  fi
elif [ "$1" == "client2" ]; then
  # start container if not already running
  docker start rasta-client2

  if [ "$3" == "rasta" ]; then
    # run rasta example
    echo "Executing RaSTA example sender 2..."
    docker exec -it rasta-client2 /bin/sh -c "cd /opt/rasta-c/build/bin/exe/examples && ./rasta_example s1"
  else
    # open shell
    docker exec -it rasta-client2 /bin/sh
  fi
elif [ "$1" == "stop" ]; then
    # stop containers
    echo "Stopping server container..."
    docker stop rasta-server
    echo "Stopping client 1 container..."
    docker stop rasta-client1
    echo "Stopping client 2 container..."
    docker stop rasta-client2
else
  echo "Usage:"
  echo "./rasta-docker.sh build"
  echo "      Builds the image and creates the containers"
  echo "./rasta-docker.sh stop"
  echo "      Stops all RaSTA containers"
  echo "./rasta-docker.sh (server|client1|client2) [EXAMPLE]"
  echo "      Executes the example (scils/scip/rasta) in the given container. If no example is passed, a shell is opened"
fi