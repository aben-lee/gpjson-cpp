/**
 * @file test_geojson.cpp
 * @brief GeoJson库独立测试程序
 *
 * 测试GeoJSON几何类型的核心功能
 * 依赖：CborObject库
 */

#include "Geometry.h"
#include "Point.h"
#include "LineString.h"
#include "Polygon.h"
#include "MultiGeometries.h"
#include "CborDocument.h"
#include <iostream>
#include <iomanip>
#include <cmath>

// 测试计数器
int totalTests = 0;
int passedTests = 0;

#define TEST_START(name) \
    std::cout << "\n========== " << name << " ==========\n"; \
    totalTests++;

#define ASSERT_TRUE(condition, message) \
    if (condition) { \
        std::cout << "✓ " << message << "\n"; \
    } else { \
        std::cout << "✗ " << message << " [失败]\n"; \
        return false; \
    }

#define TEST_PASS(message) \
    std::cout << "✓ " << message << "\n"; \
    passedTests++; \
    return true;

bool testPoint() {
    TEST_START("测试1: Point几何类型");

    // 创建Point
    Point p1(102.0, 0.5);
    ASSERT_TRUE(p1.x == 102.0 && p1.y == 0.5, "Point创建");

    // Point序列化为CborValue
    CborValue pointCoords = p1.toCborValue();
    ASSERT_TRUE(pointCoords.isArray(), "Point序列化为CborValue");

    // Point反序列化
    Point p2;
    std::string errorInfo;
    bool parseOk = Point::fromCborValue(pointCoords, p2, errorInfo);
    ASSERT_TRUE(parseOk && p2.x == 102.0 && p2.y == 0.5, "Point反序列化");

    // Point相等比较
    ASSERT_TRUE(p1 == p2, "Point相等比较");

    // Point类型名称
    ASSERT_TRUE(p1.type() == "Point", "Point类型名称");

    TEST_PASS("Point几何类型测试");
}

bool testLineString() {
    TEST_START("测试2: LineString几何类型");

    // 创建LineString
    LineString ls;
    ls.append(Point(102.0, 0.0));
    ls.append(Point(103.0, 1.0));
    ls.append(Point(104.0, 0.0));
    ls.append(Point(105.0, 1.0));

    ASSERT_TRUE(ls.size() == 4, "LineString创建（4个点）");
    ASSERT_TRUE(!ls.isEmpty(), "LineString非空");

    // LineString序列化
    CborValue lsCoords = ls.toCborValue();
    ASSERT_TRUE(lsCoords.isArray(), "LineString序列化");

    // LineString反序列化
    LineString ls2;
    std::string errorInfo;
    bool parseOk = LineString::fromCborValue(lsCoords, ls2, errorInfo);
    ASSERT_TRUE(parseOk && ls2.size() == 4, "LineString反序列化");

    // LineString相等比较
    ASSERT_TRUE(ls == ls2, "LineString相等比较");

    // 类型名称
    ASSERT_TRUE(ls.type() == "LineString", "LineString类型名称");

    TEST_PASS("LineString几何类型测试");
}

bool testPolygon() {
    TEST_START("测试3: Polygon几何类型");

    // 创建Polygon外环（矩形）
    LinearRing outerRing;
    outerRing.append(Point(100.0, 0.0));
    outerRing.append(Point(101.0, 0.0));
    outerRing.append(Point(101.0, 1.0));
    outerRing.append(Point(100.0, 1.0));
    outerRing.append(Point(100.0, 0.0));  // 闭合

    ASSERT_TRUE(outerRing.isClosed(), "LinearRing闭合检查");

    Polygon poly;
    poly.append(outerRing);
    ASSERT_TRUE(poly.size() == 1, "Polygon创建（1个环）");

    // 添加内环（洞）
    LinearRing innerRing;
    innerRing.append(Point(100.2, 0.2));
    innerRing.append(Point(100.8, 0.2));
    innerRing.append(Point(100.8, 0.8));
    innerRing.append(Point(100.2, 0.8));
    innerRing.append(Point(100.2, 0.2));  // 闭合

    poly.append(innerRing);
    ASSERT_TRUE(poly.size() == 2, "Polygon添加内环（2个环）");

    // Polygon序列化
    CborValue polyCoords = poly.toCborValue();
    ASSERT_TRUE(polyCoords.isArray(), "Polygon序列化");

    // Polygon反序列化
    Polygon poly2;
    std::string errorInfo;
    bool parseOk = Polygon::fromCborValue(polyCoords, poly2, errorInfo);
    ASSERT_TRUE(parseOk && poly2.size() == 2, "Polygon反序列化");

    // 类型名称
    ASSERT_TRUE(poly.type() == "Polygon", "Polygon类型名称");

    TEST_PASS("Polygon几何类型测试");
}

