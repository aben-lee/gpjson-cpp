#include "CborValue.h"
#include "CborArray.h"
#include "CborObject.h"
#include <cstring>

// ========== CborValuePrivate实现（PIMPL模式） ==========

class CborValue::CborValuePrivate {
public:
    cbor_value_t *value;
    bool ownsValue;

    CborValuePrivate() : value(nullptr), ownsValue(false) {}

    CborValuePrivate(cbor_value_t *val, bool takeOwnership)
        : value(val), ownsValue(takeOwnership) {}

    ~CborValuePrivate() {
        if (ownsValue && value) {
            cbor_destroy(value);
        }
    }

    CborValuePrivate(const CborValuePrivate &other)
        : value(other.value ? cbor_duplicate(other.value) : nullptr)
        , ownsValue(true) {}
};

// ========== CborValue构造函数 ==========

CborValue::CborValue()
    : d(std::shared_ptr<CborValuePrivate>(new CborValuePrivate(cbor_init_null(), true))) {}

CborValue::CborValue(const CborValue &other)
    : d(std::shared_ptr<CborValuePrivate>(new CborValuePrivate(*other.d))) {}

CborValue::CborValue(bool b)
    : d(std::shared_ptr<CborValuePrivate>(new CborValuePrivate(cbor_init_boolean(b), true))) {}

CborValue::CborValue(int i)
    : d(std::shared_ptr<CborValuePrivate>(new CborValuePrivate(cbor_init_integer(i), true))) {}

CborValue::CborValue(long long i)
    : d(std::shared_ptr<CborValuePrivate>(new CborValuePrivate(cbor_init_integer(i), true))) {}

CborValue::CborValue(double dbl)
    : d(std::shared_ptr<CborValuePrivate>(new CborValuePrivate(cbor_init_double(dbl), true))) {}

CborValue::CborValue(const std::string &s)
    : d(std::shared_ptr<CborValuePrivate>(
          new CborValuePrivate(cbor_init_string(s.c_str(), s.length()), true))) {}

CborValue::CborValue(const char *s)
    : d(std::shared_ptr<CborValuePrivate>(
          new CborValuePrivate(cbor_init_string(s, s ? strlen(s) : 0), true))) {}

CborValue::CborValue(const CborArray &arr) {
    cbor_value_t *arrVal = arr.rawValue();
    d = std::shared_ptr<CborValuePrivate>(new CborValuePrivate(arrVal ? cbor_duplicate(arrVal) : cbor_init_array(), true));
}

CborValue::CborValue(const CborObject &obj) {
    cbor_value_t *objVal = obj.rawValue();
    d = std::shared_ptr<CborValuePrivate>(new CborValuePrivate(objVal ? cbor_duplicate(objVal) : cbor_init_map(), true));
}

CborValue::CborValue(cbor_value_t *val, bool takeOwnership)
    : d(std::shared_ptr<CborValuePrivate>(new CborValuePrivate(val, takeOwnership))) {}

CborValue::~CborValue() {}

// ========== 赋值运算符 ==========

CborValue &CborValue::operator=(const CborValue &other) {
    if (this != &other) {
        d = std::shared_ptr<CborValuePrivate>(new CborValuePrivate(*other.d));
    }
    return *this;
}

// ========== 比较运算符 ==========

bool CborValue::operator==(const CborValue &other) const {
    if (!d->value || !other.d->value) {
        return d->value == other.d->value;
    }

    // 类型必须相同
    if (type() != other.type()) {
        return false;
    }

    // 根据类型进行比较
    switch (type()) {
        case Null:
            return true;
        case Bool:
            return toBool() == other.toBool();
        case Integer:
            return toLongLong() == other.toLongLong();
        case Double:
            return toDouble() == other.toDouble();
        case String:
        case ByteString:
            return toString() == other.toString();
        case Array:
            return toArray() == other.toArray();
        case Object:
            return toObject() == other.toObject();
        default:
            return false;
    }
}

bool CborValue::operator!=(const CborValue &other) const {
    return !(*this == other);
}

// ========== 类型查询 ==========

CborValue::Type CborValue::type() const {
    if (!d->value) {
        return Undefined;
    }

    if (cbor_is_null(d->value)) return Null;
    if (cbor_is_boolean(d->value)) return Bool;
    if (cbor_is_integer(d->value)) return Integer;
    if (cbor_is_double(d->value)) return Double;
    if (cbor_is_string(d->value)) return String;
    if (cbor_is_bytestring(d->value)) return ByteString;
    if (cbor_is_array(d->value)) return Array;
    if (cbor_is_map(d->value)) return Object;
    if (cbor_is_tag(d->value)) return Tag;

    return Undefined;
}

bool CborValue::isNull() const {
    return d->value && cbor_is_null(d->value);
}

bool CborValue::isBool() const {
    return d->value && cbor_is_boolean(d->value);
}

bool CborValue::isInteger() const {
    return d->value && cbor_is_integer(d->value);
}

bool CborValue::isDouble() const {
    return d->value && cbor_is_double(d->value);
}

bool CborValue::isString() const {
    return d->value && cbor_is_string(d->value);
}

bool CborValue::isByteString() const {
    return d->value && cbor_is_bytestring(d->value);
}

bool CborValue::isArray() const {
    return d->value && cbor_is_array(d->value);
}

bool CborValue::isObject() const {
    return d->value && cbor_is_map(d->value);
}

bool CborValue::isTag() const {
    return d->value && cbor_is_tag(d->value);
}

// ========== 类型转换 ==========

bool CborValue::toBool(bool defaultValue) const {
    if (!d->value || !cbor_is_boolean(d->value)) {
        return defaultValue;
    }
    return cbor_boolean(d->value);
}

int CborValue::toInt(int defaultValue) const {
    if (!d->value || !cbor_is_integer(d->value)) {
        return defaultValue;
    }
    return static_cast<int>(cbor_integer(d->value));
}

long long CborValue::toLongLong(long long defaultValue) const {
    if (!d->value || !cbor_is_integer(d->value)) {
        return defaultValue;
    }
    return cbor_integer(d->value);
}

double CborValue::toDouble(double defaultValue) const {
    if (!d->value) {
        return defaultValue;
    }
    if (cbor_is_double(d->value)) {
        return cbor_real(d->value);
    }
    if (cbor_is_integer(d->value)) {
        return static_cast<double>(cbor_integer(d->value));
    }
    return defaultValue;
}

std::string CborValue::toString(const std::string &defaultValue) const {
    if (!d->value) {
        return defaultValue;
    }

    if (cbor_is_string(d->value) || cbor_is_bytestring(d->value)) {
        const char *str = cbor_string(d->value);
        int size = cbor_string_size(d->value);
        if (str && size > 0) {
            return std::string(str, size);
        }
    }

    return defaultValue;
}

CborArray CborValue::toArray() const {
    if (!d->value || !cbor_is_array(d->value)) {
        return CborArray();
    }
    return CborArray(d->value, false);
}

CborObject CborValue::toObject() const {
    if (!d->value || !cbor_is_map(d->value)) {
        return CborObject();
    }
    return CborObject(d->value, false);
}

// ========== 内部访问 ==========

cbor_value_t *CborValue::rawValue() const {
    return d->value;
}
