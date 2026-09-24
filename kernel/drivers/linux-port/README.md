# Alien Linux Driver Compatibility Layer

Linux drivers cannot be linked directly into UNIOS.

The porting layer will eventually provide replacements for selected Linux
interfaces:

Linux-style API                 Alien API
------------------------------------------------
printk()                         alien_log()
kmalloc()/kfree()                alien_alloc()/alien_free()
ioremap()/iounmap()              alien_mmio_map()/alien_mmio_unmap()
request_irq()                    alien_irq_register()
spin_lock()/spin_unlock()        alien_spin_lock()/alien_spin_unlock()
pci_register_driver()            alien_pci_register_driver()
usb_register()                   alien_usb_register()

Initial driver targets:

1. 8250 serial
2. PS/2 keyboard
3. IDE/PATA storage
4. PCI
5. Intel e1000 network
6. USB HID
7. framebuffer

Each driver must be reviewed and ported to Alien APIs. Do not copy Linux
kernel headers into the UNIOS kernel.
