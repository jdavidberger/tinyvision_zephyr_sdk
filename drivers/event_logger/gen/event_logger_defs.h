#pragma once
//Generated do not edit!
#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"

#include "event_logger.h"

/****

**/

uint32_t GlobalLogger_INDEX_BITS = 5;
uint32_t GlobalLogger_SIGNATURE = 0xe7ac35a;
uint32_t GlobalLogger_EVENT_COUNT = 18;

static GlobalLogger_info_t GlobalLogger_info_get(volatile uint32_t* base) {
  GlobalLogger_info_t rtn = (GlobalLogger_info_t) {
    .ctrl = base[0],
    .captured_events = base[1],
    .checksum = base[5],
    .sysclk_lsb = base[6],
    .fifo_occupancy = base[7],
    .inactive_mask = base[9],
    .signature = base[12],
    .dropped_events = base[13]
  };
  for(int i = 0;i < 18;i++) {
     rtn.event_counter[i] = base[56/4 + i];
  }
  return rtn;
}

typedef struct GlobalLogger_ctx {
    void* user;
    uint32_t ctrl;
    uint64_t last_timestamp;
} GlobalLogger_ctx;

static void GlobalLogger_enable_memory_dump(GlobalLogger_ctx* ctx, volatile uint32_t* base, bool enable) {
  if(ctx->ctrl != enable) {
    base[0] = enable;
    ctx->ctrl = enable;
  }
}

typedef struct GlobalLogger_transaction {
  uint32_t l[3];
} GlobalLogger_transaction;

static void GlobalLogger_handle(GlobalLogger_ctx* ctx, const struct GlobalLogger_transaction* tx, uint32_t mask);
bool GlobalLogger_poll(GlobalLogger_ctx* ctx, volatile uint32_t* ip_location, uint32_t mask) {
  GlobalLogger_enable_memory_dump(ctx, ip_location, 1);
  struct GlobalLogger_transaction tx = {0};
  tx.l[0] = ip_location[2 + 0];
  if((tx.l[0] & 1) == 1) {
      tx.l[1] = ip_location[2 + 1];
      tx.l[2] = ip_location[2 + 2];

    GlobalLogger_handle(ctx, &tx, mask);
    return true;
  }
  return false;
}

static uint64_t shift(uint64_t d, int16_t shift) {
  if(shift >= 0) {
    return d >> shift;
  }
  return d << (-shift);
}
static uint64_t GlobalLogger_parse_field(const struct GlobalLogger_transaction* tx, int8_t bit_offset, int8_t bit_width) {
    if(bit_width == 0) {
        return 0;
    }
  uint64_t l0 = tx->l[0];
  uint64_t l1 = tx->l[1];
  uint64_t l2 = tx->l[2];
  uint64_t mask = bit_width == 64 ? 0xffffffffffffffffll : ((1ll << bit_width) - 1);
  return (shift(l0, bit_offset) |
     shift(l1, bit_offset-32) |
     shift(l2, bit_offset-64)) & mask;
}


static uint64_t GlobalLogger_full_time(const struct GlobalLogger_transaction* tx, uint64_t gtime, uint8_t time_bit_width) {
  uint64_t time_part = GlobalLogger_parse_field(tx, 96 - time_bit_width, 64);
  if(time_bit_width >= 64) {
      return time_part;
  }
  uint64_t mask_bits = time_bit_width;
  uint64_t next_incr = (1LL << mask_bits);
  uint64_t mask = next_incr - 1;
  uint64_t time_reconstruction = (gtime & ~mask) | time_part;
  if(time_reconstruction + (next_incr/4) < gtime) {
      return time_reconstruction + next_incr;
  }
  return time_reconstruction;
}

#define GlobalLogger_DEFINITIONS(HANDLE_DEFINE) \
   HANDLE_DEFINE(Axi4B_10)\
   HANDLE_DEFINE(Axi4R_75)\
   HANDLE_DEFINE(Axi4W_73)\
   HANDLE_DEFINE(Axi4Aw_45)\
   HANDLE_DEFINE(usb_irq_4)\
   HANDLE_DEFINE(TRBSink_15)\
   HANDLE_DEFINE(usbReadsPerMs_64)\
   HANDLE_DEFINE(Fragment_11)\
   HANDLE_DEFINE(Axi4Ar_45)\
   HANDLE_DEFINE(PipelinedMemoryBusCmd_69)


#define GLOBALLOGGER_FULL_TIME_ID 0x1f
typedef struct GlobalLogger_Axi4B_10_t {
	uint8_t id;
	uint8_t resp;
} GlobalLogger_Axi4B_10_t;
#define GlobalLogger_Axi4B_10_FIELDS(HANDLE_FIELD) \
	HANDLE_FIELD(id) \
	HANDLE_FIELD(resp) \

