#ifndef EVENT_H
#define EVENT_H

#include <stdint.h>
#include "mouse.h"

#define MOUSE_EVENT_QSIZE 32   // 队列大小, QUEUE_SIZE缩写为QSIZE

typedef struct {
     mouse_event_type_t type;

     int32_t dx;
     int32_t dy;

     uint8_t buttons;
     int8_t wheel;
/*因为我的个人失误 导致缩进变成了五个 这个代码就这么错下去吧 不用改了*/
} mouse_event_t;

typedef struct {                          /* 队列结构 */
     mouse_event_t buf[MOUSE_EVENT_QSIZE];
     uint16_t head;                       /* 出队位置 */
     uint16_t tail;                       /* 入队位置 */
     uint16_t count;                      /* 当前元素数 == 0 → 空，QSIZE 为满 */
/*
* 其实这里可以
* uint16_t head, tail, count;
* 的, 但是我又考虑到可读性
* 但后来我发现注释好像可以分开解释
* 总之就这样吧
*/
} mouse_event_queue_t;

#endif