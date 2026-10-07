#include "CborDocument.h"
#include "cbor/cbor.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdint>

namespace {
// 大文件保护栏：当前 fromJsonFile/fromCborFile 是全量加载，
// 大文件支持（索引 + 延迟加载）按 CborObject/TODO-大文件支持.md 尚未实现。
// 在这些阈值上发警告，让调用方提前预期内存压力。
constexpr std::int64_t kSoftWarnBytes = 100ll * 1024 * 1024;   //  100 MB
constexpr std::int64_t kHardWarnBytes = 1024ll * 1024 * 1024;  // 1024 MB

void warnIfLarge(const std::string& filePath, std::int64_t sizeBytes, const char* api)
{
    if (sizeBytes <= 0) return;
    const double mb = static_cast<double>(sizeBytes) / (1024.0 * 1024.0);
    if (sizeBytes >= kHardWarnBytes) {
        // 估算内存峰值：源串 1× + DOM 树 ~1.5×（CBOR 树展开会膨胀）
        std::cerr << "[CborDocument][WARN] " << api << " loading "
                  << filePath << " (" << mb << " MB) — 全量加载，预计阻塞 "
                  << "数秒、峰值内存 ~" << (mb * 2.5) << " MB；"
                  << "大文件延迟加载尚未实现 (见 GPjson/CborObject/TODO-大文件支持.md)。\n";
    } else if (sizeBytes >= kSoftWarnBytes) {
        std::cerr << "[CborDocument][INFO] " << api << " loading "
                  << filePath << " (" << mb << " MB) — 全量加载，"
                  << "预计内存峰值 ~" << (mb * 2.5) << " MB；"
                  << "建议把大体积负载用 dataset.type=\"URI\" 引用而非 \"Binary\" 嵌入。\n";
    }
}

std::int64_t fileSize(const std::string& path)
{
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f.is_open()) return -1;
    return static_cast<std::int64_t>(f.tellg());
}
} // namespace

// ========== CborDocumentPrivate实现 ==========

class CborDocument::CborDocumentPrivate {
public:
    cbor_value_t *value;
    bool ownsValue;

    CborDocumentPrivate()
        : value(nullptr), ownsValue(false) {}

    CborDocumentPrivate(cbor_value_t *val, bool takeOwnership)
        : value(val), ownsValue(takeOwnership) {}

    ~CborDocumentPrivate() {
        if (ownsValue && value) {
            cbor_destroy(value);
        }
    }

    CborDocumentPrivate(const CborDocumentPrivate &other)
        : value(other.value ? cbor_duplicate(other.value) : nullptr)
        , ownsValue(true) {}
};

// ========== CborDocument构造函数 ==========

CborDocument::CborDocument()
    : d(std::shared_ptr<CborDocumentPrivate>(new CborDocumentPrivate())) {}

CborDocument::CborDocument(const CborObject &object)
    : d(std::shared_ptr<CborDocumentPrivate>(
          new CborDocumentPrivate(cbor_duplicate(object.rawValue()), true))) {}

CborDocument::CborDocument(const CborArray &array)
    : d(std::shared_ptr<CborDocumentPrivate>(
          new CborDocumentPrivate(cbor_duplicate(array.rawValue()), true))) {}

CborDocument::CborDocument(const CborDocument &other)
    : d(std::shared_ptr<CborDocumentPrivate>(new CborDocumentPrivate(*other.d))) {}

CborDocument::CborDocument(cbor_value_t *val, bool takeOwnership)
    : d(std::shared_ptr<CborDocumentPrivate>(new CborDocumentPrivate(val, takeOwnership))) {}

CborDocument::~CborDocument() {}

// ========== 赋值运算符 ==========

CborDocument &CborDocument::operator=(const CborDocument &other) {
    if (this != &other) {
        d = std::shared_ptr<CborDocumentPrivate>(new CborDocumentPrivate(*other.d));
    }
    return *this;
}

// ========== JSON互操作 ==========

CborDocument CborDocument::fromJson(const std::string &json, std::string *errorInfo) {
    cbor_value_t *val = cbor_json_loads(json.c_str(), json.length());

    if (!val) {
        if (errorInfo) {
            *errorInfo = "Failed to parse JSON string";
        }
        return CborDocument();
    }

    return CborDocument(val, true);
}

