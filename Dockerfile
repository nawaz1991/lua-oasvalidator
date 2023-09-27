FROM kong/kong:3.3.1-alpine

# Root needed to install dependencies
USER root


WORKDIR /tmp

COPY ./*.rockspec /tmp
COPY ./LICENSE /tmp/LICENSE
COPY ./src /tmp/src
COPY ./include /tmp/include
COPY ./rapidjson /tmp/rapidjson
COPY ./cmake /tmp/cmake
COPY ./CMakeLists.txt /tmp

# Install cmake, make, gcc, and g++
RUN apk --no-cache add git cmake make gcc g++

RUN luarocks build oasvalidator-0.1.0-1.rockspec
RUN luarocks show oasvalidator

# RUN luarocks make && luarocks pack oasvalidator 0.1.0-1

