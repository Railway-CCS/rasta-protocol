FROM alpine:3.12
RUN apk update && apk add --no-cache cmake gcc make musl-dev linux-headers

# copy source and build files to container
COPY src/rasta /opt/rasta-c/src/rasta
COPY src/sci /opt/rasta-c/src/sci
COPY examples/rasta /opt/rasta-c/examples/rasta
COPY examples/scils /opt/rasta-c/examples/scils
COPY examples/scip /opt/rasta-c/examples/scip
COPY examples/localhost /opt/rasta-c/examples/localhost
COPY examples/CMakeLists.txt /opt/rasta-c/examples
COPY ./CMakeLists.txt /opt/rasta-c

# copy config files
COPY config /opt/rasta-c/config

# set build args as env variables during image build process
ARG SERVER_CH1
ARG SERVER_CH2
ARG CLIENT1_CH1
ARG CLIENT1_CH2
ARG CLIENT2_CH1
ARG CLIENT2_CH2

# replace IPs in remote example config files
RUN sed -i -e "s/10.0.0.100/$SERVER_CH1/g" /opt/rasta-c/config/rasta_server.cfg &&\
    sed -i -e "s/10.0.0.101/$SERVER_CH2/g" /opt/rasta-c/config/rasta_server.cfg &&\
    sed -i -e "s/10.0.0.200/$CLIENT1_CH1/g" /opt/rasta-c/config/rasta_client1.cfg &&\
    sed -i -e "s/10.0.0.201/$CLIENT1_CH2/g" /opt/rasta-c/config/rasta_client1.cfg &&\
    sed -i -e "s/10.0.0.250/$CLIENT2_CH1/g" /opt/rasta-c/config/rasta_client2.cfg &&\
    sed -i -e "s/10.0.0.251/$CLIENT2_CH2/g" /opt/rasta-c/config/rasta_client2.cfg

# set build args as env variables for the container
ENV SERVER_CH1=${SERVER_CH1} \
    SERVER_CH2=${SERVER_CH2}

# build librasta and examples
RUN cd /opt/rasta-c/ && mkdir -p build && cd build && \
    cmake -DEXAMPLE_IP_OVERRIDE:BOOL=ON -DBUILD_TESTING:BOOL=OFF .. && make
