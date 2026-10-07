#include "Geometry.h"
#include <sstream>

// ============ GeometryCollection 实现 ============

GeometryCollection::GeometryCollection() {}

GeometryCollection::GeometryCollection(const std::vector<std::shared_ptr<GeometryBase>> &geometries_)
    : geometries(geometries_) {}

GeometryCollection::GeometryCollection(const GeometryCollection &other) {
    geometries.clear();
    for (const auto &geom : other.geometries) {
        if (geom) {
            geometries.push_back(geom->clone());
        }
    }
}

GeometryCollection &GeometryCollection::operator=(const GeometryCollection &other) {
    if (this != &other) {
        geometries.clear();
        for (const auto &geom : other.geometries) {
            if (geom) {
                geometries.push_back(geom->clone());
            }
        }
    }
    return *this;
}

bool GeometryCollection::operator==(const GeometryCollection &other) const {
    if (geometries.size() != other.geometries.size()) {
        return false;
    }
    for (size_t i = 0; i < geometries.size(); ++i) {
        if (!geometries[i] || !other.geometries[i]) {
            if (geometries[i] != other.geometries[i]) {
                return false;
            }
            continue;
        }
        if (!geometries[i]->equals(other.geometries[i].get())) {
            return false;
        }
    }
    return true;
}

bool GeometryCollection::operator!=(const GeometryCollection &other) const {
    return !(*this == other);
}

int GeometryCollection::size() const {
    return static_cast<int>(geometries.size());
}

bool GeometryCollection::isEmpty() const {
    return geometries.empty();
}

void GeometryCollection::append(const std::shared_ptr<GeometryBase> &geometry) {
    geometries.push_back(geometry);
}

void GeometryCollection::clear() {
    geometries.clear();
}

std::string GeometryCollection::type() const {
    return "GeometryCollection";
}

CborValue GeometryCollection::toCborValue() const {
    CborArray geoms;
    for (const auto &geom : geometries) {
        if (geom) {
            // GeometryCollection中的每个几何对象需要包装为{type, coordinates}格式
            CborObject geomObj;
            geomObj.insert("type", CborValue(geom->type()));
            geomObj.insert("coordinates", geom->toCborValue());
            geoms.append(CborValue(geomObj));
        }
    }
    return CborValue(geoms);
}

std::shared_ptr<GeometryBase> GeometryCollection::clone() const {
    return std::make_shared<GeometryCollection>(*this);
}

bool GeometryCollection::equals(const GeometryBase *other) const {
    if (!other || other->type() != "GeometryCollection") {
        return false;
    }
    const GeometryCollection *otherCollection = dynamic_cast<const GeometryCollection*>(other);
    return otherCollection && (*this == *otherCollection);
}

bool GeometryCollection::fromCborValue(const CborValue &value, GeometryCollection &collection, std::string &errorInfo) {
    if (!value.isArray()) {
        errorInfo = "GeometryCollection geometries must be an array";
        return false;
    }

    return fromCborArray(value.toArray(), collection, errorInfo);
}

bool GeometryCollection::fromCborArray(const CborArray &geoms, GeometryCollection &collection, std::string &errorInfo) {
    collection.clear();

    for (int i = 0; i < geoms.size(); ++i) {
        CborValue geomVal = geoms.at(i);
        if (!geomVal.isObject()) {
            std::ostringstream oss;
            oss << "Geometry at index " << i << " must be an object";
            errorInfo = oss.str();
            return false;
        }

        Geometry geometry;
        if (!Geometry::fromCborValue(geomVal, geometry, errorInfo)) {
            std::ostringstream oss;
            oss << "Failed to parse geometry at index " << i << ": " << errorInfo;
            errorInfo = oss.str();
            return false;
        }

        collection.append(geometry.get());
    }

    return true;
}

// ============ Geometry 实现 ============

Geometry::Geometry() : m_geometry(nullptr) {}

Geometry::Geometry(const std::shared_ptr<GeometryBase> &geometry)
    : m_geometry(geometry) {}

Geometry::Geometry(const Geometry &other) {
    if (other.m_geometry) {
        m_geometry = other.m_geometry->clone();
    } else {
        m_geometry = nullptr;
    }
}

Geometry &Geometry::operator=(const Geometry &other) {
    if (this != &other) {
        if (other.m_geometry) {
            m_geometry = other.m_geometry->clone();
        } else {
            m_geometry = nullptr;
        }
    }
    return *this;
}

bool Geometry::operator==(const Geometry &other) const {
    if (!m_geometry && !other.m_geometry) {
        return true;
    }
    if (!m_geometry || !other.m_geometry) {
        return false;
    }
    return m_geometry->equals(other.m_geometry.get());
}

bool Geometry::operator!=(const Geometry &other) const {
    return !(*this == other);
}

bool Geometry::isNull() const {
    return m_geometry == nullptr;
}

std::string Geometry::type() const {
    if (!m_geometry) {
        return "Null";
    }
    return m_geometry->type();
}

bool Geometry::isPoint() const {
    return type() == "Point";
}

bool Geometry::isLineString() const {
    return type() == "LineString";
}

bool Geometry::isPolygon() const {
    return type() == "Polygon";
}

bool Geometry::isMultiPoint() const {
    return type() == "MultiPoint";
}

bool Geometry::isMultiLineString() const {
    return type() == "MultiLineString";
}

bool Geometry::isMultiPolygon() const {
    return type() == "MultiPolygon";
}

