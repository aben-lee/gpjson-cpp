#ifndef MULTIGEOMETRIES_H
#define MULTIGEOMETRIES_H

#include "GeometryBase.h"
#include "Point.h"
#include "LineString.h"
#include "Polygon.h"
#include "CborValue.h"
#include "CborArray.h"
#include <vector>
#include <string>
#include <memory>

/**
 * @brief MultiPoint - GeoJSON多点几何类型
 *
 * 表示多个独立的点
 * 坐标格式：[[x1, y1], [x2, y2], ...]
 */
class MultiPoint : public GeometryBase {
public:
    std::vector<Point> points;  ///< 点集合

public:
    MultiPoint();
    MultiPoint(const std::vector<Point> &points_);
    MultiPoint(const MultiPoint &other);

    MultiPoint &operator=(const MultiPoint &other);
    bool operator==(const MultiPoint &other) const;
    bool operator!=(const MultiPoint &other) const;

    int size() const;
    bool isEmpty() const;
    void append(const Point &point);
    void clear();

    // 实现GeometryBase接口
    std::string type() const override;
    CborValue toCborValue() const override;
    std::shared_ptr<GeometryBase> clone() const override;
    bool equals(const GeometryBase *other) const override;

    static bool fromCborValue(const CborValue &value, MultiPoint &multiPoint, std::string &errorInfo);
    static bool fromCborArray(const CborArray &coords, MultiPoint &multiPoint, std::string &errorInfo);
};

/**
 * @brief MultiLineString - GeoJSON多线串几何类型
 *
 * 表示多条独立的线串
 * 坐标格式：[[[x1, y1], [x2, y2], ...], [[x3, y3], [x4, y4], ...], ...]
 */
class MultiLineString : public GeometryBase {
public:
    std::vector<LineString> lineStrings;  ///< 线串集合

public:
    MultiLineString();
    MultiLineString(const std::vector<LineString> &lineStrings_);
    MultiLineString(const MultiLineString &other);

    MultiLineString &operator=(const MultiLineString &other);
    bool operator==(const MultiLineString &other) const;
    bool operator!=(const MultiLineString &other) const;

    int size() const;
    bool isEmpty() const;
    void append(const LineString &lineString);
    void clear();

    // 实现GeometryBase接口
    std::string type() const override;
    CborValue toCborValue() const override;
    std::shared_ptr<GeometryBase> clone() const override;
    bool equals(const GeometryBase *other) const override;

    static bool fromCborValue(const CborValue &value, MultiLineString &multiLineString, std::string &errorInfo);
    static bool fromCborArray(const CborArray &coords, MultiLineString &multiLineString, std::string &errorInfo);
};

/**
 * @brief MultiPolygon - GeoJSON多多边形几何类型
 *
 * 表示多个独立的多边形
 * 坐标格式：[[[[x1, y1], ...]], [[[x2, y2], ...]], ...]
 */
class MultiPolygon : public GeometryBase {
public:
    std::vector<Polygon> polygons;  ///< 多边形集合

public:
    MultiPolygon();
    MultiPolygon(const std::vector<Polygon> &polygons_);
    MultiPolygon(const MultiPolygon &other);

    MultiPolygon &operator=(const MultiPolygon &other);
    bool operator==(const MultiPolygon &other) const;
    bool operator!=(const MultiPolygon &other) const;

    int size() const;
    bool isEmpty() const;
    void append(const Polygon &polygon);
    void clear();

    // 实现GeometryBase接口
    std::string type() const override;
    CborValue toCborValue() const override;
    std::shared_ptr<GeometryBase> clone() const override;
    bool equals(const GeometryBase *other) const override;

    static bool fromCborValue(const CborValue &value, MultiPolygon &multiPolygon, std::string &errorInfo);
    static bool fromCborArray(const CborArray &coords, MultiPolygon &multiPolygon, std::string &errorInfo);
};

#endif // MULTIGEOMETRIES_H
