#include "ring_buf.h"

log_ring_t *log_ring_init(uint32_t capacity){
    if (capacity == 0)  return NULL;

    log_ring_t* ring = (log_ring_t*)calloc(1,sizeof(log_ring_t));
    if(!ring)   return NULL;

    ring->slots = (log_item_t*)calloc(capacity,sizeof(log_item_t));
    if(!ring->slots)   {
        free(ring);
        return NULL;
    } 
    ring->capacity = capacity;
    ring->next_seq = 1;
    return ring;
}

void log_ring_push(log_ring_t* ring,const char* text){
    char* copy = strdup(text);
    
}