bool Geometry::isGeometryCollection() const {
    return type() == "GeometryCollection";
}

Point Geometry::toPoint() const {
    if (isPoint()) {
        Point *p = dynamic_cast<Point*>(m_geometry.get());
        if (p) {
            return *p;
        }
    }
    return Point();
}

LineString Geometry::toLineString() const {
    if (isLineString()) {
        LineString *ls = dynamic_cast<LineString*>(m_geometry.get());
        if (ls) {
            return *ls;
        }
    }
    return LineString();
}

Polygon Geometry::toPolygon() const {
    if (isPolygon()) {
        Polygon *pg = dynamic_cast<Polygon*>(m_geometry.get());
        if (pg) {
            return *pg;
        }
    }
    return Polygon();
}

MultiPoint Geometry::toMultiPoint() const {
    if (isMultiPoint()) {
        MultiPoint *mp = dynamic_cast<MultiPoint*>(m_geometry.get());
        if (mp) {
            return *mp;
        }
    }
    return MultiPoint();
}

MultiLineString Geometry::toMultiLineString() const {
    if (isMultiLineString()) {
        MultiLineString *mls = dynamic_cast<MultiLineString*>(m_geometry.get());
        if (mls) {
            return *mls;
        }
    }
    return MultiLineString();
}

MultiPolygon Geometry::toMultiPolygon() const {
    if (isMultiPolygon()) {
        MultiPolygon *mpg = dynamic_cast<MultiPolygon*>(m_geometry.get());
        if (mpg) {
            return *mpg;
        }
    }
    return MultiPolygon();
}

GeometryCollection Geometry::toGeometryCollection() const {
    if (isGeometryCollection()) {
        GeometryCollection *gc = dynamic_cast<GeometryCollection*>(m_geometry.get());
        if (gc) {
            return *gc;
        }
    }
    return GeometryCollection();
}

CborObject Geometry::toCborObject() const {
    CborObject obj;
    if (!m_geometry) {
        obj.insert("type", CborValue("Null"));
        return obj;
    }

    obj.insert("type", CborValue(m_geometry->type()));

    // GeometryCollection的coordinates字段实际是geometries数组
    if (isGeometryCollection()) {
        obj.insert("geometries", m_geometry->toCborValue());
    } else {
        obj.insert("coordinates", m_geometry->toCborValue());
    }

    return obj;
}

bool Geometry::fromCborObject(const CborObject &obj, Geometry &geometry, std::string &errorInfo) {
    if (!obj.contains("type")) {
        errorInfo = "Geometry object must have a 'type' field";
        return false;
    }

    std::string geomType = obj.value("type").toString();

    // 处理GeometryCollection特殊情况
    if (geomType == "GeometryCollection") {
        if (!obj.contains("geometries")) {
            errorInfo = "GeometryCollection must have a 'geometries' field";
            return false;
        }

        GeometryCollection collection;
        if (!GeometryCollection::fromCborValue(obj.value("geometries"), collection, errorInfo)) {
            return false;
        }
        geometry.m_geometry = std::make_shared<GeometryCollection>(collection);
        return true;
    }

    // 处理其他几何类型
    if (!obj.contains("coordinates")) {
        errorInfo = "Geometry object must have a 'coordinates' field";
        return false;
    }

    CborValue coordinates = obj.value("coordinates");

    if (geomType == "Point") {
        Point point;
        if (!Point::fromCborValue(coordinates, point, errorInfo)) {
            return false;
        }
        geometry.m_geometry = std::make_shared<Point>(point);
    } else if (geomType == "LineString") {
        LineString lineString;
        if (!LineString::fromCborValue(coordinates, lineString, errorInfo)) {
            return false;
        }
        geometry.m_geometry = std::make_shared<LineString>(lineString);
    } else if (geomType == "Polygon") {
        Polygon polygon;
        if (!Polygon::fromCborValue(coordinates, polygon, errorInfo)) {
            return false;
        }
        geometry.m_geometry = std::make_shared<Polygon>(polygon);
    } else if (geomType == "MultiPoint") {
        MultiPoint multiPoint;
        if (!MultiPoint::fromCborValue(coordinates, multiPoint, errorInfo)) {
            return false;
        }
        geometry.m_geometry = std::make_shared<MultiPoint>(multiPoint);
    } else if (geomType == "MultiLineString") {
        MultiLineString multiLineString;
        if (!MultiLineString::fromCborValue(coordinates, multiLineString, errorInfo)) {
            return false;
        }
        geometry.m_geometry = std::make_shared<MultiLineString>(multiLineString);
    } else if (geomType == "MultiPolygon") {
        MultiPolygon multiPolygon;
        if (!MultiPolygon::fromCborValue(coordinates, multiPolygon, errorInfo)) {
            return false;
        }
        geometry.m_geometry = std::make_shared<MultiPolygon>(multiPolygon);
    } else {
        std::ostringstream oss;
        oss << "Unknown geometry type: " << geomType;
        errorInfo = oss.str();
        return false;
    }

    return true;
}

bool Geometry::fromCborValue(const CborValue &value, Geometry &geometry, std::string &errorInfo) {
    if (!value.isObject()) {
        errorInfo = "Geometry value must be an object";
        return false;
    }

    return fromCborObject(value.toObject(), geometry, errorInfo);
}

std::shared_ptr<GeometryBase> Geometry::get() const {
    return m_geometry;
}
