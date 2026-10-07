/**
 * @file 01_document_basic.cpp
 * @brief CborDocument 基本使用示例
 *
 * 本示例展示C++封装的基本用法：
 * - CborDocument 解析和序列化
 * - CborObject、CborArray、CborValue的基本操作
 * - 自动内存管理（RAII）
 */

#include "../CborDocument.h"
#include "../CborObject.h"
#include "../CborArray.h"
#include "../CborValue.h"
#include <iostream>
#include <string>

void print_separator(const std::string &title) {
    std::cout << "\n========== " << title << " ==========\n";
}

int main() {
    std::cout << "CborDocument C++ API 基础示例\n";

    // ========== 1. 从JSON字符串解析 ==========
    print_separator("1. 解析JSON");

    std::string json = R"({
        "name": "Alice",
        "age": 30,
        "scores": [85, 90, 95],
        "address": {
            "city": "Beijing",
            "zipcode": "100000"
        }
    })";

    CborDocument doc = CborDocument::fromJson(json);
    if (!doc.isNull()) {
        std::cout << "✓ JSON解析成功\n";
    }

    // ========== 2. 访问数据 ==========
    print_separator("2. 访问数据");

    // 获取根对象
    CborObject root = doc.object();

    // 访问基本类型
    std::string name = root["name"].toString();
    int age = root["age"].toInt();

    std::cout << "姓名: " << name << "\n";
    std::cout << "年龄: " << age << "\n";

    // 访问嵌套对象
    CborObject address = root["address"].toObject();
    std::string city = address["city"].toString();

    std::cout << "城市: " << city << "\n";

    // 访问数组
    CborArray scores = root["scores"].toArray();
    std::cout << "成绩: ";
    for (int i = 0; i < scores.size(); i++) {
        std::cout << scores[i].toInt() << " ";
    }
    std::cout << "\n";

    // ========== 3. 使用JSON Pointer ==========
    print_separator("3. JSON Pointer");

    // 直接通过路径访问
    std::string email_check = doc.pointer("/address/city").toString();
    std::cout << "通过Pointer访问城市: " << email_check << "\n";

    int first_score = doc.pointer("/scores/0").toInt();
    std::cout << "第一个成绩: " << first_score << "\n";

    // ========== 4. 修改数据 ==========
    print_separator("4. 修改数据");

    // 修改现有字段
    root.insert("age", 31);  // insert会覆盖已存在的键
    std::cout << "✓ 修改年龄为: 31\n";

    // 添加新字段
    root.insert("email", CborValue("alice@example.com"));
    std::cout << "✓ 添加邮箱\n";

    // ========== 5. 构建数据结构 ==========
    print_separator("5. 构建新数据");

    // 创建新对象
    CborObject newObj;
    newObj.insert("id", 12345);
    newObj.insert("name", "Bob");
    newObj.insert("active", true);

    // 创建新数组
    CborArray newArr;
    newArr.append(1);
    newArr.append(2);
    newArr.append(3);

    newObj.insert("items", newArr);

    // 创建新文档
    CborDocument newDoc(newObj);
    std::cout << "✓ 创建新文档\n";

    std::cout << "\n新文档内容:\n" << newDoc.toJson() << "\n";

    // ========== 6. JSON与CBOR互转 ==========
    print_separator("6. JSON与CBOR互转");

    // 转为JSON
    std::string jsonOutput = newDoc.toJson(CborDocument::Compact);
    std::cout << "JSON紧凑: " << jsonOutput << "\n";
    std::cout << "JSON大小: " << jsonOutput.size() << " 字节\n";

    // 转为CBOR
    std::string cborOutput = newDoc.toCbor();
    std::cout << "CBOR大小: " << cborOutput.size() << " 字节\n";
    std::cout << "节省空间: " <<
        (1.0 - (double)cborOutput.size() / jsonOutput.size()) * 100 << "%\n";

    // 从CBOR还原
    CborDocument restored = CborDocument::fromCbor(cborOutput);
    std::cout << "✓ 从CBOR还原成功\n";

    // ========== 7. 文件操作 ==========
    print_separator("7. 文件操作");

    // 保存为JSON文件
    if (newDoc.toJsonFile("output_cpp.json")) {
        std::cout << "✓ 保存JSON文件成功\n";
    }

    // 保存为CBOR文件
    if (newDoc.toCborFile("output_cpp.cbor")) {
        std::cout << "✓ 保存CBOR文件成功\n";
    }

    // 从文件加载
    CborDocument loaded = CborDocument::fromJsonFile("output_cpp.json");
    if (!loaded.isNull()) {
        std::cout << "✓ 从JSON文件加载成功\n";
    }

    // ========== 8. 自动内存管理 ==========
    print_separator("8. 自动内存管理（RAII）");

    std::cout << "\nC++封装的优势：\n";
    std::cout << "  ✓ 自动内存管理，无需手动释放\n";
    std::cout << "  ✓ 异常安全，资源自动清理\n";
    std::cout << "  ✓ 值语义，可以自由拷贝和赋值\n";
    std::cout << "  ✓ Qt风格API，易于集成Qt项目\n";

    {
        CborDocument tempDoc = CborDocument::fromJson("{\"test\":123}");
        // tempDoc会在作用域结束时自动释放
    }
    std::cout << "  ✓ 局部对象已自动释放\n";

    std::cout << "\n========== 示例完成 ==========\n";
    std::cout << "\n注意：所有对象在函数结束时自动释放，无需手动管理内存\n";

    return 0;
}
