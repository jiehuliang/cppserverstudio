#define FF_ARRAY_ELEMS(a) (sizeof(a) / sizeof((a)[0]))
#ifndef BASE64_H
#define BASE64_H

#include <cstdint>
#include <string>

/**
 * Decode a base64-encoded string.
 *
 * @param out      buffer for decoded data
 * @param in       null-terminated input string
 * @param out_size size in bytes of the out buffer, must be at
 *                 least 3/4 of the length of in
 * @return         number of bytes written, or a negative value in case of
 *                 invalid input
 */
int av_base64_decode(uint8_t* out, const char* in, int out_size);

/**
 * Encode data to base64 and null-terminate.
 *
 * @param out      buffer for encoded data
 * @param out_size size in bytes of the output buffer, must be at
 *                 least AV_BASE64_SIZE(in_size)
 * @param in_size  size in bytes of the 'in' buffer
 * @return         'out' or NULL in case of error
 */
char* av_base64_encode(char* out, int out_size, const uint8_t* in, int in_size);

/**
 * Calculate the output size needed to base64-encode x bytes.
 */
#define AV_BASE64_SIZE(x)  (((x)+2) / 3 * 4 + 1)


 /**
  * 编码base64
  * @param txt 明文
  * @return 密文
  * Encode base64
  * @param txt plaintext
  * @return ciphertext
  */
std::string encodeBase64(const std::string& txt);

/**
 * 解码base64
 * @param txt 密文
 * @return 明文
 * Decode base64
 * @param txt ciphertext
 * @return plaintext
 */
std::string decodeBase64(const std::string& txt);


#endif //BASE64_H