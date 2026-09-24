# UNIOS drivers

Drivers are compiled into UNIOS only after being adapted to the UNIOS driver API.

A driver must:

1. Declare supported hardware IDs.
2. Validate all device-provided lengths and addresses.
3. Avoid DMA until an IOMMU or restricted DMA allocator exists.
4. Never execute device-provided memory.
5. Register only after hardware identification succeeds.
6. Fail closed when initialization fails.

Unused drivers are excluded at build time. They are not self-deleted from a running kernel.
