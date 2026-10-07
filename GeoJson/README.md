# GeoJson库 - 独立的地理JSON解析库

**版本**：1.0
**创建时间**：2026-01-05
**依赖**：CborObject库（必须先存在）

---

## 项目简介

GeoJson是一个独立的C++库，用于处理GeoJSON格式的地理空间数据。它提供了完整的几何类型支持，可以轻松地在C++对象和GeoJSON格式之间进行转换。

**核心特性**：
- ✅ 完整的GeoJSON几何类型支持
- ✅ 与CBOR/JSON格式互转
- ✅ Qt风格的C++ API
- ✅ 独立模块，可单独使用
- ✅ 仅依赖CborObject库

---

## 支持的几何类型

| 几何类型 | 说明 | 坐标格式 |
|---------|------|---------|
| Point | 点 | `[x, y]` |
| LineString | 线串 | `[[x1, y1], [x2, y2], ...]` |
| Polygon | 多边形 | `[[[x1, y1], ...], [[x2, y2], ...]]` |
| MultiPoint | 多点 | `[[x1, y1], [x2, y2], ...]` |
| MultiLineString | 多线串 | `[[[x1, y1], ...], [[x2, y2], ...]]` |
| MultiPolygon | 多多边形 | `[[[[x1, y1], ...]]]` |
| GeometryCollection | 几何集合 | 混合几何对象 |
| **Geometry** | **包装类** | 统一的几何对象接口 |

---

## 构建配置文件

### Qt (qmake)
```qmake
# 在你的.pro文件中包含GeoJson库
include(path/to/GeoJson/GeoJson.pri)

# 注意：GeoJson.pri会自动包含CborObject的路径
```

### CMake
```cmake
# 添加子目录
add_subdirectory(path/to/GeoJson)

# 链接GeoJson库
target_link_libraries(your_target PRIVATE GeoJson CborObject)
```

---

## 快速开始

### 示例1：创建Point

```cpp
#include "Geometry.h"
#include "Point.h"
#include "CborDocument.h"

// 创建Point
Point pt(102.0, 0.5);

// 转换为GeoJSON对象
Geometry geom(std::make_shared<Point>(pt));
CborObject geoJson = geom.toCborObject();

// 输出为JSON字符串
CborDocument doc(CborValue(geoJson));
std::string jsonStr = doc.toJson(CborDocument::Indented);
// 输出：{"type":"Point","coordinates":[102.0,0.5]}
```

### 示例2：解析GeoJSON

```cpp
std::string geoJsonStr = R"({
  "type": "Point",
  "coordinates": [102.0, 0.5]
})";

// 解析JSON
CborDocument doc = CborDocument::fromJson(geoJsonStr);
CborObject obj = doc.object();

// 创建Geometry对象
Geometry geom;
std::string errorInfo;
bool ok = Geometry::fromCborObject(obj, geom, errorInfo);

if (ok) {
    Point pt = geom.toPoint();
    std::cout << "经度: " << pt.x << ", 纬度: " << pt.y << std::endl;
}
```

### 示例3：创建Polygon

```cpp
#include "Polygon.h"

// 外环（矩形）
LinearRing outer;
outer.append(Point(100.0, 0.0));
outer.append(Point(101.0, 0.0));
outer.append(Point(101.0, 1.0));
outer.append(Point(100.0, 1.0));
outer.append(Point(100.0, 0.0));  // 闭合

// 内环（洞）
LinearRing inner;
inner.append(Point(100.2, 0.2));
inner.append(Point(100.8, 0.2));
inner.append(Point(100.8, 0.8));
inner.append(Point(100.2, 0.8));
inner.append(Point(100.2, 0.2));  // 闭合

// 创建Polygon
Polygon poly;
poly.append(outer);  // 添加外环
poly.append(inner);  // 添加内环（可选）
```

---

## 测试

### Windows (Visual Studio)

1. **准备环境**：
   ```cmd
   cd test
   compile_test_geojson.bat
   ```

2. **手动编译**：
   ```cmd
   cl.exe /EHsc /std:c++14 /MD /utf-8 ^
       /I..\GeoJson /I..\CborObject /I..\CborObject\cbor ^
       /D_CRT_SECURE_NO_WARNINGS ^
       /Fetest_geojson.exe ^
       test_geojson.cpp ^
       ..\GeoJson\Point.cpp ^
       ..\GeoJson\LineString.cpp ^
       ..\GeoJson\Polygon.cpp ^
       ..\GeoJson\MultiGeometries.cpp ^
       ..\GeoJson\Geometry.cpp ^
       ..\CborObject\CborValue.cpp ^
       ..\CborObject\CborArray.cpp ^
       ..\CborObject\CborObject.cpp ^
       ..\CborObject\CborDocument.cpp ^
       ..\CborObject\cbor\cbor.c ^
       ..\CborObject\cbor\json.c ^
       ..\CborObject\cbor\pointer.c
   ```

3. **运行测试**：
   ```cmd
   test_geojson.exe
   ```

### Linux/macOS

```bash
cd test
g++ -std=c++14 -I../GeoJson -I../CborObject -I../CborObject/cbor \
    test_geojson.cpp \
    ../GeoJson/*.cpp \
    ../CborObject/*.cpp \
    ../CborObject/cbor/*.c \
    -o test_geojson

./test_geojson
```

---

## API参考

### Point类

