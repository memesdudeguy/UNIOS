# UniMcode execution policy

UNIOS executes native i386 code directly.

Foreign instruction sets are accepted only when a complete decoder and
translator for that architecture is compiled into the kernel.

Unknown or malformed executable formats are rejected. They are never guessed
or executed as i386 code.

Translated code must run in user mode and use a separate guest address space.
Translation pages are writable while generated and executable only after
translation is complete. Writable and executable mappings are never allowed
simultaneously.

Guest syscalls are routed through UniTL after pointer, length, descriptor,
permission, and address-space validation.

A failed translation terminates only the affected process.
