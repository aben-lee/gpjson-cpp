# Python 调用 CborObject 示例

本目录包含Python通过ctypes调用CborObject C API的示例。

## 📋 文件说明

- `cbor_wrapper.py` - Python ctypes包装器（包含完整示例）
- `README.md` - 本文档

## 🔧 编译和使用步骤

### 1. 编译C库为共享库

首先需要将CborObject编译为共享库：

**Windows:**
```bash
cd ../../cbor
cl.exe /LD /MD /utf-8 /D_CRT_SECURE_NO_WARNINGS cbor.c json.c pointer.c /FeCborObject.dll
copy CborObject.dll ..\..\Examples\Python\
```

**Linux:**
```bash
cd ../../cbor
gcc -shared -fPIC cbor.c json.c pointer.c -o libCborObject.so
cp libCborObject.so ../../Examples/Python/
```

**macOS:**
```bash
cd ../../cbor
gcc -shared -fPIC cbor.c json.c pointer.c -o libCborObject.dylib
cp libCborObject.dylib ../../Examples/Python/
```

### 2. 运行示例

```bash
python cbor_wrapper.py
```

或在Python脚本中导入：

```python
from cbor_wrapper import CborDocument

doc = CborDocument.from_json('{"name":"Alice"}')
print(doc.get_string("/name"))
```

## 💡 使用示例

### 基本用法

```python
from cbor_wrapper import CborDocument

# 从JSON字符串解析
doc = CborDocument.from_json('{"name":"Alice","age":30}')

# 读取数据
name = doc.get_string("/name")
age = doc.get_int("/age")
print(f"Name: {name}, Age: {age}")

# 修改数据
doc.set_int("/age", 31)
doc.set_string("/email", "alice@example.com")

# 输出JSON
print(doc.to_json(pretty=True))

# 保存文件
doc.to_json_file("output.json", pretty=True)

# 释放资源
doc.close()
```

### 使用with语句自动管理内存

```python
from cbor_wrapper import CborDocument

# 推荐：使用with语句自动释放
with CborDocument.from_json('{"test":123}') as doc:
    value = doc.get_int("/test")
    print(f"Value: {value}")
# 自动调用close()
```

### 文件操作

```python
from cbor_wrapper import CborDocument

# 从文件加载
doc = CborDocument.from_json_file("input.json")

# 处理数据
doc.set_string("/status", "processed")

# 保存到文件
doc.to_json_file("output.json", pretty=True)

doc.close()
```

### 处理数组

```python
from cbor_wrapper import CborDocument

doc = CborDocument.from_json('{"scores":[85,90,95]}')

# 访问数组元素（使用JSON Pointer）
score1 = doc.get_int("/scores/0")  # 85
score2 = doc.get_int("/scores/1")  # 90
score3 = doc.get_int("/scores/2")  # 95

print(f"Scores: {score1}, {score2}, {score3}")

doc.close()
```

### 处理嵌套对象

```python
from cbor_wrapper import CborDocument

json_str = '''
{
    "user": {
        "name": "Alice",
        "address": {
            "city": "Beijing",
            "zipcode": "100000"
        }
    }
}
'''

doc = CborDocument.from_json(json_str)

# 访问嵌套字段
city = doc.get_string("/user/address/city")
zipcode = doc.get_string("/user/address/zipcode")

print(f"City: {city}, Zipcode: {zipcode}")

# 修改嵌套字段
doc.set_string("/user/address/city", "Shanghai")

doc.close()
```

## 🌟 特性

1. **自动内存管理**：支持`with`语句（上下文管理器）
2. **类型安全**：封装Python类型（str、int、float、bool）
3. **JSON Pointer**：支持路径访问和修改
4. **文件I/O**：支持JSON文件的加载和保存
5. **异常处理**：操作失败时抛出Python异常

## 📊 API参考

### CborDocument类

#### 静态方法

- `from_json(json_str)` - 从JSON字符串解析
- `from_json_file(file_path)` - 从JSON文件加载

#### 实例方法

- `to_json(pretty=True)` - 转换为JSON字符串
- `to_json_file(file_path, pretty=True)` - 保存为JSON文件
- `get_string(path)` - 获取字符串值
- `get_int(path)` - 获取整数值
- `get_float(path)` - 获取浮点数值
- `set_string(path, value)` - 设置字符串值
- `set_int(path, value)` - 设置整数值
- `set_float(path, value)` - 设置浮点数值
- `set_bool(path, value)` - 设置布尔值
- `close()` - 释放CBOR对象

#### 上下文管理器

```python
with CborDocument.from_json('{}') as doc:
    # 使用doc
    pass
# 自动调用close()
```

## ⚠️ 注意事项

1. **内存管理**：
   - 务必调用`close()`释放C内存
   - 推荐使用`with`语句自动管理
   - 对象析构时会自动调用`close()`

2. **线程安全**：
   - ctypes调用不是线程安全的
   - 多线程访问需要加锁

3. **库路径**：
   - 共享库需要在Python能找到的路径
   - 可以放在脚本同目录
   - 或设置环境变量（LD_LIBRARY_PATH等）

4. **字符编码**：
   - 使用UTF-8编码
   - Python字符串自动转换

5. **错误处理**：
   - 操作失败会抛出异常
   - 建议使用try-except捕获

## 🐍 Python版本要求

- Python 3.6+
- 只依赖标准库ctypes，无需额外安装

## 🔗 更多资源

- [Python ctypes文档](https://docs.python.org/3/library/ctypes.html)
- [CborObject C API文档](../../cbor/cbor.h)
