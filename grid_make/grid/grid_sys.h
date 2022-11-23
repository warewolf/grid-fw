#ifndef GRID_SYS_H_INCLUDED
#define GRID_SYS_H_INCLUDED

#include "grid_module.h"
#include "grid_buf.h"


int32_t grid_utility_map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max);




#define GRID_SYS_BANK_MAXNUMBER					4




struct grid_sys_model
{
	
	uint32_t uptime;
	uint8_t reset_cause;
	
	uint8_t editor_connected;

	uint8_t midirx_any_enabled;
	uint8_t midirx_sync_enabled;



	
	uint8_t bank_activebank_number;

	uint8_t mapmodestate;
	
	uint32_t uniqueid_array[4];
	
	uint8_t bank_active_changed;
	
	uint8_t bank_setting_changed_flag;
	
	uint8_t bank_enabled[GRID_SYS_BANK_MAXNUMBER];
	
	uint8_t bank_color_r[GRID_SYS_BANK_MAXNUMBER];
	uint8_t bank_color_g[GRID_SYS_BANK_MAXNUMBER];
	uint8_t bank_color_b[GRID_SYS_BANK_MAXNUMBER];
	
	uint8_t bank_activebank_valid;
	
	uint8_t bank_activebank_color_r;
	uint8_t bank_activebank_color_g;
	uint8_t bank_activebank_color_b;
	
	uint8_t bank_init_flag;
	
	uint32_t realtime;


	uint32_t hwfcg;
	
	int8_t module_x;
	int8_t module_y;
	uint8_t module_rot;

	
};

volatile struct grid_sys_model grid_sys_state;


void grid_sys_init(struct grid_sys_model* mod); 







#define GRID_SYS_DEFAULT_POSITION 127
#define GRID_SYS_LOCAL_POSITION 255
#define GRID_SYS_GLOBAL_POSITION 0
#define GRID_SYS_DEFAULT_ROTATION 0
#define GRID_SYS_DEFAULT_AGE 0


void grid_sys_recall_configuration(struct grid_sys_model* sys, uint8_t bank);

void grid_debug_print_text(uint8_t* str);
void grid_websocket_print_text(uint8_t* str);

void grid_debug_printf(char const *fmt, ...);


struct io_descriptor *grid_sys_north_io;
struct io_descriptor *grid_sys_east_io;
struct io_descriptor *grid_sys_south_io;
struct io_descriptor *grid_sys_west_io;



uint32_t grid_sys_get_uptime(struct grid_sys_model* mod);

uint32_t grid_sys_rtc_get_time(struct grid_sys_model* mod);
void grid_sys_rtc_set_time(struct grid_sys_model* mod, uint32_t tvalue);
uint32_t grid_sys_rtc_get_elapsed_time(struct grid_sys_model* mod, uint32_t told);
void grid_sys_rtc_tick_time(struct grid_sys_model* mod);


uint8_t grid_sys_get_bank_num(struct grid_sys_model* mod);
uint8_t grid_sys_get_bank_next(struct grid_sys_model* mod);



uint8_t grid_sys_get_editor_connected_state(struct grid_sys_model* mod);
void grid_sys_set_editor_connected_state(struct grid_sys_model* mod, uint8_t state);

uint8_t grid_sys_get_midirx_any_state(struct grid_sys_model* mod);
uint8_t grid_sys_get_midirx_sync_state(struct grid_sys_model* mod);

void grid_sys_set_midirx_any_state(struct grid_sys_model* mod, uint8_t state);
void grid_sys_set_midirx_sync_state(struct grid_sys_model* mod, uint8_t state);


uint8_t grid_sys_get_module_x(struct grid_sys_model* mod);
uint8_t grid_sys_get_module_y(struct grid_sys_model* mod);
uint8_t grid_sys_get_module_rot(struct grid_sys_model* mod);

void grid_sys_set_module_x(struct grid_sys_model* mod, uint8_t x);
void grid_sys_set_module_y(struct grid_sys_model* mod, uint8_t y);
void grid_sys_set_module_rot(struct grid_sys_model* mod, uint8_t rot);


uint8_t grid_sys_get_bank_red(struct grid_sys_model* mod);
uint8_t grid_sys_get_bank_gre(struct grid_sys_model* mod);
uint8_t grid_sys_get_bank_blu(struct grid_sys_model* mod);


void grid_sys_set_bank_red(struct grid_sys_model* mod, uint8_t red);
void grid_sys_set_bank_gre(struct grid_sys_model* mod, uint8_t gre);
void grid_sys_set_bank_blu(struct grid_sys_model* mod, uint8_t blu);


void grid_sys_set_bank(struct grid_sys_model* mod, uint8_t value);


uint8_t	grid_sys_packet_send_everywhere(struct grid_msg* msg);

//=========================== SYS CB ============================//

static void tx_cb_USART_GRID_N(const struct usart_async_descriptor *const descr);

static void tx_cb_USART_GRID_E(const struct usart_async_descriptor *const descr);

static void tx_cb_USART_GRID_S(const struct usart_async_descriptor *const descr);

static void tx_cb_USART_GRID_W(const struct usart_async_descriptor *const descr);

void tx_cb_USART_GRID(struct grid_port* const por);


static void rx_cb_USART_GRID_N(const struct usart_async_descriptor *const descr);

static void rx_cb_USART_GRID_E(const struct usart_async_descriptor *const descr);

static void rx_cb_USART_GRID_S(const struct usart_async_descriptor *const descr);

static void rx_cb_USART_GRID_W(const struct usart_async_descriptor *const descr);

void rx_cb_USART_GRID(struct grid_port* const por);



void dma_transfer_complete_n_cb(struct _dma_resource *resource);
void dma_transfer_complete_e_cb(struct _dma_resource *resource);
void dma_transfer_complete_s_cb(struct _dma_resource *resource);
void dma_transfer_complete_w_cb(struct _dma_resource *resource);
void dma_transfer_complete(struct grid_port* por);

uint32_t grid_sys_get_hwcfg(struct grid_sys_model* mod);
uint32_t grid_sys_get_id(struct grid_sys_model* mod, uint32_t* return_array);


#endif