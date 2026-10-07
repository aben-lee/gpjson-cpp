/**
 * @file cbor-wrapper.js
 * @brief Node.js ffi-napi包装器，调用CborObject C API
 *
 * 本模块通过ffi-napi调用CborObject的C API。
 * 需要先编译C库为共享库（DLL/SO/DYLIB）。
 *
 * 安装依赖：
 *   npm install ffi-napi ref-napi
 *
 * 使用方法：
 *   const { CborDocument } = require('./cbor-wrapper');
 *
 *   // 创建文档
 *   const doc = CborDocument.fromJson('{"name":"Alice","age":30}');
 *
 *   // 访问数据
 *   const name = doc.getString("/name");
 *   const age = doc.getInt("/age");
 *
 *   // 修改数据
 *   doc.setInt("/age", 31);
 *
 *   // 输出JSON
 *   console.log(doc.toJson(true));
 *
 *   // 释放资源
 *   doc.close();
 */

const ffi = require('ffi-napi');
const ref = require('ref-napi');
const path = require('path');
const os = require('os');

// 定义类型
const voidPtr = ref.refType(ref.types.void);
const sizeTPtr = ref.refType(ref.types.size_t);

// 根据平台确定库文件名
function getLibraryPath() {
    const platform = os.platform();
    let libName;

    if (platform === 'win32') {
        libName = 'CborObject.dll';
    } else if (platform === 'darwin') {
        libName = 'libCborObject.dylib';
    } else {
        libName = 'libCborObject.so';
    }

    // 在当前目录和cbor目录查找
    const searchPaths = [
        path.join(__dirname, libName),
        path.join(__dirname, '..', '..', 'cbor', libName),
    ];

    for (const libPath of searchPaths) {
        try {
            require('fs').accessSync(libPath);
            return libPath;
        } catch (e) {
            // 继续查找
        }
    }

    throw new Error(`找不到${libName}，请先编译C库`);
}

// 加载C库
const libCbor = ffi.Library(getLibraryPath(), {
    // cbor_json_loads(const char *src, int size) -> cbor_value_t *
    'cbor_json_loads': [voidPtr, ['string', 'int']],

    // cbor_json_dumps(cbor_value_t *val, size_t *length, bool pretty) -> char *
    'cbor_json_dumps': ['string', [voidPtr, sizeTPtr, 'bool']],

    // cbor_json_loadf(const char *path) -> cbor_value_t *
    'cbor_json_loadf': [voidPtr, ['string']],

    // cbor_json_dumpf(cbor_value_t *val, const char *path, bool pretty) -> int
    'cbor_json_dumpf': ['int', [voidPtr, 'string', 'bool']],

    // cbor_pointer_gets(cbor_value_t *val, const char *path) -> const char *
    'cbor_pointer_gets': ['string', [voidPtr, 'string']],

    // cbor_pointer_geti(cbor_value_t *val, const char *path) -> long long
    'cbor_pointer_geti': ['int64', [voidPtr, 'string']],

    // cbor_pointer_getf(cbor_value_t *val, const char *path) -> double
    'cbor_pointer_getf': ['double', [voidPtr, 'string']],

    // cbor_pointer_seti(cbor_value_t *val, const char *path, long long i) -> int
    'cbor_pointer_seti': ['int', [voidPtr, 'string', 'int64']],

    // cbor_pointer_sets(cbor_value_t *val, const char *path, const char *s) -> int
    'cbor_pointer_sets': ['int', [voidPtr, 'string', 'string']],

    // cbor_pointer_setf(cbor_value_t *val, const char *path, double d) -> int
    'cbor_pointer_setf': ['int', [voidPtr, 'string', 'double']],

    // cbor_pointer_setb(cbor_value_t *val, const char *path, bool b) -> int
    'cbor_pointer_setb': ['int', [voidPtr, 'string', 'bool']],

    // cbor_destroy(cbor_value_t *val)
    'cbor_destroy': ['void', [voidPtr]]
});

/**
 * CborDocument封装类，提供自动内存管理
 */
class CborDocument {
    /**
     * 创建CborDocument实例
     * @param {Buffer} handle - cbor_value_t指针
     * @private
     */
    constructor(handle) {
        if (!handle || handle.isNull()) {
            throw new Error('无效的CBOR对象句柄');
        }
        this._handle = handle;
        this._closed = false;
    }

    /**
     * 从JSON字符串解析
     * @param {string} jsonStr - JSON字符串
     * @returns {CborDocument} CborDocument对象
     * @throws {Error} 解析失败时抛出异常
     */
    static fromJson(jsonStr) {
        const handle = libCbor.cbor_json_loads(jsonStr, -1);
        if (!handle || handle.isNull()) {
            throw new Error('JSON解析失败');
        }
        return new CborDocument(handle);
    }

    /**
     * 从JSON文件加载
     * @param {string} filePath - JSON文件路径
     * @returns {CborDocument} CborDocument对象
     * @throws {Error} 加载失败时抛出异常
     */
    static fromJsonFile(filePath) {
        const handle = libCbor.cbor_json_loadf(filePath);
        if (!handle || handle.isNull()) {
            throw new Error(`无法加载JSON文件: ${filePath}`);
        }
        return new CborDocument(handle);
    }

