/**
 * @file test_cbor_api_verify.cpp
 * @brief 验证CBOR容器操作和JSON文件I/O函数的功能
 */

#include "cbor/cbor.h"
#include <iostream>
#include <cstring>
#include <cstdio>

int totalTests = 0;
int passedTests = 0;

#define TEST_START(name) \
    std::cout << "\n========== " << name << " ==========\n"; \
    totalTests++;

#define ASSERT_TRUE(condition, message) \
    if (condition) { \
        std::cout << "✓ " << message << "\n"; \
    } else { \
        std::cout << "✗ " << message << " [失败]\n"; \
        return false; \
    }

#define TEST_PASS(message) \
    std::cout << "✓ " << message << "\n"; \
    passedTests++; \
    return true;

/**
 * 测试1: cbor_container_insert_tail
 */
bool test_container_insert_tail() {
    TEST_START("测试1: cbor_container_insert_tail");

    // 测试数组
    cbor_value_t *arr = cbor_init_array();
    ASSERT_TRUE(arr != NULL, "创建数组");

    cbor_value_t *val1 = cbor_init_integer(10);
    int result1 = cbor_container_insert_tail(arr, val1);
    ASSERT_TRUE(result1 == 0, "插入第1个元素到数组尾部");

    cbor_value_t *val2 = cbor_init_integer(20);
    int result2 = cbor_container_insert_tail(arr, val2);
    ASSERT_TRUE(result2 == 0, "插入第2个元素到数组尾部");

    cbor_value_t *val3 = cbor_init_integer(30);
    int result3 = cbor_container_insert_tail(arr, val3);
    ASSERT_TRUE(result3 == 0, "插入第3个元素到数组尾部");

    int size = cbor_container_size(arr);
    ASSERT_TRUE(size == 3, "数组大小正确（3个元素）");

    // 验证元素顺序
    cbor_value_t *first = cbor_container_first(arr);
    ASSERT_TRUE(cbor_integer(first) == 10, "第1个元素值正确");

    cbor_value_t *second = cbor_container_next(arr, first);
    ASSERT_TRUE(cbor_integer(second) == 20, "第2个元素值正确");

    cbor_value_t *third = cbor_container_next(arr, second);
    ASSERT_TRUE(cbor_integer(third) == 30, "第3个元素值正确");

    // 测试对象（需要使用pair）
    cbor_value_t *obj = cbor_init_map();
    cbor_value_t *key = cbor_init_string("name", -1);
    cbor_value_t *value = cbor_init_string("Alice", -1);
    cbor_value_t *pair = cbor_init_pair(key, value);
    int result4 = cbor_container_insert_tail(obj, pair);
    ASSERT_TRUE(result4 == 0, "插入键值对到对象尾部");

    cbor_destroy(arr);
    cbor_destroy(obj);

    TEST_PASS("cbor_container_insert_tail测试");
}

/**
 * 测试2: cbor_container_insert_head
 */
bool test_container_insert_head() {
    TEST_START("测试2: cbor_container_insert_head");

    cbor_value_t *arr = cbor_init_array();

    cbor_value_t *val1 = cbor_init_integer(10);
    cbor_container_insert_head(arr, val1);

    cbor_value_t *val2 = cbor_init_integer(20);
    cbor_container_insert_head(arr, val2);

    cbor_value_t *val3 = cbor_init_integer(30);
    cbor_container_insert_head(arr, val3);

    // 验证元素顺序（应该是逆序：30, 20, 10）
    cbor_value_t *first = cbor_container_first(arr);
    ASSERT_TRUE(cbor_integer(first) == 30, "第1个元素值正确（最后插入的）");

    cbor_value_t *second = cbor_container_next(arr, first);
    ASSERT_TRUE(cbor_integer(second) == 20, "第2个元素值正确");

    cbor_value_t *third = cbor_container_next(arr, second);
    ASSERT_TRUE(cbor_integer(third) == 10, "第3个元素值正确（最先插入的）");

    cbor_destroy(arr);

    TEST_PASS("cbor_container_insert_head测试");
}

/**
 * 测试3: cbor_container_insert_after
 */
