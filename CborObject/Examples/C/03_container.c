/**
 * @file 03_container.c
 * @brief 容器操作示例（数组和对象）
 *
 * 本示例展示：
 * - 数组的头部/尾部插入
 * - 指定位置插入（before/after）
 * - 容器遍历
 * - 对象的键值对操作
 * - 元素删除
 */

#include "../cbor/cbor.h"
#include <stdio.h>
#include <stdlib.h>

void print_separator(const char *title) {
    printf("\n========== %s ==========\n", title);
}

void print_json(cbor_value_t *val, const char *title) {
    printf("\n【%s】\n", title);
    char *json = cbor_json_dumps(val, NULL, true);
    if (json) {
        printf("%s\n", json);
        free(json);
    }
}

// 遍历数组并打印
void print_array(cbor_value_t *arr, const char *title) {
    printf("\n【%s】", title);
    if (!arr || !cbor_is_array(arr)) {
        printf(" - 不是有效的数组\n");
        return;
    }

    int size = cbor_container_size(arr);
    printf(" (大小: %d)\n", size);

    cbor_value_t *item;
    int index = 0;
    cbor_container_foreach(item, arr) {
        if (cbor_is_integer(item)) {
            printf("  [%d]: %lld\n", index, cbor_integer(item));
        } else if (cbor_is_string(item)) {
            printf("  [%d]: \"%s\"\n", index, cbor_string(item));
        } else if (cbor_is_double(item)) {
            printf("  [%d]: %f\n", index, cbor_double(item));
        } else if (cbor_is_boolean(item)) {
            printf("  [%d]: %s\n", index, cbor_boolean(item) ? "true" : "false");
        }
        index++;
    }
}

