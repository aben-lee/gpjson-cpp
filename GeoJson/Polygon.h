#ifndef POLYGON_H
#define POLYGON_H

#include "GeometryBase.h"
#include "Point.h"
#include "LineString.h"
#include "CborValue.h"
#include "CborArray.h"
#include <vector>
#include <string>
#include <memory>

/**
 * @brief LinearRing - 闭合的线串（多边形环）
 *
 * 表示一个闭合的线性环，用于构成多边形的外环和内环（洞）
 * 坐标格式：[[x1, y1], [x2, y2], ..., [x1, y1]]
 * 注意：首尾点必须相同以形成闭环
 */
class LinearRing {
public:
    std::vector<Point> points;  ///< 点集合（首尾点相同）

public:
    /**
     * @brief 默认构造函数 - 创建空环
     */
    LinearRing();

    /**
     * @brief 参数构造函数
     * @param points_ 点集合
     */
    LinearRing(const std::vector<Point> &points_);

    /**
     * @brief 拷贝构造函数
     */
    LinearRing(const LinearRing &other);

    /**
     * @brief 赋值运算符
     */
    LinearRing &operator=(const LinearRing &other);

    /**
     * @brief 相等比较运算符
     */
    bool operator==(const LinearRing &other) const;

    /**
     * @brief 不等比较运算符
     */
    bool operator!=(const LinearRing &other) const;

    /**
     * @brief 获取点的数量
     */
    int size() const;

    /**
     * @brief 判断是否为空
     */
    bool isEmpty() const;

    /**
     * @brief 添加点
     */
    void append(const Point &point);

    /**
     * @brief 清空所有点
     */
    void clear();

    /**
     * @brief 判断环是否闭合（首尾点相同）
     */
    bool isClosed() const;

    /**
     * @brief 转换为CborValue
     */
    CborValue toCborValue() const;

    /**
     * @brief 从CborValue创建LinearRing对象
     * @param value CborValue对象（应为包含点坐标数组的数组）
     * @param ring 输出的LinearRing对象
     * @param errorInfo 错误信息输出
     * @return 成功返回true，失败返回false
     */
    static bool fromCborValue(const CborValue &value, LinearRing &ring, std::string &errorInfo);

    /**
     * @brief 从坐标数组创建LinearRing对象
     * @param coords 坐标数组
     * @param ring 输出的LinearRing对象
     * @param errorInfo 错误信息输出
     * @return 成功返回true，失败返回false
     */
    static bool fromCborArray(const CborArray &coords, LinearRing &ring, std::string &errorInfo);
};

/**
 * @brief Polygon - GeoJSON多边形几何类型
 *
 * 表示一个多边形，由一个外环和零个或多个内环（洞）组成
 * 坐标格式：[[[x1, y1], [x2, y2], ...], [[hole_x1, hole_y1], ...], ...]
 * 第一个环是外环，后续环是内环（洞）
 */
class Polygon : public GeometryBase {
public:
    std::vector<LinearRing> rings;  ///< 环集合（第一个是外环，其余是内环）

public:
    /**
     * @brief 默认构造函数 - 创建空多边形
     */
    Polygon();

    /**
     * @brief 参数构造函数
     * @param rings_ 环集合
     */
    Polygon(const std::vector<LinearRing> &rings_);

    /**
     * @brief 拷贝构造函数
     */
    Polygon(const Polygon &other);

    /**
     * @brief 赋值运算符
     */
    Polygon &operator=(const Polygon &other);

    /**
     * @brief 相等比较运算符
     */
    bool operator==(const Polygon &other) const;

    /**
     * @brief 不等比较运算符
     */
    bool operator!=(const Polygon &other) const;

    /**
     * @brief 获取环的数量
     */
    int size() const;

    /**
     * @brief 判断是否为空
     */
    bool isEmpty() const;

    /**
     * @brief 添加环
     */
    void append(const LinearRing &ring);

    /**
     * @brief 清空所有环
     */
    void clear();

    /**
     * @brief 获取外环
     */
    const LinearRing& outerRing() const;

    // 实现GeometryBase接口
    std::string type() const override;
    CborValue toCborValue() const override;
    std::shared_ptr<GeometryBase> clone() const override;
    bool equals(const GeometryBase *other) const override;

    /**
     * @brief 从CborValue创建Polygon对象
     * @param value CborValue对象（应为包含环坐标数组的数组）
     * @param polygon 输出的Polygon对象
     * @param errorInfo 错误信息输出
     * @return 成功返回true，失败返回false
     */
    static bool fromCborValue(const CborValue &value, Polygon &polygon, std::string &errorInfo);

    /**
     * @brief 从坐标数组创建Polygon对象
     * @param coords 坐标数组
     * @param polygon 输出的Polygon对象
     * @param errorInfo 错误信息输出
     * @return 成功返回true，失败返回false
     */
    static bool fromCborArray(const CborArray &coords, Polygon &polygon, std::string &errorInfo);
};

#endif // POLYGON_H
