/**
 * @file cbor.h
 * @brief CBOR（Concise Binary Object Representation）数据格式处理库
 *
 * 这是一个功能完整的CBOR/JSON数据处理库，支持CBOR和JSON格式的互转、
 * JSON Pointer操作、JSON Merge Patch等功能。
 *
 * ============================================================================
 * 一、库简介
 * ============================================================================
 *
 * CBOR是RFC 7049定义的二进制数据序列化格式，相比JSON具有以下优势：
 * - 更紧凑的二进制格式，节省30-50%存储空间
 * - 更快的序列化/反序列化速度（快1.5-2倍）
 * - 支持二进制数据（bytestring）
 * - 自描述数据结构
 *
 * 本库同时支持CBOR和JSON格式，提供以下核心功能：
 * 1. CBOR/JSON数据的创建、解析和序列化
 * 2. JSON Pointer (RFC 6901) - 通过路径访问/修改JSON数据
 * 3. JSON Merge Patch (RFC 7396) - 合并JSON对象
 * 4. 容器操作（数组/对象的增删改查）
 * 5. 字符串处理工具函数
 *
 * ============================================================================
 * 二、基本使用流程
 * ============================================================================
 *
 * 典型的使用流程如下：
 *
 * 1. 创建数据对象
 *    - 从JSON字符串创建：cbor_json_loads()
 *    - 从CBOR二进制创建：cbor_loads()
 *    - 手动构建：cbor_init_xxx()系列函数
 *
 * 2. 操作数据对象
 *    - 访问数据：cbor_pointer_get()、cbor_integer()、cbor_string()等
 *    - 修改数据：cbor_pointer_set()、cbor_pointer_replace()等
 *    - 容器操作：cbor_container_insert_xxx()、cbor_container_remove()等
 *
 * 3. 输出数据对象
 *    - 转为JSON字符串：cbor_json_dumps()
 *    - 转为CBOR二进制：cbor_dumps()
 *
 * 4. 释放资源
 *    - 使用cbor_destroy()释放cbor_value_t对象
 *    - 使用free()释放字符串等内存
 *
 * ============================================================================
 * 三、核心数据类型
 * ============================================================================
 *
 * cbor_value_t - 核心数据类型，可以表示以下任意一种值：
 *   - 基本类型：boolean、null、integer、double、string、bytestring
 *   - 容器类型：array（数组）、map（键值对对象）
 *   - 特殊类型：tag（CBOR标签）、pair（map中的键值对）
 *
 * ============================================================================
 * 四、使用示例
 * ============================================================================
 *
 * 示例1：从JSON字符串加载并访问数据
 * --------------------------------------
 * ```c
 * #include "cbor.h"
 *
 * // 1. 从JSON字符串加载数据
 * const char *json_str = "{\"name\":\"Alice\",\"age\":30,\"scores\":[85,90,95]}";
 * cbor_value_t *root = cbor_json_loads(json_str, -1);
 *
 * // 2. 使用JSON Pointer访问数据
 * const char *name = cbor_pointer_gets(root, "/name");        // "Alice"
 * long long age = cbor_pointer_geti(root, "/age");            // 30
 * cbor_value_t *scores = cbor_pointer_get(root, "/scores");   // [85,90,95]
 *
 * // 3. 输出为JSON字符串（美化格式）
 * size_t length;
 * char *output = cbor_json_dumps(root, &length, true);
 * printf("%s\n", output);
 *
 * // 4. 释放资源
 * free(output);
 * cbor_destroy(root);
 * ```
 *
 * 示例2：使用JSON Pointer修改数据
 * --------------------------------------
 * ```c
 * // 1. 创建JSON对象
 * cbor_value_t *obj = cbor_json_loads("{\"a\":1,\"b\":2}", -1);
 *
 * // 2. 插入新字段（如果已存在则不修改，使用cbor_pointer_insert）
 * cbor_value_t *val1 = cbor_init_integer(3);
 * cbor_value_t *result1 = cbor_pointer_insert(obj, "/c", val1);
 * // 成功: {"a":1,"b":2,"c":3}, result1指向插入的val1
 * // 如果/c已存在，返回NULL，需要手动destroy val1
 *
 * cbor_value_t *val2 = cbor_init_integer(99);
 * cbor_value_t *result2 = cbor_pointer_insert(obj, "/a", val2);
 * // 失败: {"a":1,"b":2,"c":3}, result2为NULL，/a已存在
 * if (result2 == NULL) {
 *     cbor_destroy(val2);  // 需要手动释放
 * }
 *
 * // 3. 替换已有字段（如果不存在则不操作，使用cbor_pointer_replace）
 * cbor_value_t *val3 = cbor_init_integer(100);
 * cbor_pointer_replace(obj, "/a", val3);  // {"a":100,"b":2,"c":3}
 *
 * // 4. 设置字段（存在则替换，不存在则插入，使用cbor_pointer_set）
 * cbor_value_t *val4 = cbor_init_integer(4);
 * cbor_pointer_set(obj, "/d", val4);  // {"a":100,"b":2,"c":3,"d":4}
 *
 * cbor_value_t *val5 = cbor_init_integer(200);
 * cbor_pointer_set(obj, "/a", val5);  // {"a":200,"b":2,"c":3,"d":4}
 *
 * // 5. 使用便捷函数直接设置值（推荐方式）
 * cbor_pointer_seti(obj, "/e", 5);       // 设置整数，返回int状态码
 * cbor_pointer_sets(obj, "/name", "test");  // 设置字符串
 * cbor_pointer_setb(obj, "/flag", true);    // 设置布尔值
 * cbor_pointer_setf(obj, "/pi", 3.14);      // 设置浮点数
 *
 * // 6. 移除字段
 * cbor_pointer_remove(obj, "/b");     // {"a":200,"c":3,"d":4,"e":5,...}
 *
 * cbor_destroy(obj);
 * ```
 *
 * 示例3：数组操作
 * --------------------------------------
 * ```c
 * // 1. 创建数组
 * cbor_value_t *arr = cbor_json_loads("[1,2,3,4]", -1);
 *
 * // 2. 在数组末尾追加元素（/-表示数组末尾）
 * // 方式1：使用便捷函数（推荐）
 * cbor_pointer_seti(arr, "/-", 99);        // [1,2,3,4,99]
 *
 * // 方式2：使用完整API
 * cbor_value_t *val = cbor_init_integer(100);
 * cbor_pointer_set(arr, "/-", val);        // [1,2,3,4,99,100]
 *
 * // 3. 在嵌套数组中追加
 * cbor_value_t *nested = cbor_json_loads("[1,[2,3],4]", -1);
 * cbor_pointer_seti(nested, "/1/-", 99);   // [1,[2,3,99],4]
 *
 * // 4. 替换数组元素
 * cbor_value_t *val2 = cbor_init_integer(88);
 * cbor_pointer_replace(nested, "/1/0", val2);  // [1,[88,3,99],4]
 *
 * // 或使用便捷函数
 * cbor_pointer_seti(nested, "/1/0", 77);   // [1,[77,3,99],4]
 *
 * cbor_destroy(arr);
 * cbor_destroy(nested);
 * ```
 *
 * 示例4：JSON Merge Patch（合并对象）
 * --------------------------------------
 * ```c
 * // 1. 原始对象
 * cbor_value_t *target = cbor_json_loads("{\"a\":1,\"b\":2}", -1);
 *
 * // 2. 补丁对象
 * cbor_value_t *patch = cbor_json_loads("{\"c\":3,\"d\":4}", -1);
 *
 * // 3. 合并（将patch中的字段合并到target）
 * cbor_patch(target, patch);  // target变为: {"a":1,"b":2,"c":3,"d":4}
 *
 * // 4. 覆盖已有字段
 * cbor_value_t *patch2 = cbor_json_loads("{\"a\":9}", -1);
 * cbor_patch(target, patch2);  // target变为: {"a":9,"b":2,"c":3,"d":4}
 *
 * // 5. 删除字段（使用null值）
 * cbor_value_t *patch3 = cbor_json_loads("{\"b\":null}", -1);
 * cbor_patch(target, patch3);  // target变为: {"a":9,"c":3,"d":4}
 *
 * cbor_destroy(target);
 * cbor_destroy(patch);
 * cbor_destroy(patch2);
 * cbor_destroy(patch3);
 * ```
 *
 * 示例5：CBOR二进制序列化
 * --------------------------------------
 * ```c
 * // 1. 创建数据对象
 * cbor_value_t *obj = cbor_json_loads("{\"name\":\"Bob\",\"age\":25}", -1);
 *
 * // 2. 序列化为CBOR二进制数据
 * size_t cbor_length;
 * char *cbor_data = cbor_dumps(obj, &cbor_length);
 * printf("CBOR size: %zu bytes\n", cbor_length);
 *
 * // 3. 从CBOR二进制数据反序列化
 * size_t parse_length = cbor_length;  // CRITICAL: 必须设置输入数据长度！
 * cbor_value_t *restored = cbor_loads(cbor_data, &parse_length);
 * // parse_length现在包含实际解析的字节数
 *
 * // 4. 验证数据
 * char *json_output;
 * cbor_json_dumps(restored, &json_output, false);
 * printf("%s\n", json_output);  // {"name":"Bob","age":25}
 *
 * // 5. 释放资源
 * free(cbor_data);
 * free(json_output);
 * cbor_destroy(obj);
 * cbor_destroy(restored);
 * ```
 *
 * 示例6：手动构建数据结构
 * --------------------------------------
 * ```c
 * // 1. 创建对象容器
 * cbor_value_t *obj = cbor_init_map();
 *
 * // 2. 创建键值对并插入
 * cbor_value_t *key1 = cbor_init_string("name", -1);
 * cbor_value_t *val1 = cbor_init_string("Charlie", -1);
 * cbor_value_t *pair1 = cbor_init_pair(key1, val1);
 * cbor_container_insert_tail(obj, pair1);
 *
 * cbor_value_t *key2 = cbor_init_string("age", -1);
 * cbor_value_t *val2 = cbor_init_integer(35);
 * cbor_value_t *pair2 = cbor_init_pair(key2, val2);
 * cbor_container_insert_tail(obj, pair2);
 *
 * // 3. 创建数组
 * cbor_value_t *arr = cbor_init_array();
 * cbor_container_insert_tail(arr, cbor_init_integer(10));
 * cbor_container_insert_tail(arr, cbor_init_integer(20));
 * cbor_container_insert_tail(arr, cbor_init_integer(30));
 *
 * // 4. 将数组添加到对象
 * cbor_value_t *key3 = cbor_init_string("scores", -1);
 * cbor_value_t *pair3 = cbor_init_pair(key3, arr);
 * cbor_container_insert_tail(obj, pair3);
 *
 * // 5. 输出结果
 * size_t length;
 * char *json = cbor_json_dumps(obj, &length, true);
 * printf("%s\n", json);  // {"name":"Charlie","age":35,"scores":[10,20,30]}
 *
 * free(json);
 * cbor_destroy(obj);
 * ```
 *
 * ============================================================================
 * 五、JSON Pointer 路径语法
 * ============================================================================
 *
 * JSON Pointer (RFC 6901) 使用斜杠分隔的路径访问JSON数据：
 *
 * - ""        - 根对象
 * - "/foo"    - 访问对象的"foo"字段
 * - "/foo/0"  - 访问对象"foo"字段中数组的第0个元素
 * - "/a~1b"   - 访问包含"/"字符的字段（~1表示/）
 * - "/m~0n"   - 访问包含"~"字符的字段（~0表示~）
 * - "/-"      - 数组末尾（用于插入操作）
 *
 * 示例：
 * ```json
 * {
 *   "foo": ["bar", "baz"],
 *   "pi": 3.14,
 *   "a/b": 123,
 *   "c~d": 456
 * }
 * ```
 * - "/foo"    → ["bar", "baz"]
 * - "/foo/0"  → "bar"
 * - "/foo/1"  → "baz"
 * - "/pi"     → 3.14
 * - "/a~1b"   → 123
 * - "/c~0d"   → 456
 * - "/foo/-"  → 数组末尾（用于插入）
 *
 * ============================================================================
 * 六、重要注意事项
 * ============================================================================
 *
 * 1. 内存管理
 *    - cbor_value_t对象必须使用cbor_destroy()释放
 *    - cbor_dumps()和cbor_json_dumps()返回的字符串需要free()
 *    - cbor_pointer_insert/replace/set操作失败时，传入的value需要手动destroy
 *
 * 2. cbor_loads()的length参数
 *    - 这是输入输出参数！输入时必须传入数据总长度
 *    - 输出时返回实际解析的字节数
 *    - 常见错误：未初始化length导致解析失败
 *
 * 3. cbor_json_loads()的size参数
 *    - 传入-1表示自动计算字符串长度（推荐）
 *    - 传入具体值表示指定解析的字节数
 *
 * 4. JSON Pointer操作的区别
 *    - cbor_pointer_insert(): 仅当路径不存在时插入，存在则返回NULL
 *      成功时返回插入的value指针，失败时返回NULL（需要手动destroy传入的value）
 *    - cbor_pointer_replace(): 仅当路径存在时替换，不存在则返回NULL
 *      成功时返回替换后的value指针，失败时返回NULL（需要手动destroy传入的value）
 *    - cbor_pointer_set(): 存在则替换，不存在则插入（推荐使用）
 *      总是成功，返回设置的value指针
 *    - cbor_pointer_setx()系列便捷函数（推荐使用）：
 *      * cbor_pointer_seti(obj, path, integer) - 设置整数，返回int状态码
 *      * cbor_pointer_sets(obj, path, string)  - 设置字符串
 *      * cbor_pointer_setb(obj, path, boolean) - 设置布尔值
 *      * cbor_pointer_setf(obj, path, double)  - 设置浮点数
 *      * cbor_pointer_seta(obj, path)          - 设置为空数组
 *      * cbor_pointer_seto(obj, path)          - 设置为空对象
 *      * cbor_pointer_setn(obj, path)          - 设置为null
 *      这些函数内部自动创建cbor_value_t对象，使用更简便
 *
 * 5. 数组操作
 *    - 使用"/-"表示数组末尾
 *    - 数组索引从0开始
 *    - 负数索引表示从末尾倒数（"-1"表示最后一个元素）
 *
 * 6. 线程安全
 *    - 本库不是线程安全的
 *    - 多线程环境下需要外部同步机制
 *
 * ============================================================================
 * 七、性能建议
 * ============================================================================
 *
 * 1. 选择合适的格式
 *    - JSON：人类可读，调试方便，Web API推荐
 *    - CBOR：紧凑高效，嵌入式系统、网络传输推荐
 *
 * 2. 避免频繁的序列化/反序列化
 *    - 尽量保持cbor_value_t对象在内存中
 *    - 只在必要时进行格式转换
 *
 * 3. 使用JSON Pointer批量操作
 *    - 优先使用cbor_pointer_setx()系列便捷函数（seti/sets/setb/setf等）
 *    - 这些函数比手动创建cbor_value_t对象更高效
 *    - 避免频繁调用cbor_init_xxx() + cbor_pointer_set()组合
 *
 * 4. 复用对象
 *    - 使用cbor_duplicate()复制对象
 *    - 使用cbor_copy()复制数据
 *
 * ============================================================================
 * 八、参考标准
 * ============================================================================
 *
 * - CBOR: RFC 7049 - https://tools.ietf.org/html/rfc7049
 * - JSON: RFC 7159 - https://tools.ietf.org/html/rfc7159
 * - JSON Pointer: RFC 6901 - https://tools.ietf.org/html/rfc6901
 * - JSON Merge Patch: RFC 7396 - https://www.rfc-editor.org/rfc/rfc7396
 *
 * ============================================================================
 */