    /**
     * 转换为JSON字符串
     * @param {boolean} pretty - 是否美化输出
     * @returns {string} JSON字符串
     * @throws {Error} 序列化失败时抛出异常
     */
    toJson(pretty = true) {
        this._checkClosed();
        const lengthPtr = ref.alloc(ref.types.size_t);
        const result = libCbor.cbor_json_dumps(this._handle, lengthPtr, pretty);
        if (!result) {
            throw new Error('JSON序列化失败');
        }
        return result;
    }

    /**
     * 保存为JSON文件
     * @param {string} filePath - JSON文件路径
     * @param {boolean} pretty - 是否美化输出
     * @returns {boolean} 成功返回true
     * @throws {Error} 保存失败时抛出异常
     */
    toJsonFile(filePath, pretty = true) {
        this._checkClosed();
        const result = libCbor.cbor_json_dumpf(this._handle, filePath, pretty);
        if (result !== 0) {
            throw new Error(`保存JSON文件失败: ${filePath}`);
        }
        return true;
    }

    /**
     * 获取字符串值
     * @param {string} path - JSON Pointer路径（如 "/name"）
     * @returns {string|null} 字符串值，失败返回null
     */
    getString(path) {
        this._checkClosed();
        return libCbor.cbor_pointer_gets(this._handle, path);
    }

    /**
     * 获取整数值
     * @param {string} path - JSON Pointer路径
     * @returns {number} 整数值
     */
    getInt(path) {
        this._checkClosed();
        return libCbor.cbor_pointer_geti(this._handle, path);
    }

    /**
     * 获取浮点数值
     * @param {string} path - JSON Pointer路径
     * @returns {number} 浮点数值
     */
    getFloat(path) {
        this._checkClosed();
        return libCbor.cbor_pointer_getf(this._handle, path);
    }

    /**
     * 设置字符串值
     * @param {string} path - JSON Pointer路径
     * @param {string} value - 字符串值
     */
    setString(path, value) {
        this._checkClosed();
        libCbor.cbor_pointer_sets(this._handle, path, value);
    }

    /**
     * 设置整数值
     * @param {string} path - JSON Pointer路径
     * @param {number} value - 整数值
     */
    setInt(path, value) {
        this._checkClosed();
        libCbor.cbor_pointer_seti(this._handle, path, value);
    }

    /**
     * 设置浮点数值
     * @param {string} path - JSON Pointer路径
     * @param {number} value - 浮点数值
     */
    setFloat(path, value) {
        this._checkClosed();
        libCbor.cbor_pointer_setf(this._handle, path, value);
    }

    /**
     * 设置布尔值
     * @param {string} path - JSON Pointer路径
     * @param {boolean} value - 布尔值
     */
    setBool(path, value) {
        this._checkClosed();
        libCbor.cbor_pointer_setb(this._handle, path, value);
    }

    /**
     * 检查对象是否已关闭
     * @private
     */
    _checkClosed() {
        if (this._closed) {
            throw new Error('CborDocument已关闭');
        }
    }

    /**
     * 释放CBOR对象
     */
    close() {
        if (!this._closed && this._handle) {
            libCbor.cbor_destroy(this._handle);
            this._handle = null;
            this._closed = true;
        }
    }
}

// ========== 使用示例 ==========

function main() {
    console.log('CborObject Node.js FFI 示例\n');

    try {
        // ========== 1. 从JSON字符串解析 ==========
        console.log('========== 1. 解析JSON ==========');
        const jsonStr = '{"name":"Alice","age":30,"active":true,"scores":[85,90,95]}';
        const doc = CborDocument.fromJson(jsonStr);
        console.log('✓ JSON解析成功');

        // ========== 2. 读取数据 ==========
        console.log('\n========== 2. 读取数据 ==========');
        const name = doc.getString('/name');
        const age = doc.getInt('/age');
        const firstScore = doc.getInt('/scores/0');

        console.log(`姓名: ${name}`);
        console.log(`年龄: ${age}`);
        console.log(`第一个成绩: ${firstScore}`);

        // ========== 3. 修改数据 ==========
        console.log('\n========== 3. 修改数据 ==========');
        doc.setInt('/age', 31);
        doc.setString('/email', 'alice@example.com');
        doc.setBool('/verified', true);
        console.log('✓ 修改年龄为31');
        console.log('✓ 添加邮箱和验证状态');

        // ========== 4. 输出JSON ==========
        console.log('\n========== 4. 输出JSON（美化格式） ==========');
        const output = doc.toJson(true);
        console.log(output);

        // ========== 5. 文件操作 ==========
        console.log('\n========== 5. 保存文件 ==========');
        doc.toJsonFile('output_node.json', true);
        console.log('✓ 保存成功: output_node.json');

        // 释放资源
        doc.close();
        console.log('✓ 已释放内存');

        // ========== 6. 从文件加载 ==========
        console.log('\n========== 6. 从文件加载 ==========');
        const loadedDoc = CborDocument.fromJsonFile('output_node.json');
        const loadedName = loadedDoc.getString('/name');
        console.log(`从文件加载: ${loadedName}`);
        loadedDoc.close();

        console.log('\n========== 示例完成 ==========');

    } catch (error) {
        console.error(`错误: ${error.message}`);
        console.error(error.stack);
    }
}

// 导出
module.exports = {
    CborDocument,
    libCbor  // 导出原始库以便高级使用
};

// 如果直接运行此文件
if (require.main === module) {
    main();
}
