//
// Created by tobia on 14.01.2018.
//

#include <ctype.h>
#include <stdlib.h>
#include <rmemory.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/wireless.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "config.h"
#include "stdio.h"
#include "string.h"

struct LineParser {
    char buf[CONFIG_BUFFER_LENGTH];
    unsigned int pos;
    char current;
    int line;
    struct RastaConfig *cfg;
};

/*
 * Private parser functions
 */
/**
 * initializes the parser and copies the line in the parser
 * @param p
 * @param line
 * @param n_line linenumber
 */
void parser_init(struct LineParser *p, const char line[CONFIG_BUFFER_LENGTH], int n_line, struct RastaConfig *cfg) {
    p->pos = 0;
    strcpy(p->buf,line);
    p->current = p->buf[0];
    p->cfg = cfg;
    p->line = n_line;
}

/**
 * increases the position of the parser and set current to the current char
 * @param p
 * @return 1 if successful, 0 if end of line
 */
int parser_next(struct LineParser *p) {
    p->pos = p->pos + 1;
    if (p->pos >= CONFIG_BUFFER_LENGTH || p->pos >= strlen(p->buf) || p->buf[p->pos] == '\n') {
        p->pos = CONFIG_BUFFER_LENGTH;
        return 0;
    }
    else {
        p->current = p->buf[p->pos];
        return 1;
    }
}

/**
 * increases the parser position until
 * @param p
 */
void parser_skipBlanc(struct LineParser* p) {
    while (p->current == ' ' || p->current == '\t') {
        if (!parser_next(p)) {
            logger_log(&p->cfg->logger,LOG_LEVEL_ERROR, p->cfg->filename, "Error in line %d: Reached unexpected end of line", p->line);
            return;
        }
    }
}

/**
 * parses the identifier with maxlength 255 characters
 * @param p
 * @param identifier pointer to the output
 */
void parser_parseIdentifier(struct LineParser *p, char* identifier) {
    parser_skipBlanc(p);
    int i = 0;
    while (isdigit(p->current) || isalpha(p->current) || (p->current == '_')) {
        if (i >= 254) {
            logger_log(&p->cfg->logger,LOG_LEVEL_ERROR, p->cfg->filename, "Error in line %d: Identifiers is too long", p->line);
            return;
        }
        if (isalpha(p->current)) {
            identifier[i] = (char)toupper(p->current);
        }
        else identifier[i] = p->current;
        i++;
        if (!parser_next(p)) {
            break;
        }
    }
    identifier[i] = '\0';
}

/**
 * parses a number literal
 * @param p
 * @param number pointer to number
 * @return 1 if successful 0 else
 */
int parser_parseNumber(struct LineParser *p, int *number) {
    int neg = 1;
    if (p->current == '-') {
        neg = -1;
        parser_next(p);
    }
    if (!isdigit(p->current)) {
        logger_log(&p->cfg->logger,LOG_LEVEL_ERROR, p->cfg->filename, "Error in line %d: Expected a digit after '-'", p->line);
        return 0;
    }

    char num_buf[100];
    int i = 0;
    while (isdigit(p->current)) {
        if (i >= 100) {
            logger_log(&p->cfg->logger,LOG_LEVEL_ERROR, p->cfg->filename, "Error in line %d: Number is too long", p->line);
            return 0;
        }
        num_buf[i] = p->current;
        i++;
        if (!parser_next(p)) {
            break;
        }
    }
    num_buf[i] = '\0';

    *number = neg * atoi(num_buf);
    return 1;

}

/**
 * parses a string
 * @param p
 * @param string pointer to string
 * @return 1 if successful 0 else
 */
int parser_parseString(struct LineParser *p, char *string) {
    if (p->current == '"') parser_next(p);

    int i = 0;

    while (p->current != '"') {
        if (i >= 254) {
            logger_log(&p->cfg->logger,LOG_LEVEL_ERROR, p->cfg->filename, "Error in line %d: String is too long", p->line);
            return 0;
        }
        string[i] = p->current;

        if (!parser_next(p)) {
            logger_log(&p->cfg->logger,LOG_LEVEL_ERROR, p->cfg->filename, "Error in line %d: Missing closing '\"'", p->line);
            return 0;
        }
        i++;
    }
    string[i] = '\0';
    return 1;
}