typedef struct GlobalLogger_Axi4R_75_t {
	uint64_t data;
	uint8_t id;
	uint8_t resp;
	bool last;
} GlobalLogger_Axi4R_75_t;
#define GlobalLogger_Axi4R_75_FIELDS(HANDLE_FIELD) \
	HANDLE_FIELD(data) \
	HANDLE_FIELD(id) \
	HANDLE_FIELD(resp) \
	HANDLE_FIELD(last) \

typedef struct GlobalLogger_Axi4W_73_t {
	uint64_t data;
	uint8_t strb;
	bool last;
} GlobalLogger_Axi4W_73_t;
#define GlobalLogger_Axi4W_73_FIELDS(HANDLE_FIELD) \
	HANDLE_FIELD(data) \
	HANDLE_FIELD(strb) \
	HANDLE_FIELD(last) \

typedef struct GlobalLogger_Axi4Aw_45_t {
	uint32_t addr;
	uint8_t id;
	uint8_t len;
	uint8_t size;
	uint8_t burst;
} GlobalLogger_Axi4Aw_45_t;
#define GlobalLogger_Axi4Aw_45_FIELDS(HANDLE_FIELD) \
	HANDLE_FIELD(addr) \
	HANDLE_FIELD(id) \
	HANDLE_FIELD(len) \
	HANDLE_FIELD(size) \
	HANDLE_FIELD(burst) \

typedef struct GlobalLogger_usb_irq_4_t {
	bool usbCore_irq;
	bool mgrs_0_io_irq;
	bool mgrs_1_io_irq;
	bool irq;
} GlobalLogger_usb_irq_4_t;
#define GlobalLogger_usb_irq_4_FIELDS(HANDLE_FIELD) \
	HANDLE_FIELD(usbCore_irq) \
	HANDLE_FIELD(mgrs_0_io_irq) \
	HANDLE_FIELD(mgrs_1_io_irq) \
	HANDLE_FIELD(irq) \

typedef struct GlobalLogger_TRBSink_15_t {
	uint8_t stateReg;
	bool io_enable;
	bool io_recheck;
	bool recheckReg;
	uint8_t io_trbID;
} GlobalLogger_TRBSink_15_t;
#define GlobalLogger_TRBSink_15_FIELDS(HANDLE_FIELD) \
	HANDLE_FIELD(stateReg) \
	HANDLE_FIELD(io_enable) \
	HANDLE_FIELD(io_recheck) \
	HANDLE_FIELD(recheckReg) \
	HANDLE_FIELD(io_trbID) \

#define GlobalLogger_usbReadsPerMs_64_FIELDS(HANDLE_FIELD) HANDLE_FIELD(value)
typedef struct GlobalLogger_usbReadsPerMs_64_t {
    uint64_t value;
} GlobalLogger_usbReadsPerMs_64_t;
typedef struct GlobalLogger_Fragment_11_t {
	bool last;
	uint16_t fragment;
} GlobalLogger_Fragment_11_t;
#define GlobalLogger_Fragment_11_FIELDS(HANDLE_FIELD) \
	HANDLE_FIELD(last) \
	HANDLE_FIELD(fragment) \

typedef struct GlobalLogger_Axi4Ar_45_t {
	uint32_t addr;
	uint8_t id;
	uint8_t len;
	uint8_t size;
	uint8_t burst;
} GlobalLogger_Axi4Ar_45_t;
#define GlobalLogger_Axi4Ar_45_FIELDS(HANDLE_FIELD) \
	HANDLE_FIELD(addr) \
	HANDLE_FIELD(id) \
	HANDLE_FIELD(len) \
	HANDLE_FIELD(size) \
	HANDLE_FIELD(burst) \

typedef struct GlobalLogger_PipelinedMemoryBusCmd_69_t {
	bool write;
	uint32_t address;
	uint32_t data;
	uint8_t mask;
} GlobalLogger_PipelinedMemoryBusCmd_69_t;
#define GlobalLogger_PipelinedMemoryBusCmd_69_FIELDS(HANDLE_FIELD) \
	HANDLE_FIELD(write) \
	HANDLE_FIELD(address) \
	HANDLE_FIELD(data) \
	HANDLE_FIELD(mask) \

