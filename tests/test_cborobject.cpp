/**
 * @file test_cborobject.cpp
 * @brief CborObject库完整功能测试程序
 *
 * 测试CborValue, CborArray, CborObject, CborDocument的所有API
 */

#include "CborValue.h"
#include "CborArray.h"
#include "CborObject.h"
#include "CborDocument.h"
#include <iostream>
#include <cassert>
#include <cmath>

// 测试辅助宏
#define TEST_START(name) std::cout << "\n========== " << name << " ==========" << std::endl
#define TEST_PASS(msg) std::cout << "✓ " << msg << std::endl
#define TEST_FAIL(msg) std::cout << "✗ " << msg << std::endl; return false
#define ASSERT_TRUE(cond, msg) if (!(cond)) { TEST_FAIL(msg); }
#define ASSERT_FALSE(cond, msg) if (cond) { TEST_FAIL(msg); }
#define ASSERT_EQ(a, b, msg) if ((a) != (b)) { std::cout << "Expected: " << (b) << ", Got: " << (a) << std::endl; TEST_FAIL(msg); }

// ========== CborValue测试 ==========

bool testCborValueBasicTypes() {
    TEST_START("测试1: CborValue基本类型");

    // 测试Null
    CborValue nullVal;
    ASSERT_TRUE(nullVal.isNull(), "默认构造应该是Null");
    ASSERT_EQ(nullVal.type(), CborValue::Null, "类型应该是Null");
    TEST_PASS("Null类型测试通过");

    // 测试Bool
    CborValue boolVal(true);
    ASSERT_TRUE(boolVal.isBool(), "应该是Bool类型");
    ASSERT_EQ(boolVal.toBool(), true, "值应该是true");
    TEST_PASS("Bool类型测试通过");

    // 测试Integer
    CborValue intVal(42);
    ASSERT_TRUE(intVal.isInteger(), "应该是Integer类型");
    ASSERT_EQ(intVal.toInt(), 42, "值应该是42");
    ASSERT_EQ(intVal.toLongLong(), 42LL, "长整型值应该是42");
    TEST_PASS("Integer类型测试通过");

    // 测试Double
    CborValue doubleVal(3.14);
    ASSERT_TRUE(doubleVal.isDouble(), "应该是Double类型");
    ASSERT_TRUE(std::abs(doubleVal.toDouble() - 3.14) < 0.0001, "值应该接近3.14");
    TEST_PASS("Double类型测试通过");

    // 测试String
    CborValue strVal("Hello CBOR");
    ASSERT_TRUE(strVal.isString(), "应该是String类型");
    ASSERT_EQ(strVal.toString(), std::string("Hello CBOR"), "字符串值应该匹配");
    TEST_PASS("String类型测试通过");

    // 测试const char*构造
    CborValue cstrVal("Test C String");
    ASSERT_TRUE(cstrVal.isString(), "应该是String类型");
    ASSERT_EQ(cstrVal.toString(), std::string("Test C String"), "C字符串值应该匹配");
    TEST_PASS("C字符串构造测试通过");

    return true;
}

bool testCborValueCopy() {
    TEST_START("测试2: CborValue复制和赋值");

    CborValue original(42);
    CborValue copy(original);

    ASSERT_EQ(copy.toInt(), 42, "复制构造应该复制值");
    ASSERT_TRUE(copy == original, "复制的对象应该相等");
    TEST_PASS("复制构造测试通过");

    CborValue assigned;
    assigned = original;
    ASSERT_EQ(assigned.toInt(), 42, "赋值应该复制值");
    ASSERT_TRUE(assigned == original, "赋值的对象应该相等");
    TEST_PASS("赋值运算符测试通过");

    return true;
}

