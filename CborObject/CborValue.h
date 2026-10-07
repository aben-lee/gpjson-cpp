#ifndef CBORVALUE_H
#define CBORVALUE_H

#include "cbor/cbor.h"
#include <string>
#include <memory>

// Forward declarations
class CborArray;
class CborObject;
class CborDocument;

/**
 * @brief CborValue类封装CBOR值类型，提供Qt风格的API
 *
 * CborValue是一个轻量级的值类型包装器，内部使用共享指针管理cbor_value_t的生命周期。
 * 支持CBOR的所有基本类型：null、bool、integer、double、string、bytestring、array、object、tag。
 */
class CborValue {
public:
    /**
     * @brief CBOR值类型枚举
     */
    enum Type {
        Null,           ///< 空值类型
        Bool,           ///< 布尔类型
        Integer,        ///< 整数类型
        Double,         ///< 浮点数类型
        String,         ///< 字符串类型（UTF-8文本）
        ByteString,     ///< 字节串类型（二进制数据）
        Array,          ///< 数组类型
        Object,         ///< 对象类型（键值对映射）
        Tag,            ///< 标签类型
        Undefined       ///< 未定义类型
    };

    // 构造函数
    CborValue();
    CborValue(const CborValue &other);
    CborValue(bool b);
    CborValue(int i);
    CborValue(long long i);
    CborValue(double d);
    CborValue(const std::string &s);
    CborValue(const char *s);
    CborValue(const CborArray &arr);
    CborValue(const CborObject &obj);
    ~CborValue();

    // 赋值运算符
    CborValue &operator=(const CborValue &other);

    // 比较运算符
    bool operator==(const CborValue &other) const;
    bool operator!=(const CborValue &other) const;

    // 类型查询
    Type type() const;
    bool isNull() const;
    bool isBool() const;
    bool isInteger() const;
    bool isDouble() const;
    bool isString() const;
    bool isByteString() const;
    bool isArray() const;
    bool isObject() const;
    bool isTag() const;

    // 类型转换（带默认值）
    bool toBool(bool defaultValue = false) const;
    int toInt(int defaultValue = 0) const;
    long long toLongLong(long long defaultValue = 0) const;
    double toDouble(double defaultValue = 0.0) const;
    std::string toString(const std::string &defaultValue = std::string()) const;

    /**
     * @brief 转换为CborArray
     * @return 如果值是数组类型，返回对应的CborArray；否则返回空数组
     */
    CborArray toArray() const;

    /**
     * @brief 转换为CborObject
     * @return 如果值是对象类型，返回对应的CborObject；否则返回空对象
     */
    CborObject toObject() const;

private:
    class CborValuePrivate;
    std::shared_ptr<CborValuePrivate> d;

    // 从cbor_value_t构造（内部使用）
    explicit CborValue(cbor_value_t *val, bool takeOwnership = false);
    cbor_value_t *rawValue() const;

    friend class CborArray;
    friend class CborObject;
    friend class CborDocument;
};

#endif // CBORVALUE_H
