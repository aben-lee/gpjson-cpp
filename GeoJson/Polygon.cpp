#include "Polygon.h"
#include <sstream>

// ============ LinearRing 实现 ============

// 默认构造函数
LinearRing::LinearRing() {}

// 参数构造函数
LinearRing::LinearRing(const std::vector<Point> &points_) : points(points_) {}

// 拷贝构造函数
LinearRing::LinearRing(const LinearRing &other) : points(other.points) {}

// 赋值运算符
LinearRing &LinearRing::operator=(const LinearRing &other) {
    if (this != &other) {
        points = other.points;
    }
    return *this;
}

// 相等比较运算符
bool LinearRing::operator==(const LinearRing &other) const {
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
bool LinearRing::operator!=(const LinearRing &other) const {
    return !(*this == other);
}

// 获取点的数量
int LinearRing::size() const {
    return static_cast<int>(points.size());
}

// 判断是否为空
bool LinearRing::isEmpty() const {
    return points.empty();
}

// 添加点
void LinearRing::append(const Point &point) {
    points.push_back(point);
}

// 清空所有点
void LinearRing::clear() {
    points.clear();
}

// 判断环是否闭合
bool LinearRing::isClosed() const {
    if (points.size() < 4) {  // 至少需要4个点形成闭合的三角形
        return false;
    }
    return points.front() == points.back();
}

// 转换为CborValue
CborValue LinearRing::toCborValue() const {
    CborArray coords;
    for (const auto &point : points) {
        coords.append(point.toCborValue());
    }
    return CborValue(coords);
}

// 从CborValue创建LinearRing对象
bool LinearRing::fromCborValue(const CborValue &value, LinearRing &ring, std::string &errorInfo) {
    if (!value.isArray()) {
        errorInfo = "LinearRing coordinates must be an array";
        return false;
    }

    return fromCborArray(value.toArray(), ring, errorInfo);
}

// 从CborArray创建LinearRing对象
bool LinearRing::fromCborArray(const CborArray &coords, LinearRing &ring, std::string &errorInfo) {
    if (coords.size() < 4) {
        std::ostringstream oss;
        oss << "LinearRing must have at least 4 points, got " << coords.size();
        errorInfo = oss.str();
        return false;
    }

    ring.clear();

    for (int i = 0; i < coords.size(); ++i) {
        Point point;
        if (!Point::fromCborValue(coords.at(i), point, errorInfo)) {
            std::ostringstream oss;
            oss << "Failed to parse point at index " << i << ": " << errorInfo;
            errorInfo = oss.str();
            return false;
        }
        ring.append(point);
    }

    // 验证环是否闭合
    if (!ring.isClosed()) {
        errorInfo = "LinearRing is not closed (first and last points must be the same)";
        return false;
    }

    return true;
}

// ============ Polygon 实现 ============

// 默认构造函数
Polygon::Polygon() {}

// 参数构造函数
Polygon::Polygon(const std::vector<LinearRing> &rings_) : rings(rings_) {}

// 拷贝构造函数
Polygon::Polygon(const Polygon &other) : rings(other.rings) {}

// 赋值运算符
Polygon &Polygon::operator=(const Polygon &other) {
    if (this != &other) {
        rings = other.rings;
    }
    return *this;
}

// 相等比较运算符
bool Polygon::operator==(const Polygon &other) const {
    if (rings.size() != other.rings.size()) {
        return false;
    }
    for (size_t i = 0; i < rings.size(); ++i) {
        if (rings[i] != other.rings[i]) {
            return false;
        }
    }
    return true;
}

// 不等比较运算符
bool Polygon::operator!=(const Polygon &other) const {
    return !(*this == other);
}

// 获取环的数量
int Polygon::size() const {
    return static_cast<int>(rings.size());
}

// 判断是否为空
bool Polygon::isEmpty() const {
    return rings.empty();
}

// 添加环
void Polygon::append(const LinearRing &ring) {
    rings.push_back(ring);
}

// 清空所有环
void Polygon::clear() {
    rings.clear();
}

// 获取外环
const LinearRing& Polygon::outerRing() const {
    static LinearRing emptyRing;
    if (rings.empty()) {
        return emptyRing;
    }
    return rings[0];
}

// 获取类型名称
std::string Polygon::type() const {
    return "Polygon";
}

// 转换为CborValue
CborValue Polygon::toCborValue() const {
    CborArray coords;
    for (const auto &ring : rings) {
        coords.append(ring.toCborValue());
    }
    return CborValue(coords);
}

// 克隆对象
std::shared_ptr<GeometryBase> Polygon::clone() const {
    return std::make_shared<Polygon>(*this);
}

// 判断是否相等
bool Polygon::equals(const GeometryBase *other) const {
    if (!other || other->type() != "Polygon") {
        return false;
    }
    const Polygon *otherPolygon = dynamic_cast<const Polygon*>(other);
    return otherPolygon && (*this == *otherPolygon);
}

// 从CborValue创建Polygon对象
bool Polygon::fromCborValue(const CborValue &value, Polygon &polygon, std::string &errorInfo) {
    if (!value.isArray()) {
        errorInfo = "Polygon coordinates must be an array";
        return false;
    }

    return fromCborArray(value.toArray(), polygon, errorInfo);
}

// 从CborArray创建Polygon对象
bool Polygon::fromCborArray(const CborArray &coords, Polygon &polygon, std::string &errorInfo) {
    if (coords.size() < 1) {
        errorInfo = "Polygon must have at least 1 ring";
        return false;
    }

    polygon.clear();

    for (int i = 0; i < coords.size(); ++i) {
        LinearRing ring;
        if (!LinearRing::fromCborValue(coords.at(i), ring, errorInfo)) {
            std::ostringstream oss;
            oss << "Failed to parse ring at index " << i << ": " << errorInfo;
            errorInfo = oss.str();
            return false;
        }
        polygon.append(ring);
    }

    return true;
}
