#ifndef RING_BUF_H
#define RING_BUF_H

#include "stdint.h"
#include <string.h>

typedef struct log_item_s{
    uint64_t seq_no;    //全局递增的唯一序号
    char* content;      //动态分配的日志内容字符串
}log_item_t;

typedef struct log_ring_s{
    log_item_t* slots;      // 存储 log_item_t 的数组
    uint32_t capacity;      // 缓冲区总容量
    uint32_t head;          // 指向最旧数据的下标
    uint32_t count;         // 当前缓冲区中的有效条目数
    uint64_t next_seq;      // 下一个可用的全局序号
}log_ring_t;

/**
 * @brief 分配log_ring_t及内部slots数组内存，并初始化所有计数器为0
 * 
 * @param caoacity 
 * @return log_ring_t* 
 */
log_ring_t * log_ring_init(uint32_t caoacity);

/**
 * @brief 压入日志
 * 
 * @param ring 
 * @param text 
 */
void log_ring_push(log_ring_t* ring, const char* text);

/**
 * @brief 弹出日志
 * 
 * @param ring 
 * @return log_item_t 
 */
log_item_t log_ring_pop(log_ring_t *ring);

/**
 * @brief 销毁日志
 * 
 * @param ring 
 */
void log_ring_deinit(log_ring_t* ring);


#endif