bool testMultiPoint() {
    TEST_START("测试4: MultiPoint几何类型");

    // 创建MultiPoint
    MultiPoint mp;
    mp.append(Point(100.0, 0.0));
    mp.append(Point(101.0, 1.0));
    mp.append(Point(102.0, 2.0));

    ASSERT_TRUE(mp.size() == 3, "MultiPoint创建（3个点）");

    // MultiPoint序列化
    CborValue mpCoords = mp.toCborValue();
    ASSERT_TRUE(mpCoords.isArray(), "MultiPoint序列化");

    // MultiPoint反序列化
    MultiPoint mp2;
    std::string errorInfo;
    bool parseOk = MultiPoint::fromCborValue(mpCoords, mp2, errorInfo);
    ASSERT_TRUE(parseOk && mp2.size() == 3, "MultiPoint反序列化");

    // 相等比较
    ASSERT_TRUE(mp == mp2, "MultiPoint相等比较");

    // 类型名称
    ASSERT_TRUE(mp.type() == "MultiPoint", "MultiPoint类型名称");

    TEST_PASS("MultiPoint几何类型测试");
}

bool testMultiLineString() {
    TEST_START("测试5: MultiLineString几何类型");

    // 创建MultiLineString
    MultiLineString mls;

    LineString ls1;
    ls1.append(Point(100.0, 0.0));
    ls1.append(Point(101.0, 1.0));
    mls.append(ls1);

    LineString ls2;
    ls2.append(Point(102.0, 2.0));
    ls2.append(Point(103.0, 3.0));
    mls.append(ls2);

    ASSERT_TRUE(mls.size() == 2, "MultiLineString创建（2条线）");

    // MultiLineString序列化
    CborValue mlsCoords = mls.toCborValue();
    ASSERT_TRUE(mlsCoords.isArray(), "MultiLineString序列化");

    // MultiLineString反序列化
    MultiLineString mls2;
    std::string errorInfo;
    bool parseOk = MultiLineString::fromCborValue(mlsCoords, mls2, errorInfo);
    ASSERT_TRUE(parseOk && mls2.size() == 2, "MultiLineString反序列化");

    // 类型名称
    ASSERT_TRUE(mls.type() == "MultiLineString", "MultiLineString类型名称");

    TEST_PASS("MultiLineString几何类型测试");
}