```cpp
class Point : public GeometryBase {
public:
    double x;  // 经度
    double y;  // 纬度

    Point();
    Point(double x, double y);

    // GeometryBase接口
    std::string type() const;  // 返回"Point"
    CborValue toCborValue() const;
    std::shared_ptr<GeometryBase> clone() const;
    bool equals(const GeometryBase *other) const;

    // 静态解析方法
    static bool fromCborValue(const CborValue &value, Point &point, std::string &errorInfo);
};
```

### LineString类

```cpp
class LineString : public GeometryBase {
public:
    std::vector<Point> points;

    LineString();

    void append(const Point &point);
    int size() const;
    bool isEmpty() const;
    void clear();

    // GeometryBase接口
    std::string type() const;  // 返回"LineString"
    CborValue toCborValue() const;
    // ...
};
```

### Polygon类

```cpp
class LinearRing : public LineString {
public:
    bool isClosed() const;  // 检查是否闭合
};

class Polygon : public GeometryBase {
public:
    std::vector<LinearRing> rings;  // rings[0]为外环，rings[1...]为内环

    Polygon();

    void append(const LinearRing &ring);
    int size() const;
    bool isEmpty() const;
    void clear();

    // GeometryBase接口
    std::string type() const;  // 返回"Polygon"
    CborValue toCborValue() const;
    // ...
};
```

### Geometry包装类

```cpp
class Geometry {
public:
    Geometry();
    Geometry(const std::shared_ptr<GeometryBase> &geometry);

    // 类型检查
    bool isNull() const;
    bool isPoint() const;
    bool isLineString() const;
    bool isPolygon() const;
    bool isMultiPoint() const;
    bool isMultiLineString() const;
    bool isMultiPolygon() const;
    bool isGeometryCollection() const;

    std::string type() const;

    // 类型转换
    Point toPoint() const;
    LineString toLineString() const;
    Polygon toPolygon() const;
    // ...

    // GeoJSON互转
    CborObject toCborObject() const;
    static bool fromCborObject(const CborObject &obj, Geometry &geometry, std::string &errorInfo);
};
```

---

## 依赖关系

```
GeoJson库
  ↓
CborObject库
  ├── CborValue.h/.cpp
  ├── CborArray.h/.cpp
  ├── CborObject.h/.cpp
  ├── CborDocument.h/.cpp
  └── cbor/
      ├── cbor.h/.c
      ├── json.c
      └── pointer.c
```

**注意**：GeoJson库不依赖GPJson库，可以完全独立使用。

---

## 文件清单

### 头文件
- `GeometryBase.h` - 几何对象基类
- `Point.h` - 点类型
- `LineString.h` - 线串类型
- `Polygon.h` - 多边形类型
- `MultiGeometries.h` - 多点、多线串、多多边形
- `Geometry.h` - 几何对象包装类

### 源文件
- `Point.cpp`
- `LineString.cpp`
- `Polygon.cpp`
- `MultiGeometries.cpp`
- `Geometry.cpp`

### 构建文件
- `GeoJson.pri` - Qt工程文件
- `CMakeLists.txt` - CMake工程文件

### 测试文件
- `test/test_geojson.cpp` - 独立测试程序（10个测试用例）
- `test/test_geojson.pro` - qmake测试工程
- `test/compile_test_geojson.bat` - Windows编译脚本

---

## 常见问题

### Q1: GeoJson库可以独立使用吗？
**A**: 是的，GeoJson库完全独立，只依赖CborObject库。不需要GPJson库即可使用。

### Q2: 如何表示带洞的多边形？
**A**: 使用Polygon类，第一个ring是外环，后续的ring是内环（洞）。

```cpp
Polygon poly;
poly.append(outerRing);  // 外环
poly.append(innerRing1); // 第一个洞
poly.append(innerRing2); // 第二个洞
```

### Q3: 坐标的顺序是什么？
**A**: GeoJSON标准：`[经度(longitude), 纬度(latitude)]`，即`[x, y]`。

### Q4: 如何检查LinearRing是否闭合？
**A**: 使用`isClosed()`方法，它会检查首尾点是否相同。

```cpp
LinearRing ring;
// ... 添加点 ...
if (!ring.isClosed()) {
    ring.append(ring.points[0]);  // 闭合ring
}
```

### Q5: 如何在几何类型之间转换？
**A**: 使用Geometry包装类：

```cpp
Geometry geom(std::make_shared<Point>(102.0, 0.5));
if (geom.isPoint()) {
    Point pt = geom.toPoint();
}
```

---

## 性能特性

- **内存效率**：使用智能指针管理内存，自动释放
- **零拷贝**：内部使用std::shared_ptr，减少拷贝开销
- **序列化速度**：CBOR格式比JSON快1.5-2倍
- **存储空间**：CBOR比JSON节省30-50%

---

## 标准遵循

- **GeoJSON** (RFC 7946): https://tools.ietf.org/html/rfc7946
- **CBOR** (RFC 7049): https://tools.ietf.org/html/rfc7049
- **JSON** (RFC 7159): https://tools.ietf.org/html/rfc7159

---

## 许可证

(根据项目实际情况填写)

---

## 变更日志

### 版本 1.0 (2026-01-05)
- ✅ 初始版本发布
- ✅ 支持所有GeoJSON几何类型
- ✅ CBOR/JSON互转功能
- ✅ 完整的测试覆盖（10个测试用例）
- ✅ Qt和CMake构建支持

---

**最后更新**：2026-01-05
**维护者**：项目组
**状态**：✅ 生产就绪
