#ifndef CBORDOCUMENT_H
#define CBORDOCUMENT_H

#include "CborValue.h"
#include "CborArray.h"
#include "CborObject.h"
#include <memory>
#include <string>

/**
 * @brief CborDocument类封装CBOR文档，提供Qt风格的API
 *
 * CborDocument是CBOR数据的顶层容器，负责数据的序列化和反序列化。
 * 支持JSON和CBOR二进制格式的互相转换。
 * 提供类似QJsonDocument的接口。
 */
class CborDocument {
public:
    /**
     * @brief CBOR/JSON输出格式
     */
    enum CborFormat {
        Compact,    ///< 紧凑格式（无缩进）
        Indented    ///< 缩进格式（易读）
    };

    // 构造函数
    CborDocument();
    CborDocument(const CborObject &object);
    CborDocument(const CborArray &array);
    CborDocument(const CborDocument &other);
    ~CborDocument();

    // 赋值运算符
    CborDocument &operator=(const CborDocument &other);

    // ========== JSON互操作 ==========

    /**
     * @brief 从JSON字符串解析
     * @param json JSON字符串
     * @param errorInfo 错误信息输出（可选）
     * @return 解析后的CborDocument
     */
    static CborDocument fromJson(const std::string &json, std::string *errorInfo = nullptr);

    /**
     * @brief 从JSON文件加载
     * @param filePath JSON文件路径
     * @param errorInfo 错误信息输出（可选）
     * @return 解析后的CborDocument
     */
    static CborDocument fromJsonFile(const std::string &filePath, std::string *errorInfo = nullptr);

    /**
     * @brief 转换为JSON字符串
     * @param format 输出格式（Compact或Indented）
     * @return JSON字符串
     */
    std::string toJson(CborFormat format = Indented) const;

    /**
     * @brief 保存为JSON文件
     * @param filePath 文件路径
     * @param format 输出格式（Compact或Indented）
     * @return 成功返回true，失败返回false
     */
    bool toJsonFile(const std::string &filePath, CborFormat format = Indented) const;

    // ========== CBOR二进制格式互操作 ==========

    /**
     * @brief 从CBOR二进制数据解析
     * @param cbor CBOR二进制数据
     * @param errorInfo 错误信息输出（可选）
     * @return 解析后的CborDocument
     */
    static CborDocument fromCbor(const std::string &cbor, std::string *errorInfo = nullptr);

    /**
     * @brief 从CBOR二进制文件加载
     * @param filePath CBOR文件路径
     * @param errorInfo 错误信息输出（可选）
     * @return 解析后的CborDocument
     */
    static CborDocument fromCborFile(const std::string &filePath, std::string *errorInfo = nullptr);

    /**
     * @brief 转换为CBOR二进制数据
     * @return CBOR二进制字符串
     */
    std::string toCbor() const;

    /**
     * @brief 保存为CBOR二进制文件
     * @param filePath 文件路径
     * @return 成功返回true，失败返回false
     */
    bool toCborFile(const std::string &filePath) const;

    // ========== 文档内容访问 ==========

    bool isNull() const;
    bool isEmpty() const;
    bool isArray() const;
    bool isObject() const;

    CborObject object() const;
    CborArray array() const;

    // ========== JSON Pointer支持 (RFC 6901) ==========

    /**
     * @brief 通过JSON Pointer路径获取值
     * @param path JSON Pointer路径，例如 "/features/0/DataSet/dimensions"
     * @return 对应路径的值
     */
    CborValue pointer(const std::string &path) const;

    /**
     * @brief 通过JSON Pointer路径设置值
     * @param path JSON Pointer路径
     * @param value 要设置的值
     * @return 成功返回true，失败返回false
     */
    bool setPointer(const std::string &path, const CborValue &value);

private:
    class CborDocumentPrivate;
    std::shared_ptr<CborDocumentPrivate> d;

    // 从cbor_value_t构造（内部使用）
    explicit CborDocument(cbor_value_t *val, bool takeOwnership = false);
    cbor_value_t *rawValue() const;

    friend class CborValue;
    friend class CborArray;
    friend class CborObject;
};

#endif // CBORDOCUMENT_H
