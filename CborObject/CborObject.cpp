#include "CborObject.h"
#include "cbor/cbor.h"

// ========== CborObjectPrivate实现 ==========

class CborObject::CborObjectPrivate {
public:
    cbor_value_t *value;
    bool ownsValue;

    CborObjectPrivate()
        : value(cbor_init_map()), ownsValue(true) {}

    CborObjectPrivate(cbor_value_t *val, bool takeOwnership)
        : value(val), ownsValue(takeOwnership) {}

    ~CborObjectPrivate() {
        if (ownsValue && value) {
            cbor_destroy(value);
        }
    }

    CborObjectPrivate(const CborObjectPrivate &other)
        : value(other.value ? cbor_duplicate(other.value) : cbor_init_map())
        , ownsValue(true) {}
};

// ========== CborObject构造函数 ==========

CborObject::CborObject()
    : d(std::shared_ptr<CborObjectPrivate>(new CborObjectPrivate())) {}

CborObject::CborObject(const CborObject &other)
    : d(std::shared_ptr<CborObjectPrivate>(new CborObjectPrivate(*other.d))) {}

CborObject::CborObject(cbor_value_t *val, bool takeOwnership)
    : d(std::shared_ptr<CborObjectPrivate>(new CborObjectPrivate(val, takeOwnership))) {}

CborObject::~CborObject() {}

// ========== 赋值运算符 ==========

CborObject &CborObject::operator=(const CborObject &other) {
    if (this != &other) {
        d = std::shared_ptr<CborObjectPrivate>(new CborObjectPrivate(*other.d));
    }
    return *this;
}

// ========== 比较运算符 ==========

bool CborObject::operator==(const CborObject &other) const {
    if (!d->value || !other.d->value) {
        return d->value == other.d->value;
    }

    if (size() != other.size()) {
        return false;
    }

    std::vector<std::string> k = keys();
    for (const auto &key : k) {
        if (!other.contains(key)) {
            return false;
        }
        if (value(key) != other.value(key)) {
            return false;
        }
    }

    return true;
}

bool CborObject::operator!=(const CborObject &other) const {
    return !(*this == other);
}

// ========== 容量查询 ==========

std::vector<std::string> CborObject::keys() const {
    std::vector<std::string> result;

    if (!d->value) {
        return result;
    }

    // 遍历map中的所有pair
    cbor_value_t *pair = cbor_container_first(d->value);
    while (pair) {
        cbor_value_t *key = cbor_pair_key(pair);
        if (key && cbor_is_string(key)) {
            const char *str = cbor_string(key);
            int size = cbor_string_size(key);
            if (str && size > 0) {
                result.push_back(std::string(str, size));
            }
        }
        pair = cbor_container_next(d->value, pair);
    }

    return result;
}

int CborObject::size() const {
    if (!d->value) {
        return 0;
    }
    return cbor_container_size(d->value);
}

bool CborObject::isEmpty() const {
    return size() == 0;
}

// ========== 元素访问 ==========

CborValue CborObject::value(const std::string &key) const {
    if (!d->value) {
        return CborValue();
    }

    // 使用JSON Pointer查找（更高效）
    std::string path = "/" + key;
    cbor_value_t *val = cbor_pointer_get(d->value, path.c_str());

    if (!val) {
        return CborValue();
    }

    return CborValue(val, false);
}

CborValue CborObject::operator[](const std::string &key) const {
    return value(key);
}

// ========== 查询 ==========

bool CborObject::contains(const std::string &key) const {
    if (!d->value) {
        return false;
    }

    std::string path = "/" + key;
    cbor_value_t *val = cbor_pointer_get(d->value, path.c_str());

    return val != nullptr;
}

// ========== 元素修改 ==========

void CborObject::insert(const std::string &key, const CborValue &value) {
    if (!d->value) {
        return;
    }

    // 创建key的CBOR值
    cbor_value_t *keyVal = cbor_init_string(key.c_str(), key.length());
    if (!keyVal) {
        return;
    }

    // 复制value
    cbor_value_t *valDup = cbor_duplicate(value.rawValue());
    if (!valDup) {
        cbor_destroy(keyVal);
        return;
    }

    // 创建pair
    cbor_value_t *pair = cbor_init_pair(keyVal, valDup);
    if (!pair) {
        cbor_destroy(keyVal);
        cbor_destroy(valDup);
        return;
    }

    // 检查key是否已存在，如果存在则先删除
    if (contains(key)) {
        remove(key);
    }

    // 插入新pair
    cbor_container_insert_tail(d->value, pair);
}

void CborObject::remove(const std::string &key) {
    if (!d->value) {
        return;
    }

    // 遍历找到对应的pair
    cbor_value_t *pair = cbor_container_first(d->value);
    while (pair) {
        cbor_value_t *pairKey = cbor_pair_key(pair);
        if (pairKey && cbor_is_string(pairKey)) {
            const char *str = cbor_string(pairKey);
            int size = cbor_string_size(pairKey);
            if (str && size == static_cast<int>(key.length()) &&
                std::string(str, size) == key) {
                cbor_value_t *removed = cbor_container_remove(d->value, pair);
                if (removed) {
                    cbor_destroy(removed);
                }
                return;
            }
        }
        pair = cbor_container_next(d->value, pair);
    }
}

void CborObject::clear() {
    if (d->value) {
        cbor_container_clear(d->value);
    }
}

CborValue CborObject::take(const std::string &key) {
    CborValue val = value(key);
    remove(key);
    return val;
}

// ========== 内部访问 ==========

cbor_value_t *CborObject::rawValue() const {
    return d->value;
}