bool testCborValueComparison() {
    TEST_START("测试3: CborValue比较运算");

    CborValue int1(10);
    CborValue int2(10);
    CborValue int3(20);

    ASSERT_TRUE(int1 == int2, "相同的整数应该相等");
    ASSERT_FALSE(int1 == int3, "不同的整数不应该相等");
    ASSERT_TRUE(int1 != int3, "不同的整数应该不相等");
    TEST_PASS("整数比较测试通过");

    CborValue str1("hello");
    CborValue str2("hello");
    CborValue str3("world");

    ASSERT_TRUE(str1 == str2, "相同的字符串应该相等");
    ASSERT_FALSE(str1 == str3, "不同的字符串不应该相等");
    TEST_PASS("字符串比较测试通过");

    return true;
}

// ========== CborArray测试 ==========

bool testCborArrayBasic() {
    TEST_START("测试4: CborArray基本操作");

    CborArray arr;
    ASSERT_TRUE(arr.isEmpty(), "新数组应该为空");
    ASSERT_EQ(arr.size(), 0, "空数组大小应该是0");
    TEST_PASS("空数组测试通过");

    // 添加元素
    arr.append(CborValue(1));
    arr.append(CborValue(2));
    arr.append(CborValue("three"));

    ASSERT_EQ(arr.size(), 3, "数组应该有3个元素");
    ASSERT_FALSE(arr.isEmpty(), "非空数组isEmpty应该返回false");
    TEST_PASS("append操作测试通过");

    // 访问元素
    ASSERT_EQ(arr.at(0).toInt(), 1, "第一个元素应该是1");
    ASSERT_EQ(arr.at(1).toInt(), 2, "第二个元素应该是2");
    ASSERT_EQ(arr.at(2).toString(), std::string("three"), "第三个元素应该是'three'");
    ASSERT_EQ(arr[1].toInt(), 2, "operator[]应该正常工作");
    TEST_PASS("元素访问测试通过");

    // first和last
    ASSERT_EQ(arr.first().toInt(), 1, "first()应该返回第一个元素");
    ASSERT_EQ(arr.last().toString(), std::string("three"), "last()应该返回最后一个元素");
    TEST_PASS("first/last操作测试通过");

    return true;
}

bool testCborArrayModify() {
    TEST_START("测试5: CborArray修改操作");

    CborArray arr;

    // prepend测试
    arr.prepend(CborValue(3));
    arr.prepend(CborValue(2));
    arr.prepend(CborValue(1));

    ASSERT_EQ(arr.size(), 3, "数组应该有3个元素");
    ASSERT_EQ(arr.at(0).toInt(), 1, "prepend应该在头部添加");
    ASSERT_EQ(arr.at(2).toInt(), 3, "最后元素应该是3");
    TEST_PASS("prepend操作测试通过");

    // insert测试
    arr.insert(1, CborValue(99));
    ASSERT_EQ(arr.size(), 4, "插入后应该有4个元素");
    ASSERT_EQ(arr.at(1).toInt(), 99, "插入的元素应该在正确位置");
    TEST_PASS("insert操作测试通过");

    // removeAt测试
    arr.removeAt(1);
    ASSERT_EQ(arr.size(), 3, "删除后应该有3个元素");
    ASSERT_EQ(arr.at(1).toInt(), 2, "删除后元素应该正确");
    TEST_PASS("removeAt操作测试通过");

    // removeFirst和removeLast
    arr.removeFirst();
    ASSERT_EQ(arr.size(), 2, "removeFirst后应该有2个元素");
    arr.removeLast();
    ASSERT_EQ(arr.size(), 1, "removeLast后应该有1个元素");
    TEST_PASS("removeFirst/removeLast操作测试通过");

    // clear测试
    arr.clear();
    ASSERT_TRUE(arr.isEmpty(), "clear后数组应该为空");
    TEST_PASS("clear操作测试通过");

    return true;
}

