#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/usb/udc.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/bos.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/barrier.h>

LOG_MODULE_REGISTER(event_logger, LOG_LEVEL_DBG);
#include <zephyr/shell/shell.h>

#define GlobalLogger_INDEX_BITS 2
#define GlobalLogger_SIGNATURE 0x2425af75
#define GlobalLogger_EVENT_COUNT 3
typedef struct GlobalLogger_info_t {
   uint32_t ctrl;
   uint32_t captured_events;
   uint32_t checksum;
   uint32_t sysclk_lsb;
   uint32_t fifo_occupancy;
   uint32_t inactive_mask;
   uint32_t signature;
   uint32_t dropped_events;
   uint32_t event_counter[3];
} GlobalLogger_info_t;

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
  for(int i = 0;i < 3;i++) {
     rtn.event_counter[i] = base[56/4 + i];
  }
  return rtn;
}

typedef uint32_t u32;


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

static uint64_t strb_to_mask(uint8_t strb) {                                                                                                                                                                                                                     uint64_t mask = 0;
   for(int i = 0;i < 8;i++) {
     if(strb & 1) {
       mask = mask |  (0xffll << (i * 8));                                                                                                                                                                                                                  }
     strb = strb >> 1;
   }

   return mask;
}

static uint64_t unmask_data(uint64_t data, uint8_t strb) {
  uint64_t mask = strb_to_mask(strb);
  data = data & mask;
  if(mask == 0) {
    LOG_WRN("Impossible mask from strb %x", strb);
    return 0;
  }
  while((mask & 1) == 0) {
        mask = mask >> 1;
        data = data >> 1;
    }
    return data;
}
#define SHELL_OR_LOG(sh, ...) if(sh) { shell_print(sh, __VA_ARGS__); } else { LOG_INF(__VA_ARGS__); }

static GlobalLogger_transaction prior_txs[0xff] = {0};

static uint32_t histogram[255] = {}, old_histogram[255] = {};
static uint64_t prior_times[255] = {};

/* void GlobalLogger_handle_transaction(GlobalLogger_ctx* ctx, uint8_t id, const struct GlobalLogger_transaction* tx) { */
/*     prior_txs[id] = *tx; */
/*     histogram[id]++; */
/*     //SHELL_OR_LOG(0, "EVT 0x%08x 0x%08x 0x%08x", tx->l[2], tx->l[1], tx->l[0]); */
/* } */

#define NAME_FIELD(field) #field ": %x "
#define VALUE_FIELD(field) , pkt.field

#define NAME_BOOL_FIELD(field) "%s "
#define VALUE_BOOL_FIELD(field) , _Generic(pkt.field, bool : ((pkt.field ) ? (#field " ") : ""), default : "")

static int _write_callback(const struct device *dev, struct net_buf *buf, int err)
{
	struct udc_buf_info *bi = udc_get_buf_info(buf);

	if (err) {
		LOG_ERR("USB write error %d", err);
		return err;
	}
	LOG_DBG("%s: buf=%p data=%p len=%d size=%d zlp=%d", __func__,
		buf, buf->data, buf->len, buf->size, bi->zlp);

	/* Freeing here will unblock the memory pool */
	net_buf_unref(buf);
	return 0;
}

//#define SHELL_OR_LOG(sh, ...) LOG_INF(__VA_ARGS__);

static uint32_t inactive_mask = 0;
static bool do_log = 0;
static uint32_t dropped = 0, captured = 0;
volatile uint32_t* ip_address =  (volatile uint32_t*)0xb9000000;

void transaction_logger(void *, void *, void *) {
  //volatile uint32_t* dropped_ptr = (volatile uint32_t*)0xb9000000;

  //cdc_raw_set_write_callback(CDC0, &_write_callback);
  GlobalLogger_info_t info = GlobalLogger_info_get(ip_address);

  if(info.signature != GlobalLogger_SIGNATURE) {
    LOG_WRN("Mismatched signature -- %x vs %x", info.signature, GlobalLogger_SIGNATURE);
  }

  GlobalLogger_ctx ctx = { 0 };
  while(1) {
      k_sleep(K_MSEC(10));
    if(!do_log) continue;

    info = GlobalLogger_info_get(ip_address);

    uint32_t dropped_new = info.dropped_events;
    if(dropped_new != dropped) {
      dropped = dropped_new;
      SHELL_OR_LOG(0, "Dropped: %d", dropped);
    }

    uint32_t captured_new = info.captured_events;
    if(captured_new != captured) {
      captured = captured_new;
        SHELL_OR_LOG(0, "Captured: %d Dropped: %d", captured, dropped);
    }

    while (do_log && GlobalLogger_poll(&ctx, ip_address, 0)) {
        k_yield();
    }
  }
}


