; ------------------------------------------------------------------------------
; UNDERBARS DEFINITION
; ------------------------------------------------------------------------------
%ifdef UNDERBARS
  %macro EXP 1
    GLOBAL $_%1
    %define %1 $_%1
  %endmacro
  %macro IMP 1
    EXTERN $_%1
    %define %1 $_%1
  %endmacro
%else
  %macro EXP 1
    GLOBAL $%1
  %endmacro
  %macro IMP 1
    EXTERN $%1
  %endmacro
%endif

; ------------------------------------------------------------------------------
; IMPORTS & EXPORTS
; ------------------------------------------------------------------------------
EXP entry
EXP jump_to_virtual
EXP read_eip
EXP farJump
EXP farCall
IMP main

; ------------------------------------------------------------------------------
; MULTIBOOT DEFINITIONS
; ------------------------------------------------------------------------------
MULTIBOOT_PAGE_ALIGN   equ 1<<0
MULTIBOOT_MEMORY_INFO  equ 1<<1

MULTIBOOT_HEADER_MAGIC equ 0x1BADB002
MULTIBOOT_HEADER_FLAGS equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
CHECKSUM               equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

; ------------------------------------------------------------------------------
; CODE SECTION
; ------------------------------------------------------------------------------
[BITS 32]
[SECTION .text]
entry:
  mov esp, 0x00101000           ; make new stack right below kernel
  push ebx                      ; push MultiBootInfo pointer on new stack
  call main                     ; call c initialization function

; ------------------------------------------------------------------------------
jump_to_virtual:
  push ebp
    mov ebp, esp
    push eax
    push ebx
      call whats_this_addr      ; Call to the current physical addr
whats_this_addr:
      pop eax                   ; So we can pop a physical addr
      mov ebx, [ebp+4]          ; physical return addr
      sub ebx, eax              ; substract the unknow physical addr
      add ebx, whats_this_addr  ; add the same unknow but virtual addr
      mov [ebp+4], ebx          ; put the return addr back on the stack
    pop ebx
    pop eax
    mov esp, ebp
  pop ebp
  ret

; ------------------------------------------------------------------------------
read_eip:
  call push_eip
push_eip:
  pop eax
  ret

; ------------------------------------------------------------------------------
farJump:
  push ebp
    mov ebp, esp
      mov eax, [ebp + 8]
      mov [farJumpSel], ax
      mov eax, [ebp + 12]
      mov [farJumpOffset], eax
      db 0xEA
farJumpOffset:
      dd 0
farJumpSel:
      dw 0
    mov esp, ebp
  pop ebp
  ret

; ------------------------------------------------------------------------------
farCall:
  push ebp
    mov ebp, esp
      mov eax, [ebp + 8]
      mov [farCallSel], ax
      mov eax, [ebp + 12]
      mov [farCallOffset], eax
      db 0x9A
farCallOffset:
      dd 0
farCallSel:
      dw 0
    mov esp, ebp
  pop ebp
  ret

  
; ------------------------------------------------------------------------------
; The Multiboot header
align 4
  dd MULTIBOOT_HEADER_MAGIC
  dd MULTIBOOT_HEADER_FLAGS
  dd CHECKSUM
