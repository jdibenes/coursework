
#ifndef GOLTYPES_H
#define GOLTYPES_H

#define GOL_MAX_CELLS 0x80000000ULL

#define GOL_STATE_ALIVE   0
#define GOL_STATE_DEAD  (~0)

namespace gol {
typedef char cell;
}

typedef char      s8;
typedef short     s16;
typedef int       s32;
typedef long long s64;

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

#endif // GOLTYPES_H
