# Java 调用 CborObject 示例

本目录包含Java通过JNI调用CborObject C API的示例。

## 📋 文件说明

- `CborWrapper.java` - Java JNI包装器类（包含示例代码）
- `README.md` - 本文档

## 🔧 编译和使用步骤

### 1. 编译C库为共享库

首先需要将CborObject编译为共享库：

**Windows:**
```bash
cd ../../cbor
cl.exe /LD /MD /utf-8 /D_CRT_SECURE_NO_WARNINGS cbor.c json.c pointer.c /FeCborObject.dll
```

**Linux:**
```bash
cd ../../cbor
gcc -shared -fPIC cbor.c json.c pointer.c -o libCborObject.so
```

**macOS:**
```bash
cd ../../cbor
gcc -shared -fPIC cbor.c json.c pointer.c -o libCborObject.dylib
```

### 2. 实现JNI桥接层（需要实现）

创建`CborWrapper_jni.c`实现JNI函数：

```c
#include <jni.h>
#include "../../cbor/cbor.h"

JNIEXPORT jlong JNICALL Java_com_cborobject_CborWrapper_jsonLoads
  (JNIEnv *env, jclass cls, jstring json) {
    const char *json_str = (*env)->GetStringUTFChars(env, json, NULL);
    cbor_value_t *val = cbor_json_loads(json_str, -1);
    (*env)->ReleaseStringUTFChars(env, json, json_str);
    return (jlong)val;
}

JNIEXPORT jstring JNICALL Java_com_cborobject_CborWrapper_jsonDumps
  (JNIEnv *env, jclass cls, jlong handle, jboolean pretty) {
    cbor_value_t *val = (cbor_value_t *)handle;
    char *json = cbor_json_dumps(val, NULL, pretty);
    jstring result = (*env)->NewStringUTF(env, json);
    free(json);
    return result;
}

JNIEXPORT jstring JNICALL Java_com_cborobject_CborWrapper_pointerGetString
  (JNIEnv *env, jclass cls, jlong handle, jstring path) {
    cbor_value_t *val = (cbor_value_t *)handle;
    const char *path_str = (*env)->GetStringUTFChars(env, path, NULL);
    const char *result = cbor_pointer_gets(val, path_str);
    (*env)->ReleaseStringUTFChars(env, path, path_str);
    return result ? (*env)->NewStringUTF(env, result) : NULL;
}

// ... 其他JNI函数实现 ...

JNIEXPORT void JNICALL Java_com_cborobject_CborWrapper_destroy
  (JNIEnv *env, jclass cls, jlong handle) {
    cbor_value_t *val = (cbor_value_t *)handle;
    if (val) {
        cbor_destroy(val);
    }
}
```

### 3. 编译JNI库

**Windows:**
```bash
cl /LD /MD /I"%JAVA_HOME%\include" /I"%JAVA_HOME%\include\win32" /I..\..\cbor ^
   CborWrapper_jni.c CborObject.lib /FeCborObjectJNI.dll
```

**Linux:**
```bash
gcc -shared -fPIC -I"$JAVA_HOME/include" -I"$JAVA_HOME/include/linux" -I../../cbor \
    CborWrapper_jni.c -L../../cbor -lCborObject -o libCborObjectJNI.so
```

### 4. 编译Java类

```bash
javac CborWrapper.java
```

### 5. 运行示例

**Windows:**
```bash
set PATH=%PATH%;<build-output-dir-containing-the-shared-library>
java -Djava.library.path=. com.cborobject.CborWrapper
```

**Linux:**
```bash
export LD_LIBRARY_PATH=../../cbor:$LD_LIBRARY_PATH
java -Djava.library.path=. com.cborobject.CborWrapper
```

## 💡 使用示例

```java
import com.cborobject.CborWrapper.CborDocument;

public class Example {
    public static void main(String[] args) {
        // 使用try-with-resources自动管理内存
        try (CborDocument doc = CborDocument.fromJson(
            "{\"name\":\"Alice\",\"age\":30}"
        )) {
            // 读取数据
            String name = doc.getString("/name");
            long age = doc.getInt("/age");
            System.out.println("Name: " + name + ", Age: " + age);

            // 修改数据
            doc.setInt("/age", 31);
            doc.setString("/email", "alice@example.com");

            // 输出JSON
            String json = doc.toJson(true);
            System.out.println(json);

            // 保存文件
            doc.toJsonFile("output.json", true);

        } catch (Exception e) {
            e.printStackTrace();
        }
        // doc自动释放（close()）
    }
}
```

## 🌟 特性

1. **自动内存管理**：使用`try-with-resources`或`AutoCloseable`接口
2. **类型安全**：Java类型封装（String、long、double）
3. **JSON Pointer**：支持路径访问和修改
4. **文件I/O**：支持JSON文件的加载和保存

## ⚠️ 注意事项

1. **线程安全**：JNI层不是线程安全的，多线程访问需要加锁
2. **内存管理**：务必使用`try-with-resources`或手动调用`close()`
3. **库路径**：确保共享库（.dll/.so/.dylib）在Java库路径中
4. **字符编码**：使用UTF-8编码

## 🔗 更多资源

- [JNI官方文档](https://docs.oracle.com/javase/8/docs/technotes/guides/jni/)
- [CborObject C API文档](../../cbor/cbor.h)
