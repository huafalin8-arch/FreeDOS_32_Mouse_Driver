#include <stdint.h>
#include <stdbool.h>
#include "mouse.h"
#include "event.h"
#include "mouse_platform.h"

typedef struct {
     bool     initialized;
     uint8_t  buttons;        /* 上次按键快照，用于边沿检测 */
} mouse_dev_t;

static mouse_dev_t         g_dev;
static mouse_state_t       g_state;
static mouse_event_queue_t g_queue;

static void queue_push(const mouse_event_t *ev)
{
     mouse_event_queue_t *q = &g_queue;
     if (q->count == MOUSE_EVENT_QSIZE) {
          /* 满则丢弃最旧事件，避免按键事件被 MOVE 刷屏挤掉 */
          q->head = (q->head + 1) & (MOUSE_EVENT_QSIZE - 1);
          q->count--;
     }
     q->buf[q->tail] = *ev;
     q->tail = (q->tail + 1) & (MOUSE_EVENT_QSIZE - 1);
     q->count++;
}

int mouse_queue_pop(mouse_event_t *ev)
{
     mouse_event_queue_t *q = &g_queue;
     if (q->count == 0)
          return 0;
     *ev = q->buf[q->head];
     q->head = (q->head + 1) & (MOUSE_EVENT_QSIZE - 1);
     q->count--;
     return 1;
}

int mouse_init(void)
{
     g_dev.buttons   = 0;
     g_state.x       = 0;
     g_state.y       = 0;
     g_state.buttons = 0;
     g_queue.head = g_queue.tail = g_queue.count = 0;

     if (mouse_platform_init() != 0)
          return -1;

     g_dev.initialized = true;
     return 0;
}

void mouse_deinit(void)
{
     if (!g_dev.initialized)
          return;
     mouse_platform_deinit();
     g_dev.initialized = false;
}

const mouse_state_t *mouse_get_state(void)
{
     return &g_state;
}

int mouse_poll(void)
{
     if (!g_dev.initialized)
          return 0;

     int produced = 0;
     mouse_raw_t raw;

     while (mouse_platform_poll(&raw)) {
          int32_t dx = raw.dx;
          int32_t dy = raw.dy;                 /* 向下为正 */
          uint8_t new_btn = (uint8_t)(raw.buttons & 0x07);

          /* 更新绝对状态（屏幕坐标：Y 向上为正） */
          g_state.x += dx;
          g_state.y -= dy;
          g_state.buttons = new_btn;

          /* ---------- 移动事件 ---------- */
          if (dx != 0 || dy != 0) {
               mouse_event_t ev = {
                    .type    = MOUSE_EV_MOVE,
                    .dx      = dx,
                    .dy      = -dy,
                    .buttons = new_btn,
                    .wheel   = 0
               };
               queue_push(&ev);
               produced++;
          }

          /* ---------- 滚轮事件 ---------- */
          if (raw.wheel != 0) {
               mouse_event_t ev = {
                    .type    = MOUSE_EV_WHEEL,
                    .dx      = 0,
                    .dy      = 0,
                    .buttons = new_btn,
                    .wheel   = raw.wheel
               };
               queue_push(&ev);
               produced++;
          }

          /* ---------- 按键事件（边沿检测） ---------- */
          uint8_t changed = g_dev.buttons ^ new_btn;
          if (changed) {
               for (int bit = 0; bit < 3; bit++) {
                    uint8_t mask = (uint8_t)(1u << bit);
                    if (changed & mask) {
                         mouse_event_t ev = {
                              .type    = (new_btn & mask)
                                         ? MOUSE_EV_BUTTON_DOWN
                                         : MOUSE_EV_BUTTON_UP,
                              .dx      = 0,
                              .dy      = 0,
                              .buttons = new_btn,
                              .wheel   = 0
                         };
                         queue_push(&ev);
                         produced++;
                    }
               }
          }

          g_dev.buttons = new_btn;
     }

     return produced;
}