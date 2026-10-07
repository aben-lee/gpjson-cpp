# Third-Party Notices

This project (**gpjson-cpp**) is distributed under the BSD 3-Clause License
(see `LICENSE`). It bundles or derives from the following third-party components:

## 1. C CBOR/JSON core — `CborObject/cbor/`  (The Unlicense)

The C implementation under `CborObject/cbor/` (CBOR encode/decode, JSON and
JSON Pointer helpers: `cbor.c/.h`, `json.c`, `pointer.c` and supporting
headers) is released under **The Unlicense** (public-domain dedication); see
`CborObject/cbor/LICENSE` and <https://unlicense.org>.

The C++ wrapper in `CborObject/` (`CborValue`, `CborArray`, `CborObject`,
`CborDocument`) is the project's own code released under the BSD 3-Clause
License and links against this public-domain C core.

> Packaging note: the original working-tree `cbor/README.md` was a leftover
> upstream template describing an unrelated "Base64 Simple" project. It has
> been removed in this release to avoid attribution confusion; the license
> file and source headers are retained.

## 2. GeoJSON — IETF RFC 7946

The geometry classes under `GeoJson/` implement the geometry object model
defined by *RFC 7946 — The GeoJSON Format* (Point, LineString, Polygon and
their Multi counterparts). GeoJSON is an open IETF standard; this code is an
independent implementation released under the BSD 3-Clause License.

## 3. What is intentionally NOT bundled

- No Qt source code is bundled. An optional Qt-derived JSON drop-in that
  existed in the internal working tree (`CborObject/json/`, Qt/LGPL) and the
  Qt-specific v5 bridge/header are **excluded** from this public release.
- The unpublished GPJSON v5.0 object layer (semantic/data-flow/stream/
  provenance/signature profiles, the v4→v5 migration and the Qt bridge) is
  future work and is not part of this release.

The C, C++, Python (ctypes), Java (JNI) and JavaScript (Node FFI) snippets
under `CborObject/Examples/` call only the public-domain C core and the
BSD-licensed C++ API.
