#ifndef GEOMETRYBASE_H
#define GEOMETRYBASE_H

#include "CborValue.h"
#include "CborArray.h"
#include <string>
#include <memory>

/**
 * @brief GeometryBase - GeoJSON几何对象基类
 *
 * 定义了所有几何类型的通用接口
 * 支持与CBOR格式的互相转换
 */
class GeometryBase {
public:
    virtual ~GeometryBase() = default;

    /**
     * @brief 获取几何对象类型名称
     * @return 类型名称字符串 (Point, LineString, Polygon等)
     */
    virtual std::string type() const = 0;

    /**
     * @brief 将几何对象转换为CborValue
     * @return 包含几何数据的CborValue对象
     */
    virtual CborValue toCborValue() const = 0;

    /**
     * @brief 克隆当前几何对象
     * @return 新的几何对象智能指针
     */
    virtual std::shared_ptr<GeometryBase> clone() const = 0;

    /**
     * @brief 判断两个几何对象是否相等
     * @param other 另一个几何对象
     * @return 相等返回true，否则返回false
     */
    virtual bool equals(const GeometryBase *other) const = 0;
};

#endif // GEOMETRYBASE_H
