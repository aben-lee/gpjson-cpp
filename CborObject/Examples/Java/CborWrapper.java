/**
 * @file CborWrapper.java
 * @brief Java JNI包装器，调用CborObject C API
 *
 * 本类通过JNI（Java Native Interface）调用CborObject的C API。
 * 需要先编译C库为共享库（DLL/SO/DYLIB）。
 *
 * 编译步骤：
 * 1. 编译C库为共享库：
 *    Windows: cl /LD cbor.c json.c pointer.c /FeCborObject.dll
 *    Linux:   gcc -shared -fPIC cbor.c json.c pointer.c -o libCborObject.so
 *    macOS:   gcc -shared -fPIC cbor.c json.c pointer.c -o libCborObject.dylib
 *
 * 2. 编译Java类：
 *    javac CborWrapper.java
 *
 * 3. 生成JNI头文件：
 *    javah -jni CborWrapper
 *
 * 4. 编译JNI实现（见CborWrapper_jni.c）
 */

package com.cborobject;

public class CborWrapper {

    // 加载本地库
    static {
        try {
            System.loadLibrary("CborObjectJNI");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("无法加载CborObjectJNI库: " + e.getMessage());
            System.err.println("请确保CborObjectJNI.dll/so/dylib在库路径中");
        }
    }

    // ========== Native方法声明 ==========

    /**
     * 从JSON字符串解析为CBOR对象
     * @param json JSON字符串
     * @return CBOR对象句柄（long），失败返回0
     */
    public static native long jsonLoads(String json);

    /**
     * 将CBOR对象转换为JSON字符串
     * @param handle CBOR对象句柄
     * @param pretty 是否美化输出
     * @return JSON字符串，失败返回null
     */
    public static native String jsonDumps(long handle, boolean pretty);

    /**
     * 通过JSON Pointer获取字符串值
     * @param handle CBOR对象句柄
     * @param path JSON Pointer路径（如 "/name"）
     * @return 字符串值，失败返回null
     */
    public static native String pointerGetString(long handle, String path);

    /**
     * 通过JSON Pointer获取整数值
     * @param handle CBOR对象句柄
     * @param path JSON Pointer路径
     * @return 整数值，失败返回0
     */
    public static native long pointerGetInt(long handle, String path);

    /**
     * 通过JSON Pointer获取浮点数值
     * @param handle CBOR对象句柄
     * @param path JSON Pointer路径
     * @return 浮点数值，失败返回0.0
     */
    public static native double pointerGetDouble(long handle, String path);

    /**
     * 通过JSON Pointer设置字符串值
     * @param handle CBOR对象句柄
     * @param path JSON Pointer路径
     * @param value 字符串值
     * @return 成功返回true，失败返回false
     */
    public static native boolean pointerSetString(long handle, String path, String value);

    /**
     * 通过JSON Pointer设置整数值
     * @param handle CBOR对象句柄
     * @param path JSON Pointer路径
     * @param value 整数值
     * @return 成功返回true，失败返回false
     */
    public static native boolean pointerSetInt(long handle, String path, long value);

    /**
     * 通过JSON Pointer设置浮点数值
     * @param handle CBOR对象句柄
     * @param path JSON Pointer路径
     * @param value 浮点数值
     * @return 成功返回true，失败返回false
     */
    public static native boolean pointerSetDouble(long handle, String path, double value);

    /**
     * 释放CBOR对象
     * @param handle CBOR对象句柄
     */
    public static native void destroy(long handle);

    /**
     * 从JSON文件加载
     * @param filePath JSON文件路径
     * @return CBOR对象句柄，失败返回0
     */
    public static native long jsonLoadFile(String filePath);

    /**
     * 保存为JSON文件
     * @param handle CBOR对象句柄
     * @param filePath JSON文件路径
     * @param pretty 是否美化输出
     * @return 成功返回true，失败返回false
     */
    public static native boolean jsonDumpFile(long handle, String filePath, boolean pretty);

    // ========== Java封装类 ==========

    /**
     * CborDocument封装类，提供自动内存管理
     */
    public static class CborDocument implements AutoCloseable {
        private long handle;
        private boolean closed = false;