bool testMultiPolygon() {
    TEST_START("测试6: MultiPolygon几何类型");

    // 创建MultiPolygon
    MultiPolygon mpoly;

    // 第一个多边形
    LinearRing ring1;
    ring1.append(Point(100.0, 0.0));
    ring1.append(Point(101.0, 0.0));
    ring1.append(Point(101.0, 1.0));
    ring1.append(Point(100.0, 1.0));
    ring1.append(Point(100.0, 0.0));

    Polygon poly1;
    poly1.append(ring1);
    mpoly.append(poly1);

    // 第二个多边形
    LinearRing ring2;
    ring2.append(Point(102.0, 2.0));
    ring2.append(Point(103.0, 2.0));
    ring2.append(Point(103.0, 3.0));
    ring2.append(Point(102.0, 3.0));
    ring2.append(Point(102.0, 2.0));

    Polygon poly2;
    poly2.append(ring2);
    mpoly.append(poly2);

    ASSERT_TRUE(mpoly.size() == 2, "MultiPolygon创建（2个多边形）");

    // MultiPolygon序列化
    CborValue mpolyCoords = mpoly.toCborValue();
    ASSERT_TRUE(mpolyCoords.isArray(), "MultiPolygon序列化");

    // MultiPolygon反序列化
    MultiPolygon mpoly2;
    std::string errorInfo;
    bool parseOk = MultiPolygon::fromCborValue(mpolyCoords, mpoly2, errorInfo);
    ASSERT_TRUE(parseOk && mpoly2.size() == 2, "MultiPolygon反序列化");

    // 类型名称
    ASSERT_TRUE(mpoly.type() == "MultiPolygon", "MultiPolygon类型名称");

    TEST_PASS("MultiPolygon几何类型测试");
}

bool testGeometry() {
    TEST_START("测试7: Geometry包装类");

    // 创建Point Geometry
    Geometry geom(std::make_shared<Point>(102.0, 0.5));
    ASSERT_TRUE(geom.type() == "Point", "Geometry创建（Point）");
    ASSERT_TRUE(geom.isPoint(), "Geometry类型判断（isPoint）");
    ASSERT_TRUE(!geom.isNull(), "Geometry非空");

    // Geometry转换为CborObject
    CborObject geomObj = geom.toCborObject();
    ASSERT_TRUE(geomObj.contains("type"), "Geometry包含type字段");
    ASSERT_TRUE(geomObj.contains("coordinates"), "Geometry包含coordinates字段");
    ASSERT_TRUE(geomObj.value("type").toString() == "Point", "Geometry type字段值正确");

    // Geometry反序列化
    Geometry geom2;
    std::string errorInfo;
    bool parseOk = Geometry::fromCborObject(geomObj, geom2, errorInfo);
    ASSERT_TRUE(parseOk, "Geometry反序列化");
    ASSERT_TRUE(geom2.type() == "Point", "反序列化后类型正确");

    // 转换为Point
    Point pt = geom2.toPoint();
    ASSERT_TRUE(std::abs(pt.x - 102.0) < 0.001 && std::abs(pt.y - 0.5) < 0.001,
                "Geometry转Point");

    TEST_PASS("Geometry包装类测试");
}

bool testGeometryCollection() {
    TEST_START("测试8: GeometryCollection");

    // 创建GeometryCollection
    GeometryCollection gc;
    gc.append(std::make_shared<Point>(100.0, 0.0));
    gc.append(std::make_shared<LineString>(LineString()));

    ASSERT_TRUE(gc.size() == 2, "GeometryCollection创建（2个几何对象）");
    ASSERT_TRUE(!gc.isEmpty(), "GeometryCollection非空");

    // GeometryCollection序列化
    CborValue gcValue = gc.toCborValue();
    ASSERT_TRUE(gcValue.isArray(), "GeometryCollection序列化");

    // GeometryCollection反序列化
    GeometryCollection gc2;
    std::string errorInfo;
    bool parseOk = GeometryCollection::fromCborValue(gcValue, gc2, errorInfo);
    ASSERT_TRUE(parseOk, "GeometryCollection反序列化");
    ASSERT_TRUE(gc2.size() == 2, "反序列化后大小正确");

    // 类型名称
    ASSERT_TRUE(gc.type() == "GeometryCollection", "GeometryCollection类型名称");

    TEST_PASS("GeometryCollection测试");
}

