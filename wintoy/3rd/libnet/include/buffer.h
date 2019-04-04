#ifndef __BUFFER_H__
#define __BUFFER_H__
#include <stdint.h>
#include <stdio.h>
#define B_OK 0
#define B_ERROR -1
#include "netconf.h"

struct buf_circle
{
    int8_t* buf;
    int32_t cap;
    int32_t data_sz;
    int32_t rd_idx;
    int32_t wt_idx;
};

NET_API struct buf_circle* buf_create_circle(int32_t capacity);
NET_API void buf_destroy_circle(struct buf_circle* buf);
NET_API int32_t buf_read_circle(struct buf_circle* buf, int8_t* out_b, int32_t len);
NET_API int32_t buf_write_circle(struct buf_circle* buf, int8_t* in_b, int32_t len);
NET_API int32_t buf_peek_circle(struct buf_circle* buf, int8_t* out_b, int32_t len);
NET_API int32_t buf_offset_circle(struct buf_circle* buf, int32_t len);
NET_API int32_t buf_space_circle(struct buf_circle* buf);
NET_API void buf_clear_circle(struct buf_circle* buf);
NET_API struct buf_circle* buf_relloc_circle(struct buf_circle* buf);

struct buf_data
{
    int8_t* buf;
    int32_t cap;
    int32_t st_idx;
    int32_t ed_idx;
};

NET_API struct buf_data* buf_create_data(int32_t capacity);
NET_API int32_t buf_write_data(struct buf_data* buf, int8_t* in_b, int32_t len);
NET_API int32_t buf_read_data(struct buf_data* buf, int8_t* out_b, int32_t len);
NET_API int32_t buf_space_data(struct buf_data* buf);
NET_API int32_t buf_size_data(struct buf_data* buf);
NET_API void buf_reinit_data(struct buf_data* buf);
NET_API int32_t buf_indexof_data(struct buf_data* buf, int32_t st_idx, int8_t* checker, int32_t check_len);
NET_API void buf_destroy_data(struct buf_data* buf);
NET_API void buf_relloc_data(struct buf_data* buf);


#endif