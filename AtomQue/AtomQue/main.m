//
//  main.m
//  AtomQue
//
//  Created by lihao10 on 2020/4/15.
//  Copyright © 2020 GodL. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "atomque.h"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
     
        atomque_ref queue = atomque_create();
        for (int i=0; i<10; i++) {
            atomque_enqueue(queue, @(i).stringValue.UTF8String);
        }
        
        for (int i=0; i<10; i++) {
            const void *value = atomque_dequeue(queue);
            if (value == NULL) {
                continue;
            }
            NSLog(@"%@",[[NSString alloc] initWithUTF8String:value]);
        }
        
        atomque_destory(queue);
        // insert code here...
    }
    return 0;
}