int main() {
    printf("容器操作示例\n");

    // ========== 1. 数组尾部插入 ==========
    print_separator("1. 数组尾部插入（insert_tail）");

    cbor_value_t *arr = cbor_init_array();

    // 依次在尾部插入元素
    cbor_container_insert_tail(arr, cbor_init_integer(10));
    cbor_container_insert_tail(arr, cbor_init_integer(20));
    cbor_container_insert_tail(arr, cbor_init_integer(30));

    printf("✓ 依次在尾部插入: 10, 20, 30\n");
    print_array(arr, "结果数组");
    print_json(arr, "JSON格式");

    // ========== 2. 数组头部插入 ==========
    print_separator("2. 数组头部插入（insert_head）");

    // 在头部插入元素
    cbor_container_insert_head(arr, cbor_init_integer(5));
    cbor_container_insert_head(arr, cbor_init_integer(1));

    printf("✓ 依次在头部插入: 5, 1\n");
    printf("注意：头部插入是逆序的，先插入5，再插入1，所以1在最前面\n");
    print_array(arr, "结果数组");

    // ========== 3. 指定位置插入（after）==========
    print_separator("3. 在指定元素后插入（insert_after）");

    // 获取第二个元素（值为5）
    cbor_value_t *second = cbor_array_at(arr, 1);
    if (second) {
        cbor_container_insert_after(arr, second, cbor_init_integer(3));
        printf("✓ 在第二个元素(5)后插入: 3\n");
    }

    print_array(arr, "结果数组");

    // ========== 4. 指定位置插入（before）==========
    print_separator("4. 在指定元素前插入（insert_before）");

    // 获取值为20的元素
    cbor_value_t *target = NULL;
    cbor_value_t *item;
    cbor_container_foreach(item, arr) {
        if (cbor_is_integer(item) && cbor_integer(item) == 20) {
            target = item;
            break;
        }
    }

    if (target) {
        cbor_container_insert_before(arr, target, cbor_init_integer(15));
        printf("✓ 在元素(20)前插入: 15\n");
    }

    print_array(arr, "结果数组");

    // ========== 5. 删除数组元素 ==========
    print_separator("5. 删除数组元素");

    // 查找并删除值为3的元素
    cbor_value_t *to_remove = NULL;
    cbor_container_foreach(item, arr) {
        if (cbor_is_integer(item) && cbor_integer(item) == 3) {
            to_remove = item;
            break;
        }
    }

    if (to_remove) {
        cbor_container_remove(arr, to_remove);
        printf("✓ 删除元素: 3\n");
        cbor_destroy(to_remove);  // 删除后需要手动释放
    }

    print_array(arr, "删除后的数组");

    // ========== 6. 对象操作 ==========
    print_separator("6. 对象（Map）操作");

    cbor_value_t *obj = cbor_init_map();

    // 向对象添加键值对（使用cbor_init_pair）
    cbor_value_t *key1 = cbor_init_string("name", -1);
    cbor_value_t *value1 = cbor_init_string("Alice", -1);
    cbor_value_t *pair1 = cbor_init_pair(key1, value1);
    cbor_container_insert_tail(obj, pair1);

    cbor_value_t *key2 = cbor_init_string("age", -1);
    cbor_value_t *value2 = cbor_init_integer(30);
    cbor_value_t *pair2 = cbor_init_pair(key2, value2);
    cbor_container_insert_tail(obj, pair2);

    cbor_value_t *key3 = cbor_init_string("email", -1);
    cbor_value_t *value3 = cbor_init_string("alice@example.com", -1);
    cbor_value_t *pair3 = cbor_init_pair(key3, value3);
    cbor_container_insert_tail(obj, pair3);

    printf("✓ 添加了3个键值对\n");
    print_json(obj, "对象内容");

    // ========== 7. 遍历对象 ==========
    print_separator("7. 遍历对象");

    printf("\n遍历所有键值对：\n");
    cbor_value_t *pair;
    cbor_container_foreach(pair, obj) {
        if (cbor_is_pair(pair)) {
            const char *key = cbor_string(cbor_pair_key(pair));
            cbor_value_t *val = cbor_pair_value(pair);

            if (cbor_is_string(val)) {
                printf("  %s: \"%s\"\n", key, cbor_string(val));
            } else if (cbor_is_integer(val)) {
                printf("  %s: %lld\n", key, cbor_integer(val));
            }
        }
    }

    // ========== 8. 混合类型数组 ==========
    print_separator("8. 混合类型数组");

    cbor_value_t *mixed = cbor_init_array();

    cbor_container_insert_tail(mixed, cbor_init_integer(42));
    cbor_container_insert_tail(mixed, cbor_init_string("hello", -1));
    cbor_container_insert_tail(mixed, cbor_init_boolean(true));
    cbor_container_insert_tail(mixed, cbor_init_double(3.14));
    cbor_container_insert_tail(mixed, cbor_init_null());

    printf("✓ 创建混合类型数组：整数、字符串、布尔、浮点、null\n");
    print_array(mixed, "混合类型数组");
    print_json(mixed, "JSON格式");

    // ========== 9. 嵌套结构 ==========
    print_separator("9. 嵌套数组和对象");

    cbor_value_t *nested_obj = cbor_init_map();

    // 添加普通字段
    cbor_value_t *name_pair = cbor_init_pair(
        cbor_init_string("name", -1),
        cbor_init_string("Bob", -1)
    );
    cbor_container_insert_tail(nested_obj, name_pair);

    // 添加嵌套数组
    cbor_value_t *scores = cbor_init_array();
    cbor_container_insert_tail(scores, cbor_init_integer(85));
    cbor_container_insert_tail(scores, cbor_init_integer(90));
    cbor_container_insert_tail(scores, cbor_init_integer(95));

    cbor_value_t *scores_pair = cbor_init_pair(
        cbor_init_string("scores", -1),
        scores
    );
    cbor_container_insert_tail(nested_obj, scores_pair);

    // 添加嵌套对象
    cbor_value_t *address = cbor_init_map();
    cbor_container_insert_tail(address, cbor_init_pair(
        cbor_init_string("city", -1),
        cbor_init_string("Shanghai", -1)
    ));
    cbor_container_insert_tail(address, cbor_init_pair(
        cbor_init_string("zipcode", -1),
        cbor_init_string("200000", -1)
    ));

    cbor_value_t *address_pair = cbor_init_pair(
        cbor_init_string("address", -1),
        address
    );
    cbor_container_insert_tail(nested_obj, address_pair);

    printf("✓ 创建包含嵌套数组和对象的复杂结构\n");
    print_json(nested_obj, "嵌套结构");

    // ========== 10. 容器大小查询 ==========
    print_separator("10. 容器大小查询");

    printf("arr 大小: %d\n", cbor_container_size(arr));
    printf("obj 大小: %d\n", cbor_container_size(obj));
    printf("mixed 大小: %d\n", cbor_container_size(mixed));
    printf("nested_obj 大小: %d\n", cbor_container_size(nested_obj));
    printf("嵌套的scores 大小: %d\n", cbor_container_size(scores));

    // ========== 11. 性能优势说明 ==========
    print_separator("11. 容器操作性能特点");

    printf("\n本库使用侵入式双向链表实现容器，性能特点：\n");
    printf("  ✓ 头部插入（insert_head）: O(1)\n");
    printf("  ✓ 尾部插入（insert_tail）: O(1)\n");
    printf("  ✓ 指定位置插入（insert_after/before）: O(1)\n");
    printf("  ✓ 删除元素（remove）: O(1)\n");
    printf("  ✓ 获取大小（size）: O(n)（需要遍历）\n");
    printf("  ✓ 访问指定索引（at）: O(n)（需要遍历）\n");
    printf("\n零拷贝设计：插入和删除只修改指针，不移动数据\n");

    // ========== 12. 清理资源 ==========
    print_separator("12. 释放资源");

    cbor_destroy(arr);
    cbor_destroy(obj);
    cbor_destroy(mixed);
    cbor_destroy(nested_obj);

    printf("✓ 已释放所有内存\n");
    printf("\n注意：cbor_destroy会递归释放整个数据结构，包括所有子元素\n");

    printf("\n========== 示例完成 ==========\n");
    return 0;
}