const char* GlobalLogger_get_id_name(int id) {
   switch(id) {
   case 0: return "io_chunkInfo_payload";
   case 1: return "TRBSink";
   case 2: return "bus";
   case 3: return "io_chunkInfo_payload";
   case 4: return "TRBSink";
   case 5: return "bus";
   case 6: return "usbReadsPerMs";
   case 7: return "usb_irq";
   case 8: return "usbPiped_aw";
   case 9: return "usbPiped_ar";
   case 10: return "usbPiped_r";
   case 11: return "usbPiped_w";
   case 12: return "usbPiped_b";
   case 13: return "wb2AxiPiped_aw";
   case 14: return "wb2AxiPiped_ar";
   case 15: return "wb2AxiPiped_r";
   case 16: return "wb2AxiPiped_w";
   case 17: return "wb2AxiPiped_b";
    }
    return "UNKNOWN";
}
#define io_chunkInfo_payload_TIME_BIT_WIDTH 79
#define chunkInfo_last_BIT_OFFSET        0
#define chunkInfo_last_BIT_WIDTH         1
#define chunkInfo_fragment_BIT_OFFSET    1
#define chunkInfo_fragment_BIT_WIDTH     10
static GlobalLogger_Fragment_11_t GlobalLogger_parse_chunkInfo(const GlobalLogger_transaction* tx) {
	return (GlobalLogger_Fragment_11_t) {
		.last = GlobalLogger_parse_field(tx, chunkInfo_last_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, chunkInfo_last_BIT_WIDTH),
		.fragment = GlobalLogger_parse_field(tx, chunkInfo_fragment_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, chunkInfo_fragment_BIT_WIDTH),
	};
}
#define TRBSink_TIME_BIT_WIDTH 75
#define TRBSink_stateReg_BIT_OFFSET      0
#define TRBSink_stateReg_BIT_WIDTH       4
#define TRBSink_io_enable_BIT_OFFSET     4
#define TRBSink_io_enable_BIT_WIDTH      1
#define TRBSink_io_recheck_BIT_OFFSET    5
#define TRBSink_io_recheck_BIT_WIDTH     1
#define TRBSink_recheckReg_BIT_OFFSET    6
#define TRBSink_recheckReg_BIT_WIDTH     1
#define TRBSink_io_trbID_BIT_OFFSET      7
#define TRBSink_io_trbID_BIT_WIDTH       8
static GlobalLogger_TRBSink_15_t GlobalLogger_parse_TRBSink(const GlobalLogger_transaction* tx) {
	return (GlobalLogger_TRBSink_15_t) {
		.stateReg = GlobalLogger_parse_field(tx, TRBSink_stateReg_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, TRBSink_stateReg_BIT_WIDTH),
		.io_enable = GlobalLogger_parse_field(tx, TRBSink_io_enable_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, TRBSink_io_enable_BIT_WIDTH),
		.io_recheck = GlobalLogger_parse_field(tx, TRBSink_io_recheck_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, TRBSink_io_recheck_BIT_WIDTH),
		.recheckReg = GlobalLogger_parse_field(tx, TRBSink_recheckReg_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, TRBSink_recheckReg_BIT_WIDTH),
		.io_trbID = GlobalLogger_parse_field(tx, TRBSink_io_trbID_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, TRBSink_io_trbID_BIT_WIDTH),
	};
}
#define bus_TIME_BIT_WIDTH 21
#define bus_write_BIT_OFFSET             0
#define bus_write_BIT_WIDTH              1
#define bus_address_BIT_OFFSET           1
#define bus_address_BIT_WIDTH            32
#define bus_data_BIT_OFFSET              33
#define bus_data_BIT_WIDTH               32
#define bus_mask_BIT_OFFSET              65
#define bus_mask_BIT_WIDTH               4
static GlobalLogger_PipelinedMemoryBusCmd_69_t GlobalLogger_parse_bus(const GlobalLogger_transaction* tx) {
	return (GlobalLogger_PipelinedMemoryBusCmd_69_t) {
		.write = GlobalLogger_parse_field(tx, bus_write_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, bus_write_BIT_WIDTH),
		.address = GlobalLogger_parse_field(tx, bus_address_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, bus_address_BIT_WIDTH),
		.data = GlobalLogger_parse_field(tx, bus_data_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, bus_data_BIT_WIDTH),
		.mask = GlobalLogger_parse_field(tx, bus_mask_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, bus_mask_BIT_WIDTH),
	};
}
#define io_chunkInfo_payload_TIME_BIT_WIDTH 79
#define TRBSink_TIME_BIT_WIDTH 75
#define bus_TIME_BIT_WIDTH 21
#define usbReadsPerMs_TIME_BIT_WIDTH 26

#define usbReadsPerMs_TIME_BIT_WIDTH 26
#define usbReadsPerMs_BIT_OFFSET         0
#define usbReadsPerMs_BIT_WIDTH          64
static GlobalLogger_usbReadsPerMs_64_t GlobalLogger_parse_usbReadsPerMs(const GlobalLogger_transaction* tx) {
   return (GlobalLogger_usbReadsPerMs_64_t){ .value = GlobalLogger_parse_field(tx, usbReadsPerMs_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, usbReadsPerMs_BIT_WIDTH) };
}

