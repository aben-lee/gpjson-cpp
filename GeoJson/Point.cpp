#include "Point.h"
#include <cmath>
#include <sstream>

// 默认构造函数
Point::Point() : x(0.0), y(0.0) {}

// 参数构造函数
Point::Point(double x_, double y_) : x(x_), y(y_) {}

// 拷贝构造函数
Point::Point(const Point &other) : x(other.x), y(other.y) {}

// 赋值运算符
Point &Point::operator=(const Point &other) {
    if (this != &other) {
        x = other.x;
        y = other.y;
    }
    return *this;
}

// 相等比较运算符
bool Point::operator==(const Point &other) const {
    const double epsilon = 1e-10;
    return std::fabs(x - other.x) < epsilon && std::fabs(y - other.y) < epsilon;
}

// 不等比较运算符
bool Point::operator!=(const Point &other) const {
    return !(*this == other);
}

// 获取类型名称
std::string Point::type() const {
    return "Point";
}

// 转换为CborValue
CborValue Point::toCborValue() const {
    CborArray coords;
    coords.append(CborValue(x));
    coords.append(CborValue(y));
    return CborValue(coords);
}

// 克隆对象
std::shared_ptr<GeometryBase> Point::clone() const {
    return std::make_shared<Point>(*this);
}

// 判断是否相等
bool Point::equals(const GeometryBase *other) const {
    if (!other || other->type() != "Point") {
        return false;
    }
    const Point *otherPoint = dynamic_cast<const Point*>(other);
    return otherPoint && (*this == *otherPoint);
}

// 从CborValue创建Point对象
bool Point::fromCborValue(const CborValue &value, Point &point, std::string &errorInfo) {
    if (!value.isArray()) {
        errorInfo = "Point coordinates must be an array";
        return false;
    }

    return fromCborArray(value.toArray(), point, errorInfo);
}

// 从CborArray创建Point对象
bool Point::fromCborArray(const CborArray &coords, Point &point, std::string &errorInfo) {
    if (coords.size() < 2) {
        std::ostringstream oss;
        oss << "Point coordinates array must have at least 2 elements, got " << coords.size();
        errorInfo = oss.str();
        return false;
    }

    CborValue xVal = coords.at(0);
    CborValue yVal = coords.at(1);

    if (!xVal.isDouble() && !xVal.isInteger()) {
        errorInfo = "Point x coordinate must be a number";
        return false;
    }

    if (!yVal.isDouble() && !yVal.isInteger()) {
        errorInfo = "Point y coordinate must be a number";
        return false;
    }

    point.x = xVal.isDouble() ? xVal.toDouble() : static_cast<double>(xVal.toInt());
    point.y = yVal.isDouble() ? yVal.toDouble() : static_cast<double>(yVal.toInt());

    return true;
}
