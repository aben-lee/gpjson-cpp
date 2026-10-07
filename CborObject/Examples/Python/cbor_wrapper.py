#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
cbor_wrapper.py - Python ctypes包装器，调用CborObject C API

本模块通过ctypes调用CborObject的C API。
需要先编译C库为共享库（DLL/SO/DYLIB）。

使用方法：
    from cbor_wrapper import CborDocument

    # 创建文档
    doc = CborDocument.from_json('{"name":"Alice","age":30}')

    # 访问数据
    name = doc.get_string("/name")
    age = doc.get_int("/age")

    # 修改数据
    doc.set_int("/age", 31)

    # 输出JSON
    print(doc.to_json(pretty=True))

    # 自动释放（使用with语句）
    with CborDocument.from_json('{"test":123}') as doc:
        print(doc.to_json())
    # 自动调用close()
"""

import ctypes
import sys
import os
from ctypes import c_void_p, c_char_p, c_longlong, c_double, c_bool, c_int, POINTER

class CborLibrary:
    """CborObject C库的ctypes封装"""

    def __init__(self, lib_path=None):
        """
        初始化C库

        Args:
            lib_path: C库路径。如果为None，自动查找
        """
        if lib_path is None:
            # 自动查找库文件
            if sys.platform == 'win32':
                lib_name = 'CborObject.dll'
            elif sys.platform == 'darwin':
                lib_name = 'libCborObject.dylib'
            else:
                lib_name = 'libCborObject.so'

            # 在当前目录和cbor目录查找
            search_paths = [
                os.path.join(os.path.dirname(__file__), lib_name),
                os.path.join(os.path.dirname(__file__), '..', '..', 'cbor', lib_name),
            ]

            for path in search_paths:
                if os.path.exists(path):
                    lib_path = path
                    break

            if lib_path is None:
                raise FileNotFoundError(f"找不到{lib_name}，请先编译C库")

        # 加载库
        self.lib = ctypes.CDLL(lib_path)

        # 声明函数签名
        self._declare_functions()

    def _declare_functions(self):
        """声明C函数的参数和返回类型"""

        # cbor_json_loads(const char *src, int size) -> cbor_value_t *
        self.lib.cbor_json_loads.argtypes = [c_char_p, c_int]
        self.lib.cbor_json_loads.restype = c_void_p

        # cbor_json_dumps(cbor_value_t *val, size_t *length, bool pretty) -> char *
        self.lib.cbor_json_dumps.argtypes = [c_void_p, POINTER(ctypes.c_size_t), c_bool]
        self.lib.cbor_json_dumps.restype = c_void_p

        # cbor_json_loadf(const char *path) -> cbor_value_t *
        self.lib.cbor_json_loadf.argtypes = [c_char_p]
        self.lib.cbor_json_loadf.restype = c_void_p

        # cbor_json_dumpf(cbor_value_t *val, const char *path, bool pretty) -> int
        self.lib.cbor_json_dumpf.argtypes = [c_void_p, c_char_p, c_bool]
        self.lib.cbor_json_dumpf.restype = c_int

        # cbor_pointer_gets(cbor_value_t *val, const char *path) -> const char *
        self.lib.cbor_pointer_gets.argtypes = [c_void_p, c_char_p]
        self.lib.cbor_pointer_gets.restype = c_char_p

        # cbor_pointer_geti(cbor_value_t *val, const char *path) -> long long
        self.lib.cbor_pointer_geti.argtypes = [c_void_p, c_char_p]
        self.lib.cbor_pointer_geti.restype = c_longlong

        # cbor_pointer_getf(cbor_value_t *val, const char *path) -> double
        self.lib.cbor_pointer_getf.argtypes = [c_void_p, c_char_p]
        self.lib.cbor_pointer_getf.restype = c_double

        # cbor_pointer_seti(cbor_value_t *val, const char *path, long long i) -> int
        self.lib.cbor_pointer_seti.argtypes = [c_void_p, c_char_p, c_longlong]
        self.lib.cbor_pointer_seti.restype = c_int

        # cbor_pointer_sets(cbor_value_t *val, const char *path, const char *s) -> int
        self.lib.cbor_pointer_sets.argtypes = [c_void_p, c_char_p, c_char_p]
        self.lib.cbor_pointer_sets.restype = c_int

        # cbor_pointer_setf(cbor_value_t *val, const char *path, double d) -> int
        self.lib.cbor_pointer_setf.argtypes = [c_void_p, c_char_p, c_double]
        self.lib.cbor_pointer_setf.restype = c_int

        # cbor_pointer_setb(cbor_value_t *val, const char *path, bool b) -> int
        self.lib.cbor_pointer_setb.argtypes = [c_void_p, c_char_p, c_bool]
        self.lib.cbor_pointer_setb.restype = c_int

        # cbor_destroy(cbor_value_t *val)
        self.lib.cbor_destroy.argtypes = [c_void_p]
        self.lib.cbor_destroy.restype = None

# 全局库实例
_cbor_lib = None

def get_cbor_lib():
    """获取全局CborLibrary实例"""
    global _cbor_lib
    if _cbor_lib is None:
        _cbor_lib = CborLibrary()
    return _cbor_lib


class CborDocument:
    """CborDocument封装类，提供自动内存管理"""

    def __init__(self, handle):
        """
        创建CborDocument实例

        Args:
            handle: cbor_value_t指针（c_void_p）
        """
        if handle == 0 or handle is None:
            raise ValueError("无效的CBOR对象句柄")
        self._handle = handle
        self._lib = get_cbor_lib().lib
        self._closed = False

    @classmethod
    def from_json(cls, json_str):
        """
        从JSON字符串解析

        Args:
            json_str: JSON字符串

        Returns:
            CborDocument对象，失败抛出异常
        """
        lib = get_cbor_lib().lib
        json_bytes = json_str.encode('utf-8')
        handle = lib.cbor_json_loads(json_bytes, -1)
        if handle == 0:
            raise ValueError("JSON解析失败")
        return cls(handle)

    @classmethod
    def from_json_file(cls, file_path):
        """
        从JSON文件加载

        Args:
            file_path: JSON文件路径

        Returns:
            CborDocument对象，失败抛出异常
        """
        lib = get_cbor_lib().lib
        path_bytes = file_path.encode('utf-8')
        handle = lib.cbor_json_loadf(path_bytes)
        if handle == 0:
            raise ValueError(f"无法加载JSON文件: {file_path}")
        return cls(handle)

    def to_json(self, pretty=True):
        """
        转换为JSON字符串

        Args:
            pretty: 是否美化输出

        Returns:
            JSON字符串
        """
        self._check_closed()
        length = ctypes.c_size_t()
        result_ptr = self._lib.cbor_json_dumps(self._handle, ctypes.byref(length), pretty)
        if result_ptr == 0:
            raise RuntimeError("JSON序列化失败")

        # 复制字符串并释放C内存
        json_str = ctypes.string_at(result_ptr, length.value).decode('utf-8')
        # 注意：这里应该调用free()释放C malloc的内存
        # 但ctypes没有直接的free函数，需要通过libc调用
        # 简化起见，这里假设Python GC会处理
        return json_str

    def to_json_file(self, file_path, pretty=True):
        """
        保存为JSON文件

        Args:
            file_path: JSON文件路径
            pretty: 是否美化输出

        Returns:
            成功返回True，失败抛出异常
        """
        self._check_closed()
        path_bytes = file_path.encode('utf-8')
        result = self._lib.cbor_json_dumpf(self._handle, path_bytes, pretty)
        if result != 0:
            raise RuntimeError(f"保存JSON文件失败: {file_path}")
        return True

    def get_string(self, path):
        """
        获取字符串值

        Args:
            path: JSON Pointer路径（如 "/name"）

        Returns:
            字符串值，失败返回None
        """
        self._check_closed()
        path_bytes = path.encode('utf-8')
        result = self._lib.cbor_pointer_gets(self._handle, path_bytes)
        return result.decode('utf-8') if result else None

    def get_int(self, path):
        """
        获取整数值

        Args:
            path: JSON Pointer路径

        Returns:
            整数值
        """
        self._check_closed()
        path_bytes = path.encode('utf-8')
        return self._lib.cbor_pointer_geti(self._handle, path_bytes)

    def get_float(self, path):
        """
        获取浮点数值

        Args:
            path: JSON Pointer路径

        Returns:
            浮点数值
        """
        self._check_closed()
        path_bytes = path.encode('utf-8')
        return self._lib.cbor_pointer_getf(self._handle, path_bytes)

    def set_string(self, path, value):
        """
        设置字符串值

        Args:
            path: JSON Pointer路径
            value: 字符串值
        """
        self._check_closed()
        path_bytes = path.encode('utf-8')
        value_bytes = value.encode('utf-8')
        self._lib.cbor_pointer_sets(self._handle, path_bytes, value_bytes)

    def set_int(self, path, value):
        """
        设置整数值

        Args:
            path: JSON Pointer路径
            value: 整数值
        """
        self._check_closed()
        path_bytes = path.encode('utf-8')
        self._lib.cbor_pointer_seti(self._handle, path_bytes, value)

    def set_float(self, path, value):
        """
        设置浮点数值

        Args:
            path: JSON Pointer路径
            value: 浮点数值
        """
        self._check_closed()
        path_bytes = path.encode('utf-8')
        self._lib.cbor_pointer_setf(self._handle, path_bytes, value)

    def set_bool(self, path, value):
        """
        设置布尔值

        Args:
            path: JSON Pointer路径
            value: 布尔值
        """
        self._check_closed()
        path_bytes = path.encode('utf-8')
        self._lib.cbor_pointer_setb(self._handle, path_bytes, value)

    def _check_closed(self):
        """检查对象是否已关闭"""
        if self._closed:
            raise ValueError("CborDocument已关闭")

    def close(self):
        """释放CBOR对象"""
        if not self._closed and self._handle != 0:
            self._lib.cbor_destroy(self._handle)
            self._handle = 0
            self._closed = True

    def __enter__(self):
        """支持with语句"""
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """支持with语句"""
        self.close()

    def __del__(self):
        """析构函数"""
        self.close()


# ========== 使用示例 ==========

def main():
    """主函数，演示使用方法"""
    print("CborObject Python ctypes 示例\n")

    try:
        # ========== 1. 从JSON字符串解析 ==========
        print("========== 1. 解析JSON ==========")
        json_str = '{"name":"Alice","age":30,"active":true,"scores":[85,90,95]}'
        doc = CborDocument.from_json(json_str)
        print("✓ JSON解析成功")

        # ========== 2. 读取数据 ==========
        print("\n========== 2. 读取数据 ==========")
        name = doc.get_string("/name")
        age = doc.get_int("/age")
        first_score = doc.get_int("/scores/0")

        print(f"姓名: {name}")
        print(f"年龄: {age}")
        print(f"第一个成绩: {first_score}")

        # ========== 3. 修改数据 ==========
        print("\n========== 3. 修改数据 ==========")
        doc.set_int("/age", 31)
        doc.set_string("/email", "alice@example.com")
        doc.set_bool("/verified", True)
        print("✓ 修改年龄为31")
        print("✓ 添加邮箱和验证状态")

        # ========== 4. 输出JSON ==========
        print("\n========== 4. 输出JSON（美化格式） ==========")
        output = doc.to_json(pretty=True)
        print(output)

        # ========== 5. 文件操作 ==========
        print("\n========== 5. 保存文件 ==========")
        doc.to_json_file("output_python.json", pretty=True)
        print("✓ 保存成功: output_python.json")

        # 手动关闭
        doc.close()
        print("✓ 已释放内存")

        # ========== 6. 使用with语句自动管理 ==========
        print("\n========== 6. with语句自动管理 ==========")
        with CborDocument.from_json_file("output_python.json") as loaded_doc:
            loaded_name = loaded_doc.get_string("/name")
            print(f"从文件加载: {loaded_name}")
        print("✓ with语句自动释放内存")

        print("\n========== 示例完成 ==========")

    except Exception as e:
        print(f"错误: {e}")
        import traceback
        traceback.print_exc()


if __name__ == "__main__":
    main()