#define usb_irq_TIME_BIT_WIDTH 86
#define usb_irq_usbCore_irq_BIT_OFFSET   0
#define usb_irq_usbCore_irq_BIT_WIDTH    1
#define usb_irq_mgrs_0_io_irq_BIT_OFFSET 1
#define usb_irq_mgrs_0_io_irq_BIT_WIDTH  1
#define usb_irq_mgrs_1_io_irq_BIT_OFFSET 2
#define usb_irq_mgrs_1_io_irq_BIT_WIDTH  1
#define usb_irq_irq_BIT_OFFSET           3
#define usb_irq_irq_BIT_WIDTH            1
static GlobalLogger_usb_irq_4_t GlobalLogger_parse_usb_irq(const GlobalLogger_transaction* tx) {
	return (GlobalLogger_usb_irq_4_t) {
		.usbCore_irq = GlobalLogger_parse_field(tx, usb_irq_usbCore_irq_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, usb_irq_usbCore_irq_BIT_WIDTH),
		.mgrs_0_io_irq = GlobalLogger_parse_field(tx, usb_irq_mgrs_0_io_irq_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, usb_irq_mgrs_0_io_irq_BIT_WIDTH),
		.mgrs_1_io_irq = GlobalLogger_parse_field(tx, usb_irq_mgrs_1_io_irq_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, usb_irq_mgrs_1_io_irq_BIT_WIDTH),
		.irq = GlobalLogger_parse_field(tx, usb_irq_irq_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, usb_irq_irq_BIT_WIDTH),
	};
}
#define usbPiped_aw_TIME_BIT_WIDTH 45
#define usbPiped_aw_addr_BIT_OFFSET      0
#define usbPiped_aw_addr_BIT_WIDTH       24
#define usbPiped_aw_id_BIT_OFFSET        24
#define usbPiped_aw_id_BIT_WIDTH         8
#define usbPiped_aw_len_BIT_OFFSET       32
#define usbPiped_aw_len_BIT_WIDTH        8
#define usbPiped_aw_size_BIT_OFFSET      40
#define usbPiped_aw_size_BIT_WIDTH       3
#define usbPiped_aw_burst_BIT_OFFSET     43
#define usbPiped_aw_burst_BIT_WIDTH      2
static GlobalLogger_Axi4Aw_45_t GlobalLogger_parse_usbPiped_aw(const GlobalLogger_transaction* tx) {
	return (GlobalLogger_Axi4Aw_45_t) {
		.addr = GlobalLogger_parse_field(tx, usbPiped_aw_addr_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, usbPiped_aw_addr_BIT_WIDTH),
		.id = GlobalLogger_parse_field(tx, usbPiped_aw_id_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, usbPiped_aw_id_BIT_WIDTH),
		.len = GlobalLogger_parse_field(tx, usbPiped_aw_len_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, usbPiped_aw_len_BIT_WIDTH),
		.size = GlobalLogger_parse_field(tx, usbPiped_aw_size_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, usbPiped_aw_size_BIT_WIDTH),
		.burst = GlobalLogger_parse_field(tx, usbPiped_aw_burst_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, usbPiped_aw_burst_BIT_WIDTH),
	};
}
#define usbPiped_ar_TIME_BIT_WIDTH 45
#define usbPiped_ar_addr_BIT_OFFSET      0
#define usbPiped_ar_addr_BIT_WIDTH       24
#define usbPiped_ar_id_BIT_OFFSET        24
#define usbPiped_ar_id_BIT_WIDTH         8
#define usbPiped_ar_len_BIT_OFFSET       32
#define usbPiped_ar_len_BIT_WIDTH        8
#define usbPiped_ar_size_BIT_OFFSET      40
#define usbPiped_ar_size_BIT_WIDTH       3
#define usbPiped_ar_burst_BIT_OFFSET     43
#define usbPiped_ar_burst_BIT_WIDTH      2
static GlobalLogger_Axi4Ar_45_t GlobalLogger_parse_usbPiped_ar(const GlobalLogger_transaction* tx) {
	return (GlobalLogger_Axi4Ar_45_t) {
		.addr = GlobalLogger_parse_field(tx, usbPiped_ar_addr_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, usbPiped_ar_addr_BIT_WIDTH),
		.id = GlobalLogger_parse_field(tx, usbPiped_ar_id_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, usbPiped_ar_id_BIT_WIDTH),
		.len = GlobalLogger_parse_field(tx, usbPiped_ar_len_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, usbPiped_ar_len_BIT_WIDTH),
		.size = GlobalLogger_parse_field(tx, usbPiped_ar_size_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, usbPiped_ar_size_BIT_WIDTH),
		.burst = GlobalLogger_parse_field(tx, usbPiped_ar_burst_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, usbPiped_ar_burst_BIT_WIDTH),
	};
}
#define usbPiped_r_TIME_BIT_WIDTH 15
#define usbPiped_r_data_BIT_OFFSET       0
#define usbPiped_r_data_BIT_WIDTH        64
#define usbPiped_r_id_BIT_OFFSET         64
#define usbPiped_r_id_BIT_WIDTH          8
#define usbPiped_r_resp_BIT_OFFSET       72
#define usbPiped_r_resp_BIT_WIDTH        2
#define usbPiped_r_last_BIT_OFFSET       74
#define usbPiped_r_last_BIT_WIDTH        1
static GlobalLogger_Axi4R_75_t GlobalLogger_parse_usbPiped_r(const GlobalLogger_transaction* tx) {
	return (GlobalLogger_Axi4R_75_t) {
		.data = GlobalLogger_parse_field(tx, usbPiped_r_data_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, usbPiped_r_data_BIT_WIDTH),
		.id = GlobalLogger_parse_field(tx, usbPiped_r_id_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, usbPiped_r_id_BIT_WIDTH),
		.resp = GlobalLogger_parse_field(tx, usbPiped_r_resp_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, usbPiped_r_resp_BIT_WIDTH),
		.last = GlobalLogger_parse_field(tx, usbPiped_r_last_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, usbPiped_r_last_BIT_WIDTH),
	};
}
#define usbPiped_w_TIME_BIT_WIDTH 17
#define usbPiped_w_data_BIT_OFFSET       0
#define usbPiped_w_data_BIT_WIDTH        64
#define usbPiped_w_strb_BIT_OFFSET       64
#define usbPiped_w_strb_BIT_WIDTH        8
#define usbPiped_w_last_BIT_OFFSET       72
#define usbPiped_w_last_BIT_WIDTH        1
static GlobalLogger_Axi4W_73_t GlobalLogger_parse_usbPiped_w(const GlobalLogger_transaction* tx) {
	return (GlobalLogger_Axi4W_73_t) {
		.data = GlobalLogger_parse_field(tx, usbPiped_w_data_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, usbPiped_w_data_BIT_WIDTH),
		.strb = GlobalLogger_parse_field(tx, usbPiped_w_strb_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, usbPiped_w_strb_BIT_WIDTH),
		.last = GlobalLogger_parse_field(tx, usbPiped_w_last_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, usbPiped_w_last_BIT_WIDTH),
	};
}
#define usbPiped_b_TIME_BIT_WIDTH 80
#define usbPiped_b_id_BIT_OFFSET         0
#define usbPiped_b_id_BIT_WIDTH          8
#define usbPiped_b_resp_BIT_OFFSET       8
#define usbPiped_b_resp_BIT_WIDTH        2
static GlobalLogger_Axi4B_10_t GlobalLogger_parse_usbPiped_b(const GlobalLogger_transaction* tx) {
	return (GlobalLogger_Axi4B_10_t) {
		.id = GlobalLogger_parse_field(tx, usbPiped_b_id_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, usbPiped_b_id_BIT_WIDTH),
		.resp = GlobalLogger_parse_field(tx, usbPiped_b_resp_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, usbPiped_b_resp_BIT_WIDTH),
	};
}
#define wb2AxiPiped_aw_TIME_BIT_WIDTH 45
#define wb2AxiPiped_aw_addr_BIT_OFFSET   0
#define wb2AxiPiped_aw_addr_BIT_WIDTH    24
#define wb2AxiPiped_aw_id_BIT_OFFSET     24
#define wb2AxiPiped_aw_id_BIT_WIDTH      8
#define wb2AxiPiped_aw_len_BIT_OFFSET    32
#define wb2AxiPiped_aw_len_BIT_WIDTH     8
#define wb2AxiPiped_aw_size_BIT_OFFSET   40
#define wb2AxiPiped_aw_size_BIT_WIDTH    3
#define wb2AxiPiped_aw_burst_BIT_OFFSET  43
#define wb2AxiPiped_aw_burst_BIT_WIDTH   2
static GlobalLogger_Axi4Aw_45_t GlobalLogger_parse_wb2AxiPiped_aw(const GlobalLogger_transaction* tx) {
	return (GlobalLogger_Axi4Aw_45_t) {
		.addr = GlobalLogger_parse_field(tx, wb2AxiPiped_aw_addr_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, wb2AxiPiped_aw_addr_BIT_WIDTH),
		.id = GlobalLogger_parse_field(tx, wb2AxiPiped_aw_id_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, wb2AxiPiped_aw_id_BIT_WIDTH),
		.len = GlobalLogger_parse_field(tx, wb2AxiPiped_aw_len_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, wb2AxiPiped_aw_len_BIT_WIDTH),
		.size = GlobalLogger_parse_field(tx, wb2AxiPiped_aw_size_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, wb2AxiPiped_aw_size_BIT_WIDTH),
		.burst = GlobalLogger_parse_field(tx, wb2AxiPiped_aw_burst_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, wb2AxiPiped_aw_burst_BIT_WIDTH),
	};
}
#define wb2AxiPiped_ar_TIME_BIT_WIDTH 45
#define wb2AxiPiped_ar_addr_BIT_OFFSET   0
#define wb2AxiPiped_ar_addr_BIT_WIDTH    24
#define wb2AxiPiped_ar_id_BIT_OFFSET     24
#define wb2AxiPiped_ar_id_BIT_WIDTH      8
#define wb2AxiPiped_ar_len_BIT_OFFSET    32
#define wb2AxiPiped_ar_len_BIT_WIDTH     8
#define wb2AxiPiped_ar_size_BIT_OFFSET   40
#define wb2AxiPiped_ar_size_BIT_WIDTH    3
#define wb2AxiPiped_ar_burst_BIT_OFFSET  43
#define wb2AxiPiped_ar_burst_BIT_WIDTH   2
static GlobalLogger_Axi4Ar_45_t GlobalLogger_parse_wb2AxiPiped_ar(const GlobalLogger_transaction* tx) {
	return (GlobalLogger_Axi4Ar_45_t) {
		.addr = GlobalLogger_parse_field(tx, wb2AxiPiped_ar_addr_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, wb2AxiPiped_ar_addr_BIT_WIDTH),
		.id = GlobalLogger_parse_field(tx, wb2AxiPiped_ar_id_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, wb2AxiPiped_ar_id_BIT_WIDTH),
		.len = GlobalLogger_parse_field(tx, wb2AxiPiped_ar_len_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, wb2AxiPiped_ar_len_BIT_WIDTH),
		.size = GlobalLogger_parse_field(tx, wb2AxiPiped_ar_size_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, wb2AxiPiped_ar_size_BIT_WIDTH),
		.burst = GlobalLogger_parse_field(tx, wb2AxiPiped_ar_burst_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, wb2AxiPiped_ar_burst_BIT_WIDTH),
	};
}
#define wb2AxiPiped_r_TIME_BIT_WIDTH 15
#define wb2AxiPiped_r_data_BIT_OFFSET    0
#define wb2AxiPiped_r_data_BIT_WIDTH     64
#define wb2AxiPiped_r_id_BIT_OFFSET      64
#define wb2AxiPiped_r_id_BIT_WIDTH       8
#define wb2AxiPiped_r_resp_BIT_OFFSET    72
#define wb2AxiPiped_r_resp_BIT_WIDTH     2
#define wb2AxiPiped_r_last_BIT_OFFSET    74
#define wb2AxiPiped_r_last_BIT_WIDTH     1
static GlobalLogger_Axi4R_75_t GlobalLogger_parse_wb2AxiPiped_r(const GlobalLogger_transaction* tx) {
	return (GlobalLogger_Axi4R_75_t) {
		.data = GlobalLogger_parse_field(tx, wb2AxiPiped_r_data_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, wb2AxiPiped_r_data_BIT_WIDTH),
		.id = GlobalLogger_parse_field(tx, wb2AxiPiped_r_id_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, wb2AxiPiped_r_id_BIT_WIDTH),
		.resp = GlobalLogger_parse_field(tx, wb2AxiPiped_r_resp_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, wb2AxiPiped_r_resp_BIT_WIDTH),
		.last = GlobalLogger_parse_field(tx, wb2AxiPiped_r_last_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, wb2AxiPiped_r_last_BIT_WIDTH),
	};
}
#define wb2AxiPiped_w_TIME_BIT_WIDTH 17
#define wb2AxiPiped_w_data_BIT_OFFSET    0
#define wb2AxiPiped_w_data_BIT_WIDTH     64
#define wb2AxiPiped_w_strb_BIT_OFFSET    64
#define wb2AxiPiped_w_strb_BIT_WIDTH     8
#define wb2AxiPiped_w_last_BIT_OFFSET    72
#define wb2AxiPiped_w_last_BIT_WIDTH     1
static GlobalLogger_Axi4W_73_t GlobalLogger_parse_wb2AxiPiped_w(const GlobalLogger_transaction* tx) {
	return (GlobalLogger_Axi4W_73_t) {
		.data = GlobalLogger_parse_field(tx, wb2AxiPiped_w_data_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, wb2AxiPiped_w_data_BIT_WIDTH),
		.strb = GlobalLogger_parse_field(tx, wb2AxiPiped_w_strb_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, wb2AxiPiped_w_strb_BIT_WIDTH),
		.last = GlobalLogger_parse_field(tx, wb2AxiPiped_w_last_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, wb2AxiPiped_w_last_BIT_WIDTH),
	};
}
#define wb2AxiPiped_b_TIME_BIT_WIDTH 80
#define wb2AxiPiped_b_id_BIT_OFFSET      0
#define wb2AxiPiped_b_id_BIT_WIDTH       8
#define wb2AxiPiped_b_resp_BIT_OFFSET    8
#define wb2AxiPiped_b_resp_BIT_WIDTH     2
static GlobalLogger_Axi4B_10_t GlobalLogger_parse_wb2AxiPiped_b(const GlobalLogger_transaction* tx) {
	return (GlobalLogger_Axi4B_10_t) {
		.id = GlobalLogger_parse_field(tx, wb2AxiPiped_b_id_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, wb2AxiPiped_b_id_BIT_WIDTH),
		.resp = GlobalLogger_parse_field(tx, wb2AxiPiped_b_resp_BIT_OFFSET + GlobalLogger_INDEX_BITS + 1 /* VALID bit */, wb2AxiPiped_b_resp_BIT_WIDTH),
	};
}
void GlobalLogger_handle_transaction(GlobalLogger_ctx* ctx, uint8_t id, const struct GlobalLogger_transaction* tx);
void GlobalLogger_handle_Axi4Aw_45 (GlobalLogger_ctx* ctx, uint64_t time, uint8_t id, const GlobalLogger_Axi4Aw_45_t pkt);
void GlobalLogger_handle_usb_irq_4 (GlobalLogger_ctx* ctx, uint64_t time, uint8_t id, const GlobalLogger_usb_irq_4_t pkt);
void GlobalLogger_handle_Axi4R_75 (GlobalLogger_ctx* ctx, uint64_t time, uint8_t id, const GlobalLogger_Axi4R_75_t pkt);
void GlobalLogger_handle_Axi4B_10 (GlobalLogger_ctx* ctx, uint64_t time, uint8_t id, const GlobalLogger_Axi4B_10_t pkt);
void GlobalLogger_handle_Axi4Ar_45 (GlobalLogger_ctx* ctx, uint64_t time, uint8_t id, const GlobalLogger_Axi4Ar_45_t pkt);
void GlobalLogger_handle_PipelinedMemoryBusCmd_69 (GlobalLogger_ctx* ctx, uint64_t time, uint8_t id, const GlobalLogger_PipelinedMemoryBusCmd_69_t pkt);
void GlobalLogger_handle_TRBSink_15 (GlobalLogger_ctx* ctx, uint64_t time, uint8_t id, const GlobalLogger_TRBSink_15_t pkt);
void GlobalLogger_handle_usbReadsPerMs_64 (GlobalLogger_ctx* ctx, uint64_t time, uint8_t id, const GlobalLogger_usbReadsPerMs_64_t pkt);
void GlobalLogger_handle_Axi4W_73 (GlobalLogger_ctx* ctx, uint64_t time, uint8_t id, const GlobalLogger_Axi4W_73_t pkt);
void GlobalLogger_handle_Fragment_11 (GlobalLogger_ctx* ctx, uint64_t time, uint8_t id, const GlobalLogger_Fragment_11_t pkt);

