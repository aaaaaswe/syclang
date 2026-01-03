// SysLang EFI 系统类型定义

#ifndef SYSLANG_EFI_TYPES_H
#define SYSLANG_EFI_TYPES_H

#include <stdint.h>

// EFI 状态码
typedef uint32_t EFI_STATUS;
#define EFI_SUCCESS              0
#define EFI_LOAD_ERROR           1
#define EFI_INVALID_PARAMETER    2
#define EFI_UNSUPPORTED          3
#define EFI_BAD_BUFFER_SIZE     4
#define EFI_BUFFER_TOO_SMALL    5
#define EFI_NOT_READY           6
#define EFI_DEVICE_ERROR        7
#define EFI_WRITE_PROTECTED     8
#define EFI_OUT_OF_RESOURCES    9
#define EFI_VOLUME_CORRUPTED    10
#define EFI_VOLUME_FULL         11
#define EFI_NO_MEDIA            12
#define EFI_MEDIA_CHANGED       13
#define EFI_NOT_FOUND           14
#define EFI_ACCESS_DENIED       15
#define EFI_NO_RESPONSE         16
#define EFI_NO_MAPPING          17
#define EFI_TIMEOUT             18
#define EFI_NOT_STARTED         19
#define EFI_ALREADY_STARTED     20
#define EFI_ABORTED            21
#define EFI_ICMP_ERROR         22
#define EFI_TFTP_ERROR         23
#define EFI_PROTOCOL_ERROR      24

// EFI 句柄
typedef void* EFI_HANDLE;

// GUID 结构
typedef struct {
    uint32_t data1;
    uint16_t data2;
    uint16_t data3;
    uint8_t  data4[8];
} EFI_GUID;

// 物理地址
typedef uint64_t EFI_PHYSICAL_ADDRESS;

// 虚拟地址
typedef uint64_t EFI_VIRTUAL_ADDRESS;

// 内存描述符
typedef struct {
    uint32_t                Type;
    uint32_t                Pad;
    EFI_PHYSICAL_ADDRESS    PhysicalStart;
    EFI_VIRTUAL_ADDRESS    VirtualStart;
    uint64_t                NumberOfPages;
    uint64_t                Attribute;
} EFI_MEMORY_DESCRIPTOR;

// PCI 设备类
typedef enum {
    PCI_CLASS_DISPLAY = 0x03,
    PCI_CLASS_NETWORK = 0x02,
    PCI_CLASS_STORAGE = 0x01,
    PCI_CLASS_SERIAL = 0x0C
} PCI_CLASS;

// 显卡接口
typedef struct _EFI_GRAPHICS_OUTPUT_PROTOCOL EFI_GRAPHICS_OUTPUT_PROTOCOL;

// 网卡接口
typedef struct _EFI_SIMPLE_NETWORK_PROTOCOL EFI_SIMPLE_NETWORK_PROTOCOL;

// 块设备接口
typedef struct _EFI_BLOCK_IO_PROTOCOL EFI_BLOCK_IO_PROTOCOL;

#endif // SYSLANG_EFI_TYPES_H
