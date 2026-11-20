#ifndef MOUSE_PLATFORM_H
#define MOUSE_PLATFORM_H

#include <stdint.h>

typedef struct {
     int32_t dx;
     int32_t dy;          /* 向下为正 */
     uint8_t buttons;     /* MOUSE_LEFT / RIGHT / MIDDLE 位掩码 */
     int8_t  wheel;       /* 0 表示无滚动 */
} mouse_raw_t;

/* 初始化平台鼠标源，成功返回 0 */
int  mouse_platform_init(void);
void mouse_platform_deinit(void);

/* 取一个原始采样：有数据返回 1，无数据返回 0 */
int  mouse_platform_poll(mouse_raw_t *out);

#endif