bool test_container_insert_after() {
    TEST_START("测试3: cbor_container_insert_after");

    cbor_value_t *arr = cbor_init_array();

    // 先插入两个元素
    cbor_value_t *val1 = cbor_init_integer(10);
    cbor_container_insert_tail(arr, val1);

    cbor_value_t *val3 = cbor_init_integer(30);
    cbor_container_insert_tail(arr, val3);

    // 在第一个元素之后插入20
    cbor_value_t *val2 = cbor_init_integer(20);
    int result = cbor_container_insert_after(arr, val1, val2);
    ASSERT_TRUE(result == 0, "在指定元素后插入");

    // 验证顺序：10, 20, 30
    cbor_value_t *first = cbor_container_first(arr);
    ASSERT_TRUE(cbor_integer(first) == 10, "第1个元素：10");

    cbor_value_t *second = cbor_container_next(arr, first);
    ASSERT_TRUE(cbor_integer(second) == 20, "第2个元素：20（新插入的）");

    cbor_value_t *third = cbor_container_next(arr, second);
    ASSERT_TRUE(cbor_integer(third) == 30, "第3个元素：30");

    cbor_destroy(arr);

    TEST_PASS("cbor_container_insert_after测试");
}

/**
 * 测试4: cbor_container_insert_before
 */
bool test_container_insert_before() {
    TEST_START("测试4: cbor_container_insert_before");

    cbor_value_t *arr = cbor_init_array();

    // 先插入两个元素
    cbor_value_t *val1 = cbor_init_integer(10);
    cbor_container_insert_tail(arr, val1);

    cbor_value_t *val3 = cbor_init_integer(30);
    cbor_container_insert_tail(arr, val3);

    // 在第二个元素之前插入20
    cbor_value_t *val2 = cbor_init_integer(20);
    int result = cbor_container_insert_before(arr, val3, val2);
    ASSERT_TRUE(result == 0, "在指定元素前插入");

    // 验证顺序：10, 20, 30
    cbor_value_t *first = cbor_container_first(arr);
    ASSERT_TRUE(cbor_integer(first) == 10, "第1个元素：10");

    cbor_value_t *second = cbor_container_next(arr, first);
    ASSERT_TRUE(cbor_integer(second) == 20, "第2个元素：20（新插入的）");

    cbor_value_t *third = cbor_container_next(arr, second);
    ASSERT_TRUE(cbor_integer(third) == 30, "第3个元素：30");

    cbor_destroy(arr);

    TEST_PASS("cbor_container_insert_before测试");
}

/**
 * 测试5: cbor_json_loads 和 cbor_json_dumps
 */
bool test_json_loads_dumps() {
    TEST_START("测试5: cbor_json_loads 和 cbor_json_dumps");

    const char *json_str = "{\"name\":\"Bob\",\"age\":25,\"scores\":[85,90,95]}";

    // 加载JSON
    cbor_value_t *root = cbor_json_loads(json_str, -1);
    ASSERT_TRUE(root != NULL, "cbor_json_loads成功");
    ASSERT_TRUE(cbor_is_map(root), "根对象是map");

    // 输出为JSON（非美化）
    size_t length1;
    char *output1 = cbor_json_dumps(root, &length1, false);
    ASSERT_TRUE(output1 != NULL, "cbor_json_dumps成功（非美化）");
    ASSERT_TRUE(length1 > 0, "输出长度大于0");
    std::cout << "  非美化输出: " << output1 << "\n";

    // 输出为JSON（美化）
    size_t length2;
    char *output2 = cbor_json_dumps(root, &length2, true);
    ASSERT_TRUE(output2 != NULL, "cbor_json_dumps成功（美化）");
    ASSERT_TRUE(length2 > length1, "美化输出长度更长");
    std::cout << "  美化输出:\n" << output2 << "\n";

    free(output1);
    free(output2);
    cbor_destroy(root);

    TEST_PASS("cbor_json_loads/dumps测试");
}

/**
 * 测试6: cbor_json_loads_ex（带扩展选项）
 */
