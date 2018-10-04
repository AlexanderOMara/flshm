# flshm

Flash Player LocalConnection Shared Memory Native Code Library


## Overview

This library is a native code library for connecting to the LocalConnection API used by Flash Player. This is done by connecting to the same shared memory and semaphores, to read and write to the messaging system. All of this was made possible by reverse engineering the API's and messaging formats used by Flash Player.


## Compatibility

The library was tested against Flash Player 6 and up (Flash Player 6 introduced LocalConnection), and Flash Player on Windows, Mac, and Linux are all supported.

This library has been tested against the compilers Clang, GCC, and MinGW, but the source files should work with other compatible compilers.


## Usage

See the `util` directory for some sample usage. Some things to keep in mind.

 - Message and connection properties vary by different versions, refer to header comments for details.
 - Message `data` is a series of AMF encoded data, AMF0 or AMF3 depending on the message (`size` defines how large the encoded data is).
 - AMF encoders and decoders are not included, the library only handles the subset of AMF0 necessary to parse the message headers.
 - AMF0 arguments are encoded in reverse order, however AMF3 arguments are not.
 - Some data types like an AMF0 string are trivial to encode/decode, but if full AMF0 or AMF3 support is required, consider some existing AMF libraries.
 - There is a maximum of 8 connections imposed by Flash Player.
 - Connection names must be in one of the following formats, else it is invalid.
   - `hostname:connection-name` (`example.com:name`, `localhost:name`)
   - `_global-connection-name` (`_domainshared:name`)
 - Connection names do not include the lowest subdomain.
   - `example.com` = `example.com`
   - `www.example.com` = `example.com`
   - `a.b.example.com` = `b.example.com`).
 - Set `is_per_user` to match the `isPerUser` property used in ActionScript (the default is and will likely stay `false`, and this property is not available in older versions of Flash Player itself).
 - Sandboxing is not implemented in the library itself, so validate the message before handling it.
 - Clear a message when received, and intended for the connection, else it will timeout and the connection will be removed from the registered list.
 - Use the `flshm_lock` and `flshm_unlock` functions to lock the semaphore for exclusive access to the shared memory while reading and writing messages and connections to avoid problems with race conditions.
 - Use the `flshm_close` and `flshm_message_free` functions to free memory allocated by the library, and avoid memory leaks.


## Bugs

If you find a bug or have compatibility issues, please open a ticket under issues section for this repository.

Pull requests are also welcome, but any changes must be cross-platform to be merged.


## License

Copyright (c) 2016-2018 Alexander O'Mara

Licensed under the Mozilla Public License, v. 2.0.

If this license does not work for you, feel free to contact me.


## Donations

If you find my software useful, please consider supporting independent and open-source software development by making a modest donation on my website at [alexomara.com](http://alexomara.com).