K_THREAD_DEFINE(my_tid, 4096,
 		transaction_logger, NULL, NULL, NULL,
 		14, 0, 0);

static int cmd_info(const struct shell *sh, size_t argc, char **argv) {
  GlobalLogger_info_t info = GlobalLogger_info_get(ip_address);
  SHELL_OR_LOG(sh, "Info for %p:", (void*)ip_address);
  SHELL_OR_LOG(sh, "  ctrl            = %d", info.ctrl);
  SHELL_OR_LOG(sh, "  dropped_events  = %d", info.dropped_events);
  SHELL_OR_LOG(sh, "  captured_events = %d", info.captured_events);
  SHELL_OR_LOG(sh, "  checksum        = 0x%x", info.checksum);
  SHELL_OR_LOG(sh, "  sysclk_lsb      = 0x%x", info.sysclk_lsb);
  SHELL_OR_LOG(sh, "  fifo_occupancy  = %d", info.fifo_occupancy);
  SHELL_OR_LOG(sh, "  inactive_mask   = 0x%x", info.inactive_mask);
  SHELL_OR_LOG(sh, "  signature       = 0x%x (expected; 0x%x)", info.signature, GlobalLogger_SIGNATURE);
  SHELL_OR_LOG(sh, "Hits:");
  for(int i = 0;i < GlobalLogger_EVENT_COUNT;i++) {
    SHELL_OR_LOG(sh, "%s %20s [%3d]: %11d -> %11d -> %11d",
		 ((info.inactive_mask & (1 << i)) == 0) ? " " : "*",
		 GlobalLogger_get_id_name(i), i, info.event_counter[i], info.event_counter[i] - old_histogram[i], info.event_counter[i]);
    old_histogram[i] = info.event_counter[i];
  }

  return 0;
}

static int cmd_dump(const struct shell *sh, size_t argc, char **argv) {
  do_log = !do_log;
  return 0;
}

static int cmd_clear(const struct shell *sh, size_t argc, char **argv) {
  ip_address[7] = 0;
  k_sleep(K_MSEC(10));
  ip_address[7] = 0;

  return cmd_info(sh, argc, argv);
}
static int cmd_trigger(const struct shell *sh, size_t argc, char **argv) {
    ip_address[8] = 0xffffffff;
    return 0;
}

static int cmd_mask(const struct shell *sh, size_t argc, char **argv) {
  GlobalLogger_info_t info = GlobalLogger_info_get(ip_address);
  if(argc < 2) {
    info.inactive_mask |= 0xffffffff;
  } else {

    for(int i = 1;i < argc;i++) {
      info.inactive_mask |= (1 << atoi(argv[i]));
    }
  }
  SHELL_OR_LOG(sh, "New mask %x", info.inactive_mask);
  ip_address[9] = info.inactive_mask;
  inactive_mask = info.inactive_mask;
  return cmd_info(sh, argc, argv);
}


static int cmd_unmask(const struct shell *sh, size_t argc, char **argv) {
  if(argc < 2) return -1;
  GlobalLogger_info_t info = GlobalLogger_info_get(ip_address);

  for(int i = 1;i < argc;i++) {
    info.inactive_mask &= ~(1 << atoi(argv[i]));
  }

  SHELL_OR_LOG(sh, "New mask %x", info.inactive_mask);
  ip_address[9] = info.inactive_mask;
  inactive_mask = info.inactive_mask;
  return cmd_info(sh, argc, argv);
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_tx,
			       SHELL_CMD(dump, NULL,
					     "Dump the transacation log\n"
					     "Usage: dump",
					     cmd_dump),
			       SHELL_CMD(info, NULL,
					     "Dump the transacation log info\n"
					     "Usage: info",
					     cmd_info),
			       SHELL_CMD(clear, NULL,
					     "Clear the transacation log info\n"
					     "Usage: clear",
					     cmd_clear),
			       SHELL_CMD(trigger, NULL,
					 "Manually trigger events",
					 cmd_trigger),
 			       SHELL_CMD(mask, NULL,
					 "",
					 cmd_mask),
			       SHELL_CMD(unmask, NULL,
					 "",
					 cmd_unmask),
			       SHELL_SUBCMD_SET_END);


SHELL_CMD_REGISTER(event_logger, &sub_tx, "Inspect registers and such", NULL);