#ifndef __CBOR_H__
#define __CBOR_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    JSON_PARSER_ALLOW_COMMENT = 1 << 0,
    JSON_PARSER_ALLOW_INF     = 1 << 1,
    JSON_PARSER_ALLOW_NAN     = 1 << 2,
    JSON_PARSER_REPORT_ERROR  = 1 << 3
};

typedef enum {
    CBOR_ITER_AFTER,
    CBOR_ITER_BEFORE,
} cbor_iter_dir;

typedef struct _cbor_value cbor_value_t;

typedef struct _cbor_iter {
    const cbor_value_t *container;
    cbor_value_t *next;
    cbor_iter_dir dir;
} cbor_iter_t;

const char *cbor_type_str(const cbor_value_t *val);
int cbor_destroy(cbor_value_t *val);

int cbor_blob_append(cbor_value_t *val, const char *src, size_t length);
int cbor_blob_append_v(cbor_value_t *val, const char *fmt, ...);
int cbor_blob_append_byte(cbor_value_t *val, uint8_t byte);
int cbor_blob_append_word(cbor_value_t *val, uint16_t word);
int cbor_blob_append_dword(cbor_value_t *val, uint32_t dword);
int cbor_blob_append_qword(cbor_value_t *val, uint64_t qword);


/**
 * @brief 检查容器是否为空
 * @param container 容器对象（数组或对象）
 * @return 为空返回true，否则返回false
 */