/**
 * parses a hex
 * @param p
 * @param hex
 * @return
 */
int parser_parseHex(struct LineParser *p, int *hex) {
    if (p->current == '#') parser_next(p);

    char num_buf[100];
    char c;
    int i = 0;

    if (isalpha(p->current)) c = (char)tolower(p->current); else c = p->current;

    while (isdigit(c) || c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f') {
        if (i >= 100) {
            logger_log(&p->cfg->logger,LOG_LEVEL_ERROR, p->cfg->filename, "Error in line %d: Hex is too long", p->line);
            return 0;
        }
        num_buf[i] = c;
        i++;

        if (!parser_next(p)) {
            break;
        }

        if (isalpha(p->current)) c = (char)tolower(p->current); else c = p->current;

    }
    num_buf[i] = '\0';

    *hex = (int)strtol(num_buf,NULL,16);
    return 1;

}
/**
 * parses an array
 * @param p
 * @param array array must be allocated with size 1
 * @return
 */
int parser_parseArray(struct LineParser *p, struct DictionaryArray * array) {
    if (p->current == '{') parser_next(p);

    unsigned int i = 0;
    while (p->current != '}') {
        parser_skipBlanc(p);
        // skip number arrays
        if(p->current == '0' || p->current == '1' || p->current == '2' || p->current == '3'
           || p->current == '4' || p->current == '5' || p->current == '6' || p->current == '7'
           || p->current == '8' || p->current == '9'){
            return 1;
        }

        if (p->current != '"') {
            logger_log(&p->cfg->logger,LOG_LEVEL_ERROR, p->cfg->filename, "Error in line %d: Expected '\"' but found %c", p->line, p->current);
            return 0;
        }

        char string[256];
        if (!parser_parseString(p,string)) {
            return 0;
        }

        //string is okay, lets check the arrays size
        if (array->count <= i) reallocate_DictionaryArray(array,i+1);
        strcpy(array->data[i].c,string);
        i++;

        if (p->current == '"') parser_next(p);
        parser_skipBlanc(p);
        if (p->current == ';' || p->current == ',') {
            parser_next(p);
            continue;
        }
        else {
            if (p->current == '}') break;
            else {
                logger_log(&p->cfg->logger,LOG_LEVEL_ERROR, p->cfg->filename, "Error in line %d: Expected ';' or '}'", p->line);
                return 0;
            }
        }
    }

    return 1;

}



/**
 * parses the value and adds the key value pair to the dictionary
 * @param p
 * @param key
 * @return
 */
void parser_parseValue(struct LineParser *p, const char key[256]) {
    //skip empty start
    parser_skipBlanc(p);

    if (p->current == '-' || isdigit(p->current)) {
        //parse number
        int number;
        if (parser_parseNumber(p,&number)) {
            dictionary_addNumber(&p->cfg->dictionary, key, number);
        }
    }
    else if (p->current == '"') {
        //parse string
        struct DictionaryString string;
        if (parser_parseString(p,string.c)) {
            dictionary_addString(&p->cfg->dictionary,key,string);
        }
    }
    else if (p->current == '{') {
        //parse array
        struct DictionaryArray array;
        array = allocate_DictionaryArray(1);
        if (parser_parseArray(p,&array)) {
            dictionary_addArray(&p->cfg->dictionary,key,array);
        }
        else {
            free_DictionaryArray(&array);
        }
    }
    else if (p->current == '#') {
        int hex;
        if (parser_parseHex(p, &hex)) {
            dictionary_addNumber(&p->cfg->dictionary, key, hex);
        }
    }
    else {
        //parse identifier
        struct DictionaryString identifier;
        parser_parseIdentifier(p,identifier.c);
        dictionary_addString(&p->cfg->dictionary,key,identifier);
    }

}

/**
 * Checks if a given NIC is a wireless interface
 * @param ifname  the name of the NIC
 * @param protocol  the protocol
 * @return {@code true} if the NIC is a wireless interface, {@code false} otherwise
 */
