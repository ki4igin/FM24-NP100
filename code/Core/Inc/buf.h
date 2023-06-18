#ifndef __BUF_H
#define __BUF_H

#define buf_declare(_id, _size) \
    struct _id {                \
        uint32_t count;         \
        uint8_t data[_size];    \
    } _id

#endif