bool testCborArraySearch() {
    TEST_START("测试6: CborArray搜索操作");

    CborArray arr;
    arr.append(CborValue(10));
    arr.append(CborValue(20));
    arr.append(CborValue(30));

    ASSERT_TRUE(arr.contains(CborValue(20)), "应该包含20");
    ASSERT_FALSE(arr.contains(CborValue(40)), "不应该包含40");
    TEST_PASS("contains操作测试通过");

    // toVector测试
    std::vector<CborValue> vec = arr.toVector();
    ASSERT_EQ(vec.size(), static_cast<size_t>(3), "vector大小应该是3");
    ASSERT_EQ(vec[1].toInt(), 20, "vector元素应该正确");
    TEST_PASS("toVector操作测试通过");

    return true;
}

bool testCborArrayEquality() {
    TEST_START("测试7: CborArray相等性测试");

    CborArray arr1;
    arr1.append(CborValue(1));
    arr1.append(CborValue(2));

    CborArray arr2;
    arr2.append(CborValue(1));
    arr2.append(CborValue(2));

    CborArray arr3;
    arr3.append(CborValue(1));
    arr3.append(CborValue(3));

    ASSERT_TRUE(arr1 == arr2, "相同内容的数组应该相等");
    ASSERT_FALSE(arr1 == arr3, "不同内容的数组不应该相等");
    ASSERT_TRUE(arr1 != arr3, "不同内容的数组应该不相等");
    TEST_PASS("相等性测试通过");

    return true;
}

// ========== CborObject测试 ==========

bool testCborObjectBasic() {
    TEST_START("测试8: CborObject基本操作");

    CborObject obj;
    ASSERT_TRUE(obj.isEmpty(), "新对象应该为空");
    ASSERT_EQ(obj.size(), 0, "空对象大小应该是0");
    TEST_PASS("空对象测试通过");

    // 插入键值对
    obj.insert("name", CborValue("Alice"));
    obj.insert("age", CborValue(25));
    obj.insert("score", CborValue(95.5));

    ASSERT_EQ(obj.size(), 3, "对象应该有3个键值对");
    ASSERT_FALSE(obj.isEmpty(), "非空对象isEmpty应该返回false");
    TEST_PASS("insert操作测试通过");

    // 访问值
    ASSERT_EQ(obj.value("name").toString(), std::string("Alice"), "name应该是Alice");
    ASSERT_EQ(obj.value("age").toInt(), 25, "age应该是25");
    ASSERT_TRUE(std::abs(obj["score"].toDouble() - 95.5) < 0.0001, "score应该是95.5");
    TEST_PASS("值访问测试通过");

    return true;
}

bool testCborObjectKeys() {
    TEST_START("测试9: CborObject键操作");

    CborObject obj;
    obj.insert("x", CborValue(10));
    obj.insert("y", CborValue(20));
    obj.insert("z", CborValue(30));

    std::vector<std::string> keys = obj.keys();
    ASSERT_EQ(keys.size(), static_cast<size_t>(3), "应该有3个键");
    TEST_PASS("keys()操作测试通过");

    // contains测试
    ASSERT_TRUE(obj.contains("x"), "应该包含键'x'");
    ASSERT_TRUE(obj.contains("y"), "应该包含键'y'");
    ASSERT_FALSE(obj.contains("w"), "不应该包含键'w'");
    TEST_PASS("contains操作测试通过");

    return true;
}

bool testCborObjectModify() {
    TEST_START("测试10: CborObject修改操作");

    CborObject obj;
    obj.insert("a", CborValue(1));
    obj.insert("b", CborValue(2));

    // 更新值（重新插入）
    obj.insert("a", CborValue(99));
    ASSERT_EQ(obj.value("a").toInt(), 99, "更新后的值应该是99");
    ASSERT_EQ(obj.size(), 2, "更新不应该增加大小");
    TEST_PASS("更新值测试通过");

    // remove测试
    obj.remove("b");
    ASSERT_EQ(obj.size(), 1, "删除后应该有1个键值对");
    ASSERT_FALSE(obj.contains("b"), "删除后不应该包含'b'");
    TEST_PASS("remove操作测试通过");

    // take测试
    obj.insert("c", CborValue(3));
    CborValue takenVal = obj.take("c");
    ASSERT_EQ(takenVal.toInt(), 3, "take应该返回正确的值");
    ASSERT_FALSE(obj.contains("c"), "take后不应该包含该键");
    TEST_PASS("take操作测试通过");

    // clear测试
    obj.clear();
    ASSERT_TRUE(obj.isEmpty(), "clear后对象应该为空");
    TEST_PASS("clear操作测试通过");

    return true;
}

