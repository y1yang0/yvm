#ifndef YVM_BYTE_H
#define YVM_BYTE_H

#include <cstdint>

typedef uint32_t u4;
typedef uint16_t u2;
typedef uint8_t u1;

#define getu4(buf) \
	(((*(u4*) buf) & 0x000000FF) << 24) | (((*(u4*) buf) & 0x0000FF00) << 8) | (((*(u4*) buf) & 0x00FF0000) >> 8) | (((*(u4*) buf) & 0xFF000000) >> 24)

#define getu2(buf) \
	(u2)((((*(u2*) buf) & 0x00FF) << 8) | (((*(u2*) buf) & 0xFF00) >> 8))

#define getu1(buf) *(u1*) buf

#endif // YVM_BYTE_H
