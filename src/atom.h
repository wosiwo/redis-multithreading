//
// Created by onceme on 2019/3/28.
//

#ifndef REDIS_3_0_ANNOTATED_ATOM_H
#define REDIS_3_0_ANNOTATED_ATOM_H

#endif //REDIS_3_0_ANNOTATED_ATOM_H

/*原子操作*/
/* 原子获取 */
#define AO_GET(ptr)               ({ (void) volatile *_val = (ptr); barrier(); (*_val); })
/
/* 通过值与旧值进行算术与位操作，返回新值 */
#define AO_ADD_F(ptr, value)      ((void)__sync_add_and_fetch((ptr), (value)))
#define AO_SUB_F(ptr, value)      ((void)__sync_sub_and_fetch((ptr), (value)))

/*原子设置，如果原值和新值不一样则设置*/
#define AO_SET(ptr, value)        ((void)__sync_lock_test_and_set((ptr), (value)))

/* 原子比较交换，如果当前值等于旧指，则新值被设置，返回真值，否则返回假 */
#define AO_CASB(ptr, comp, value) (__sync_bool_compare_and_swap((ptr), (comp), (value)) != 0 ? true : false)
