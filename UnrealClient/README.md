# UnrealClient

Presentation layer. Currently empty by design.

Per **Decision 006 (Unreal 深度接入后置)**, the Unreal side stays untouched
until Task 35 (minimal Input Bridge prototype + Debug Draw). Task 91-93 build
the real `RuntimeBridge`.

Dependency direction is one-way and must never be inverted:

```text
Runtime  <--  RuntimeBridge  <--  UnrealClient
```

- `Runtime/` may not include Unreal headers (`Tests/TestRuntimeIndependence.cpp` enforces this).
- `UnrealClient/` links against `arpg::runtime` and adapts it to UE types.
- Conversion code lives in the bridge, never inside the Runtime.