int cbor_container_empty(const cbor_value_t *container);

/**
 * @brief 获取容器的元素数量
 * @param container 容器对象（数组或对象）
 * @return 元素数量
 */
int cbor_container_size(const cbor_value_t *container);

int cbor_container_swap(cbor_value_t *ca, cbor_value_t *cb);

/**
 * @brief 清空容器中的所有元素
 * @param container 容器对象（数组或对象）
 * @return 成功返回0，失败返回-1
 * @note 会自动释放所有元素的内存
 */
int cbor_container_clear(cbor_value_t *container);

/**
 * @brief 在容器尾部插入元素
 *
 * @param container 容器对象（数组或对象）
 * @param val 要插入的元素
 *             - 对于数组：可以是任意类型的cbor_value_t
 *             - 对于对象：必须是pair类型（使用cbor_init_pair创建）
 *
 * @return 成功返回0，失败返回-1
 *
 * @note 重要事项：
 *       - val的parent必须为NULL（不能已属于其他容器）
 *       - 插入后val的所有权转移给container，不需要手动destroy
 *       - 对于对象容器，val必须是cbor_init_pair创建的键值对
 *
 * @code
 * // 数组示例
 * cbor_value_t *arr = cbor_init_array();
 * cbor_container_insert_tail(arr, cbor_init_integer(10));
 * cbor_container_insert_tail(arr, cbor_init_string("hello", -1));
 *
 * // 对象示例
 * cbor_value_t *obj = cbor_init_map();
 * cbor_value_t *key = cbor_init_string("name", -1);
 * cbor_value_t *value = cbor_init_string("Alice", -1);
 * cbor_value_t *pair = cbor_init_pair(key, value);
 * cbor_container_insert_tail(obj, pair);
 * @endcode
 */