        /**
         * 从JSON字符串解析
         * @param json JSON字符串
         * @return CborDocument对象，失败返回null
         */
        public static CborDocument fromJson(String json) {
            long handle = jsonLoads(json);
            if (handle == 0) {
                return null;
            }
            return new CborDocument(handle);
        }

        /**
         * 从JSON文件加载
         * @param filePath JSON文件路径
         * @return CborDocument对象，失败返回null
         */
        public static CborDocument fromJsonFile(String filePath) {
            long handle = jsonLoadFile(filePath);
            if (handle == 0) {
                return null;
            }
            return new CborDocument(handle);
        }

        private CborDocument(long handle) {
            this.handle = handle;
        }

        /**
         * 转换为JSON字符串
         * @param pretty 是否美化输出
         * @return JSON字符串
         */
        public String toJson(boolean pretty) {
            checkClosed();
            return jsonDumps(handle, pretty);
        }

        /**
         * 保存为JSON文件
         * @param filePath JSON文件路径
         * @param pretty 是否美化输出
         * @return 成功返回true
         */
        public boolean toJsonFile(String filePath, boolean pretty) {
            checkClosed();
            return jsonDumpFile(handle, filePath, pretty);
        }

        /**
         * 获取字符串值
         * @param path JSON Pointer路径
         * @return 字符串值
         */
        public String getString(String path) {
            checkClosed();
            return pointerGetString(handle, path);
        }

        /**
         * 获取整数值
         * @param path JSON Pointer路径
         * @return 整数值
         */
        public long getInt(String path) {
            checkClosed();
            return pointerGetInt(handle, path);
        }

        /**
         * 获取浮点数值
         * @param path JSON Pointer路径
         * @return 浮点数值
         */
        public double getDouble(String path) {
            checkClosed();
            return pointerGetDouble(handle, path);
        }

        /**
         * 设置字符串值
         * @param path JSON Pointer路径
         * @param value 字符串值
         */
        public void setString(String path, String value) {
            checkClosed();
            pointerSetString(handle, path, value);
        }

        /**
         * 设置整数值
         * @param path JSON Pointer路径
         * @param value 整数值
         */
        public void setInt(String path, long value) {
            checkClosed();
            pointerSetInt(handle, path, value);
        }

        /**
         * 设置浮点数值
         * @param path JSON Pointer路径
         * @param value 浮点数值
         */
        public void setDouble(String path, double value) {
            checkClosed();
            pointerSetDouble(handle, path, value);
        }

        private void checkClosed() {
            if (closed) {
                throw new IllegalStateException("CborDocument已关闭");
            }
        }

        @Override
        public void close() {
            if (!closed && handle != 0) {
                destroy(handle);
                handle = 0;
                closed = true;
            }
        }

        @Override
        protected void finalize() throws Throwable {
            close();
            super.finalize();
        }
    }

    // ========== 使用示例 ==========

    public static void main(String[] args) {
        System.out.println("CborObject Java JNI 示例\n");

        // 使用try-with-resources自动管理资源
        try (CborDocument doc = CborDocument.fromJson(
            "{\"name\":\"Alice\",\"age\":30,\"active\":true}"
        )) {
            if (doc == null) {
                System.err.println("✗ 解析JSON失败");
                return;
            }

            System.out.println("========== 1. 读取数据 ==========");
            String name = doc.getString("/name");
            long age = doc.getInt("/age");
            System.out.println("姓名: " + name);
            System.out.println("年龄: " + age);

            System.out.println("\n========== 2. 修改数据 ==========");
            doc.setInt("/age", 31);
            doc.setString("/email", "alice@example.com");
            System.out.println("✓ 修改年龄为31");
            System.out.println("✓ 添加邮箱");

            System.out.println("\n========== 3. 输出JSON ==========");
            String json = doc.toJson(true);
            System.out.println(json);

            System.out.println("\n========== 4. 保存文件 ==========");
            if (doc.toJsonFile("output_java.json", true)) {
                System.out.println("✓ 保存成功");
            }

        } catch (Exception e) {
            System.err.println("错误: " + e.getMessage());
            e.printStackTrace();
        }

        System.out.println("\n========== 示例完成 ==========");
        System.out.println("注意：CborDocument自动释放内存（try-with-resources）");
    }
}