bool testCborObjectEquality() {
    TEST_START("测试11: CborObject相等性测试");

    CborObject obj1;
    obj1.insert("x", CborValue(1));
    obj1.insert("y", CborValue(2));

    CborObject obj2;
    obj2.insert("x", CborValue(1));
    obj2.insert("y", CborValue(2));

    CborObject obj3;
    obj3.insert("x", CborValue(1));
    obj3.insert("y", CborValue(3));

    ASSERT_TRUE(obj1 == obj2, "相同内容的对象应该相等");
    ASSERT_FALSE(obj1 == obj3, "不同内容的对象不应该相等");
    ASSERT_TRUE(obj1 != obj3, "不同内容的对象应该不相等");
    TEST_PASS("相等性测试通过");

    return true;
}

// ========== CborDocument测试 ==========

bool testCborDocumentJSON() {
    TEST_START("测试12: CborDocument JSON互操作");

    // 从JSON解析
    std::string jsonStr = R"({"name":"Bob","age":30,"active":true})";
    std::string errorInfo;
    CborDocument doc = CborDocument::fromJson(jsonStr, &errorInfo);

    ASSERT_FALSE(doc.isNull(), "解析后文档不应该为null");
    ASSERT_TRUE(doc.isObject(), "文档应该是对象类型");
    TEST_PASS("fromJson操作测试通过");

    // 访问对象
    CborObject obj = doc.object();
    ASSERT_EQ(obj.value("name").toString(), std::string("Bob"), "name应该是Bob");
    ASSERT_EQ(obj.value("age").toInt(), 30, "age应该是30");
    ASSERT_TRUE(obj.value("active").toBool(), "active应该是true");
    TEST_PASS("对象访问测试通过");

    // 转换回JSON
    std::string jsonOutput = doc.toJson(CborDocument::Compact);
    ASSERT_FALSE(jsonOutput.empty(), "JSON输出不应该为空");
    TEST_PASS("toJson操作测试通过");

    return true;
}

bool testCborDocumentArray() {
    TEST_START("测试13: CborDocument数组支持");

    // 创建数组文档
    CborArray arr;
    arr.append(CborValue(1));
    arr.append(CborValue(2));
    arr.append(CborValue(3));

    CborDocument doc(arr);
    ASSERT_TRUE(doc.isArray(), "文档应该是数组类型");
    TEST_PASS("数组文档创建测试通过");

    // 访问数组
    CborArray docArr = doc.array();
    ASSERT_EQ(docArr.size(), 3, "数组应该有3个元素");
    ASSERT_EQ(docArr.at(0).toInt(), 1, "第一个元素应该是1");
    TEST_PASS("数组访问测试通过");

    // 转换为JSON
    std::string jsonStr = doc.toJson();
    ASSERT_FALSE(jsonStr.empty(), "JSON输出不应该为空");
    TEST_PASS("数组toJson测试通过");

    return true;
}

