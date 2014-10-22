PROJECT(reduce-rpc-interface)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6) 

AUX_SOURCE_DIRECTORY(. DIR_SRC)

SET(header_files
    ./ILocaleEndpoint.h
    ./ILocaleObj.h
    ./IRemoteEndpoint.h
    ./IRemoteObj.h
    ./ITimeCallback.h
)

SET(SRC_LIST 
    ${DIR_SRC}
)

ADD_LIBRARY(reduce-rpc-interface ${SRC_LIST} ${header_files})