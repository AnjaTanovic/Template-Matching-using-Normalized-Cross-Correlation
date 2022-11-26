#ifndef ADDR_HPP
#define ADDR_HPP

#define SC_INCLUDE_FX
#include <systemc>
#include <tlm>

typedef tlm::tlm_base_protocol_types::tlm_payload_type pl_t;
typedef tlm::tlm_base_protocol_types::tlm_phase_type ph_t;

//registers in Hard
#define ADDR_START 0x00
#define ADDR_TMPL_COLS 0x04
#define ADDR_TMPL_ROWS 0x08
#define ADDR_IMG_COLS 0x12
#define ADDR_READY 0x16
#define ADDR_SUM2 0x20

//bram in Hard
#define BRAM_TMPL 0x24
#define BRAM_IMG 51036
#define BRAM_RESP 211036
#define BRAM_SUM1 211836
#define BRAM_END 212636
#define RESET 212637

// macro for offset (DELAY = T = 1/f = 1/100MHz = 10ns)
#define DELAY 10

// 32-bit data bus, 4 bytes
#define BUS_WIDTH 4

// locations for memory and ip 
//#define VP_ADDR_BRAM_L 0x00000000
//#define VP_ADDR_BRAM_H 0x00000000 + BRAM_SIZE
#define VP_ADDR_HARD_L 0x400000000
#define VP_ADDR_HARD_H 0x500000000

#endif // ADDR_HPP
