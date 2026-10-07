/**
 * @file 01_basic.c
 * @brief CborObject C API 基础示例
 *
 * 本示例展示：
 * - 从JSON字符串解析数据
 * - 使用JSON Pointer访问数据
 * - 修改和添加数据
 * - 序列化为JSON
 * - 正确的内存管理
 */

#include "../cbor/cbor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_separator(const char *title) {
    printf("\n========== %s ==========\n", title);
}

int main() {
    printf("CborObject C API 基础示例\n");

    // ========== 1. 从JSON字符串解析数据 ==========
    print_separator("1. 解析JSON字符串");

    const char *json_str =
        "{"
        "  \"name\": \"Alice\","
        "  \"age\": 30,"
        "  \"email\": \"alice@example.com\","
        "  \"scores\": [85, 90, 95],"
        "  \"address\": {"
        "    \"city\": \"Beijing\","
        "    \"zipcode\": \"100000\""
        "  }"
        "}";

    cbor_value_t *root = cbor_json_loads(json_str, -1);
    if (!root) {
        fprintf(stderr, "错误：无法解析JSON\n");
        return 1;
    }

    printf("✓ 成功解析JSON数据\n");

    // ========== 2. 使用JSON Pointer访问数据 ==========
    print_separator("2. 访问数据");

    // 访问基本类型
    const char *name = cbor_pointer_gets(root, "/name");
    long long age = cbor_pointer_geti(root, "/age");
    const char *email = cbor_pointer_gets(root, "/email");

    printf("姓名: %s\n", name);
    printf("年龄: %lld\n", age);
    printf("邮箱: %s\n", email);

    // 访问嵌套对象
    const char *city = cbor_pointer_gets(root, "/address/city");
    const char *zipcode = cbor_pointer_gets(root, "/address/zipcode");

    printf("城市: %s\n", city);
    printf("邮编: %s\n", zipcode);

    // 访问数组元素
    long long score1 = cbor_pointer_geti(root, "/scores/0");
    long long score2 = cbor_pointer_geti(root, "/scores/1");
    long long score3 = cbor_pointer_geti(root, "/scores/2");

    printf("成绩: %lld, %lld, %lld\n", score1, score2, score3);

    // ========== 3. 修改现有数据 ==========
    print_separator("3. 修改数据");

    // 修改年龄
    cbor_pointer_seti(root, "/age", 31);
    printf("✓ 修改年龄为: 31\n");

    // 修改邮箱
    cbor_pointer_sets(root, "/email", "alice.new@example.com");
    printf("✓ 修改邮箱为: alice.new@example.com\n");

    // ========== 4. 添加新数据 ==========
    print_separator("4. 添加新字段");

    // 添加电话号码
    cbor_pointer_sets(root, "/phone", "13800138000");
    printf("✓ 添加电话号码\n");

    // 添加布尔值
    cbor_pointer_setb(root, "/is_active", true);
    printf("✓ 添加活跃状态\n");

    // 添加浮点数
    cbor_pointer_setf(root, "/salary", 12500.50);
    printf("✓ 添加工资\n");

    // ========== 5. 输出修改后的JSON ==========
    print_separator("5. 输出JSON（美化格式）");

    size_t length;
    char *output_json = cbor_json_dumps(root, &length, true);  // true表示美化输出
    if (output_json) {
        printf("%s\n", output_json);
        free(output_json);
    }

    // ========== 6. 紧凑格式输出 ==========
    print_separator("6. 输出JSON（紧凑格式）");

    char *compact_json = cbor_json_dumps(root, &length, false);  // false表示紧凑输出
    if (compact_json) {
        printf("%s\n", compact_json);
        printf("\n长度: %zu 字节\n", length);
        free(compact_json);
    }

    // ========== 7. 检查字段是否存在 ==========
    print_separator("7. 检查字段存在性");

    cbor_value_t *phone_val = cbor_pointer_get(root, "/phone");
    if (phone_val) {
        printf("✓ /phone 字段存在\n");
    }

    cbor_value_t *unknown_val = cbor_pointer_get(root, "/unknown");
    if (!unknown_val) {
        printf("✓ /unknown 字段不存在\n");
    }

    // ========== 8. 删除字段 ==========
    print_separator("8. 删除字段");

    cbor_value_t *removed = cbor_pointer_remove(root, "/salary");
    if (removed) {
        printf("✓ 成功删除 /salary 字段\n");
        cbor_destroy(removed);  // 删除的值需要手动释放
    }

    // ========== 9. 保存到文件 ==========
    print_separator("9. 保存到文件");

    int save_result = cbor_json_dumpf(root, "output_basic.json", true);
    if (save_result == 0) {
        printf("✓ 成功保存到 output_basic.json\n");
    } else {
        printf("✗ 保存失败\n");
    }

    // ========== 10. 从文件加载 ==========
    print_separator("10. 从文件加载");

    cbor_value_t *loaded = cbor_json_loadf("output_basic.json");
    if (loaded) {
        printf("✓ 成功从文件加载\n");

        // 验证数据
        const char *loaded_name = cbor_pointer_gets(loaded, "/name");
        printf("加载的姓名: %s\n", loaded_name);

        cbor_destroy(loaded);
    }

    // ========== 11. 内存清理 ==========
    print_separator("11. 释放资源");

    cbor_destroy(root);
    printf("✓ 已释放所有内存\n");

    printf("\n========== 示例完成 ==========\n");
    return 0;
}
