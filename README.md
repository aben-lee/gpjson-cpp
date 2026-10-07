# gpjson-cpp

[![build-and-test](https://github.com/aben-lee/gpjson-cpp/actions/workflows/ci.yml/badge.svg)](https://github.com/aben-lee/gpjson-cpp/actions/workflows/ci.yml)
[![License](https://img.shields.io/badge/License-BSD--3--Clause-blue.svg)](LICENSE)

A small, **Qt-free C++14/C11 foundation for GPJSON**: a Qt-style
CBOR/JSON object layer (`CborObject`) built on a public-domain C CBOR core,
together with standalone **RFC 7946 GeoJSON geometry** (`GeoJson`). It builds
with plain CMake and is meant to be easy to embed and to call from other
languages.

[GPJSON] is a GeoJSON-compatible representation for geophysical sensor data
that adds typed **DataSet** payloads (Sheet / Matrix / Binary / URI) and a
compact **CBOR** binary encoding (typically 30–50% smaller than the equivalent
JSON) for streaming and IoT-friendly transport.

## What is in this repository

| Path | Contents |
|---|---|
| `CborObject/` | Dependency-free C++14 wrapper (`CborValue`, `CborArray`, `CborObject`, `CborDocument`) for CBOR/JSON/JSON-Pointer |
| `CborObject/cbor/` | Public-domain C CBOR/JSON core (The Unlicense) |
| `GeoJson/` | RFC 7946 geometry: Point, LineString, Polygon, Multi-geometries |
| `CborObject/Examples/` | C, C++, Python (ctypes), Java (JNI), JavaScript (Node FFI) snippets |
| `tests/` | Three return-code-checked test drivers (CMake/CTest) |
| `docs/` | GPJSON wire-format specification v4.0 and the GPJSON 4.1 protocol note |

## Scope and relationship to the paper

This repository is the **C++ data-layer foundation** used with GPJSON 4.x —
compact binary (CBOR) serialization and GeoJSON-compatible geometry.

- The GPJSON 4.x **Feature/DataSet** model, the **IGDO** state/intent profile,
  the self-organizing **Data Engine**, the Node Knowledge Graph and the
  microseismic/InSAR experiments are provided, as runnable Python, in the
  sister repository **[igdo-engine-repro](https://github.com/aben-lee/igdo-engine-repro)**.
- The formal GPJSON 4.1 orchestration semantics are documented in
  [`docs/GPJSON-4.1-Protocol-Specification.md`](docs/GPJSON-4.1-Protocol-Specification.md);
  the v4.0 wire format is in
  [`docs/GPJSON-Format-Specification-v4.0.md`](docs/GPJSON-Format-Specification-v4.0.md).
- A newer **v5.0** object layer (extended profiles, a v4→v5 migration and a
  Qt bridge) is unpublished future work and is **not included** here.

## Build and test (no Qt required)

Requirements: CMake ≥ 3.10 and a C11/C++14 compiler (GCC, Clang or MSVC).

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build --output-on-failure        # add: -C Release on Windows
```

The CTest suite runs:

- `cborobject` — full CborObject API (types, arrays, maps, documents, JSON↔CBOR)
- `cbor_api_verify` — the public C CBOR/JSON core
- `geojson` — RFC 7946 geometry construction and (de)serialization

The default build covers the two libraries and the three test drivers. The
snippets under [`CborObject/Examples/`](CborObject/Examples) are reference
code; the pure-C examples can also be configured standalone with
`cmake -S CborObject/Examples/C -B build-c`.

## Examples

See [`CborObject/Examples/`](CborObject/Examples/README.md) for embedding in
C/C++ and for calling the core from Python (ctypes), Java (JNI) and
JavaScript (Node FFI).

## License

BSD 3-Clause License for the project's own code. The bundled C core is under
The Unlicense (public domain). See
[THIRD-PARTY-NOTICES.md](THIRD-PARTY-NOTICES.md). No Qt source code is
bundled.

## Citation

Citation metadata is maintained in [`CITATION.cff`](CITATION.cff). If you use
this software, please cite it together with the companion article:

> X. Li, Z. Luo, X. Yu, S. Cui, S. Jin, *Autonomous Geophysical Monitoring for
> Infrastructure Digital Twins with Intelligent Geo-Data and Self-organizing
> Pipelines*, IEEE Geoscience and Remote Sensing Magazine (2026).
