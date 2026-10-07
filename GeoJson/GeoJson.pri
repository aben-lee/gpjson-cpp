# GeoJson库 - Qt工程管理文件
# GeoJSON几何类型的C++封装，用于地理空间数据处理
#
# 依赖：CborObject库（必须先包含CborObject.pri）

INCLUDEPATH += $$PWD \
               $$PWD/../CborObject

HEADERS += \
    $$PWD/GeometryBase.h \
    $$PWD/Point.h \
    $$PWD/LineString.h \
    $$PWD/Polygon.h \
    $$PWD/MultiGeometries.h \
    $$PWD/Geometry.h

SOURCES += \
    $$PWD/Point.cpp \
    $$PWD/LineString.cpp \
    $$PWD/Polygon.cpp \
    $$PWD/MultiGeometries.cpp \
    $$PWD/Geometry.cpp

# 编译选项
CONFIG += c++14

# 库说明
# 本库提供以下几何类型：
# - Point: 点
# - LineString: 线串
# - Polygon: 多边形（支持外环和内环）
# - MultiPoint: 多点
# - MultiLineString: 多线串
# - MultiPolygon: 多多边形
# - GeometryCollection: 几何集合
# - Geometry: 几何对象包装类
#
# 所有几何类型都支持：
# - 与CborValue/CborObject互转
# - GeoJSON格式序列化/反序列化
# - 拷贝、赋值、比较操作
# - 类型检查和类型转换
