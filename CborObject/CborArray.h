#ifndef CBORARRAY_H
#define CBORARRAY_H

#include "CborValue.h"
#include <memory>
#include <vector>

/**
 * @brief CborArray类封装CBOR数组类型，提供Qt风格的API
 *
 * CborArray表示一个CBOR数组，内部管理cbor_value_t的数组容器。
 * 提供类似QJsonArray的接口，支持添加、删除、访问元素等操作。
 */
class CborArray {
public:
    // 构造函数
    CborArray();
    CborArray(const CborArray &other);
    ~CborArray();

    // 赋值运算符
    CborArray &operator=(const CborArray &other);

    // 比较运算符
    bool operator==(const CborArray &other) const;
    bool operator!=(const CborArray &other) const;

    // 容量查询
    int size() const;
    int count() const { return size(); }
    bool isEmpty() const;

    // 元素访问
    CborValue at(int i) const;
    CborValue first() const;
    CborValue last() const;
    CborValue operator[](int i) const;

    // 元素修改
    void append(const CborValue &value);
    void prepend(const CborValue &value);
    void insert(int i, const CborValue &value);
    void removeAt(int i);
    void removeFirst();
    void removeLast();
    void clear();

    // 搜索
    bool contains(const CborValue &value) const;

    // 转换为标准容器
    std::vector<CborValue> toVector() const;

private:
    class CborArrayPrivate;
    std::shared_ptr<CborArrayPrivate> d;

    // 从cbor_value_t构造（内部使用）
    explicit CborArray(cbor_value_t *val, bool takeOwnership = false);
    cbor_value_t *rawValue() const;

    friend class CborValue;
    friend class CborObject;
    friend class CborDocument;
};

#endif // CBORARRAY_H
