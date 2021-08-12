#ifndef LST_SIMULATOR_RASTAFACTORYTEST_H
#define LST_SIMULATOR_RASTAFACTORYTEST_H

/**
 * tests the correct creation of a redundancy layer PDU
 */
void testCreateRedundancyPacket();

/**
 * tests the correct creation of a redundancy layer PDU without a checksum (opt a)
 */
void testCreateRedundancyPacketNoChecksum();

/**
 * tests the connection packages
 */
void checkConnectionPacket();

/**
 * tests all packages with no special data
 */
void checkNormalPacket();

/**
 * tests the disconnectionrequest package
 */
void checkDisconnectionRequest();

/**
 * tests all packages, that transmit different data
 */
void checkMessagePacket();

#endif //LST_SIMULATOR_RASTAFACTORYTEST_H
