PROJECT(test)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6) 

ADD_SUBDIRECTORY(test_container )
ADD_SUBDIRECTORY(test_remote_queue\\client)
ADD_SUBDIRECTORY(test_remote_queue\\server)