int isWirelessNic(const char* ifname, char* protocol) {
    int sock = -1;
    struct iwreq pwrq;
    memset(&pwrq, 0, sizeof(pwrq));
    strncpy(pwrq.ifr_name, ifname, IFNAMSIZ-1);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return 0;
    }

    if (ioctl(sock, SIOCGIWNAME, &pwrq) != -1) {
        if (protocol) strncpy(protocol, pwrq.u.name, IFNAMSIZ);
        close(sock);
        return 1;
    }

    close(sock);
    return 0;
}

/**
 * Gets the IP address of a wired NIC that matches the given index.
 * If only one wired NIC exists, the function will returns this NIC's IP without considering the index.
 * If multiple wired NIC's exists, the function will return the IP of the NIC with index index.
 *
 * Loopback interfaces are ignored.
 * @param index the index of the wired NIC
 * @return the IP address of a wired NIC that matches the given index
 */
char* getIpByNic(int index){
    char * ip = rmalloc(16);
    struct ifaddrs *ifaddr, *ifa;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs failed");
        return NULL;
    }

    int eth_nics_count = 0;
    char *nic_ip = NULL;
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        char protocol[IFNAMSIZ]  = {0};

        if (ifa->ifa_addr == NULL ||
            ifa->ifa_addr->sa_family != AF_PACKET) continue;

        if (!isWirelessNic(ifa->ifa_name, protocol)) {

            // check if it's loop interface and skip in that case
            if (ifa->ifa_flags & IFF_LOOPBACK){
                continue;
            }

            eth_nics_count++;
            int fd;
            struct ifreq ifr;

            fd = socket(AF_INET, SOCK_DGRAM, 0);
            ifr.ifr_addr.sa_family = AF_INET;
            strncpy(ifr.ifr_name, ifa->ifa_name, IFNAMSIZ-1);
            ioctl(fd, SIOCGIFADDR, &ifr);
            close(fd);
            nic_ip = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);

            if (eth_nics_count == index + 1){
                rmemcpy(ip, nic_ip, 16);
                break;
            }
        }
    }

    // no ip found, return last eth nic address
    if (nic_ip != NULL){
        rmemcpy(ip, nic_ip, 16);
    }

    freeifaddrs(ifaddr);

    return ip;
}

/**
 * accepts a string like 192.168.2.1:80 and returns the record
 * @param data
 * @return the record. Port is set to 0 if wrong format
 */
struct RastaIPData extractIPData(char data[256], int arrayIndex) {
    int points = 0;
    int numbers = 0;
    int pos = 0;
    char port[10];
    struct RastaIPData result;

    if (data[0] == '*'){
        char * ip = getIpByNic(arrayIndex);
        rmemcpy(result.ip, ip, 16);
        rfree(ip);

        pos = 1;
    } else {
        //check ip format
        for (unsigned int i = 0; i < strlen(data); i++) {
            if (isdigit(data[i])) {
                numbers++;
                if (numbers > 3) {
                    result.port = 0;
                    return result;
                }
                result.ip[i] = data[i];
            }
            else if (data[i] == '.') {
                numbers = 0;
                points++;
                if (points > 3) {
                    result.port = 0;
                    return result;
                }
                result.ip[i] = data[i];
            }
            else if (data[i] == ':') {
                if (points == 3 && numbers > 0) {
                    pos = i;
                    result.ip[i] = '\0';
                    break;
                }
            }
            else {
                result.port = 0;
                return result;
            }
        }
    }

    //get port
    int j = 0;
    for (unsigned int i = pos+1; i < strlen(data); i++) {
        if (isdigit(data[i])) {
            port[j] = data[i];
        }
        else {
            result.port = 0;
            return result;
        }
        j++;
    }
    port[j] = '\0';
    result.port = atoi(port);
    return result;
}

/**
 * sets the standard values in config
 * @param cfg
 */