int cbor_container_insert_tail(cbor_value_t *container, cbor_value_t *val);

/**
 * @brief 在容器头部插入元素
 *
 * @param container 容器对象（数组或对象）
 * @param val 要插入的元素（对于对象必须是pair类型）
 * @return 成功返回0，失败返回-1
 *
 * @note 使用方法与cbor_container_insert_tail相同
 *       插入后val成为容器的第一个元素
 */
int cbor_container_insert_head(cbor_value_t *container, cbor_value_t *val);

/**
 * @brief 在指定元素之后插入新元素
 *
 * @param container 容器对象（数组或对象）
 * @param elm 参考元素（必须已在container中）
 * @param val 要插入的新元素（对于对象必须是pair类型）
 * @return 成功返回0，失败返回-1
 *
 * @note 重要事项：
 *       - elm必须已经在container中（elm->parent == container）
 *       - val将被插入到elm之后的位置
 *
 * @code
 * cbor_value_t *arr = cbor_init_array();
 * cbor_value_t *val1 = cbor_init_integer(10);
 * cbor_value_t *val3 = cbor_init_integer(30);
 * cbor_container_insert_tail(arr, val1);
 * cbor_container_insert_tail(arr, val3);
 *
 * // 在val1之后插入20
 * cbor_value_t *val2 = cbor_init_integer(20);
 * cbor_container_insert_after(arr, val1, val2);
 * // 结果：[10, 20, 30]
 * @endcode
 */
