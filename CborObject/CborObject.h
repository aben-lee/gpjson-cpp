#ifndef CBOROBJECT_H
#define CBOROBJECT_H

#include "CborValue.h"
#include <memory>
#include <vector>
#include <string>

/**
 * @brief CborObject类封装CBOR对象类型（Map），提供Qt风格的API
 *
 * CborObject表示一个CBOR映射（键值对集合），内部管理cbor_value_t的map容器。
 * 提供类似QJsonObject的接口，支持添加、删除、访问键值对等操作。
 */
class CborObject {
public:
    // 构造函数
    CborObject();
    CborObject(const CborObject &other);
    ~CborObject();

    // 赋值运算符
    CborObject &operator=(const CborObject &other);

    // 比较运算符
    bool operator==(const CborObject &other) const;
    bool operator!=(const CborObject &other) const;

    // 容量查询
    std::vector<std::string> keys() const;
    int size() const;
    int count() const { return size(); }
    bool isEmpty() const;

    // 元素访问
    CborValue value(const std::string &key) const;
    CborValue operator[](const std::string &key) const;

    // 查询
    bool contains(const std::string &key) const;

    // 元素修改
    void insert(const std::string &key, const CborValue &value);
    void remove(const std::string &key);
    void clear();

    /**
     * @brief 取出指定键的值并从对象中移除
     * @param key 键名
     * @return 对应的值，如果键不存在返回空值
     */
    CborValue take(const std::string &key);

private:
    class CborObjectPrivate;
    std::shared_ptr<CborObjectPrivate> d;

    // 从cbor_value_t构造（内部使用）
    explicit CborObject(cbor_value_t *val, bool takeOwnership = false);
    cbor_value_t *rawValue() const;

    friend class CborValue;
    friend class CborArray;
    friend class CborDocument;
};

#endif // CBOROBJECT_H
