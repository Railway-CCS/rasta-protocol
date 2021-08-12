//
// Created by tobia on 28.11.2017.
//

#ifndef LST_SIMULATOR_RASTAMODULETEST_H
#define LST_SIMULATOR_RASTAMODULETEST_H

void testConversion();

/**
 * test if the conversion of redundancy packets with a crc checksum with width > 0 is working
 */
void testRedundancyConversionWithCrcChecksumCorrect();

/**
 * test if the conversion of redundancy packets without a crc checksum (opt a) is working
 */
void testRedundancyConversionWithoutChecksum();

/**
 * test if the checksum is marked incorrect after a manipulation of the data
 */
void testRedundancyConversionIncorrectChecksum();

#endif //LST_SIMULATOR_RASTAMODULETEST_H
