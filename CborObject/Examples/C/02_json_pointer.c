/**
 * @file 02_json_pointer.c
 * @brief JSON Pointer (RFC 6901) 高级用法示例
 *
 * 本示例展示：
 * - JSON Pointer路径语法
 * - 插入、替换、删除操作的区别
 * - 嵌套对象和数组的操作
 * - 错误处理
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

int main() {
    printf("JSON Pointer 高级用法示例\n");

    // ========== 1. 创建初始数据 ==========
    print_separator("1. 创建初始数据");

    cbor_value_t *root = cbor_json_loads(
        "{"
        "  \"users\": ["
        "    {\"id\": 1, \"name\": \"Alice\"},"
        "    {\"id\": 2, \"name\": \"Bob\"}"
        "  ],"
        "  \"config\": {"
        "    \"timeout\": 30"
        "  }"
        "}", -1);

    print_json(root, "初始数据");

    // ========== 2. insert vs replace vs set 的区别 ==========
    print_separator("2. insert/replace/set 操作对比");

    printf("\n--- insert操作：仅在字段不存在时插入 ---\n");

    // insert: 字段不存在时插入
    cbor_value_t *val1 = cbor_init_integer(100);
    cbor_value_t *insert_result = cbor_pointer_insert(root, "/config/max_connections", val1);
    if (insert_result) {
        printf("✓ insert成功：/config/max_connections 不存在，成功插入\n");
    } else {
        printf("✗ insert失败：字段已存在\n");
        cbor_destroy(val1);  // 失败时需要手动释放val1
    }

    // 再次尝试insert相同路径（应该失败）
    cbor_value_t *val2 = cbor_init_integer(200);
    cbor_value_t *insert_result2 = cbor_pointer_insert(root, "/config/max_connections", val2);
    if (!insert_result2) {
        printf("✓ insert失败（预期行为）：/config/max_connections 已存在\n");
        cbor_destroy(val2);  // 失败时需要手动释放
    }

    printf("\n--- replace操作：仅在字段存在时替换 ---\n");

    // replace: 字段存在时替换
    cbor_value_t *val3 = cbor_init_integer(300);
    cbor_value_t *old_val = cbor_pointer_replace(root, "/config/max_connections", val3);
    if (old_val) {
        long long old_value = cbor_integer(old_val);
        printf("✓ replace成功：替换了旧值 %lld\n", old_value);
        cbor_destroy(old_val);  // 需要释放被替换的旧值
    } else {
        printf("✗ replace失败：字段不存在\n");
        cbor_destroy(val3);
    }

    // 尝试replace不存在的字段（应该失败）
    cbor_value_t *val4 = cbor_init_integer(400);
    cbor_value_t *replace_result = cbor_pointer_replace(root, "/config/unknown_field", val4);
    if (!replace_result) {
        printf("✓ replace失败（预期行为）：/config/unknown_field 不存在\n");
        cbor_destroy(val4);
    }

    printf("\n--- set操作：总是成功（插入或替换） ---\n");

    // set: 存在则替换，不存在则插入
    cbor_value_t *val5 = cbor_init_integer(500);
    cbor_value_t *old_val2 = cbor_pointer_set(root, "/config/max_connections", val5);
    if (old_val2) {
        printf("✓ set成功：替换了已存在的字段\n");
        cbor_destroy(old_val2);
    } else {
        printf("✓ set成功：插入了新字段\n");
    }

    cbor_value_t *val6 = cbor_init_integer(600);
    cbor_value_t *old_val3 = cbor_pointer_set(root, "/config/new_field", val6);
    if (!old_val3) {
        printf("✓ set成功：插入了不存在的字段 /config/new_field\n");
    }

    print_json(root, "insert/replace/set操作后");

    // ========== 3. 便捷函数（推荐）==========
    print_separator("3. 便捷函数（推荐使用）");

    printf("使用便捷函数可以直接设置值，无需手动创建cbor_value_t\n\n");

    // 直接设置各种类型的值
    cbor_pointer_seti(root, "/config/port", 8080);
    printf("✓ seti: 设置整数 /config/port = 8080\n");

    cbor_pointer_sets(root, "/config/host", "localhost");
    printf("✓ sets: 设置字符串 /config/host = localhost\n");

    cbor_pointer_setb(root, "/config/debug", true);
    printf("✓ setb: 设置布尔值 /config/debug = true\n");

    cbor_pointer_setf(root, "/config/version", 1.5);
    printf("✓ setf: 设置浮点数 /config/version = 1.5\n");

    print_json(root, "便捷函数设置后");

    // ========== 4. 数组操作 ==========
    print_separator("4. 数组元素操作");

    // 访问数组元素
    const char *first_user = cbor_pointer_gets(root, "/users/0/name");
    printf("第一个用户: %s\n", first_user);

    // 修改数组元素
    cbor_pointer_sets(root, "/users/1/name", "Bob Smith");
    printf("✓ 修改第二个用户名为: Bob Smith\n");

    // 向数组添加新元素（需要使用JSON Merge Patch或手动操作）
    cbor_value_t *users_array = cbor_pointer_get(root, "/users");
    if (users_array && cbor_is_array(users_array)) {
        // 创建新用户对象
        cbor_value_t *new_user = cbor_json_loads("{\"id\":3,\"name\":\"Charlie\"}", -1);
        cbor_container_insert_tail(users_array, new_user);
        printf("✓ 添加新用户 Charlie\n");
    }

    print_json(root, "数组操作后");

    // ========== 5. 深层嵌套访问 ==========
    print_separator("5. 深层嵌套访问");

    // 创建更深层的嵌套结构
    cbor_pointer_sets(root, "/config/database/primary/host", "db1.example.com");
    cbor_pointer_seti(root, "/config/database/primary/port", 5432);
    cbor_pointer_sets(root, "/config/database/replica/host", "db2.example.com");
    cbor_pointer_seti(root, "/config/database/replica/port", 5433);

    printf("✓ 创建深层嵌套结构：/config/database/primary 和 /config/database/replica\n");

    // 访问深层数据
    const char *primary_host = cbor_pointer_gets(root, "/config/database/primary/host");
    long long primary_port = cbor_pointer_geti(root, "/config/database/primary/port");

    printf("主数据库: %s:%lld\n", primary_host, primary_port);

    print_json(root, "深层嵌套结构");

    // ========== 6. 删除操作 ==========
    print_separator("6. 删除操作");

    // 删除单个字段
    cbor_value_t *removed1 = cbor_pointer_remove(root, "/config/debug");
    if (removed1) {
        printf("✓ 删除 /config/debug\n");
        cbor_destroy(removed1);
    }

    // 删除嵌套对象
    cbor_value_t *removed2 = cbor_pointer_remove(root, "/config/database/replica");
    if (removed2) {
        printf("✓ 删除 /config/database/replica\n");
        cbor_destroy(removed2);
    }

    // 删除数组元素
    cbor_value_t *removed3 = cbor_pointer_remove(root, "/users/2");
    if (removed3) {
        printf("✓ 删除 /users/2 (Charlie)\n");
        cbor_destroy(removed3);
    }

    print_json(root, "删除操作后");

    // ========== 7. 错误处理 ==========
    print_separator("7. 错误处理示例");

    // 访问不存在的路径
    cbor_value_t *not_found = cbor_pointer_get(root, "/non/existent/path");
    if (!not_found) {
        printf("✓ 正确处理：路径 /non/existent/path 不存在\n");
    }

    // 访问类型不匹配的路径（尝试访问非对象/数组的子元素）
    cbor_value_t *type_error = cbor_pointer_get(root, "/config/port/invalid");
    if (!type_error) {
        printf("✓ 正确处理：/config/port 是整数，不能访问子元素\n");
    }

    // ========== 8. JSON Pointer路径特殊字符 ==========
    print_separator("8. 路径中的特殊字符");

    printf("JSON Pointer路径中的特殊字符需要转义：\n");
    printf("  '/' 转义为 '~1'\n");
    printf("  '~' 转义为 '~0'\n\n");

    // 包含斜杠的键名（实际应用中应使用转义）
    cbor_pointer_sets(root, "/config/feature~1flag", "enabled");
    printf("✓ 设置键名包含特殊字符的字段（feature/flag）\n");

    const char *feature_flag = cbor_pointer_gets(root, "/config/feature~1flag");
    printf("读取值: %s\n", feature_flag);

    print_json(root, "特殊字符处理");

    // ========== 9. 清理资源 ==========
    print_separator("9. 释放资源");

    cbor_destroy(root);
    printf("✓ 已释放所有内存\n");

    printf("\n========== 示例完成 ==========\n");
    printf("\n总结：\n");
    printf("- insert: 仅在不存在时插入，失败时需手动释放val\n");
    printf("- replace: 仅在存在时替换，失败时需手动释放val，成功时需释放旧值\n");
    printf("- set: 总是成功（插入或替换），成功时如有旧值需释放\n");
    printf("- 便捷函数（seti/sets/setb/setf）: 推荐使用，无需手动管理内存\n");

    return 0;
}
