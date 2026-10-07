# JavaScript (Node.js) 调用 CborObject 示例

本目录包含Node.js通过ffi-napi调用CborObject C API的示例。

## 📋 文件说明

- `cbor-wrapper.js` - Node.js FFI包装器（包含完整示例）
- `package.json` - npm包配置
- `README.md` - 本文档

## 🔧 编译和使用步骤

### 1. 安装Node.js依赖

```bash
npm install
```

这将安装：
- `ffi-napi` - Node.js FFI库（调用C函数）
- `ref-napi` - Node.js类型定义库

### 2. 编译C库为共享库

**Windows:**
```bash
cd ../../cbor
cl.exe /LD /MD /utf-8 /D_CRT_SECURE_NO_WARNINGS cbor.c json.c pointer.c /FeCborObject.dll
copy CborObject.dll ..\..\Examples\JavaScript\
```

**Linux:**
```bash
cd ../../cbor
gcc -shared -fPIC cbor.c json.c pointer.c -o libCborObject.so
cp libCborObject.so ../../Examples/JavaScript/
```

**macOS:**
```bash
cd ../../cbor
gcc -shared -fPIC cbor.c json.c pointer.c -o libCborObject.dylib
cp libCborObject.dylib ../../Examples/JavaScript/
```

### 3. 运行示例

```bash
node cbor-wrapper.js
```

或在Node.js项目中导入：

```javascript
const { CborDocument } = require('./cbor-wrapper');

const doc = CborDocument.fromJson('{"name":"Alice"}');
console.log(doc.getString("/name"));
doc.close();
```

## 💡 使用示例

### 基本用法

```javascript
const { CborDocument } = require('./cbor-wrapper');

// 从JSON字符串解析
const doc = CborDocument.fromJson('{"name":"Alice","age":30}');

// 读取数据
const name = doc.getString('/name');
const age = doc.getInt('/age');
console.log(`Name: ${name}, Age: ${age}`);

// 修改数据
doc.setInt('/age', 31);
doc.setString('/email', 'alice@example.com');

// 输出JSON
console.log(doc.toJson(true));

// 保存文件
doc.toJsonFile('output.json', true);

// 释放资源
doc.close();
```

### 文件操作

```javascript
const { CborDocument } = require('./cbor-wrapper');

// 从文件加载
const doc = CborDocument.fromJsonFile('input.json');

// 处理数据
doc.setString('/status', 'processed');

// 保存到文件
doc.toJsonFile('output.json', true);

doc.close();
```

### 处理数组

```javascript
const { CborDocument } = require('./cbor-wrapper');

const doc = CborDocument.fromJson('{"scores":[85,90,95]}');

// 访问数组元素（使用JSON Pointer）
const score1 = doc.getInt('/scores/0');  // 85
const score2 = doc.getInt('/scores/1');  // 90
const score3 = doc.getInt('/scores/2');  // 95

console.log(`Scores: ${score1}, ${score2}, ${score3}`);

doc.close();
```

### 处理嵌套对象

```javascript
const { CborDocument } = require('./cbor-wrapper');

const jsonStr = `
{
    "user": {
        "name": "Alice",
        "address": {
            "city": "Beijing",
            "zipcode": "100000"
        }
    }
}
`;

const doc = CborDocument.fromJson(jsonStr);

// 访问嵌套字段
const city = doc.getString('/user/address/city');
const zipcode = doc.getString('/user/address/zipcode');

console.log(`City: ${city}, Zipcode: ${zipcode}`);

// 修改嵌套字段
doc.setString('/user/address/city', 'Shanghai');

doc.close();
```

### Express.js集成示例

```javascript
const express = require('express');
const { CborDocument } = require('./cbor-wrapper');

const app = express();
app.use(express.json());

// API endpoint
app.post('/api/process', (req, res) => {
    try {
        // 将请求体转为JSON字符串
        const jsonStr = JSON.stringify(req.body);

        // 解析为CborDocument
        const doc = CborDocument.fromJson(jsonStr);

        // 处理数据
        doc.setString('/status', 'processed');
        doc.setInt('/timestamp', Date.now());

        // 转回JSON发送响应
        const resultJson = doc.toJson(false);
        doc.close();

        res.json(JSON.parse(resultJson));
    } catch (error) {
        res.status(400).json({ error: error.message });
    }
});

app.listen(3000, () => {
    console.log('Server running on http://localhost:3000');
});
```

## 🌟 特性

1. **FFI集成**：通过ffi-napi无缝调用C库
2. **类型安全**：封装JavaScript类型（string、number、boolean）
3. **JSON Pointer**：支持路径访问和修改
4. **文件I/O**：支持JSON文件的加载和保存
5. **错误处理**：操作失败时抛出JavaScript异常

## 📊 API参考

### CborDocument类

#### 静态方法

- `fromJson(jsonStr)` - 从JSON字符串解析
- `fromJsonFile(filePath)` - 从JSON文件加载

#### 实例方法

- `toJson(pretty = true)` - 转换为JSON字符串
- `toJsonFile(filePath, pretty = true)` - 保存为JSON文件
- `getString(path)` - 获取字符串值
- `getInt(path)` - 获取整数值
- `getFloat(path)` - 获取浮点数值
- `setString(path, value)` - 设置字符串值
- `setInt(path, value)` - 设置整数值
- `setFloat(path, value)` - 设置浮点数值
- `setBool(path, value)` - 设置布尔值
- `close()` - 释放CBOR对象

## ⚠️ 注意事项

1. **内存管理**：
   - 务必调用`close()`释放C内存
   - 不支持自动垃圾回收（需要手动管理）
   - 可以在`finally`块中调用`close()`

2. **线程安全**：
   - Node.js是单线程的，但要注意异步操作
   - 不要在异步回调之间共享CborDocument实例

3. **库路径**：
   - 共享库需要在脚本同目录或系统库路径
   - Windows: PATH环境变量
   - Linux: LD_LIBRARY_PATH
   - macOS: DYLD_LIBRARY_PATH

4. **依赖安装**：
   - ffi-napi需要编译原生模块
   - 需要安装Python和构建工具（Windows需要VS Build Tools）

5. **错误处理**：
   - 操作失败会抛出异常
   - 建议使用try-catch-finally

## 🔧 完整示例：带错误处理

```javascript
const { CborDocument } = require('./cbor-wrapper');

let doc = null;
try {
    doc = CborDocument.fromJson('{"name":"Alice","age":30}');

    const name = doc.getString('/name');
    console.log(`Name: ${name}`);

    doc.setInt('/age', 31);
    doc.toJsonFile('output.json', true);

    console.log('Success!');
} catch (error) {
    console.error('Error:', error.message);
} finally {
    if (doc) {
        doc.close();
    }
}
```

## 📦 Node.js版本要求

- Node.js 12.x 或更高版本
- npm 6.x 或更高版本

## 🛠️ 依赖说明

- **ffi-napi**: Node.js FFI库，用于调用C函数
- **ref-napi**: Node.js类型转换库，用于C类型映射

## 🔗 更多资源

- [ffi-napi文档](https://github.com/node-ffi-napi/node-ffi-napi)
- [ref-napi文档](https://github.com/node-ffi-napi/ref-napi)
- [CborObject C API文档](../../cbor/cbor.h)
