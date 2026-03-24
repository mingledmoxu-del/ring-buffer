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
    if(!ring || !text)  return;
    char* copy = strdup(text);
    if(!copy)   return;

    if(ring->count == ring->capacity){
        if(ring->slots[ring->head].content!=NULL){
        free(ring->slots[ring->head]);
        ring->slots[ring->head].content=NULL;
        }

        ring->head=(ring->head+1)%ring->capacity;
        ring->count--;
    }
    int32_t tail = (ring->count + ring->head)%ring->capacity;
    ring->slots
}