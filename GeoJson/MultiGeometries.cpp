#include "MultiGeometries.h"
#include <sstream>

// ============ MultiPoint 实现 ============

MultiPoint::MultiPoint() {}

MultiPoint::MultiPoint(const std::vector<Point> &points_) : points(points_) {}

MultiPoint::MultiPoint(const MultiPoint &other) : points(other.points) {}

MultiPoint &MultiPoint::operator=(const MultiPoint &other) {
    if (this != &other) {
        points = other.points;
    }
    return *this;
}

bool MultiPoint::operator==(const MultiPoint &other) const {
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

bool MultiPoint::operator!=(const MultiPoint &other) const {
    return !(*this == other);
}

int MultiPoint::size() const {
    return static_cast<int>(points.size());
}

bool MultiPoint::isEmpty() const {
    return points.empty();
}

void MultiPoint::append(const Point &point) {
    points.push_back(point);
}

void MultiPoint::clear() {
    points.clear();
}

std::string MultiPoint::type() const {
    return "MultiPoint";
}

CborValue MultiPoint::toCborValue() const {
    CborArray coords;
    for (const auto &point : points) {
        coords.append(point.toCborValue());
    }
    return CborValue(coords);
}

std::shared_ptr<GeometryBase> MultiPoint::clone() const {
    return std::make_shared<MultiPoint>(*this);
}

bool MultiPoint::equals(const GeometryBase *other) const {
    if (!other || other->type() != "MultiPoint") {
        return false;
    }
    const MultiPoint *otherMultiPoint = dynamic_cast<const MultiPoint*>(other);
    return otherMultiPoint && (*this == *otherMultiPoint);
}

bool MultiPoint::fromCborValue(const CborValue &value, MultiPoint &multiPoint, std::string &errorInfo) {
    if (!value.isArray()) {
        errorInfo = "MultiPoint coordinates must be an array";
        return false;
    }

    return fromCborArray(value.toArray(), multiPoint, errorInfo);
}

bool MultiPoint::fromCborArray(const CborArray &coords, MultiPoint &multiPoint, std::string &errorInfo) {
    multiPoint.clear();

    for (int i = 0; i < coords.size(); ++i) {
        Point point;
        if (!Point::fromCborValue(coords.at(i), point, errorInfo)) {
            std::ostringstream oss;
            oss << "Failed to parse point at index " << i << ": " << errorInfo;
            errorInfo = oss.str();
            return false;
        }
        multiPoint.append(point);
    }

    return true;
}

// ============ MultiLineString 实现 ============

MultiLineString::MultiLineString() {}

MultiLineString::MultiLineString(const std::vector<LineString> &lineStrings_)
    : lineStrings(lineStrings_) {}

MultiLineString::MultiLineString(const MultiLineString &other)
    : lineStrings(other.lineStrings) {}

MultiLineString &MultiLineString::operator=(const MultiLineString &other) {
    if (this != &other) {
        lineStrings = other.lineStrings;
    }
    return *this;
}

bool MultiLineString::operator==(const MultiLineString &other) const {
    if (lineStrings.size() != other.lineStrings.size()) {
        return false;
    }
    for (size_t i = 0; i < lineStrings.size(); ++i) {
        if (lineStrings[i] != other.lineStrings[i]) {
            return false;
        }
    }
    return true;
}

bool MultiLineString::operator!=(const MultiLineString &other) const {
    return !(*this == other);
}

int MultiLineString::size() const {
    return static_cast<int>(lineStrings.size());
}

bool MultiLineString::isEmpty() const {
    return lineStrings.empty();
}

void MultiLineString::append(const LineString &lineString) {
    lineStrings.push_back(lineString);
}

void MultiLineString::clear() {
    lineStrings.clear();
}

std::string MultiLineString::type() const {
    return "MultiLineString";
}

CborValue MultiLineString::toCborValue() const {
    CborArray coords;
    for (const auto &lineString : lineStrings) {
        coords.append(lineString.toCborValue());
    }
    return CborValue(coords);
}

std::shared_ptr<GeometryBase> MultiLineString::clone() const {
    return std::make_shared<MultiLineString>(*this);
}

bool MultiLineString::equals(const GeometryBase *other) const {
    if (!other || other->type() != "MultiLineString") {
        return false;
    }
    const MultiLineString *otherMultiLineString = dynamic_cast<const MultiLineString*>(other);
    return otherMultiLineString && (*this == *otherMultiLineString);
}

bool MultiLineString::fromCborValue(const CborValue &value, MultiLineString &multiLineString, std::string &errorInfo) {
    if (!value.isArray()) {
        errorInfo = "MultiLineString coordinates must be an array";
        return false;
    }

    return fromCborArray(value.toArray(), multiLineString, errorInfo);
}

bool MultiLineString::fromCborArray(const CborArray &coords, MultiLineString &multiLineString, std::string &errorInfo) {
    multiLineString.clear();

    for (int i = 0; i < coords.size(); ++i) {
        LineString lineString;
        if (!LineString::fromCborValue(coords.at(i), lineString, errorInfo)) {
            std::ostringstream oss;
            oss << "Failed to parse LineString at index " << i << ": " << errorInfo;
            errorInfo = oss.str();
            return false;
        }
        multiLineString.append(lineString);
    }

    return true;
}

// ============ MultiPolygon 实现 ============

MultiPolygon::MultiPolygon() {}

MultiPolygon::MultiPolygon(const std::vector<Polygon> &polygons_)
    : polygons(polygons_) {}

MultiPolygon::MultiPolygon(const MultiPolygon &other)
    : polygons(other.polygons) {}

MultiPolygon &MultiPolygon::operator=(const MultiPolygon &other) {
    if (this != &other) {
        polygons = other.polygons;
    }
    return *this;
}

bool MultiPolygon::operator==(const MultiPolygon &other) const {
    if (polygons.size() != other.polygons.size()) {
        return false;
    }
    for (size_t i = 0; i < polygons.size(); ++i) {
        if (polygons[i] != other.polygons[i]) {
            return false;
        }
    }
    return true;
}

bool MultiPolygon::operator!=(const MultiPolygon &other) const {
    return !(*this == other);
}

int MultiPolygon::size() const {
    return static_cast<int>(polygons.size());
}

bool MultiPolygon::isEmpty() const {
    return polygons.empty();
}

void MultiPolygon::append(const Polygon &polygon) {
    polygons.push_back(polygon);
}

void MultiPolygon::clear() {
    polygons.clear();
}

std::string MultiPolygon::type() const {
    return "MultiPolygon";
}

CborValue MultiPolygon::toCborValue() const {
    CborArray coords;
    for (const auto &polygon : polygons) {
        coords.append(polygon.toCborValue());
    }
    return CborValue(coords);
}

std::shared_ptr<GeometryBase> MultiPolygon::clone() const {
    return std::make_shared<MultiPolygon>(*this);
}

bool MultiPolygon::equals(const GeometryBase *other) const {
    if (!other || other->type() != "MultiPolygon") {
        return false;
    }
    const MultiPolygon *otherMultiPolygon = dynamic_cast<const MultiPolygon*>(other);
    return otherMultiPolygon && (*this == *otherMultiPolygon);
}

bool MultiPolygon::fromCborValue(const CborValue &value, MultiPolygon &multiPolygon, std::string &errorInfo) {
    if (!value.isArray()) {
        errorInfo = "MultiPolygon coordinates must be an array";
        return false;
    }

    return fromCborArray(value.toArray(), multiPolygon, errorInfo);
}

bool MultiPolygon::fromCborArray(const CborArray &coords, MultiPolygon &multiPolygon, std::string &errorInfo) {
    multiPolygon.clear();

    for (int i = 0; i < coords.size(); ++i) {
        Polygon polygon;
        if (!Polygon::fromCborValue(coords.at(i), polygon, errorInfo)) {
            std::ostringstream oss;
            oss << "Failed to parse Polygon at index " << i << ": " << errorInfo;
            errorInfo = oss.str();
            return false;
        }
        multiPolygon.append(polygon);
    }

    return true;
}