bool testCborDocumentPointer() {
    TEST_START("测试14: CborDocument JSON Pointer支持");

    std::string jsonStr = R"({
        "user": {
            "name": "Charlie",
            "contact": {
                "email": "charlie@example.com"
            }
        }
    })";

    CborDocument doc = CborDocument::fromJson(jsonStr);

    // 使用JSON Pointer访问
    CborValue name = doc.pointer("/user/name");
    ASSERT_EQ(name.toString(), std::string("Charlie"), "通过pointer访问name");
    TEST_PASS("pointer读取测试通过");

    CborValue email = doc.pointer("/user/contact/email");
    ASSERT_EQ(email.toString(), std::string("charlie@example.com"), "通过pointer访问嵌套值");
    TEST_PASS("pointer嵌套访问测试通过");

    return true;
}

bool testCborDocumentCBOR() {
    TEST_START("测试15: CborDocument CBOR二进制格式");

    // 创建文档
    CborObject obj;
    obj.insert("test", CborValue(123));
    CborDocument doc(obj);

    // 转换为CBOR二进制
    std::string cborData = doc.toCbor();
    ASSERT_FALSE(cborData.empty(), "CBOR数据不应该为空");
    TEST_PASS("toCbor操作测试通过");

    // 从CBOR解析回来
    std::string errorInfo;
    CborDocument doc2 = CborDocument::fromCbor(cborData, &errorInfo);
    ASSERT_FALSE(doc2.isNull(), "解析后文档不应该为null");

    CborObject obj2 = doc2.object();
    ASSERT_EQ(obj2.value("test").toInt(), 123, "CBOR往返后值应该正确");
    TEST_PASS("fromCbor操作测试通过");

    return true;
}

bool testCborDocumentFile() {
    TEST_START("测试16: CborDocument文件I/O");

    // 创建测试数据
    CborObject obj;
    obj.insert("fileTest", CborValue("success"));
    obj.insert("number", CborValue(42));
    CborDocument doc(obj);

    // 保存到JSON文件
    std::string jsonFile = "gpjson_test_tmp.json";  // relative to the test working directory
    bool saveOk = doc.toJsonFile(jsonFile);
    ASSERT_TRUE(saveOk, "保存JSON文件应该成功");
    TEST_PASS("toJsonFile操作测试通过");

    // 从JSON文件读取
    std::string errorInfo;
    CborDocument doc2 = CborDocument::fromJsonFile(jsonFile, &errorInfo);
    ASSERT_FALSE(doc2.isNull(), "从文件读取应该成功");

    CborObject obj2 = doc2.object();
    ASSERT_EQ(obj2.value("fileTest").toString(), std::string("success"), "文件I/O后值应该正确");
    TEST_PASS("fromJsonFile操作测试通过");

    // 保存到CBOR文件
    std::string cborFile = "gpjson_test_tmp.cbor";  // relative to the test working directory
    bool saveCborOk = doc.toCborFile(cborFile);
    ASSERT_TRUE(saveCborOk, "保存CBOR文件应该成功");
    TEST_PASS("toCborFile操作测试通过");

    // 从CBOR文件读取
    CborDocument doc3 = CborDocument::fromCborFile(cborFile, &errorInfo);
    ASSERT_FALSE(doc3.isNull(), "从CBOR文件读取应该成功");

    CborObject obj3 = doc3.object();
    ASSERT_EQ(obj3.value("number").toInt(), 42, "CBOR文件I/O后值应该正确");
    TEST_PASS("fromCborFile操作测试通过");

    return true;
}

// ========== 复杂场景测试 ==========

