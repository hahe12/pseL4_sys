pseL4  (Posix seL4 operating system)

# Introduction

pseL4 is a microkernel operating system based on seL4, our goal is to create a posix compatible system, which allow any user to run Linux-like application easily, although sel4 doesn't recommend to do this, however, Linux eco-chain is the key to develop application, no matter the existed library or the developing way,.

# pseL4 system architecture

```
  ┌─────────────┐                                                                  
  │             │                                                                  
  │             │                                                                  
  │ core        │                                                                  
  │             │                                                                  
  │ 1. process  │                                                                  
  │ 2. memory   │    ┌───────┐ ┌───────┐ ┌───────┐ ┌───────┐           ┌───────┐   
  │ 3. namespace│    │       │ │       │ │       │ │       │           │       │   
  │ 4. device   │    │ stack │ │ driver│ │ file  │ │ Linux │           │  APP  │   
  │             │    │       │ │       │ │system │ │  VM   │           │       │   
  │             │    │       │ │       │ │       │ │       │   ......  │       │   
  │             │    │       │ │       │ │       │ │       │           │       │   
  │             │    │       │ │       │ │       │ │       │           │       │   
  │             │    │service│ │service│ │service│ │service│           │       │   
  ├─────────────┴────┴───────┴─┴───────┴─┴───────┴─┴───────┴───────────┴───────┴───┐
  │                                  seL4 kernel                                   │
  ├────────────────────────────────────────────────────────────────────────────────┤
  │                                   hardware                                     │
  └────────────────────────────────────────────────────────────────────────────────┘

```

# System design

## Process management

The pseL4-core will create /proc file system, and also provide fork/exec simulate system call service

## Namespace management

The pseL4-core will create namespace serive for process system and other sevice.

## Memory management

The pseL4-core will manage systme memory, service for the mmap like posix system call.

## Device Management

There are two types device descriptions, ACPI/FDT, both of them are widely used, pseL4 allow user developer a single driver as a application, the most important work is to create a device in core namespace, however, to allow the driver make use FDT/ACPI is meaningless, because with modern hardware, the device generate dependency by each other, for example, a device may require GPIO/PINMUX/CLOCK/.. various resource and configuration, if the process only have single driver, it couldn't complete all of tasks, so in pseL4, we create a generic device assignment framework, the full fdt resource tree and the object described by FDT are described by core, all of basic initialize work such as clock/pinmux/... initialize work are done by core, the specific driver just need to implement function with io/irq resource.

The device resource assignment could be static or dynamic, and for VM, the dtb should be prepared on host firstly.


# Posix interface

All of basic service are provided by core, which implemented all of basic core services, such as process/memory and namespace, any other serice who want to use these core service calling the service in core thought IPC, but the won't do this by their self, a libc library capsule these service, so the service process (system service such as stack/driver/... or user )just need to call service libc, this flow allow use developer process by writing posix portable program.

For example, if a application want to access a file in a usb storage, the work follow is:

Posix application call open ("/mount/blk1", ...)
   libc open contact the namespace service though IPC
      The core know the path belong the file system service, it will routine the request to file system service
         the file system know the blk1 device are mounted by /dev/usb/blk1, so it will call open("/dev/usb/blk1") 
            The core know the the path belong the usb block driver, it will request the usb block driver.

In the real impletation, the file system may already open block device during mout, so it doesn't need to open block device when do open every time.

Beside open/read/write/... posix IO interface, the libc also implement process/thread/global sem/share memory/... support, all of them are done similarly.









