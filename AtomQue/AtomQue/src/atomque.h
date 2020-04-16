//
//  atomque.h
//  AtomQue
//
//  Created by lihao10 on 2020/4/15.
//  Copyright © 2020 GodL. All rights reserved.
//

#ifndef atomque_h
#define atomque_h

#include <stdio.h>

typedef const void *(*atomque_enqueue_callback)(const void *);
typedef void (*atomque_dequeue_callback)(const void *);
typedef void (*atomque_destory_callback)(const void *);

typedef struct _atomque_callback {
    atomque_enqueue_callback enqueue_callback;
    atomque_dequeue_callback dequeue_callback;
    atomque_destory_callback destory_callback;
}atomque_callback;

extern atomque_callback atomque_string_callback;

typedef struct atomque * atomque_ref;

atomque_ref __attribute__((overloadable)) atomque_create(void);

atomque_ref __attribute__((overloadable)) atomque_create(unsigned int limit);

atomque_ref __attribute__((overloadable)) atomque_create(atomque_callback *callback);

atomque_ref __attribute__((overloadable)) atomque_create(unsigned int limit,atomque_callback *callback);

void atomque_enqueue(atomque_ref queue, const void *value);

const void *atomque_dequeue(atomque_ref queue);

void atomque_destory(atomque_ref queue);

#endif /* atomque_h */
