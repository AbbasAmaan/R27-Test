#include "en_dc.h"
#include <stdlib.h>

/*****************************************************************************
 * Defines
 ****************************************************************************/

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif

/*****************************************************************************
 * Functions
 ****************************************************************************/

/* Encode using COBS (Consistent Overhead Byte Stuffing) */
encode_result frame_encode(void *dst_buf_ptr, size_t dst_buf_len,
                           const void *src_ptr, size_t src_len)
{
    encode_result result = {0u, ENCODE_OK};

    if (dst_buf_ptr == NULL || src_ptr == NULL) {
        result.status = ENCODE_NULL_POINTER;
        return result;
    }

    if (dst_buf_len == 0u) {
        result.status = ENCODE_OUT_BUFFER_OVERFLOW;
        return result;
    }

    const uint8_t *src = (const uint8_t *)src_ptr;
    uint8_t *dst = (uint8_t *)dst_buf_ptr;

    size_t read_index = 0u;
    size_t write_index = 1u;
    size_t code_index = 0u;
    uint8_t code = 1u;

    while (read_index < src_len) {

        if (src[read_index] == 0u) {

            dst[code_index] = code;

            if (write_index >= dst_buf_len) {
                result.status = ENCODE_OUT_BUFFER_OVERFLOW;
                return result;
            }

            code = 1u;
            code_index = write_index;
            write_index++;
            read_index++;
        }
        else {

            if (write_index >= dst_buf_len) {
                result.status = ENCODE_OUT_BUFFER_OVERFLOW;
                return result;
            }

            dst[write_index] = src[read_index];
            write_index++;
            read_index++;
            code++;

            if (code == 0xFFu && read_index < src_len) {

                dst[code_index] = code;

                if (write_index >= dst_buf_len) {
                    result.status = ENCODE_OUT_BUFFER_OVERFLOW;
                    return result;
                }

                code = 1u;
                code_index = write_index;
                write_index++;
            }
        }
    }

    if (code_index >= dst_buf_len) {
        result.status = ENCODE_OUT_BUFFER_OVERFLOW;
        return result;
    }

    dst[code_index] = code;

    result.out_len = write_index;

    return result;
}


/* Decode COBS encoded data */
decode_result frame_decode(void *dst_buf_ptr, size_t dst_buf_len,
                           const void *src_ptr, size_t src_len)
{
    decode_result result = {0u, DECODE_OK};

    if (dst_buf_ptr == NULL || src_ptr == NULL) {
        result.status = DECODE_NULL_POINTER;
        return result;
    }

    const uint8_t *src = (const uint8_t *)src_ptr;
    uint8_t *dst = (uint8_t *)dst_buf_ptr;

    size_t src_index = 0u;
    size_t dst_index = 0u;

    while (src_index < src_len) {

        uint8_t code = src[src_index++];

        if (code == 0u) {
            result.status |= DECODE_ZERO_BYTE_IN_INPUT;
            break;
        }

        size_t data_len = (size_t)code - 1u;

        if (src_index + data_len > src_len) {
            result.status |= DECODE_INPUT_TOO_SHORT;
            data_len = src_len - src_index;
        }

        if (dst_index + data_len > dst_buf_len) {
            result.status |= DECODE_OUT_BUFFER_OVERFLOW;
            data_len = dst_buf_len - dst_index;
        }

        for (size_t i = 0u; i < data_len; i++) {
            dst[dst_index++] = src[src_index++];

            if (dst[dst_index - 1u] == 0u) {
                result.status |= DECODE_ZERO_BYTE_IN_INPUT;
            }
        }

        /*
         * A code value smaller than 0xFF means that a zero byte
         * was removed from the original data.
         */
        if (code != 0xFFu && src_index < src_len) {

            if (dst_index >= dst_buf_len) {
                result.status |= DECODE_OUT_BUFFER_OVERFLOW;
                break;
            }

            dst[dst_index++] = 0u;
        }

        if (result.status & DECODE_INPUT_TOO_SHORT) {
            break;
        }
    }

    result.out_len = dst_index;

    return result;
}