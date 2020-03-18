#ifndef TWG_DEFS_E_H
#define TWG_DEFS_E_H

// 512 bytes for length
#define TWG_MAX_BUF 512

typedef struct twg_data_T {
 uint16_t blen;			// in bits
 uint8_t  buff[ TWG_MAX_BUF/8];
} twg_data_t;

#endif // TPSID_DEFS_E_H