bool testGeoJSONFormat() {
    TEST_START("测试9: GeoJSON格式互转");

    // 创建一个Point的GeoJSON对象
    Geometry geom(std::make_shared<Point>(102.0, 0.5));
    CborObject geoJsonObj = geom.toCborObject();

    // 转换为JSON字符串
    CborDocument doc(geoJsonObj);
    std::string jsonStr = doc.toJson(CborDocument::Indented);

    std::cout << "生成的GeoJSON:\n" << jsonStr << "\n";

    ASSERT_TRUE(!jsonStr.empty(), "GeoJSON字符串非空");
    ASSERT_TRUE(jsonStr.find("\"type\"") != std::string::npos, "包含type字段");
    ASSERT_TRUE(jsonStr.find("\"coordinates\"") != std::string::npos, "包含coordinates字段");
    ASSERT_TRUE(jsonStr.find("Point") != std::string::npos, "类型为Point");

    // 从JSON字符串解析回来
    std::string errorInfo;
    CborDocument doc2 = CborDocument::fromJson(jsonStr, &errorInfo);
    ASSERT_TRUE(!doc2.isNull(), "JSON解析成功");

    Geometry geom2;
    CborObject obj = doc2.object();
    bool parseOk = Geometry::fromCborObject(obj, geom2, errorInfo);
    ASSERT_TRUE(parseOk, "从JSON重建Geometry");

    Point pt = geom2.toPoint();
    ASSERT_TRUE(std::abs(pt.x - 102.0) < 0.001 && std::abs(pt.y - 0.5) < 0.001,
                "坐标值正确");

    TEST_PASS("GeoJSON格式互转测试");
}

bool testComplexGeometry() {
    TEST_START("测试10: 复杂几何对象");

    // 创建一个带洞的多边形
    LinearRing outer;
    outer.append(Point(0.0, 0.0));
    outer.append(Point(10.0, 0.0));
    outer.append(Point(10.0, 10.0));
    outer.append(Point(0.0, 10.0));
    outer.append(Point(0.0, 0.0));

    LinearRing inner;
    inner.append(Point(2.0, 2.0));
    inner.append(Point(8.0, 2.0));
    inner.append(Point(8.0, 8.0));
    inner.append(Point(2.0, 8.0));
    inner.append(Point(2.0, 2.0));

    Polygon poly;
    poly.append(outer);
    poly.append(inner);

    // 序列化为GeoJSON
    Geometry geom(std::make_shared<Polygon>(poly));
    CborObject geoJsonObj = geom.toCborObject();
    CborDocument doc(geoJsonObj);
    std::string jsonStr = doc.toJson(CborDocument::Indented);

    std::cout << "带洞多边形GeoJSON:\n" << jsonStr << "\n";

    // 反序列化
    std::string errorInfo;
    CborDocument doc2 = CborDocument::fromJson(jsonStr, &errorInfo);
    Geometry geom2;
    bool parseOk = Geometry::fromCborObject(doc2.object(), geom2, errorInfo);
    ASSERT_TRUE(parseOk, "复杂多边形解析成功");

    Polygon poly2 = geom2.toPolygon();
    ASSERT_TRUE(poly2.size() == 2, "多边形有2个环（外环+内环）");

    TEST_PASS("复杂几何对象测试");
}

int main() {
    std::cout << "==================== GeoJson库独立测试 ====================\n";
    std::cout << "测试GeoJSON几何类型的核心功能\n";
    std::cout << "依赖：仅CborObject库\n";
    std::cout << "============================================================\n";

    try {
        testPoint();
        testLineString();
        testPolygon();
        testMultiPoint();
        testMultiLineString();
        testMultiPolygon();
        testGeometry();
        testGeometryCollection();
        testGeoJSONFormat();
        testComplexGeometry();

        std::cout << "\n============================================================\n";
        std::cout << "测试完成！\n";
        std::cout << "通过: " << passedTests << "/" << totalTests << " ("
                  << std::fixed << std::setprecision(1)
                  << (100.0 * passedTests / totalTests) << "%)\n";
        std::cout << "============================================================\n";

        return (passedTests == totalTests) ? 0 : 1;
    } catch (const std::exception &e) {
        std::cerr << "\n❌ 测试过程中发生异常: " << e.what() << "\n";
        return 1;
    }
}
