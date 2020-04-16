//
//  atomque.c
//  AtomQue
//
//  Created by lihao10 on 2020/4/15.
//  Copyright © 2020 GodL. All rights reserved.
//

#include "atomque.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
struct _atomque_node{
    const void *value;
    struct _atomque_node *next;
};

typedef struct _atomque_node * node_ref;

static inline node_ref node_create(const void *value) {
    if (value == NULL) return NULL;
    node_ref ref = malloc(sizeof(struct _atomque_node));
    if (ref == NULL) return NULL;
    ref->value = value;
    ref->next = NULL;
    return ref;
}

struct atomque {
    unsigned int length;
    unsigned int limit;
    atomque_callback callback;
    volatile node_ref head;
    volatile node_ref tail;
};

atomque_callback atomque_string_callback = {
    (atomque_enqueue_callback)strdup,
    NULL,
    (atomque_destory_callback)free
};

atomque_callback atomque_null_callback = {
    NULL,
    NULL,
    NULL
};

atomque_ref __attribute__((overloadable)) atomque_create(void) {
    return atomque_create(INT32_MAX);
}

atomque_ref __attribute__((overloadable)) atomque_create(unsigned int limit) {
    return atomque_create(limit, &atomque_null_callback);
}

atomque_ref __attribute__((overloadable)) atomque_create(atomque_callback *callback) {
    return atomque_create(INT32_MAX, callback);
}

atomque_ref __attribute__((overloadable)) atomque_create(unsigned int limit,atomque_callback *callback) {
    atomque_ref que = malloc(sizeof(struct atomque));
    if (que == NULL) return NULL;
    que->length = 0;
    que->limit = limit;
    que->head = que->tail = NULL;
    if (callback) que->callback = *callback;
    return que;
}

void atomque_enqueue(atomque_ref queue, const void *value) {
    if (queue == NULL || value == NULL) return;
    if (queue->length == queue->limit) return;
    if (queue->callback.enqueue_callback) {
        value = queue->callback.enqueue_callback(value);
    }
    node_ref node = node_create(value);
    if (node == NULL) return;
    while (true) {
        if (__sync_bool_compare_and_swap(&queue->tail,NULL,node)) {
            queue->head = node;
            queue->length = 1;
            break;
        }
        if (queue->length == queue->limit || queue->length == 0) {
            break;
        }
        node_ref tail = queue->tail;
        tail->next = node;
        if (__sync_bool_compare_and_swap(&queue->tail,tail,node)) {
            __sync_fetch_and_add_4(&queue->length, 1);
            break;
        }
    }
}

const void *atomque_dequeue(atomque_ref queue) {
    if (queue == NULL) return NULL;
    node_ref ret;
    while (true) {
        ret = queue->head;
        if (ret == NULL) break;
        if (__sync_bool_compare_and_swap(&queue->head,ret,ret->next)) {
            __sync_fetch_and_sub_4(&queue->length, 1);
            if (queue->length == 0) {
                queue->tail = NULL;
            }
            break;
        }
    }
    const void *value = NULL;
    if (ret) {
        value = ret->value;
        if (queue->callback.dequeue_callback) queue->callback.dequeue_callback(value);
        free(ret);
        ret = NULL;
    }
    return value;
}

void atomque_destory(atomque_ref queue) {
    while (true) {
        atomque_ref temp = queue;
        if (temp == NULL) return;
        if (__sync_bool_compare_and_swap(&queue,temp,NULL)) {
            node_ref node = temp->head;
            while (node) {
                if (temp->callback.destory_callback) temp->callback.destory_callback(node->value);
                node_ref temp_node = node;
                node = node->next;
                free(temp_node);
                temp_node = NULL;
                temp->length --;
            }
            free(temp);
            temp = NULL;
            break;
        }
    }
}