int cbor_container_insert_after(cbor_value_t *container, cbor_value_t *elm, cbor_value_t *val);

/**
 * @brief 在指定元素之前插入新元素
 *
 * @param container 容器对象（数组或对象）
 * @param elm 参考元素（必须已在container中）
 * @param val 要插入的新元素（对于对象必须是pair类型）
 * @return 成功返回0，失败返回-1
 *
 * @note 使用方法与cbor_container_insert_after相同
 *       val将被插入到elm之前的位置
 */
int cbor_container_insert_before(cbor_value_t *container, cbor_value_t *elm, cbor_value_t *val);

long long cbor_integer(const cbor_value_t *val);
double cbor_real(const cbor_value_t *val);
int cbor_string_size(const cbor_value_t *val);
const char *cbor_string(const cbor_value_t *val);
bool cbor_boolean(const cbor_value_t *val);

bool cbor_is_boolean(const cbor_value_t *val);
bool cbor_is_integer(const cbor_value_t *val);
bool cbor_is_double(const cbor_value_t *val);
bool cbor_is_bytestring(const cbor_value_t *val);
bool cbor_is_string(const cbor_value_t *val);
bool cbor_is_map(const cbor_value_t *val);
bool cbor_is_array(const cbor_value_t *val);
bool cbor_is_tag(const cbor_value_t *val);
bool cbor_is_null(const cbor_value_t *val);
bool cbor_is_number(const cbor_value_t *val);