void config_setstd(struct RastaConfig * cfg) {
    struct DictionaryEntry entr;

    /*
     * sending part
     */

    //tmax
    entr = config_get(cfg, "RASTA_T_MAX");
    if (entr.type != DICTIONARY_NUMBER || entr.value.number < 0) {
        //set std
        cfg->values.sending.t_max = 1800;
    }
    else {
        //check valid format
        cfg->values.sending.t_max = (unsigned int)entr.value.number;
    }

    //t_h
    entr = config_get(cfg, "RASTA_T_H");
    if (entr.type != DICTIONARY_NUMBER || entr.value.number < 0) {
        //set std
        cfg->values.sending.t_h = 300;
    }
    else {
        //check valid format
        cfg->values.sending.t_h = (unsigned int)entr.value.number;
    }

    //checksum type
    entr = config_get(cfg, "RASTA_MD4_TYPE");

    // the RASTA_MD4_TYPE key is only used for compatibility reasons, otherwise its called RASTA_SR_CHECKSUM_LEN
    if (entr.type == DICTIONARY_ERROR){
        entr = config_get(cfg, "RASTA_SR_CHECKSUM_LEN");
    }

    if (entr.type != DICTIONARY_STRING) {
        //set std
        cfg->values.sending.md4_type = RASTA_CHECKSUM_8B;
    }
    else {
        //check right parameters
        if (strcmp(entr.value.string.c, "NONE") == 0) {
            cfg->values.sending.md4_type = RASTA_CHECKSUM_NONE;
        }
        else if (strcmp(entr.value.string.c, "HALF") == 0) {
            cfg->values.sending.md4_type = RASTA_CHECKSUM_8B;
        }
        else if (strcmp(entr.value.string.c, "FULL") == 0) {
            cfg->values.sending.md4_type = RASTA_CHECKSUM_16B;
        }
        else {
            //set std
            logger_log(&cfg->logger,LOG_LEVEL_ERROR, cfg->filename, "RASTA_MD4_TYPE or RASTA_SR_CHECKSUM_LEN  may only be NONE, HALF or FULL");
            cfg->values.sending.md4_type = RASTA_CHECKSUM_8B;
        }
    }

    // hash function key
    entr = config_get(cfg, "RASTA_SR_CHECKSUM_KEY");
    // only accept numbers
    if (entr.type == DICTIONARY_NUMBER){
        cfg->values.sending.sr_hash_key = (unsigned int)entr.value.number;
    }

    // hash algorithm
    entr = config_get(cfg, "RASTA_SR_CHECKSUM_ALGO");
    if (entr.type != DICTIONARY_STRING){
        // set MD4 as default value
        cfg->values.sending.sr_hash_algorithm = RASTA_ALGO_MD4;
    } else{
        if (strcmp(entr.value.string.c, "MD4") == 0){
            cfg->values.sending.sr_hash_algorithm = RASTA_ALGO_MD4;
        } else if (strcmp(entr.value.string.c, "BLAKE2B") == 0) {
            cfg->values.sending.sr_hash_algorithm = RASTA_ALGO_BLAKE2B;
        } else if (strcmp(entr.value.string.c, "SIPHASH-2-4") == 0){
            cfg->values.sending.sr_hash_algorithm = RASTA_ALGO_SIPHASH_2_4;
        }
    }


    //md4_a
    entr = config_get(cfg, "RASTA_MD4_A");
    if (entr.type != DICTIONARY_NUMBER) {
        //set std
        cfg->values.sending.md4_a = 0x67452301;
    }
    else {
        //check valid format
        cfg->values.sending.md4_a = (unsigned int)entr.value.number;
    }

    //md4_b
    entr = config_get(cfg, "RASTA_MD4_B");
    if (entr.type != DICTIONARY_NUMBER) {
        //set std
        cfg->values.sending.md4_b = 0xefcdab89;
    }
    else {
        //check valid format
        cfg->values.sending.md4_b = (unsigned int)entr.value.number;
    }

    //md4_c
    entr = config_get(cfg, "RASTA_MD4_C");
    if (entr.type != DICTIONARY_NUMBER) {
        //set std
        cfg->values.sending.md4_c = 0x98badcfe;
    }
    else {
        //check valid format
        cfg->values.sending.md4_c = (unsigned int)entr.value.number;
    }

    //md4_d
    entr = config_get(cfg, "RASTA_MD4_D");
    if (entr.type != DICTIONARY_NUMBER) {
        //set std
        cfg->values.sending.md4_d = 0x10325476;
    }
    else {
        //check valid format
        cfg->values.sending.md4_d = (unsigned int)entr.value.number;
    }

    //sendmax
    entr = config_get(cfg, "RASTA_SEND_MAX");
    if (entr.type != DICTIONARY_NUMBER || entr.value.number < 0) {
        //set std
        cfg->values.sending.send_max = 20;
    }
    else {
        //check valid format
        cfg->values.sending.send_max = (unsigned short)entr.value.number;
    }

    //mwa
    entr = config_get(cfg, "RASTA_MWA");
    if (entr.type != DICTIONARY_NUMBER || entr.value.number < 0) {
        //set std
        cfg->values.sending.mwa = 10;
    }
    else {
        //check valid format
        cfg->values.sending.mwa = (unsigned short)entr.value.number;
    }

    //maxpacket
    entr = config_get(cfg, "RASTA_MAX_PACKET");
    if (entr.type != DICTIONARY_NUMBER || entr.value.number < 0) {
        //set std
        cfg->values.sending.max_packet = 3;
    }
    else {
        //check valid format
        cfg->values.sending.max_packet = (unsigned int)entr.value.number;
    }

    //diagwindow
    entr = config_get(cfg, "RASTA_DIAG_WINDOW");
    if (entr.type != DICTIONARY_NUMBER || entr.value.number < 0) {
        //set std
        cfg->values.sending.diag_window = 5000;
    }
    else {
        //check valid format
        cfg->values.sending.diag_window = (unsigned int)entr.value.number;
    }

    /*
     * Redundancy part
     */

    //redundancy channels
    entr = config_get(cfg, "RASTA_REDUNDANCY_CONNECTIONS");
    if (entr.type != DICTIONARY_ARRAY || entr.value.array.count == 0) {
        //set std
        cfg->values.redundancy.connections.count = 0;
    }
    else {
        cfg->values.redundancy.connections.data = rmalloc(sizeof(struct RastaIPData) * entr.value.array.count);
        cfg->values.redundancy.connections.count = entr.value.array.count;
        //check valid format
        for (unsigned int i = 0; i < entr.value.array.count; i++) {
            struct RastaIPData ip = extractIPData(entr.value.array.data[i].c, i);
            if (ip.port == 0) {
                logger_log(&cfg->logger,LOG_LEVEL_ERROR, cfg->filename, "RASTA_REDUNDANCY_CONNECTIONS may only contain strings in format ip:port or *:port");
                rfree(entr.value.array.data);
                entr.value.array.count = 0;
                break;
            }
            cfg->values.redundancy.connections.data[i] = ip;
        }
    }

    //crc type
    entr = config_get(cfg, "RASTA_CRC_TYPE");
    if (entr.type != DICTIONARY_STRING) {
        //set std
        cfg->values.redundancy.crc_type = crc_init_opt_a();
    }
    else {
        //check right parameters
        if (strcmp(entr.value.string.c, "TYPE_A") == 0) {
            cfg->values.redundancy.crc_type = crc_init_opt_a();
        }
        else if (strcmp(entr.value.string.c, "TYPE_B") == 0) {
            cfg->values.redundancy.crc_type = crc_init_opt_b();
        }
        else if (strcmp(entr.value.string.c, "TYPE_C") == 0) {
            cfg->values.redundancy.crc_type = crc_init_opt_c();
        }
        else if (strcmp(entr.value.string.c, "TYPE_D") == 0) {
            cfg->values.redundancy.crc_type = crc_init_opt_d();
        }
        else if (strcmp(entr.value.string.c, "TYPE_E") == 0) {
            cfg->values.redundancy.crc_type = crc_init_opt_e();
        }
        else {
            //set std
            logger_log(&cfg->logger,LOG_LEVEL_ERROR, cfg->filename, "RASTA_CRC_TYPE may only be TYPE_A, TYPE_B, TYPE_C, TYPE_D or TYPE_E");
            cfg->values.redundancy.crc_type = crc_init_opt_a();
        }
    }

    //tseq
    entr = config_get(cfg, "RASTA_T_SEQ");
    if (entr.type != DICTIONARY_NUMBER || entr.value.number < 0) {
        //set std
        cfg->values.redundancy.t_seq = 100;
    }
    else {
        //check valid format
        cfg->values.redundancy.t_seq = (unsigned short)entr.value.number;
    }

    //ndiagnose
    entr = config_get(cfg, "RASTA_N_DIAGNOSE");
    if (entr.type != DICTIONARY_NUMBER || entr.value.number < 0) {
        //set std
        cfg->values.redundancy.n_diagnose = 200;
    }
    else {
        //check valid format
        cfg->values.redundancy.n_diagnose = (unsigned short)entr.value.number;
    }

    //ndeferqueue
    entr = config_get(cfg, "RASTA_N_DEFERQUEUE_SIZE");
    if (entr.type != DICTIONARY_NUMBER || entr.value.number < 0) {
        //set std
        cfg->values.redundancy.n_deferqueue_size = 4;
    }
    else {
        //check valid format
        cfg->values.redundancy.n_deferqueue_size = (unsigned short)entr.value.number;
    }

    /*
     * General
     */

    //network
    entr = config_get(cfg, "RASTA_NETWORK");
    if (entr.type != DICTIONARY_NUMBER || entr.value.number < 0) {
        //set std
        cfg->values.general.rasta_network = 0;
    }
    else {
        //check valid format
        cfg->values.general.rasta_network = (unsigned long)entr.value.number;
    }

    //receiver
    entr = config_get(cfg, "RASTA_ID");
    if (entr.type != DICTIONARY_NUMBER) {
        //set std
        cfg->values.general.rasta_id = 0;
    }
    else {
        //check valid format
        cfg->values.general.rasta_id = (unsigned long)entr.value.unumber;
    }



}

