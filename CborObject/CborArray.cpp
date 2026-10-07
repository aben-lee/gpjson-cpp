#include "CborArray.h"
#include "cbor/cbor.h"

// ========== CborArrayPrivate实现 ==========

class CborArray::CborArrayPrivate {
public:
    cbor_value_t *value;
    bool ownsValue;

    CborArrayPrivate()
        : value(cbor_init_array()), ownsValue(true) {}

    CborArrayPrivate(cbor_value_t *val, bool takeOwnership)
        : value(val), ownsValue(takeOwnership) {}

    ~CborArrayPrivate() {
        if (ownsValue && value) {
            cbor_destroy(value);
        }
    }

    CborArrayPrivate(const CborArrayPrivate &other)
        : value(other.value ? cbor_duplicate(other.value) : cbor_init_array())
        , ownsValue(true) {}
};

// ========== CborArray构造函数 ==========

CborArray::CborArray()
    : d(std::shared_ptr<CborArrayPrivate>(new CborArrayPrivate())) {}

CborArray::CborArray(const CborArray &other)
    : d(std::shared_ptr<CborArrayPrivate>(new CborArrayPrivate(*other.d))) {}

CborArray::CborArray(cbor_value_t *val, bool takeOwnership)
    : d(std::shared_ptr<CborArrayPrivate>(new CborArrayPrivate(val, takeOwnership))) {}

CborArray::~CborArray() {}

// ========== 赋值运算符 ==========

CborArray &CborArray::operator=(const CborArray &other) {
    if (this != &other) {
        d = std::shared_ptr<CborArrayPrivate>(new CborArrayPrivate(*other.d));
    }
    return *this;
}

// ========== 比较运算符 ==========

bool CborArray::operator==(const CborArray &other) const {
    if (!d->value || !other.d->value) {
        return d->value == other.d->value;
    }

    int sz = size();
    if (sz != other.size()) {
        return false;
    }

    for (int i = 0; i < sz; ++i) {
        if (at(i) != other.at(i)) {
            return false;
        }
    }

    return true;
}

bool CborArray::operator!=(const CborArray &other) const {
    return !(*this == other);
}

// ========== 容量查询 ==========

int CborArray::size() const {
    if (!d->value) {
        return 0;
    }
    return cbor_container_size(d->value);
}

bool CborArray::isEmpty() const {
    return size() == 0;
}

// ========== 元素访问 ==========

CborValue CborArray::at(int i) const {
    if (!d->value || i < 0 || i >= size()) {
        return CborValue();
    }

    // 遍历到第i个元素
    cbor_value_t *elem = cbor_container_first(d->value);
    for (int idx = 0; idx < i && elem; ++idx) {
        elem = cbor_container_next(d->value, elem);
    }

    if (!elem) {
        return CborValue();
    }

    return CborValue(elem, false);
}

CborValue CborArray::first() const {
    return at(0);
}

CborValue CborArray::last() const {
    return at(size() - 1);
}

CborValue CborArray::operator[](int i) const {
    return at(i);
}

// ========== 元素修改 ==========

void CborArray::append(const CborValue &value) {
    if (!d->value) {
        return;
    }

    cbor_value_t *newVal = cbor_duplicate(value.rawValue());
    if (newVal) {
        cbor_container_insert_tail(d->value, newVal);
    }
}

void CborArray::prepend(const CborValue &value) {
    if (!d->value) {
        return;
    }

    cbor_value_t *newVal = cbor_duplicate(value.rawValue());
    if (newVal) {
        cbor_container_insert_head(d->value, newVal);
    }
}

void CborArray::insert(int i, const CborValue &value) {
    if (!d->value || i < 0) {
        return;
    }

    if (i == 0) {
        prepend(value);
        return;
    }

    if (i >= size()) {
        append(value);
        return;
    }

    // 找到第i-1个元素
    cbor_value_t *elem = cbor_container_first(d->value);
    for (int idx = 0; idx < i - 1 && elem; ++idx) {
        elem = cbor_container_next(d->value, elem);
    }

    if (elem) {
        cbor_value_t *newVal = cbor_duplicate(value.rawValue());
        if (newVal) {
            cbor_container_insert_after(d->value, elem, newVal);
        }
    }
}

void CborArray::removeAt(int i) {
    if (!d->value || i < 0 || i >= size()) {
        return;
    }

    // 找到第i个元素
    cbor_value_t *elem = cbor_container_first(d->value);
    for (int idx = 0; idx < i && elem; ++idx) {
        elem = cbor_container_next(d->value, elem);
    }

    if (elem) {
        cbor_value_t *removed = cbor_container_remove(d->value, elem);
        if (removed) {
            cbor_destroy(removed);
        }
    }
}

void CborArray::removeFirst() {
    removeAt(0);
}

void CborArray::removeLast() {
    removeAt(size() - 1);
}

void CborArray::clear() {
    if (d->value) {
        cbor_container_clear(d->value);
    }
}

// ========== 搜索 ==========

bool CborArray::contains(const CborValue &value) const {
    int sz = size();
    for (int i = 0; i < sz; ++i) {
        if (at(i) == value) {
            return true;
        }
    }
    return false;
}

// ========== 转换 ==========

std::vector<CborValue> CborArray::toVector() const {
    std::vector<CborValue> result;
    int sz = size();
    result.reserve(sz);

    for (int i = 0; i < sz; ++i) {
        result.push_back(at(i));
    }

    return result;
}

// ========== 内部访问 ==========

cbor_value_t *CborArray::rawValue() const {
    return d->value;
}