cbor_value_t *cbor_pair_key(const cbor_value_t *pair);
cbor_value_t *cbor_pair_value(const cbor_value_t *pair);
cbor_value_t *cbor_pair_set_key(cbor_value_t *pair, cbor_value_t *key);
cbor_value_t *cbor_pair_set_value(cbor_value_t *pair, cbor_value_t *val);

cbor_value_t *cbor_init_boolean(bool b);
cbor_value_t *cbor_init_null();
cbor_value_t *cbor_init_map();
cbor_value_t *cbor_init_array();
cbor_value_t *cbor_init_integer(long long l);
cbor_value_t *cbor_init_string(const char *str, int len);
cbor_value_t *cbor_init_double(double d);
cbor_value_t *cbor_init_bytestring(const char *str, int len);
cbor_value_t *cbor_init_pair(cbor_value_t *key, cbor_value_t *val);
cbor_value_t *cbor_init_tag(long item, cbor_value_t *content);

/**
 * @brief 获取容器中的第一个元素
 * @param container 容器对象（数组或对象）
 * @return 第一个元素的指针，容器为空则返回NULL
 */
cbor_value_t *cbor_container_first(const cbor_value_t *container);

/**
 * @brief 获取容器中的最后一个元素
 * @param container 容器对象（数组或对象）
 * @return 最后一个元素的指针，容器为空则返回NULL
 */
cbor_value_t *cbor_container_last(const cbor_value_t *container);

/**
 * @brief 获取容器中指定元素的下一个元素
 * @param container 容器对象
 * @param elm 当前元素
 * @return 下一个元素的指针，没有则返回NULL
 */
cbor_value_t *cbor_container_next(const cbor_value_t *container, cbor_value_t *elm);

/**
 * @brief 获取容器中指定元素的前一个元素
 * @param container 容器对象
 * @param elm 当前元素
 * @return 前一个元素的指针，没有则返回NULL
 */
cbor_value_t *cbor_container_prev(const cbor_value_t *container, cbor_value_t *elm);

/**
 * @brief 从容器中移除指定元素
 * @param container 容器对象
 * @param elm 要移除的元素
 * @return 被移除的元素指针（需要手动destroy）
 * @note 移除后需要手动调用cbor_destroy(elm)释放元素
 */
cbor_value_t *cbor_container_remove(cbor_value_t *container, cbor_value_t *elm);

int cbor_container_concat(cbor_value_t *dst, cbor_value_t *src);

/* JSON Pointer ref: https://tools.ietf.org/html/rfc6901 */
cbor_value_t *cbor_pointer_get(const cbor_value_t *container, const char *path);
cbor_value_t *cbor_pointer_insert(cbor_value_t *container, const char *path, cbor_value_t *value);
cbor_value_t *cbor_pointer_remove(cbor_value_t *container, const char *path);
cbor_value_t *cbor_pointer_set(cbor_value_t *container, const char *from, cbor_value_t *value);
cbor_value_t *cbor_pointer_replace(cbor_value_t *container, const char *path, cbor_value_t *value);
/* JSON Merge Patch ref: https://www.rfc-editor.org/rfc/rfc7396 */
int cbor_patch(cbor_value_t *dst, const cbor_value_t *src);
int cbor_pointer_join(char *buf, size_t size, ...);

int cbor_pointer_seta(cbor_value_t *container, const char *path);
int cbor_pointer_setb(cbor_value_t *container, const char *path, bool boolean);
int cbor_pointer_setf(cbor_value_t *container, const char *path, double dbl);
int cbor_pointer_seti(cbor_value_t *container, const char *path, long long integer);
int cbor_pointer_seto(cbor_value_t *container, const char *path);
int cbor_pointer_setn(cbor_value_t *container, const char *path);
int cbor_pointer_sets(cbor_value_t *container, const char *path, const char *str);
int cbor_pointer_setv(cbor_value_t *container, const char *path, cbor_value_t *val);

long long cbor_pointer_geti(const cbor_value_t *container, const char *path);
const char *cbor_pointer_gets(const cbor_value_t *container, const char *path);
bool cbor_pointer_getb(const cbor_value_t *container, const char *path);
double cbor_pointer_getf(const cbor_value_t *container, const char *path);

cbor_value_t *cbor_duplicate(const cbor_value_t *val);

void cbor_iter_init(cbor_iter_t *iter, const cbor_value_t *container, cbor_iter_dir dir);
cbor_value_t *cbor_iter_next(cbor_iter_t *iter);

cbor_value_t *cbor_get_parent(cbor_value_t *val);

