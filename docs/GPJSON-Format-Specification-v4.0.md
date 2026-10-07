# GPJSON Format Specification v4.0

**Network Working Group**
**Request for Comments: GPJSON-v4.0**
**Category: Standards Track**
**Date: January 2026**

---

## Status of This Document

This document specifies the GPJSON (Geophysics JSON) Format Specification version 4.0, an extension of GeoJSON (RFC 7946) for geophysical and IoT sensor data interchange.

This is version 4.0 of the GPJSON specification.

---

## Abstract

GPJSON is a format for encoding geophysical data, sensor measurements, and multi-dimensional scientific datasets in JSON format. It extends GeoJSON (RFC 7946) by adding a "dataset" member to Feature objects, enabling rich scientific data representation while maintaining full backward compatibility with GeoJSON.

GPJSON supports four DataSet types (Sheet, Matrix, Binary, URI) and optional CBOR (RFC 7049) binary serialization for efficient data transmission.

---

## Copyright Notice

Copyright (c) 2026. All rights reserved.

---

## Table of Contents

1. [Introduction](#1-introduction)
2. [Conventions Used in This Document](#2-conventions-used-in-this-document)
3. [GPJSON Objects](#3-gpjson-objects)
4. [DataSet Object](#4-dataset-object)
5. [DataSet Types](#5-dataset-types)
6. [CBOR Serialization](#6-cbor-serialization)
7. [Data Validation](#7-data-validation)
8. [JSON Schema](#8-json-schema)
9. [Versioning](#9-versioning)
10. [Examples](#10-examples)
11. [Security Considerations](#11-security-considerations)
12. [IANA Considerations](#12-iana-considerations)
13. [References](#13-references)

---

## 1. Introduction

### 1.1 Purpose

GPJSON (Geophysics JSON) is designed to represent:
- Geophysical survey data (gravity, magnetic, seismic)
- IoT sensor measurements (temperature, humidity, pressure)
- Multi-dimensional scientific datasets
- Time-series data with spatial correlation

### 1.2 Design Philosophy

**Separation of Concerns**:
- **Geometry** = WHERE - Spatial location (GeoJSON standard)
- **DataSet** = WHEN + HOW - Time-series and multi-dimensional data (GPJSON extension)
- **Properties** = WHAT - Metadata and attributes (GeoJSON standard)

### 1.3 Relationship to GeoJSON

GPJSON is a **superset** of GeoJSON:
- All valid GeoJSON documents are valid GPJSON documents
- GPJSON extends Feature objects with an optional "dataset" member
- GPJSON parsers MUST correctly handle pure GeoJSON data
- GeoJSON parsers SHOULD ignore the "dataset" member (forward compatibility)

### 1.4 Key Features

- ✅ **GeoJSON Compatible**: 100% backward compatible with RFC 7946
- ✅ **Four DataSet Types**: Sheet, Matrix, Binary, URI
- ✅ **CBOR Support**: Binary serialization (30-50% size reduction)
- ✅ **JSON Schema**: Formal validation specification
- ✅ **Data Validation**: Comprehensive constraint rules
- ✅ **Simplified Design**: Removed redundant Sequence type from v3.0

---

## 2. Conventions Used in This Document

The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT", "SHOULD", "SHOULD NOT", "RECOMMENDED", "MAY", and "OPTIONAL" in this document are to be interpreted as described in RFC 2119.

### 2.1 Terminology

- **Feature**: A GeoJSON Feature object, optionally extended with a "dataset" member
- **DataSet**: A GPJSON-specific object containing multi-dimensional data
- **CBOR**: Concise Binary Object Representation (RFC 7049)
- **dimensions**: Array of column/dimension labels for data interpretation

### 2.2 JSON Notation

Examples use standard JSON notation with the following conventions:
- `//` denotes comments (not part of JSON)
- `...` indicates omitted content
- Indentation is for readability only

---

## 3. GPJSON Objects

### 3.1 Feature Object

A GPJSON Feature extends GeoJSON Feature (RFC 7946) with an optional "dataset" member.

**Structure**:
```json
{
  "type": "Feature",
  "geometry": { /* GeoJSON Geometry or null */ },
  "dataset": { /* GPJSON DataSet or omitted */ },
  "properties": { /* Metadata object or null */ }
}
```

### 3.2 Required Members

A Feature object MUST have:
- `"type"`: MUST be the string "Feature"

A Feature object MUST have at least ONE of:
- `"geometry"`: A GeoJSON Geometry object or null
- `"dataset"`: A GPJSON DataSet object

### 3.3 Optional Members

- `"id"`: String or number identifying the feature
- `"properties"`: Object containing feature metadata
- `"bbox"`: Bounding box array (GeoJSON standard)

### 3.4 Validity Matrix

| Scenario | geometry | dataset | Valid? | Use Case |
|----------|----------|---------|--------|----------|
| Pure GeoJSON | Present | Absent | ✅ Yes | Standard spatial data |
| Pure Sensor Data | Absent | Present | ✅ Yes | IoT sensors without location |
| Complete GPJSON | Present | Present | ✅ Yes | Geophysical surveys |
| Empty Feature | Absent | Absent | ❌ No | Invalid - must have one |

### 3.5 FeatureCollection Object

A GPJSON FeatureCollection extends GeoJSON FeatureCollection.

**Structure**:
```json
{
  "type": "FeatureCollection",
  "version": "4.0",
  "features": [ /* Array of Feature objects */ ]
}
```

**New in v4.0**: The `"version"` member is RECOMMENDED for FeatureCollection objects.

---

## 4. DataSet Object

### 4.1 Definition

A DataSet object contains multi-dimensional scientific or sensor data.

**Structure**:
```json
{
  "type": "<DataSetType>",
  "dimensions": [ /* Optional dimension labels */ ],
  "source": /* Type-specific data source */
}
```

### 4.2 Required Members

- `"type"`: MUST be one of: "Sheet", "Matrix", "Binary", "URI"
- `"source"`: MUST be present and type-appropriate

### 4.3 Optional Members

- `"dimensions"`: Array of strings or a single string describing data columns/dimensions
  - RECOMMENDED for self-documenting data
  - Format: `"Name/Unit"` or `"Name"` (e.g., `"Temperature/Celsius"`)

### 4.4 DataSet Type Summary

| Type | source Type | Use Case | Example |
|------|------------|----------|---------|
| **Sheet** | Array (2D) | Tabular data, CSV-like | Sensor logs, measurements |
| **Matrix** | Array (multi-D) | Numerical matrices | Images, grids, tensors |
| **Binary** | String (base64) | Binary files | Instrument data, NetCDF |
| **URI** | String (URL) | External data | Remote files, cloud storage |

**Note**: Sequence type from v3.0 has been removed due to redundancy (see Section 13.2).

---

## 5. DataSet Types

### 5.1 Sheet Type

**Purpose**: Tabular data with labeled columns (similar to CSV or spreadsheet).

**Structure**:
```json
{
  "type": "Sheet",
  "dimensions": ["Time/s", "Temperature/C", "Humidity/%"],
  "source": [
    [0.0, 22.5, 65.2],
    [1.0, 22.6, 65.1],
    [2.0, 22.7, 65.0]
  ]
}
```

**Constraints**:
1. `source` MUST be a 2D array (array of arrays)
2. Each row MUST have the same length
3. If `dimensions` is provided, its length MUST match row length
4. Empty array `[]` is valid

**Example Use Cases**:
- Time-series sensor data
- CSV file imports
- Laboratory measurements
- Tabular survey data

---

### 5.2 Matrix Type

**Purpose**: Multi-dimensional numerical matrices (images, grids, tensors).

**Structure**:
```json
{
  "type": "Matrix",
  "dimensions": "100x100x3",
  "source": [
    [ /* Row 0 */ ],
    [ /* Row 1 */ ],
    ...
  ]
}
```

**Constraints**:
1. `source` MUST be a multi-dimensional array
2. All dimensions MUST have uniform size
3. `dimensions` SHOULD describe shape (e.g., "100x100x3" for RGB image)

**Example Use Cases**:
- Gravity/magnetic field grids
- Seismic survey volumes
- Image data
- Multi-dimensional simulations

---

### 5.3 Binary Type

**Purpose**: Embed binary data as base64-encoded strings.

**Structure**:
```json
{
  "type": "Binary",
  "dimensions": "NetCDF",
  "source": [[
    "data:application/x-netcdf;base64,iVBORw0KGgoAAAANSUhEUgAA..."
  ]]
}
```

**Constraints**:
1. `source` MUST be a 2D array with single element
2. Element MUST be a string (base64-encoded or data URI)
3. SHOULD use data URI format: `data:<mime-type>;base64,<data>`

**Example Use Cases**:
- NetCDF files
- SEG-Y seismic data
- Proprietary instrument formats
- Small binary files (< 1MB recommended)

**Best Practice**: For files > 1MB, use URI type instead.

---

### 5.4 URI Type

**Purpose**: Reference external data sources.

**Structure**:
```json
{
  "type": "URI",
  "dimensions": "Gravity Survey Grid",
  "source": [[
    "https://example.com/data/gravity-survey-2023.nc"
  ]]
}
```

**Constraints**:
1. `source` MUST be a 2D array with single element
2. Element MUST be a valid URI (RFC 3986)
3. Supported schemes: `http`, `https`, `file`, `ftp`, `s3`, `mqtt`

**Example Use Cases**:
- Large files (> 1MB)
- Cloud storage references
- Database connections
- Streaming data sources (MQTT, WebSocket)

**IoT Streaming**: For real-time IoT data, use Sheet type with `source: null` and add stream metadata to `properties.stream`:

```json
{
  "type": "Feature",
  "dataset": {
    "type": "Sheet",
    "dimensions": ["Timestamp/ms", "SequenceNumber", "Temp/C"],
    "source": null
  },
  "properties": {
    "deviceId": "SENSOR_001",
    "stream": {
      "sessionId": "session-20230630-120000",
      "status": "active",
      "startTime": "2023-06-30T12:00:00.000Z"
    }
  }
}
```

Data transmission via separate `StreamDataBatch` messages (see Section 10.3).

---

## 6. CBOR Serialization

### 6.1 Overview

GPJSON MAY be serialized using CBOR (RFC 7049) for efficient binary transmission.

**Benefits**:
- 30-50% size reduction vs JSON
- 1.5-2x faster parsing
- Native binary data support
- Maintains full semantic equivalence

### 6.2 MIME Types

| Format | MIME Type |
|--------|-----------|
| JSON | `application/vnd.gpjson+json` |
| CBOR | `application/vnd.gpjson+cbor` |

### 6.3 Conversion Rules

1. All JSON values map directly to CBOR equivalents
2. Numbers: JSON number → CBOR integer or float
3. Strings: JSON string → CBOR text string
4. Arrays: JSON array → CBOR array
5. Objects: JSON object → CBOR map

### 6.4 Example

**JSON (82 bytes)**:
```json
{"type":"Feature","dataset":{"type":"Sheet","source":[[1.0,2.0]]}}
```

**CBOR (47 bytes, 43% reduction)**:
```
A2                      # map(2)
   64 74797065          # text(4) "type"
   67 4665617475726520  # text(7) "Feature"
   67 64617461736574    # text(7) "dataset"
   A2                   # map(2)
      64 74797065       # text(4) "type"
      65 5368656574     # text(5) "Sheet"
      66 736F75726365   # text(6) "source"
      81                # array(1)
         82             # array(2)
            FB 3FF0000000000000  # float(1.0)
            FB 4000000000000000  # float(2.0)
```

---

## 7. Data Validation

### 7.1 Common Rules (All DataSet Types)

**Rule 1**: `type` MUST be one of: "Sheet", "Matrix", "Binary", "URI"

**Rule 2**: `source` MUST be present

**Rule 3**: `dimensions` is OPTIONAL but RECOMMENDED

**Rule 4**: Unknown members SHOULD be ignored (forward compatibility)

### 7.2 Sheet Type Validation

**Constraint 1**: If `dimensions` is provided, its length MUST equal the number of columns in each row

**Constraint 2**: All rows MUST have the same length

**Constraint 3**: `source` MUST be a 2D array

**Example Validation**:
```javascript
function validateSheet(dataset) {
  const {dimensions, source} = dataset;

  if (!Array.isArray(source)) {
    throw new Error("Sheet source must be array");
  }

  const columnCount = dimensions ? dimensions.length : null;

  for (let i = 0; i < source.length; i++) {
    if (!Array.isArray(source[i])) {
      throw new Error(`Row ${i} is not an array`);
    }

    if (columnCount && source[i].length !== columnCount) {
      throw new Error(
        `Row ${i}: expected ${columnCount} columns, got ${source[i].length}`
      );
    }
  }

  return true;
}
```

### 7.3 Matrix Type Validation

**Constraint 1**: `source` MUST be a multi-dimensional array

**Constraint 2**: All dimensions MUST have uniform size

**Constraint 3**: `dimensions` SHOULD describe shape (e.g., "100x100x3")

### 7.4 Binary Type Validation

**Constraint 1**: `source` MUST be `[[<string>]]` (2D array, single element)

**Constraint 2**: String SHOULD use data URI format

**Constraint 3**: Recommended max size: 1MB (use URI for larger files)

### 7.5 URI Type Validation

**Constraint 1**: `source` MUST be `[[<uri>]]` (2D array, single element)

**Constraint 2**: URI MUST follow RFC 3986 syntax

**Constraint 3**: Supported schemes: `http`, `https`, `file`, `ftp`, `s3`, `mqtt`

**Example Validation**:
```javascript
function validateURI(dataset) {
  const {source} = dataset;

  if (!Array.isArray(source) || source.length !== 1) {
    throw new Error("URI source must be 2D array with single element");
  }

  if (!Array.isArray(source[0]) || source[0].length !== 1) {
    throw new Error("URI source must be [[<uri>]]");
  }

  const uri = source[0][0];
  if (typeof uri !== 'string') {
    throw new Error("URI must be string");
  }

  try {
    new URL(uri);
  } catch (e) {
    throw new Error(`Invalid URI: ${uri}`);
  }

  return true;
}
```

---

## 8. JSON Schema

### 8.1 Feature Schema

```json
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://gpjson.org/schemas/feature.json",
  "title": "GPJSON Feature",
  "type": "object",
  "required": ["type"],
  "properties": {
    "type": {"enum": ["Feature"]},
    "id": {"oneOf": [{"type": "string"}, {"type": "number"}]},
    "geometry": {
      "oneOf": [
        {"$ref": "https://geojson.org/schema/Geometry.json"},
        {"type": "null"}
      ]
    },
    "dataset": {"$ref": "#/definitions/DataSet"},
    "properties": {"type": "object"},
    "bbox": {
      "type": "array",
      "minItems": 4,
      "items": {"type": "number"}
    }
  },
  "definitions": {
    "DataSet": {
      "type": "object",
      "required": ["type", "source"],
      "properties": {
        "type": {"enum": ["Sheet", "Matrix", "Binary", "URI"]},
        "dimensions": {
          "oneOf": [
            {"type": "array", "items": {"type": "string"}},
            {"type": "string"}
          ]
        },
        "source": {}
      },
      "oneOf": [
        {"$ref": "#/definitions/SheetDataSet"},
        {"$ref": "#/definitions/MatrixDataSet"},
        {"$ref": "#/definitions/BinaryDataSet"},
        {"$ref": "#/definitions/URIDataSet"}
      ]
    },
    "SheetDataSet": {
      "type": "object",
      "required": ["type", "source"],
      "properties": {
        "type": {"enum": ["Sheet"]},
        "dimensions": {"type": "array", "items": {"type": "string"}},
        "source": {
          "type": "array",
          "items": {"type": "array"}
        }
      }
    },
    "MatrixDataSet": {
      "type": "object",
      "required": ["type", "source"],
      "properties": {
        "type": {"enum": ["Matrix"]},
        "dimensions": {"type": "string"},
        "source": {"type": "array"}
      }
    },
    "BinaryDataSet": {
      "type": "object",
      "required": ["type", "source"],
      "properties": {
        "type": {"enum": ["Binary"]},
        "dimensions": {"type": "string"},
        "source": {
          "type": "array",
          "minItems": 1,
          "maxItems": 1,
          "items": {
            "type": "array",
            "minItems": 1,
            "maxItems": 1,
            "items": {"type": "string"}
          }
        }
      }
    },
    "URIDataSet": {
      "type": "object",
      "required": ["type", "source"],
      "properties": {
        "type": {"enum": ["URI"]},
        "dimensions": {"type": "string"},
        "source": {
          "type": "array",
          "minItems": 1,
          "maxItems": 1,
          "items": {
            "type": "array",
            "minItems": 1,
            "maxItems": 1,
            "items": {"type": "string", "format": "uri"}
          }
        }
      }
    }
  }
}
```

---

## 9. Versioning

### 9.1 Version String

GPJSON uses semantic versioning: `MAJOR.MINOR.PATCH`

**Current Version**: `4.0.0`

### 9.2 Version Member

FeatureCollection objects SHOULD include a "version" member:

```json
{
  "type": "FeatureCollection",
  "version": "4.0",
  "features": [...]
}
```

### 9.3 Backward Compatibility

- **v4.0** is backward compatible with GeoJSON (RFC 7946)
- **v4.0** removes Sequence type from v3.0 (breaking change)
- Migration: Sequence → Sheet + properties.stream (see Section 13.2)

---

## 10. Examples

### 10.1 Seismic Station (Geometry + DataSet)

```json
{
  "type": "Feature",
  "id": "seismic-station-001",
  "geometry": {
    "type": "Point",
    "coordinates": [116.4074, 39.9042]
  },
  "dataset": {
    "type": "Sheet",
    "dimensions": ["Time/s", "Velocity/m/s", "Amplitude"],
    "source": [
      [0.0, 0.12, 0.005],
      [0.1, 0.15, 0.008],
      [0.2, 0.18, 0.012]
    ]
  },
  "properties": {
    "stationId": "BJ001",
    "instrumentType": "Seismometer",
    "samplingRate": 10
  }
}
```

### 10.2 IoT Sensor (DataSet Only)

```json
{
  "type": "Feature",
  "dataset": {
    "type": "Sheet",
    "dimensions": ["Timestamp/ms", "Temperature/C", "Humidity/%"],
    "source": null
  },
  "properties": {
    "deviceId": "ENV_SENSOR_999",
    "location": [116.4074, 39.9042],
    "stream": {
      "sessionId": "session-20230630-120000",
      "status": "active",
      "startTime": "2023-06-30T12:00:00.000Z",
      "bufferSize": 100
    }
  }
}
```

**Data Transmission**:
```json
{
  "type": "StreamDataBatch",
  "sessionId": "session-20230630-120000",
  "startSequence": 0,
  "endSequence": 2,
  "data": [
    [1687869600000, 22.5, 65.2],
    [1687869601000, 22.6, 65.1],
    [1687869602000, 22.7, 65.0]
  ],
  "checksum": "CRC32:A1B2C3D4"
}
```

### 10.3 Gravity Survey (Binary Data)

```json
{
  "type": "Feature",
  "geometry": {
    "type": "Polygon",
    "coordinates": [[
      [116.0, 39.0],
      [117.0, 39.0],
      [117.0, 40.0],
      [116.0, 40.0],
      [116.0, 39.0]
    ]]
  },
  "dataset": {
    "type": "Binary",
    "dimensions": "NetCDF Grid Data",
    "source": [[
      "data:application/x-netcdf;base64,Q0RGAQAAAAAAAAAAAAAAAAoAAAAEAAAAAA..."
    ]]
  },
  "properties": {
    "surveyId": "GRAV_2023_001",
    "gridResolution": "1km",
    "dataFormat": "NetCDF-4"
  }
}
```

### 10.4 Remote Data Reference (URI)

```json
{
  "type": "Feature",
  "geometry": {
    "type": "Point",
    "coordinates": [116.4074, 39.9042]
  },
  "dataset": {
    "type": "URI",
    "dimensions": "Magnetic Survey Data",
    "source": [[
      "https://example.com/surveys/magnetic-2023-06.cbor"
    ]]
  },
  "properties": {
    "surveyDate": "2023-06-30",
    "dataSize": "125MB",
    "format": "CBOR"
  }
}
```

---

## 11. Security Considerations

### 11.1 Data URI Risks

Binary type using data URIs can embed arbitrary data:
- **Risk**: Large embedded data can cause memory exhaustion
- **Mitigation**: Limit Binary data to < 1MB, use URI for larger files
- **Recommendation**: Validate data URI mime-types

### 11.2 External URI Risks

URI type references external resources:
- **Risk**: Server-Side Request Forgery (SSRF)
- **Mitigation**: Validate URI schemes, restrict to whitelisted domains
- **Recommendation**: Use HTTPS for remote resources

### 11.3 JSON Injection

Malicious JSON can exploit parser vulnerabilities:
- **Risk**: Code injection, DoS via deeply nested objects
- **Mitigation**: Use secure JSON parsers, limit nesting depth
- **Recommendation**: Validate with JSON Schema before processing

### 11.4 CBOR Decoding

CBOR decoding has specific risks:
- **Risk**: Integer overflow, buffer overflow
- **Mitigation**: Use well-tested CBOR libraries (e.g., cn-cbor)
- **Recommendation**: Validate data size limits

---

## 12. IANA Considerations

### 12.1 MIME Type Registration

**JSON Format**:
- Type name: `application`
- Subtype name: `vnd.gpjson+json`
- Required parameters: None
- Optional parameters: `version`
- Encoding: UTF-8
- Security: See Section 11
- Interoperability: Compatible with GeoJSON
- Published specification: This document
- Contact: GPJSON Working Group

**CBOR Format**:
- Type name: `application`
- Subtype name: `vnd.gpjson+cbor`
- Required parameters: None
- Optional parameters: `version`
- Encoding: Binary
- Security: See Section 11
- Interoperability: CBOR (RFC 7049)
- Published specification: This document
- Contact: GPJSON Working Group

---

## 13. References

### 13.1 Normative References

- **[RFC2119]**: Bradner, S., "Key words for use in RFCs to Indicate Requirement Levels", BCP 14, RFC 2119, March 1997.

- **[RFC7946]**: Butler, H., Daly, M., Doyle, A., Gillies, S., Hagen, S., and T. Schaub, "The GeoJSON Format", RFC 7946, August 2016.

- **[RFC7049]**: Bormann, C. and P. Hoffman, "Concise Binary Object Representation (CBOR)", RFC 7049, October 2013.

- **[RFC3986]**: Berners-Lee, T., Fielding, R., and L. Masinter, "Uniform Resource Identifier (URI): Generic Syntax", RFC 3986, January 2005.

- **[RFC6901]**: Bryan, P., Ed., Zyp, K., and M. Nottingham, Ed., "JavaScript Object Notation (JSON) Pointer", RFC 6901, April 2013.

### 13.2 Informative References

- **[GPJSON-v3.0]**: GPJSON Format Specification v3.0, November 2022.
  - **Note**: v4.0 removes Sequence type due to redundancy
  - **Migration**: Replace Sequence with Sheet + properties.stream

- **[JSON-Schema]**: "JSON Schema: A Media Type for Describing JSON Documents", draft-handrews-json-schema-02, September 2019.

---

## Appendix A. Changes from v3.0

### A.1 Breaking Changes

1. **Removed Sequence Type**
   - **Reason**: Redundant with Sheet, URI, and Binary types
   - **Migration**: Use Sheet with `source: null` + `properties.stream`
   - **Impact**: v3.0 Sequence data requires conversion

### A.2 New Features

1. **CBOR Serialization** (Section 6)
   - Binary format support
   - MIME type: `application/vnd.gpjson+cbor`
   - 30-50% size reduction

2. **JSON Schema** (Section 8)
   - Formal validation specification
   - Tool support for validation

3. **Versioning** (Section 9)
   - FeatureCollection "version" member
   - Semantic versioning

4. **Enhanced Validation** (Section 7)
   - Comprehensive constraint rules
   - Validation examples

### A.3 Clarifications

1. **Geometry-DataSet Relationship**
   - Explicit "Separation of Concerns" principle
   - Validity matrix for all scenarios

2. **DataSet Type Purposes**
   - Clear use cases for each type
   - Decision guidance

---

## Appendix B. Design Rationale

### B.1 Why Remove Sequence Type?

**Problem**: Sequence overlapped with existing types:
- `source: Array` → Same as Sheet
- `source: String` (URI) → Same as URI
- `source: String` (base64) → Same as Binary
- `source: Object` (metadata) → Can use properties.stream

**Solution**: Use Sheet + properties.stream for streaming data.

**Benefits**:
- Simplified specification (4 types vs 5)
- Clearer type boundaries
- Easier for developers to choose

### B.2 Why Support CBOR?

**Requirements**:
- Efficient IoT data transmission
- Binary data support
- Reduced bandwidth usage

**CBOR Advantages**:
- 30-50% smaller than JSON
- Native binary types
- Faster parsing
- Industry standard (RFC 7049)

### B.3 Why JSON Schema?

**Requirements**:
- Formal validation
- Tool ecosystem support
- Developer confidence

**Benefits**:
- Automated validation
- IDE autocomplete
- Documentation generation
- Interoperability testing

---

## Appendix C. Complete Example

**Geophysical Survey FeatureCollection**:

```json
{
  "type": "FeatureCollection",
  "version": "4.0",
  "features": [
    {
      "type": "Feature",
      "id": "gravity-station-001",
      "geometry": {
        "type": "Point",
        "coordinates": [116.4074, 39.9042, 50.5]
      },
      "dataset": {
        "type": "Sheet",
        "dimensions": ["Time/s", "Gravity/mGal", "Tilt/degrees"],
        "source": [
          [0.0, 978325.42, 0.02],
          [1.0, 978325.45, 0.03],
          [2.0, 978325.48, 0.02]
        ]
      },
      "properties": {
        "stationId": "GS001",
        "instrument": "Scintrex CG-6",
        "operator": "John Doe",
        "surveyDate": "2023-06-30"
      }
    },
    {
      "type": "Feature",
      "id": "magnetic-grid",
      "geometry": {
        "type": "Polygon",
        "coordinates": [[
          [116.0, 39.0],
          [117.0, 39.0],
          [117.0, 40.0],
          [116.0, 40.0],
          [116.0, 39.0]
        ]]
      },
      "dataset": {
        "type": "URI",
        "dimensions": "100x100 Magnetic Intensity Grid",
        "source": [[
          "https://example.com/surveys/mag-grid-2023.cbor"
        ]]
      },
      "properties": {
        "gridSpacing": "1km",
        "dataFormat": "CBOR",
        "totalAnomaly": true
      }
    }
  ]
}
```

---

## Authors' Addresses

GPJSON Working Group
Email: gpjson@example.com

---

**End of GPJSON Format Specification v4.0**
