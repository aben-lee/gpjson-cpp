#include "LineString.h"
#include <sstream>

// 默认构造函数
LineString::LineString() {}

// 参数构造函数
LineString::LineString(const std::vector<Point> &points_) : points(points_) {}

// 拷贝构造函数
LineString::LineString(const LineString &other) : points(other.points) {}

// 赋值运算符
LineString &LineString::operator=(const LineString &other) {
    if (this != &other) {
        points = other.points;
    }
    return *this;
}

// 相等比较运算符
bool LineString::operator==(const LineString &other) const {
    if (points.size() != other.points.size()) {
        return false;
    }
    for (size_t i = 0; i < points.size(); ++i) {
        if (points[i] != other.points[i]) {
            return false;
        }
    }
    return true;
}

// 不等比较运算符
bool LineString::operator!=(const LineString &other) const {
    return !(*this == other);
}

// 获取点的数量
int LineString::size() const {
    return static_cast<int>(points.size());
}

// 判断是否为空
bool LineString::isEmpty() const {
    return points.empty();
}

// 添加点
void LineString::append(const Point &point) {
    points.push_back(point);
}

// 清空所有点
void LineString::clear() {
    points.clear();
}

// 获取类型名称
std::string LineString::type() const {
    return "LineString";
}

// 转换为CborValue
CborValue LineString::toCborValue() const {
    CborArray coords;
    for (const auto &point : points) {
        coords.append(point.toCborValue());
    }
    return CborValue(coords);
}

// 克隆对象
std::shared_ptr<GeometryBase> LineString::clone() const {
    return std::make_shared<LineString>(*this);
}

// 判断是否相等
bool LineString::equals(const GeometryBase *other) const {
    if (!other || other->type() != "LineString") {
        return false;
    }
    const LineString *otherLineString = dynamic_cast<const LineString*>(other);
    return otherLineString && (*this == *otherLineString);
}

// 从CborValue创建LineString对象
bool LineString::fromCborValue(const CborValue &value, LineString &lineString, std::string &errorInfo) {
    if (!value.isArray()) {
        errorInfo = "LineString coordinates must be an array";
        return false;
    }

    return fromCborArray(value.toArray(), lineString, errorInfo);
}

// 从CborArray创建LineString对象
bool LineString::fromCborArray(const CborArray &coords, LineString &lineString, std::string &errorInfo) {
    if (coords.size() < 2) {
        std::ostringstream oss;
        oss << "LineString must have at least 2 points, got " << coords.size();
        errorInfo = oss.str();
        return false;
    }

    lineString.clear();

    for (int i = 0; i < coords.size(); ++i) {
        Point point;
        if (!Point::fromCborValue(coords.at(i), point, errorInfo)) {
            std::ostringstream oss;
            oss << "Failed to parse point at index " << i << ": " << errorInfo;
            errorInfo = oss.str();
            return false;
        }
        lineString.append(point);
    }

    return true;
}