long cbor_tag_set_item(cbor_value_t *val, long item);
cbor_value_t *cbor_tag_set_content(cbor_value_t *val, cbor_value_t *content);
long cbor_tag_get_item(const cbor_value_t *val);
cbor_value_t *cbor_tag_get_content(const cbor_value_t *val);

/* CBOR ref: https://tools.ietf.org/html/rfc7049 */

/**
 * @brief 从CBOR二进制数据解析为cbor_value_t对象
 *
 * @param src CBOR二进制数据指针（非NULL）
 * @param length [输入/输出参数] CRITICAL: 这是一个输入输出参数！
 *               - 输入时：必须传入待解析数据的总长度（字节数）
 *               - 输出时：返回实际消耗的字节数
 *               - 示例：
 *                   size_t len = dataSize;  // 输入：数据总长度
 *                   cbor_value_t *val = cbor_loads(data, &len);
 *                   // 输出：len现在包含实际解析的字节数
 *
 * @return 成功返回cbor_value_t指针，失败返回NULL
 *
 * @note 失败的常见原因：
 *       - src为NULL
 *       - length为NULL
 *       - *length为0（未传入数据长度！）
 *       - CBOR数据格式错误
 *
 * @warning 必须使用cbor_destroy()释放返回的对象
 */
cbor_value_t *cbor_loads(const char *src, size_t *length);

/**
 * @brief 将cbor_value_t对象序列化为CBOR二进制数据
 *
 * @param src cbor_value_t对象指针
 * @param length [输出参数] 返回生成的CBOR数据长度（字节数）
 *
 * @return 成功返回CBOR二进制数据指针，失败返回NULL
 *
 * @warning 返回的指针需要使用free()释放
 */
char *cbor_dumps(const cbor_value_t *src, size_t *length);

/* JSON ref: https://tools.ietf.org/html/rfc7159 */

/**
 * @brief 从JSON字符串解析为cbor_value_t对象（扩展版本）
 *
 * @param src JSON字符串指针
 * @param size JSON字符串长度，传-1表示自动计算（推荐）
 * @param flag 解析标志位，可以是以下值的组合（使用 | 运算符）：
 *             - JSON_PARSER_ALLOW_COMMENT: 允许JSON中包含注释（//和/*...* /）
 *             - JSON_PARSER_ALLOW_INF: 允许Infinity值
 *             - JSON_PARSER_ALLOW_NAN: 允许NaN值
 *             - JSON_PARSER_REPORT_ERROR: 解析失败时输出错误信息
 * @param consume [输出参数] 返回实际解析的字节数，可以传NULL
 *
 * @return 成功返回cbor_value_t指针，失败返回NULL
 *
 * @note - 返回的对象需要使用cbor_destroy()释放
 *       - 支持宽松的JSON格式（根据flag设置）
 *
 * @code
 * // 解析标准JSON
 * cbor_value_t *val = cbor_json_loads_ex("{\"a\":1}", -1, 0, NULL);
 *
 * // 解析带注释的JSON
 * const char *json_with_comment = "{\n  // comment\n  \"value\": 42\n}";
 * int consumed;
 * cbor_value_t *val2 = cbor_json_loads_ex(
 *     json_with_comment, -1,
 *     JSON_PARSER_ALLOW_COMMENT | JSON_PARSER_REPORT_ERROR,
 *     &consumed
 * );
 * @endcode
 */
cbor_value_t *cbor_json_loads_ex(const void *src, int size, int flag, int *consume);

/**
 * @brief 从JSON字符串解析为cbor_value_t对象（标准版本）
 *
 * @param src JSON字符串指针
 * @param size JSON字符串长度，传-1表示自动计算（推荐）
 * @return 成功返回cbor_value_t指针，失败返回NULL
 *
 * @note - 这是cbor_json_loads_ex的简化版本，不支持扩展选项
 *       - 返回的对象需要使用cbor_destroy()释放
 *
 * @code
 * cbor_value_t *val = cbor_json_loads("{\"name\":\"Alice\",\"age\":30}", -1);
 * if (val) {
 *     // 使用val...
 *     cbor_destroy(val);
 * }
 * @endcode
 */
cbor_value_t *cbor_json_loads(const void *src, int size);