bool test_json_loads_ex() {
    TEST_START("测试6: cbor_json_loads_ex");

    // 测试带注释的JSON（需要JSON_PARSER_ALLOW_COMMENT标志）
    const char *json_with_comment = "{\n  // 这是注释\n  \"value\": 42\n}";

    int consumed = 0;
    cbor_value_t *root = cbor_json_loads_ex(
        json_with_comment,
        -1,
        JSON_PARSER_ALLOW_COMMENT,
        &consumed
    );

    ASSERT_TRUE(root != NULL, "解析带注释的JSON成功");
    ASSERT_TRUE(consumed > 0, "consumed参数返回解析字节数");

    size_t length;
    char *output = cbor_json_dumps(root, &length, false);
    std::cout << "  解析结果: " << output << "\n";

    free(output);
    cbor_destroy(root);

    TEST_PASS("cbor_json_loads_ex测试");
}

/**
 * 测试7: cbor_json_loadf 和 cbor_json_dumpf（文件I/O）
 */
bool test_json_file_io() {
    TEST_START("测试7: cbor_json_loadf 和 cbor_json_dumpf");

    const char *test_file = "test_temp_cbor_api.json";

    // 创建测试数据
    cbor_value_t *obj = cbor_init_map();

    cbor_value_t *key1 = cbor_init_string("name", -1);
    cbor_value_t *val1 = cbor_init_string("TestUser", -1);
    cbor_value_t *pair1 = cbor_init_pair(key1, val1);
    cbor_container_insert_tail(obj, pair1);

    cbor_value_t *key2 = cbor_init_string("version", -1);
    cbor_value_t *val2 = cbor_init_integer(100);
    cbor_value_t *pair2 = cbor_init_pair(key2, val2);
    cbor_container_insert_tail(obj, pair2);

    // 写入文件（美化格式）
    int write_result = cbor_json_dumpf(obj, test_file, true);
    ASSERT_TRUE(write_result == 0, "cbor_json_dumpf写入文件成功");

    // 从文件加载
    cbor_value_t *loaded = cbor_json_loadf(test_file);
    if (loaded == NULL) {
        std::cerr << "  DEBUG: 无法加载文件 " << test_file << "\n";
        // 检查文件是否存在
        FILE *check = fopen(test_file, "r");
        if (check) {
            std::cerr << "  DEBUG: 文件存在，读取其内容:\n";
            fseek(check, 0, SEEK_END);
            long fsize = ftell(check);
            fseek(check, 0, SEEK_SET);
            char *buf = (char*)malloc(fsize + 1);
            fread(buf, 1, fsize, check);
            buf[fsize] = 0;
            std::cerr << buf << "\n";
            free(buf);
            fclose(check);
        } else {
            std::cerr << "  DEBUG: 文件不存在！\n";
        }
    }
    ASSERT_TRUE(loaded != NULL, "cbor_json_loadf读取文件成功");

    // 验证内容
    cbor_value_t *name_val = cbor_pointer_get(loaded, "/name");
    ASSERT_TRUE(name_val != NULL, "获取/name字段");
    ASSERT_TRUE(strcmp(cbor_string(name_val), "TestUser") == 0, "name字段值正确");

    cbor_value_t *ver_val = cbor_pointer_get(loaded, "/version");
    ASSERT_TRUE(ver_val != NULL, "获取/version字段");
    ASSERT_TRUE(cbor_integer(ver_val) == 100, "version字段值正确");

    // 清理
    cbor_destroy(obj);
    cbor_destroy(loaded);
    std::remove(test_file);

    TEST_PASS("cbor_json_loadf/dumpf测试");
}

int main() {
    std::cout << "==================== CBOR API功能验证测试 ====================\n";
    std::cout << "测试容器操作函数和JSON文件I/O函数\n";
    std::cout << "============================================================\n";

    try {
        test_container_insert_tail();
        test_container_insert_head();
        test_container_insert_after();
        test_container_insert_before();
        test_json_loads_dumps();
        test_json_loads_ex();
        test_json_file_io();

        std::cout << "\n============================================================\n";
        std::cout << "测试完成！\n";
        std::cout << "通过: " << passedTests << "/" << totalTests << " ("
                  << (100.0 * passedTests / totalTests) << "%)\n";
        std::cout << "============================================================\n";

        return (passedTests == totalTests) ? 0 : 1;
    } catch (const std::exception &e) {
        std::cerr << "\n❌ 测试过程中发生异常: " << e.what() << "\n";
        return 1;
    }
}