/*
 * Public functions
 */
struct RastaConfig config_load(const char filename[256]) {

    FILE *f;
    char buf[CONFIG_BUFFER_LENGTH];
    struct RastaConfig config;
    strcpy(config.filename,filename);

    config.logger = logger_init(LOG_LEVEL_INFO,LOGGER_TYPE_CONSOLE);

    f = fopen(config.filename,"r");
    if (!f){
        logger_log(&config.logger, LOG_LEVEL_ERROR, config.filename, "File not found");
        return config;
    }

    config.dictionary = dictionary_create(2);

    int n = 1;
    while (fgets(buf,CONFIG_BUFFER_LENGTH, f)!=NULL) {
        //initialize parser
        struct LineParser p;
        parser_init(&p,buf,n,&config);


        //skip empty start
        parser_skipBlanc(&p);

        //ignore comments
        if (p.current == ';'){
            n++;
            continue;
        }

        //ignore empty lines
        if (p.pos +1 >= strlen(buf)){
            n++;
            continue;
        }

        //ignore lines starting with unexpected characters
        if (!(isdigit(p.current) || isalpha(p.current) || (p.current == '_'))) {
            n++;
            continue;
        }

        //parse key
        char key[256];
        parser_parseIdentifier(&p,key);

        //skip empty start
        parser_skipBlanc(&p);

        if (p.current != '=') {
            logger_log(&p.cfg->logger,LOG_LEVEL_ERROR, p.cfg->filename, "Error in line %d: Expected '=' but found '%c'", p.line,p.current);
            n++;
            continue;
        }
        parser_next(&p);

        //skip empty start
        parser_skipBlanc(&p);

        parser_parseValue(&p,key);



        n++;
    }

    fclose(f);

    //initialize standart value
    config_setstd(&config);

    return config;
}


struct DictionaryEntry config_get(struct RastaConfig * cfg, char key[256]) {
    return dictionary_get(&cfg->dictionary,key);
}

void config_free(struct RastaConfig *cfg) {
    dictionary_free(&cfg->dictionary);
    if (cfg->values.redundancy.connections.count > 0) rfree(cfg->values.redundancy.connections.data);
}
