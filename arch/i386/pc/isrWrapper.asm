; ------------------------------------------------------------------------------
; UNDERBARS DEFINITION
; ------------------------------------------------------------------------------
%ifdef UNDERBARS      ; ADD underbars
  %macro EXP 1
    GLOBAL $_%1
    %define %1 $_%1
  %endmacro
  %macro IMP 1
    EXTERN $_%1
    %define %1 $_%1
  %endmacro
%else                 ; DON'T ADD UNDERBARS
  %macro EXP 1
    GLOBAL $%1
  %endmacro
  %macro IMP 1
    EXTERN $%1
  %endmacro
%endif

; ------------------------------------------------------------------------------
; MACRO for interrupts WITHOUT ERROR CODE, push fake error code
; ------------------------------------------------------------------------------
%macro  WRAPPER 1
EXP isr_%1_wrapper
isr_%1_wrapper:
  push dword 0
  push dword %1
  jmp all_ints
%endmacro

; ------------------------------------------------------------------------------
; MACRO for interrupts WITH ERROR CODE
; ------------------------------------------------------------------------------
%macro  ERROR_WRAPPER 1
EXP isr_%1_wrapper
isr_%1_wrapper:
  push dword %1
  jmp all_ints
%endmacro

; ------------------------------------------------------------------------------
; IMPORTS & EXPORTS
; ------------------------------------------------------------------------------
IMP isr

; ------------------------------------------------------------------------------
; CODE SECTION
; ------------------------------------------------------------------------------
[SEGMENT .text]
[BITS 32]
all_ints:
  push gs
  push fs
  push es
  push ds
  push ss
  pusha
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax,esp
    push eax
      call isr
    pop eax
  popa
  pop ss
  pop ds
  pop es
  pop fs
  pop gs
  add esp, 8         ; drop exception number and error code
  iret


; ------------------------------------------------------------------------------
; Processor generated interrupts
WRAPPER 0x00
WRAPPER 0x01
WRAPPER 0x02
WRAPPER 0x03
WRAPPER 0x04
WRAPPER 0x05
WRAPPER 0x06
WRAPPER 0x07
ERROR_WRAPPER 0x08
WRAPPER 0x09
ERROR_WRAPPER 0x0A
ERROR_WRAPPER 0x0B
ERROR_WRAPPER 0x0C
ERROR_WRAPPER 0x0D
ERROR_WRAPPER 0x0E
WRAPPER 0x0F
WRAPPER 0x10
WRAPPER 0x11
WRAPPER 0x12
WRAPPER 0x13
WRAPPER 0x14
WRAPPER 0x15
WRAPPER 0x16
WRAPPER 0x17
WRAPPER 0x18
WRAPPER 0x19
WRAPPER 0x1A
WRAPPER 0x1B
WRAPPER 0x1C
WRAPPER 0x1D
WRAPPER 0x1E
WRAPPER 0x1F
; PIC generated interrupts
WRAPPER 0x20
WRAPPER 0x21
WRAPPER 0x22
WRAPPER 0x23
WRAPPER 0x24
WRAPPER 0x25
WRAPPER 0x26
WRAPPER 0x27
WRAPPER 0x28
WRAPPER 0x29
WRAPPER 0x2A
WRAPPER 0x2B
WRAPPER 0x2C
WRAPPER 0x2D
WRAPPER 0x2E
WRAPPER 0x2F
; Software generated interrupts
WRAPPER 0x30
