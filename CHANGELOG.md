# Changelog

This project follows [Semantic Versioning](https://semver.org/).

## 1.0.0 — 2026-10-07

First public reference release of the C++ foundation used with GPJSON 4.x.

- **CborObject** — a Qt-style, dependency-free C++14 API over CBOR
  (RFC 7049/8949), JSON (RFC 7159) and JSON Pointer (RFC 6901), with file I/O
  and a full value-type system (null, bool, int, double, string, byte string,
  array, map, tag).
- **C core** (`CborObject/cbor/`) — public-domain CBOR/JSON C implementation
  (The Unlicense), enabling compact binary encoding roughly 30–50% smaller
  than equivalent JSON.
- **GeoJson** — standalone RFC 7946 geometry classes
  (Point / LineString / Polygon / Multi-geometries), no Qt dependency.
- **Examples** in C, C++, Python (ctypes), Java (JNI) and JavaScript (Node FFI).
- **Build & test** — CMake/CTest with three return-code-checked test drivers
  and GitHub Actions CI on Linux (GCC) and Windows (MSVC).
- **Specifications** — GPJSON wire-format specification v4.0 and the GPJSON
  4.1 orchestration protocol note under `docs/`.

The GPJSON 4.x Feature/DataSet orchestration, the IGDO state/intent profile
and the self-organizing Data Engine are provided in the sister repository
`igdo-engine-repro`. The v5.0 object layer is unpublished future work and is
not included in this release.
