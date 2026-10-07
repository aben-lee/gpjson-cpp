/**
 * @file 04_cbor_binary.c
 * @brief CBOR二进制序列化示例
 *
 * 本示例展示：
 * - JSON与CBOR互转
 * - 空间占用对比
 * - 二进制数据（bytestring）处理
 * - 文件存储
 */

#include "../cbor/cbor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_separator(const char *title) {
    printf("\n========== %s ==========\n", title);
}

void print_hex(const unsigned char *data, size_t len, const char *title) {
    printf("\n【%s】(%zu 字节)\n", title, len);
    for (size_t i = 0; i < len; i++) {
        printf("%02x ", data[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    if (len % 16 != 0) printf("\n");
}

int main() {
    printf("CBOR 二进制序列化示例\n");

    // ========== 1. JSON与CBOR互转基础 ==========
    print_separator("1. JSON转CBOR");

    const char *json_str = "{\"name\":\"Alice\",\"age\":30,\"active\":true}";
    printf("原始JSON: %s\n", json_str);
    printf("JSON大小: %zu 字节\n", strlen(json_str));

    // 解析JSON
    cbor_value_t *data = cbor_json_loads(json_str, -1);

    // 序列化为CBOR
    size_t cbor_len;
    unsigned char *cbor_data = cbor_dumps(data, &cbor_len);
    printf("\nCBOR大小: %zu 字节\n", cbor_len);
    printf("节省空间: %.1f%%\n", (1.0 - (double)cbor_len / strlen(json_str)) * 100);

    print_hex(cbor_data, cbor_len, "CBOR二进制数据");

    // ========== 2. CBOR转JSON ==========
    print_separator("2. CBOR转JSON");

    // 从CBOR二进制还原
    cbor_value_t *restored = cbor_loads(cbor_data, cbor_len);

    // 转回JSON
    char *restored_json = cbor_json_dumps(restored, NULL, false);
    printf("还原的JSON: %s\n", restored_json);
    printf("✓ 数据完全一致\n");

    free(restored_json);
    free(cbor_data);
    cbor_destroy(data);
    cbor_destroy(restored);

    // ========== 3. 不同数据类型的CBOR编码 ==========
    print_separator("3. 不同数据类型的CBOR编码");

    // 整数
    cbor_value_t *val_int = cbor_init_integer(42);
    unsigned char *cbor_int = cbor_dumps(val_int, &cbor_len);
    printf("整数 42 的CBOR编码: ");
    for (size_t i = 0; i < cbor_len; i++) printf("%02x ", cbor_int[i]);
    printf("(%zu字节)\n", cbor_len);
    free(cbor_int);
    cbor_destroy(val_int);

    // 字符串
    cbor_value_t *val_str = cbor_init_string("Hello", -1);
    unsigned char *cbor_str = cbor_dumps(val_str, &cbor_len);
    printf("字符串 \"Hello\" 的CBOR编码: ");
    for (size_t i = 0; i < cbor_len; i++) printf("%02x ", cbor_str[i]);
    printf("(%zu字节)\n", cbor_len);
    free(cbor_str);
    cbor_destroy(val_str);

    // 布尔
    cbor_value_t *val_bool = cbor_init_boolean(true);
    unsigned char *cbor_bool = cbor_dumps(val_bool, &cbor_len);
    printf("布尔 true 的CBOR编码: ");
    for (size_t i = 0; i < cbor_len; i++) printf("%02x ", cbor_bool[i]);
    printf("(%zu字节)\n", cbor_len);
    free(cbor_bool);
    cbor_destroy(val_bool);

    // Null
    cbor_value_t *val_null = cbor_init_null();
    unsigned char *cbor_null = cbor_dumps(val_null, &cbor_len);
    printf("null 的CBOR编码: ");
    for (size_t i = 0; i < cbor_len; i++) printf("%02x ", cbor_null[i]);
    printf("(%zu字节)\n", cbor_len);
    free(cbor_null);
    cbor_destroy(val_null);

    // ========== 4. 数组的CBOR编码 ==========
    print_separator("4. 数组的CBOR编码");

    const char *array_json = "[1,2,3,4,5]";
    cbor_value_t *arr = cbor_json_loads(array_json, -1);

    printf("JSON数组: %s (%zu字节)\n", array_json, strlen(array_json));

    unsigned char *arr_cbor = cbor_dumps(arr, &cbor_len);
    printf("CBOR编码: %zu字节\n", cbor_len);
    print_hex(arr_cbor, cbor_len, "数组CBOR编码");

    free(arr_cbor);
    cbor_destroy(arr);

    // ========== 5. 对象的CBOR编码 ==========
    print_separator("5. 对象的CBOR编码");

    const char *obj_json = "{\"a\":1,\"b\":2,\"c\":3}";
    cbor_value_t *obj = cbor_json_loads(obj_json, -1);

    printf("JSON对象: %s (%zu字节)\n", obj_json, strlen(obj_json));

    unsigned char *obj_cbor = cbor_dumps(obj, &cbor_len);
    printf("CBOR编码: %zu字节\n", cbor_len);
    print_hex(obj_cbor, cbor_len, "对象CBOR编码");

    free(obj_cbor);
    cbor_destroy(obj);

    // ========== 6. 复杂嵌套结构的空间对比 ==========
    print_separator("6. 复杂结构空间对比");

    const char *complex_json =
        "{"
        "  \"users\": ["
        "    {\"id\":1,\"name\":\"Alice\",\"age\":30,\"scores\":[85,90,95]},"
        "    {\"id\":2,\"name\":\"Bob\",\"age\":25,\"scores\":[78,88,92]},"
        "    {\"id\":3,\"name\":\"Charlie\",\"age\":35,\"scores\":[92,95,98]}"
        "  ],"
        "  \"metadata\": {"
        "    \"version\":\"1.0\","
        "    \"timestamp\":1609459200,"
        "    \"count\":3"
        "  }"
        "}";

    cbor_value_t *complex = cbor_json_loads(complex_json, -1);

    // JSON格式（紧凑）
    char *json_compact = cbor_json_dumps(complex, &cbor_len, false);
    size_t json_size = strlen(json_compact);
    printf("JSON紧凑格式: %zu 字节\n", json_size);

    // JSON格式（美化）
    char *json_pretty = cbor_json_dumps(complex, &cbor_len, true);
    size_t json_pretty_size = strlen(json_pretty);
    printf("JSON美化格式: %zu 字节\n", json_pretty_size);

    // CBOR格式
    unsigned char *cbor_compact = cbor_dumps(complex, &cbor_len);
    printf("CBOR二进制: %zu 字节\n", cbor_len);

    printf("\n空间节省：\n");
    printf("  CBOR vs JSON紧凑: %.1f%%\n", (1.0 - (double)cbor_len / json_size) * 100);
    printf("  CBOR vs JSON美化: %.1f%%\n", (1.0 - (double)cbor_len / json_pretty_size) * 100);

    free(json_compact);
    free(json_pretty);
    free(cbor_compact);
    cbor_destroy(complex);

    // ========== 7. Bytestring（二进制数据）==========
    print_separator("7. Bytestring处理");

    // CBOR支持纯二进制数据（bytestring），这是JSON不支持的
    unsigned char binary_data[] = {0x00, 0x01, 0x02, 0x03, 0xFF, 0xFE, 0xFD};
    cbor_value_t *bytestr = cbor_init_bytestring(binary_data, sizeof(binary_data));

    printf("创建bytestring: %zu字节二进制数据\n", sizeof(binary_data));

    // 序列化
    size_t bytestr_cbor_len;
    unsigned char *bytestr_cbor = cbor_dumps(bytestr, &bytestr_cbor_len);
    print_hex(bytestr_cbor, bytestr_cbor_len, "Bytestring的CBOR编码");

    // 注意：JSON不能直接表示二进制数据，会用Base64等编码
    char *bytestr_json = cbor_json_dumps(bytestr, NULL, false);
    printf("JSON表示（可能是Base64或其他编码）: %s\n", bytestr_json);

    free(bytestr_cbor);
    free(bytestr_json);
    cbor_destroy(bytestr);

    // ========== 8. 保存CBOR到文件 ==========
    print_separator("8. CBOR文件存储");

    cbor_value_t *file_data = cbor_json_loads(
        "{\"title\":\"Test\",\"value\":12345,\"items\":[1,2,3]}", -1);

    // 序列化为CBOR
    size_t file_cbor_len;
    unsigned char *file_cbor = cbor_dumps(file_data, &file_cbor_len);

    // 保存到文件
    FILE *fp = fopen("output_cbor.bin", "wb");
    if (fp) {
        fwrite(file_cbor, 1, file_cbor_len, fp);
        fclose(fp);
        printf("✓ CBOR数据已保存到 output_cbor.bin (%zu字节)\n", file_cbor_len);
    }

    free(file_cbor);

    // 从文件加载
    fp = fopen("output_cbor.bin", "rb");
    if (fp) {
        fseek(fp, 0, SEEK_END);
        long file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        unsigned char *loaded_cbor = (unsigned char *)malloc(file_size);
        fread(loaded_cbor, 1, file_size, fp);
        fclose(fp);

        cbor_value_t *loaded_data = cbor_loads(loaded_cbor, file_size);
        if (loaded_data) {
            printf("✓ 从文件加载CBOR成功\n");

            char *verify_json = cbor_json_dumps(loaded_data, NULL, false);
            printf("验证数据: %s\n", verify_json);

            free(verify_json);
            cbor_destroy(loaded_data);
        }

        free(loaded_cbor);
    }

    cbor_destroy(file_data);

    // ========== 9. 性能总结 ==========
    print_separator("9. CBOR优势总结");

    printf("\nCBOR相比JSON的优势：\n");
    printf("  ✓ 空间占用：节省30-50%%存储空间\n");
    printf("  ✓ 解析速度：二进制格式，解析速度快1.5-2倍\n");
    printf("  ✓ 数据类型：支持bytestring（二进制数据）\n");
    printf("  ✓ 精确性：整数和浮点数类型明确，无精度损失\n");
    printf("  ✓ 扩展性：支持标签（tag）扩展类型系统\n");

    printf("\n适用场景：\n");
    printf("  ✓ 网络传输（减少带宽占用）\n");
    printf("  ✓ 嵌入式系统（节省存储空间）\n");
    printf("  ✓ 高性能应用（更快的序列化/反序列化）\n");
    printf("  ✓ 二进制数据传输（图片、文件等）\n");

    printf("\nJSON适用场景：\n");
    printf("  ✓ 人类可读性要求高\n");
    printf("  ✓ 调试和日志记录\n");
    printf("  ✓ Web API（浏览器原生支持）\n");
    printf("  ✓ 配置文件\n");

    printf("\n========== 示例完成 ==========\n");
    return 0;
}