bool testComplexStructure() {
    TEST_START("测试17: 复杂嵌套结构");

    // 创建复杂的嵌套结构
    CborObject root;

    // 添加基本类型
    root.insert("id", CborValue(1001));
    root.insert("name", CborValue("Complex Test"));

    // 添加数组
    CborArray tags;
    tags.append(CborValue("tag1"));
    tags.append(CborValue("tag2"));
    tags.append(CborValue("tag3"));
    root.insert("tags", CborValue(tags));

    // 添加嵌套对象
    CborObject metadata;
    metadata.insert("created", CborValue("2026-01-05"));
    metadata.insert("version", CborValue(2));
    root.insert("metadata", CborValue(metadata));

    // 验证结构
    ASSERT_EQ(root.value("id").toInt(), 1001, "id应该正确");
    ASSERT_EQ(root.value("tags").toArray().size(), 3, "tags数组应该有3个元素");
    ASSERT_EQ(root.value("metadata").toObject().value("version").toInt(), 2, "嵌套对象值应该正确");
    TEST_PASS("复杂结构创建测试通过");

    // JSON序列化和反序列化
    CborDocument doc(root);
    std::string jsonStr = doc.toJson(CborDocument::Indented);

    CborDocument doc2 = CborDocument::fromJson(jsonStr);
    CborObject root2 = doc2.object();

    ASSERT_EQ(root2.value("id").toInt(), 1001, "JSON往返后id应该正确");
    ASSERT_EQ(root2.value("tags").toArray().at(1).toString(), std::string("tag2"), "JSON往返后数组元素应该正确");
    TEST_PASS("复杂结构JSON序列化测试通过");

    return true;
}

bool testCborValueFromArray() {
    TEST_START("测试18: CborValue从Array/Object构造");

    // 从CborArray构造CborValue
    CborArray arr;
    arr.append(CborValue(1));
    arr.append(CborValue(2));

    CborValue valFromArray(arr);
    ASSERT_TRUE(valFromArray.isArray(), "应该是数组类型");
    ASSERT_EQ(valFromArray.toArray().size(), 2, "数组大小应该是2");
    TEST_PASS("从CborArray构造CborValue测试通过");

    // 从CborObject构造CborValue
    CborObject obj;
    obj.insert("key", CborValue("value"));

    CborValue valFromObj(obj);
    ASSERT_TRUE(valFromObj.isObject(), "应该是对象类型");
    ASSERT_EQ(valFromObj.toObject().value("key").toString(), std::string("value"), "对象值应该正确");
    TEST_PASS("从CborObject构造CborValue测试通过");

    return true;
}

// ========== 主测试函数 ==========

int main() {
    std::cout << "==================== CborObject库完整功能测试 ====================" << std::endl;
    std::cout << "测试环境: Windows 10 + MSVC 19.44" << std::endl;
    std::cout << "测试日期: 2026-01-05" << std::endl;

    int passCount = 0;
    int failCount = 0;

    #define RUN_TEST(func) \
        if (func()) { \
            passCount++; \
        } else { \
            failCount++; \
        }

    // 运行所有测试
    RUN_TEST(testCborValueBasicTypes);
    RUN_TEST(testCborValueCopy);
    RUN_TEST(testCborValueComparison);
    RUN_TEST(testCborArrayBasic);
    RUN_TEST(testCborArrayModify);
    RUN_TEST(testCborArraySearch);
    RUN_TEST(testCborArrayEquality);
    RUN_TEST(testCborObjectBasic);
    RUN_TEST(testCborObjectKeys);
    RUN_TEST(testCborObjectModify);
    RUN_TEST(testCborObjectEquality);
    RUN_TEST(testCborDocumentJSON);
    RUN_TEST(testCborDocumentArray);
    RUN_TEST(testCborDocumentPointer);
    RUN_TEST(testCborDocumentCBOR);
    RUN_TEST(testCborDocumentFile);
    RUN_TEST(testComplexStructure);
    RUN_TEST(testCborValueFromArray);

    // 输出结果
    std::cout << "\n==================== 测试结果总结 ====================" << std::endl;
    std::cout << "通过: " << passCount << " 个测试" << std::endl;
    std::cout << "失败: " << failCount << " 个测试" << std::endl;
    std::cout << "总计: " << (passCount + failCount) << " 个测试" << std::endl;

    if (failCount == 0) {
        std::cout << "\n✓ 所有测试通过！CborObject库功能正常。" << std::endl;
        return 0;
    } else {
        std::cout << "\n✗ 有测试失败，请检查输出信息。" << std::endl;
        return 1;
    }
}