static uint8_t GlobalLogger_get_id(const struct GlobalLogger_transaction* tx){ return GlobalLogger_parse_field(tx, 1, 5); }
static void GlobalLogger_handle(GlobalLogger_ctx* ctx, const struct GlobalLogger_transaction* tx, uint32_t mask){
    uint8_t id = GlobalLogger_get_id(tx);
    if(mask & (1 << id)) return;
    GlobalLogger_handle_transaction(ctx, id, tx);
    switch(id) {

   case 0: {
      ctx->last_timestamp = GlobalLogger_full_time(tx, ctx->last_timestamp, io_chunkInfo_payload_TIME_BIT_WIDTH);
      GlobalLogger_handle_Fragment_11(ctx, ctx->last_timestamp, id, GlobalLogger_parse_chunkInfo(tx));
      break;
    }
   case 1: {
      ctx->last_timestamp = GlobalLogger_full_time(tx, ctx->last_timestamp, TRBSink_TIME_BIT_WIDTH);
      GlobalLogger_handle_TRBSink_15(ctx, ctx->last_timestamp, id, GlobalLogger_parse_TRBSink(tx));
      break;
    }
   case 2: {
      ctx->last_timestamp = GlobalLogger_full_time(tx, ctx->last_timestamp, bus_TIME_BIT_WIDTH);
      GlobalLogger_handle_PipelinedMemoryBusCmd_69(ctx, ctx->last_timestamp, id, GlobalLogger_parse_bus(tx));
      break;
    }
   case 3: {
      ctx->last_timestamp = GlobalLogger_full_time(tx, ctx->last_timestamp, io_chunkInfo_payload_TIME_BIT_WIDTH);
      GlobalLogger_handle_Fragment_11(ctx, ctx->last_timestamp, id, GlobalLogger_parse_chunkInfo(tx));
      break;
    }
   case 4: {
      ctx->last_timestamp = GlobalLogger_full_time(tx, ctx->last_timestamp, TRBSink_TIME_BIT_WIDTH);
      GlobalLogger_handle_TRBSink_15(ctx, ctx->last_timestamp, id, GlobalLogger_parse_TRBSink(tx));
      break;
    }
   case 5: {
      ctx->last_timestamp = GlobalLogger_full_time(tx, ctx->last_timestamp, bus_TIME_BIT_WIDTH);
      GlobalLogger_handle_PipelinedMemoryBusCmd_69(ctx, ctx->last_timestamp, id, GlobalLogger_parse_bus(tx));
      break;
    }
   case 6: {
      ctx->last_timestamp = GlobalLogger_full_time(tx, ctx->last_timestamp, usbReadsPerMs_TIME_BIT_WIDTH);
      GlobalLogger_handle_usbReadsPerMs_64(ctx, ctx->last_timestamp, id, GlobalLogger_parse_usbReadsPerMs(tx));
      break;
    }
   case 7: {
      ctx->last_timestamp = GlobalLogger_full_time(tx, ctx->last_timestamp, usb_irq_TIME_BIT_WIDTH);
      GlobalLogger_handle_usb_irq_4(ctx, ctx->last_timestamp, id, GlobalLogger_parse_usb_irq(tx));
      break;
    }
   case 8: {
      ctx->last_timestamp = GlobalLogger_full_time(tx, ctx->last_timestamp, usbPiped_aw_TIME_BIT_WIDTH);
      GlobalLogger_handle_Axi4Aw_45(ctx, ctx->last_timestamp, id, GlobalLogger_parse_usbPiped_aw(tx));
      break;
    }
   case 9: {
      ctx->last_timestamp = GlobalLogger_full_time(tx, ctx->last_timestamp, usbPiped_ar_TIME_BIT_WIDTH);
      GlobalLogger_handle_Axi4Ar_45(ctx, ctx->last_timestamp, id, GlobalLogger_parse_usbPiped_ar(tx));
      break;
    }
   case 10: {
      ctx->last_timestamp = GlobalLogger_full_time(tx, ctx->last_timestamp, usbPiped_r_TIME_BIT_WIDTH);
      GlobalLogger_handle_Axi4R_75(ctx, ctx->last_timestamp, id, GlobalLogger_parse_usbPiped_r(tx));
      break;
    }
   case 11: {
      ctx->last_timestamp = GlobalLogger_full_time(tx, ctx->last_timestamp, usbPiped_w_TIME_BIT_WIDTH);
      GlobalLogger_handle_Axi4W_73(ctx, ctx->last_timestamp, id, GlobalLogger_parse_usbPiped_w(tx));
      break;
    }
   case 12: {
      ctx->last_timestamp = GlobalLogger_full_time(tx, ctx->last_timestamp, usbPiped_b_TIME_BIT_WIDTH);
      GlobalLogger_handle_Axi4B_10(ctx, ctx->last_timestamp, id, GlobalLogger_parse_usbPiped_b(tx));
      break;
    }
   case 13: {
      ctx->last_timestamp = GlobalLogger_full_time(tx, ctx->last_timestamp, wb2AxiPiped_aw_TIME_BIT_WIDTH);
      GlobalLogger_handle_Axi4Aw_45(ctx, ctx->last_timestamp, id, GlobalLogger_parse_wb2AxiPiped_aw(tx));
      break;
    }
   case 14: {
      ctx->last_timestamp = GlobalLogger_full_time(tx, ctx->last_timestamp, wb2AxiPiped_ar_TIME_BIT_WIDTH);
      GlobalLogger_handle_Axi4Ar_45(ctx, ctx->last_timestamp, id, GlobalLogger_parse_wb2AxiPiped_ar(tx));
      break;
    }
   case 15: {
      ctx->last_timestamp = GlobalLogger_full_time(tx, ctx->last_timestamp, wb2AxiPiped_r_TIME_BIT_WIDTH);
      GlobalLogger_handle_Axi4R_75(ctx, ctx->last_timestamp, id, GlobalLogger_parse_wb2AxiPiped_r(tx));
      break;
    }
   case 16: {
      ctx->last_timestamp = GlobalLogger_full_time(tx, ctx->last_timestamp, wb2AxiPiped_w_TIME_BIT_WIDTH);
      GlobalLogger_handle_Axi4W_73(ctx, ctx->last_timestamp, id, GlobalLogger_parse_wb2AxiPiped_w(tx));
      break;
    }
   case 17: {
      ctx->last_timestamp = GlobalLogger_full_time(tx, ctx->last_timestamp, wb2AxiPiped_b_TIME_BIT_WIDTH);
      GlobalLogger_handle_Axi4B_10(ctx, ctx->last_timestamp, id, GlobalLogger_parse_wb2AxiPiped_b(tx));
      break;
    }

   case 31: ctx->last_timestamp = GlobalLogger_parse_field(tx, 1 + 26, 64); break;
   default: fprintf(stderr, "Unknown id %d\n", id);
  }
}
      
