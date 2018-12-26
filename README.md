# RS485-comminicator

## NOTE: repository & library under development (26-12-2018)

RS485-comminicatoris a serial synchronization library for master to multi-slave communication designed to work on a RS485 bus. The main purposes of this library are:

* **Organize serial data transfer** in order to avoid collisions when multiple devices uses the same shared medium (like a RS485 bus). This is done by a token-driven logic.
* **Message integrity** in order to detect when errors occurs during trnsmission. This is done by crc8 checks

# Wiki
More informations can be found in the wiki pages: 

[Wiki home](https://github.com/grisson2/rs485-communicator/wiki)

[Synchronization protocol & frame structure](https://github.com/grisson2/rs485-communicator/wiki/Synchronization-protocol-&-frame-structure)

[Tuning the token return timeout value](https://github.com/grisson2/rs485-communicator/wiki/Tuning-the-token-return-timeout-value)

[RS485 trnsceivers](https://github.com/grisson2/rs485-communicator/wiki/RS485-transceivers)

[Circuit sample & cables](https://github.com/grisson2/rs485-communicator/wiki/Circuit-sample-&-cable)

# Samples

See [Samples folder](https://github.com/grisson2/rs485-communicator/tree/master/examples) in the source code directory for a quick review on both Master and slave implementations.


# License
This library is distribuited under GPLv3. See [GPLv3 disclaimer](https://github.com/grisson2/rs485-communicator/blob/master/LICENSE).



