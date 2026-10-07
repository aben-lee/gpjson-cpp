#ifndef POINT_H
#define POINT_H

#include "GeometryBase.h"
#include "CborValue.h"
#include "CborArray.h"
#include <string>
#include <memory>

/**
 * @brief Point - GeoJSON点几何类型
 *
 * 表示一个地理坐标点，包含x(经度)和y(纬度)
 * 坐标格式：[x, y] 或 [经度, 纬度]
 */
class Point : public GeometryBase {
public:
    double x;  ///< X坐标 (经度)
    double y;  ///< Y坐标 (纬度)

public:
    /**
     * @brief 默认构造函数 - 创建原点(0, 0)
     */
    Point();

    /**
     * @brief 参数构造函数
     * @param x_ X坐标
     * @param y_ Y坐标
     */
    Point(double x_, double y_);

    /**
     * @brief 拷贝构造函数
     */
    Point(const Point &other);

    /**
     * @brief 赋值运算符
     */
    Point &operator=(const Point &other);

    /**
     * @brief 相等比较运算符
     */
    bool operator==(const Point &other) const;

    /**
     * @brief 不等比较运算符
     */
    bool operator!=(const Point &other) const;

    // 实现GeometryBase接口
    std::string type() const override;
    CborValue toCborValue() const override;
    std::shared_ptr<GeometryBase> clone() const override;
    bool equals(const GeometryBase *other) const override;

    /**
     * @brief 从CborValue创建Point对象
     * @param value CborValue对象（应为包含两个数字的数组）
     * @param errorInfo 错误信息输出
     * @return 成功返回true，失败返回false
     */
    static bool fromCborValue(const CborValue &value, Point &point, std::string &errorInfo);

    /**
     * @brief 从坐标数组创建Point对象
     * @param coords 坐标数组（应包含至少两个元素）
     * @param errorInfo 错误信息输出
     * @return 成功返回true，失败返回false
     */
    static bool fromCborArray(const CborArray &coords, Point &point, std::string &errorInfo);
};

#endif // POINT_H
