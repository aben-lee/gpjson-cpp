#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "GeometryBase.h"
#include "Point.h"
#include "LineString.h"
#include "Polygon.h"
#include "MultiGeometries.h"
#include "CborValue.h"
#include "CborObject.h"
#include <vector>
#include <string>
#include <memory>

// 前向声明
class Geometry;

/**
 * @brief GeometryCollection - GeoJSON几何集合类型
 *
 * 表示一个包含多种不同类型几何对象的集合
 */
class GeometryCollection : public GeometryBase {
public:
    std::vector<std::shared_ptr<GeometryBase>> geometries;  ///< 几何对象集合

public:
    GeometryCollection();
    GeometryCollection(const std::vector<std::shared_ptr<GeometryBase>> &geometries_);
    GeometryCollection(const GeometryCollection &other);

    GeometryCollection &operator=(const GeometryCollection &other);
    bool operator==(const GeometryCollection &other) const;
    bool operator!=(const GeometryCollection &other) const;

    int size() const;
    bool isEmpty() const;
    void append(const std::shared_ptr<GeometryBase> &geometry);
    void clear();

    // 实现GeometryBase接口
    std::string type() const override;
    CborValue toCborValue() const override;
    std::shared_ptr<GeometryBase> clone() const override;
    bool equals(const GeometryBase *other) const override;

    static bool fromCborValue(const CborValue &value, GeometryCollection &collection, std::string &errorInfo);
    static bool fromCborArray(const CborArray &geoms, GeometryCollection &collection, std::string &errorInfo);
};

/**
 * @brief Geometry - GeoJSON几何对象包装类
 *
 * 可以包装任意类型的几何对象（Point, LineString, Polygon等）
 * 提供统一的接口用于几何对象的创建、访问和序列化
 */
class Geometry {
private:
    std::shared_ptr<GeometryBase> m_geometry;  ///< 内部几何对象

public:
    /**
     * @brief 默认构造函数 - 创建空几何对象
     */
    Geometry();

    /**
     * @brief 从GeometryBase指针构造
     */
    Geometry(const std::shared_ptr<GeometryBase> &geometry);

    /**
     * @brief 拷贝构造函数
     */
    Geometry(const Geometry &other);

    /**
     * @brief 赋值运算符
     */
    Geometry &operator=(const Geometry &other);

    /**
     * @brief 相等比较运算符
     */
    bool operator==(const Geometry &other) const;

    /**
     * @brief 不等比较运算符
     */
    bool operator!=(const Geometry &other) const;

    /**
     * @brief 判断是否为空（无几何对象）
     */
    bool isNull() const;

    /**
     * @brief 获取几何类型名称
     */
    std::string type() const;

    /**
     * @brief 判断是否为Point类型
     */
    bool isPoint() const;

    /**
     * @brief 判断是否为LineString类型
     */
    bool isLineString() const;

    /**
     * @brief 判断是否为Polygon类型
     */
    bool isPolygon() const;

    /**
     * @brief 判断是否为MultiPoint类型
     */
    bool isMultiPoint() const;

    /**
     * @brief 判断是否为MultiLineString类型
     */
    bool isMultiLineString() const;

    /**
     * @brief 判断是否为MultiPolygon类型
     */
    bool isMultiPolygon() const;

    /**
     * @brief 判断是否为GeometryCollection类型
     */
    bool isGeometryCollection() const;

    /**
     * @brief 转换为Point（如果类型匹配）
     */
    Point toPoint() const;

    /**
     * @brief 转换为LineString（如果类型匹配）
     */
    LineString toLineString() const;

    /**
     * @brief 转换为Polygon（如果类型匹配）
     */
    Polygon toPolygon() const;

    /**
     * @brief 转换为MultiPoint（如果类型匹配）
     */
    MultiPoint toMultiPoint() const;

    /**
     * @brief 转换为MultiLineString（如果类型匹配）
     */
    MultiLineString toMultiLineString() const;

    /**
     * @brief 转换为MultiPolygon（如果类型匹配）
     */
    MultiPolygon toMultiPolygon() const;

    /**
     * @brief 转换为GeometryCollection（如果类型匹配）
     */
    GeometryCollection toGeometryCollection() const;

    /**
     * @brief 转换为CborObject（包含type和coordinates字段）
     */
    CborObject toCborObject() const;

    /**
     * @brief 从CborObject创建Geometry对象
     * @param obj CborObject对象（应包含type和coordinates字段）
     * @param geometry 输出的Geometry对象
     * @param errorInfo 错误信息输出
     * @return 成功返回true，失败返回false
     */
    static bool fromCborObject(const CborObject &obj, Geometry &geometry, std::string &errorInfo);

    /**
     * @brief 从CborValue创建Geometry对象
     * @param value CborValue对象（应为包含type和coordinates的对象）
     * @param geometry 输出的Geometry对象
     * @param errorInfo 错误信息输出
     * @return 成功返回true，失败返回false
     */
    static bool fromCborValue(const CborValue &value, Geometry &geometry, std::string &errorInfo);

    /**
     * @brief 获取内部几何对象指针
     */
    std::shared_ptr<GeometryBase> get() const;
};

#endif // GEOMETRY_H
