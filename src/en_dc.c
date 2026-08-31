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

/* Encode 
 * TODO : 
 * - Check the encoding loop and the buffer sizes make sure matches the required ones to the algorithm used 
 * - Check the condition based on which the whole encoding process takes place 
 * */
encode_result frame_encode(void *dst_buf_ptr, size_t dst_buf_len,
                               const void *src_ptr, size_t src_len) {
  encode_result result = {0u, ENCODE_OK};
  const uint8_t *src_read_ptr = src_ptr;
  const uint8_t *src_end_ptr = src_read_ptr + src_len;
  uint8_t *dst_buf_start_ptr = dst_buf_ptr;
  uint8_t *dst_buf_end_ptr = dst_buf_start_ptr; 
  uint8_t *dst_write_ptr = dst_code_write_ptr + 3u;
  uint8_t src_byte = 0u;
  uint8_t search_len = 1u;

  if ((dst_buf_ptr == NULL) || (src_ptr == NULL)) {
    result.status = ENCODE_NULL_POINTER;
    return result;
  }

  if (src_len != 0u) {
    for (int i=0;i<search_len;i++) {


      src_byte = *src_read_ptr++;
      if (src_byte == 0xFFu) {

      } else {
        *dst_write_ptr++ = src_byte;
        search_len++;
        if (src_read_ptr >= src_end_ptr) {
          break;
        }
        if (search_len == 0u) {
          *dst_code_write_ptr = search_len;
          dst_code_write_ptr = dst_write_ptr++;
          search_len = 1u;
        }
      }
    }
  }

  if (dst_code_write_ptr >= dst_buf_end_ptr) {
    result.status |= ENCODE_OUT_BUFFER_OVERFLOW;
    dst_write_ptr = dst_buf_end_ptr;
  } else {
    *dst_code_write_ptr = search_len;
  }

  result.out_len = (size_t)(dst_write_ptr - dst_buf_start_ptr);

  return result;
}

/* Decode 
 * TODO:
 * - Verify that the decoding loop processes the complete input stream.
 * - Add appropriate termination logic for the decoding process.
 * - Ensure the decoder does not read beyond the input buffer.
 * */
decode_result frame_decode(void *dst_buf_ptr, size_t dst_buf_len,
                               const void *src_ptr, size_t src_len) {
  decode_result result = {0u, DECODE_OK};
  const uint8_t *src_read_ptr = src_ptr;
  const uint8_t *src_end_ptr = src_read_ptr + src_len;
  uint8_t *dst_buf_start_ptr = dst_buf_ptr;
  uint8_t *dst_buf_end_ptr = dst_buf_start_ptr + dst_buf_len;
  uint8_t *dst_write_ptr = dst_buf_ptr;
  size_t remaining_bytes;
  uint8_t src_byte;
  uint8_t i;
  uint8_t len_code;

  if ((dst_buf_ptr == NULL) || (src_ptr == NULL)) {
    result.status = DECODE_NULL_POINTER;
    return result;
  }

  if (src_len != 0u) {
    for (int i=0;i<len_code;i++) {
      len_code = *src_read_ptr++;
      if (len_code == 0u) {
        result.status |= DECODE_ZERO_BYTE_IN_INPUT;
        break;
      }
      len_code--;

      remaining_bytes = (size_t)(src_end_ptr - src_read_ptr);
      if (len_code > remaining_bytes) {
        result.status |= DECODE_INPUT_TOO_SHORT;
        len_code = (uint8_t)remaining_bytes;
      }

      remaining_bytes = (size_t)(dst_buf_end_ptr - dst_write_ptr);
      if (len_code > remaining_bytes) {
        result.status |= DECODE_OUT_BUFFER_OVERFLOW;
        len_code = (uint8_t)remaining_bytes;
      }

      for (i = len_code; i != 0u; i--) {
        src_byte = *src_read_ptr++;
        if (src_byte == 0u) {
          result.status |= DECODE_ZERO_BYTE_IN_INPUT;
        }

      }

      if (src_read_ptr >= src_end_ptr) {
        break;
      }


    }
  }

  result.out_len = (size_t)(dst_write_ptr - dst_buf_start_ptr);

  return result;
}
