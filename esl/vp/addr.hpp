#ifndef ADDR_HPP
#define ADDR_HPP

#define SC_INCLUDE_FX
#include <systemc>
#include <tlm>

typedef tlm::tlm_base_protocol_types::tlm_payload_type pl_t;
typedef tlm::tlm_base_protocol_types::tlm_phase_type ph_t;

//registers in Ip_hard
#define ADDR_TMPL_ROWS 0x00
#define ADDR_TMPL_COLS 0x01
#define ADDR_IMG_COLS 0x02
#define ADDR_START 0x04
#define ADDR_READY 0x05

//bram size is 240KB
#define BRAM_SIZE 0x3A980

// macro for offset (DELAY = T = 1/f = 1/100MHz = 10ns)
#define DELAY 10

// 32-bit data bus, 4 bytes
#define BUS_WIDTH 4

// locations for memory and ip 
#define VP_ADDR_BRAM_L 0x00000000
#define VP_ADDR_BRAM_H 0x00000000 + BRAM_SIZE
#define VP_ADDR_IP_HARD_L 0x40000000
#define VP_ADDR_IP_HARD_H 0x4000000F

//global variables for arguments 
/*
char *img_name;
char *tmpl_name;
int tr = 240;
*/
#endif // ADDR_HPP