CborDocument CborDocument::fromJsonFile(const std::string &filePath, std::string *errorInfo) {
    warnIfLarge(filePath, fileSize(filePath), "fromJsonFile");

    std::ifstream file(filePath, std::ios::in);
    if (!file.is_open()) {
        if (errorInfo) {
            *errorInfo = "Failed to open file: " + filePath;
        }
        return CborDocument();
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return fromJson(buffer.str(), errorInfo);
}

std::string CborDocument::toJson(CborFormat format) const {
    if (!d->value) {
        return "null";
    }

    size_t length = 0;
    char *jsonStr = cbor_json_dumps(d->value, &length, format == Indented);

    if (!jsonStr) {
        return "null";
    }

    std::string result(jsonStr, length);
    free(jsonStr);

    return result;
}

bool CborDocument::toJsonFile(const std::string &filePath, CborFormat format) const {
    if (!d->value) {
        return false;
    }

    int result = cbor_json_dumpf(d->value, filePath.c_str(), format == Indented);
    return result == 0;
}

// ========== CBOR二进制格式互操作 ==========

CborDocument CborDocument::fromCbor(const std::string &cbor, std::string *errorInfo) {
    size_t consumed = cbor.length();  // 修复：必须传入数据长度
    cbor_value_t *val = cbor_loads(cbor.c_str(), &consumed);

    if (!val) {
        if (errorInfo) {
            *errorInfo = "Failed to parse CBOR binary data";
        }
        return CborDocument();
    }

    return CborDocument(val, true);
}

CborDocument CborDocument::fromCborFile(const std::string &filePath, std::string *errorInfo) {
    warnIfLarge(filePath, fileSize(filePath), "fromCborFile");

    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        if (errorInfo) {
            *errorInfo = "Failed to open file: " + filePath;
        }
        return CborDocument();
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return fromCbor(buffer.str(), errorInfo);
}

std::string CborDocument::toCbor() const {
    if (!d->value) {
        return std::string();
    }

    size_t length = 0;
    char *cborData = cbor_dumps(d->value, &length);

    if (!cborData) {
        return std::string();
    }

    std::string result(cborData, length);
    free(cborData);

    return result;
}

bool CborDocument::toCborFile(const std::string &filePath) const {
    if (!d->value) {
        return false;
    }

    std::string data = toCbor();
    if (data.empty()) {
        return false;
    }

    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    file.write(data.c_str(), data.length());
    file.close();

    return true;
}

// ========== 文档内容访问 ==========

bool CborDocument::isNull() const {
    return !d->value || cbor_is_null(d->value);
}

bool CborDocument::isEmpty() const {
    if (!d->value) {
        return true;
    }

    if (cbor_is_array(d->value) || cbor_is_map(d->value)) {
        return cbor_container_empty(d->value);
    }

    return isNull();
}

bool CborDocument::isArray() const {
    return d->value && cbor_is_array(d->value);
}

bool CborDocument::isObject() const {
    return d->value && cbor_is_map(d->value);
}

CborObject CborDocument::object() const {
    if (!d->value || !cbor_is_map(d->value)) {
        return CborObject();
    }
    return CborObject(d->value, false);
}

CborArray CborDocument::array() const {
    if (!d->value || !cbor_is_array(d->value)) {
        return CborArray();
    }
    return CborArray(d->value, false);
}

// ========== JSON Pointer支持 ==========

CborValue CborDocument::pointer(const std::string &path) const {
    if (!d->value) {
        return CborValue();
    }

    cbor_value_t *val = cbor_pointer_get(d->value, path.c_str());
    if (!val) {
        return CborValue();
    }

    return CborValue(val, false);
}

bool CborDocument::setPointer(const std::string &path, const CborValue &value) {
    if (!d->value) {
        return false;
    }

    cbor_value_t *newVal = cbor_duplicate(value.rawValue());
    if (!newVal) {
        return false;
    }

    cbor_value_t *result = cbor_pointer_set(d->value, path.c_str(), newVal);
    return result != nullptr;
}

// ========== 内部访问 ==========

cbor_value_t *CborDocument::rawValue() const {
    return d->value;
}
