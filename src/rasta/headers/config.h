//
// Created by tobia on 14.01.2018.
//

#ifndef LST_SIMULATOR_CONFIG_H
#define LST_SIMULATOR_CONFIG_H

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

#include "dictionary.h"
#include "logging.h"
#include "rastafactory.h"
#define CONFIG_BUFFER_LENGTH 10000

/**
 * defined in 7.2
 */
struct RastaConfigInfoSending {
    unsigned int t_max;
    unsigned int t_h;
    rasta_checksum_type md4_type;
    MD4_u32plus md4_a;
    MD4_u32plus md4_b;
    MD4_u32plus md4_c;
    MD4_u32plus md4_d;
    unsigned short mwa;
    unsigned short send_max;
    unsigned int max_packet;
    unsigned int diag_window;
    unsigned int sr_hash_key;
    rasta_hash_algorithm sr_hash_algorithm;
};

/**
 * represents an IP and Port
 */
struct RastaIPData {
    char ip[16];
    int port;
};

/**
 * represents a list of IP-Port
 */
struct RastaConfigRedundancyConnections {
    struct RastaIPData *data;
    unsigned int count;
};

/**
 * defined in 7.3
 */
struct RastaConfigInfoRedundancy {
    struct RastaConfigRedundancyConnections connections;
    struct crc_options crc_type;
    unsigned int t_seq;
    int n_diagnose;
    unsigned int n_deferqueue_size;
};

/**
 * defined in 8.1
 */
struct RastaConfigInfoGeneral {
    unsigned long rasta_network;
    unsigned long rasta_id;
};
/**
 * stores all presets after load
 */
struct RastaConfigInfo {
    /**
     * all values for the sending part
     */
    struct RastaConfigInfoSending sending;
    /**
     * all values for the redundancy part
     */
    struct RastaConfigInfoRedundancy redundancy;
    /**
     * includes rastanetwork, receiver and sender id
     * values are 0 if not set in config
     */
    struct RastaConfigInfoGeneral general;
};

/**
 * represents a rasta config
 * NOTE: please use the functions provided in config.h to access the dictionarys elements or use values for standart values
 */
struct RastaConfig {
    /**
     * the dictionary
     */
    struct Dictionary dictionary;

    /**
     * console logger for debug information
     */
    struct logger_t logger;

    /**
     * the filename (do not set manually)
     */
    char filename[512];
    /*
     * the standard values
     */
    struct RastaConfigInfo values;
};

/**
 * loads a config file and returns the config representing the values in the file
 * @param filename
 * @return
 */
struct RastaConfig config_load(const char filename[256]);

/**
 * returns the entry behind the key
 * NOTE: check the type before accessing the value. ERROR means, the key is not in the dictionary
 * @param cfg
 * @param key
 * @return
 */
struct DictionaryEntry config_get(struct RastaConfig * cfg, const char* key);

/**
 * frees the config
 * @param cfg
 */
void config_free(struct RastaConfig *cfg);

#ifdef __cplusplus
}
#endif

#endif //LST_SIMULATOR_CONFIG_H
