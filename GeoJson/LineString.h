#ifndef LINESTRING_H
#define LINESTRING_H

#include "GeometryBase.h"
#include "Point.h"
#include "CborValue.h"
#include "CborArray.h"
#include <vector>
#include <string>
#include <memory>

/**
 * @brief LineString - GeoJSON线串几何类型
 *
 * 表示一条由多个点连接而成的折线
 * 坐标格式：[[x1, y1], [x2, y2], ...]
 */
class LineString : public GeometryBase {
public:
    std::vector<Point> points;  ///< 点集合

public:
    /**
     * @brief 默认构造函数 - 创建空线串
     */
    LineString();

    /**
     * @brief 参数构造函数
     * @param points_ 点集合
     */
    LineString(const std::vector<Point> &points_);

    /**
     * @brief 拷贝构造函数
     */
    LineString(const LineString &other);

    /**
     * @brief 赋值运算符
     */
    LineString &operator=(const LineString &other);

    /**
     * @brief 相等比较运算符
     */
    bool operator==(const LineString &other) const;

    /**
     * @brief 不等比较运算符
     */
    bool operator!=(const LineString &other) const;

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

    // 实现GeometryBase接口
    std::string type() const override;
    CborValue toCborValue() const override;
    std::shared_ptr<GeometryBase> clone() const override;
    bool equals(const GeometryBase *other) const override;

    /**
     * @brief 从CborValue创建LineString对象
     * @param value CborValue对象（应为包含点坐标数组的数组）
     * @param lineString 输出的LineString对象
     * @param errorInfo 错误信息输出
     * @return 成功返回true，失败返回false
     */
    static bool fromCborValue(const CborValue &value, LineString &lineString, std::string &errorInfo);

    /**
     * @brief 从坐标数组创建LineString对象
     * @param coords 坐标数组
     * @param lineString 输出的LineString对象
     * @param errorInfo 错误信息输出
     * @return 成功返回true，失败返回false
     */
    static bool fromCborArray(const CborArray &coords, LineString &lineString, std::string &errorInfo);
};

#endif // LINESTRING_H
