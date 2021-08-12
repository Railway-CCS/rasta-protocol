#ifndef LST_SIMULATOR_SCITESTS_H
#define LST_SIMULATOR_SCITESTS_H

void testEncode();
void testDecode();
void testDecodeInvalid();
void testSetSender();
void testSetReceiver();
void testGetName();
void testSetMessageType();
void testGetMessageType();

void testCreateVersionRequest();
void testCreateVersionResponse();
void testCreateStatusRequest();
void testCreateStatusBegin();
void testCreateStatusFinish();

void testParseVersionRequest();
void testParseVersionResponse();

#endif //LST_SIMULATOR_SCITESTS_H