/**
 * @brief 将cbor_value_t对象序列化为JSON字符串
 *
 * @param src cbor_value_t对象指针
 * @param length [输出参数] 返回生成的JSON字符串长度（不包含'\0'）
 * @param pretty 是否美化输出
 *               - true: 输出带缩进和换行的美化JSON
 *               - false: 输出紧凑的JSON（无多余空格）
 *
 * @return 成功返回JSON字符串指针，失败返回NULL
 *
 * @note - 返回的字符串需要使用free()释放
 *       - 美化输出使用4个空格作为缩进
 *
 * @code
 * cbor_value_t *obj = cbor_json_loads("{\"a\":1,\"b\":2}", -1);
 *
 * // 紧凑输出
 * size_t len1;
 * char *compact = cbor_json_dumps(obj, &len1, false);
 * // 输出: {"a": 1, "b": 2}
 *
 * // 美化输出
 * size_t len2;
 * char *pretty = cbor_json_dumps(obj, &len2, true);
 * // 输出:
 * // {
 * //     "a": 1,
 * //     "b": 2
 * // }
 *
 * free(compact);
 * free(pretty);
 * cbor_destroy(obj);
 * @endcode
 */
char *cbor_json_dumps(const cbor_value_t *src, size_t *length, bool pretty);

/**
 * @brief 从JSON文件加载数据
 *
 * @param path JSON文件路径
 * @return 成功返回cbor_value_t指针，失败返回NULL
 *
 * @note - 返回的对象需要使用cbor_destroy()释放
 *       - 自动支持扩展JSON格式（注释、Infinity、NaN）
 *       - 失败时会输出错误信息到stderr
 *
 * @code
 * cbor_value_t *data = cbor_json_loadf("/path/to/data.json");
 * if (data) {
 *     // 使用data...
 *     cbor_destroy(data);
 * } else {
 *     fprintf(stderr, "Failed to load JSON file\n");
 * }
 * @endcode
 */
cbor_value_t *cbor_json_loadf(const char *path);

/**
 * @brief 将cbor_value_t对象保存到JSON文件
 *
 * @param val cbor_value_t对象指针
 * @param path 输出文件路径
 * @param pretty 是否美化输出（true=美化，false=紧凑）
 * @return 成功返回0，失败返回-1
 *
 * @note - 如果文件已存在会被覆盖
 *       - 确保有写入权限
 *       - 失败原因可能是：val为NULL、文件无法打开、写入失败
 *
 * @code
 * cbor_value_t *obj = cbor_init_map();
 * // ... 构建obj ...
 *
 * // 保存为美化JSON
 * if (cbor_json_dumpf(obj, "output.json", true) == 0) {
 *     printf("File saved successfully\n");
 * } else {
 *     fprintf(stderr, "Failed to save file\n");
 * }
 *
 * cbor_destroy(obj);
 * @endcode
 */
int cbor_json_dumpf(cbor_value_t *val, const char *path, bool pretty);

int cbor_copy(cbor_value_t *dst, const cbor_value_t *src);

cbor_value_t *cbor_string_split(const char *str, const char *f);
cbor_value_t *cbor_string_splitn(const char *str, int l, const char *f);
cbor_value_t *cbor_string_split_linebreak(const char *str);
cbor_value_t *cbor_string_split_whitespace(const char *str);
cbor_value_t *cbor_string_split_character(const char *str, int length, const char *characters, int size);
cbor_value_t *cbor_string_join(cbor_value_t *array, const char *join);
void cbor_string_trim(cbor_value_t *val);
char *cbor_string_release(cbor_value_t *val);
int cbor_string_replace(cbor_value_t *str, const char *find, const char *replace);
bool cbor_string_startswith(const char *str, int length, const char *first);
bool cbor_string_endswith(const char *str, int length, const char *last);
int cbor_string_strip(cbor_value_t *str);
int cbor_string_rstrip(cbor_value_t *str);
int cbor_string_lstrip(cbor_value_t *str);
int string_find(const char *str, int len, const char *find, int count);
int string_rfind(const char *str, int len, const char *find, int count);
char **string_split(const char *str, const char *f);
char *string_join(const char *join, const char **result);
char *string__join_v(const char *join, ...);
#define string_join_v(join, ...) string__join_v(join, ## __VA_ARGS, NULL)
void string_free_split_result(char **result);
void string_free_join_result(char *result);

/* string slice:
 *   include `start` and `stop` character, start must greate equal 0, e.g.:
 *     slice("abcde", 0, -1) => "abcde"
 *     slice("abcde", 0, -2) => "abcd"
 *     slice("abcde", 0,  1) => "ab"
 */
int cbor_string_slice(cbor_value_t *str, int start, int stop);

#ifdef __cplusplus
}
#endif

#endif  /* !__CBOR_H__ */
