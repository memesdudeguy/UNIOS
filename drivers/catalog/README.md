# UNIOS Driver Catalog

The device manager scans buses and compares discovered device IDs against
this catalog.

A driver is selected by:

1. Exact bus match
2. Exact vendor/device match
3. Class/subclass/interface match
4. Highest priority
5. Dependency validation
6. Signature and permission validation

Drivers with no matching hardware are not loaded. They are marked as
garbage only if they are removable, unused, and not required by another
package.

Builtin drivers are never deleted.
