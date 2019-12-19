# Pre-Threaded-Image-Processing-Server

The main thread (master) accepts connection
requests puts the connection descriptors in a bounded buffer and the in-
dividual worker threads remove requests from the bounded buffer for
processing.The server has three worker threads and the priority of the master thread is higher than the worker threads
