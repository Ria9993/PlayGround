TITLE Stat Printer

.DOSSEG
.8086
.8087
.MODEL TINY

; print_str (string) | <ax, dx>
print_str MACRO string
    mov ah, 09h
    lea dx, string
    int 21h
ENDM

.DATA
msg_prompt_input_filename DB "Input file name : ", 0Dh, 0Ah, '$'
FILENAME_MAX    EQU 255
filename_input  DB FILENAME_MAX
                DB ?
                DB FILENAME_MAX DUP (?)
read_buf        DB 8 DUP (?)
size_val        DW ?
us_unit_recp    REAL8 0.001
ms_unit_recp    REAL8 1.0
mean_val        DD ?

print_buf       DB "00.000"
                DB '$'

.CODE
.STARTUP
    finit
input_filename:
    print_str msg_prompt_input_filename
    mov ah, 0Ah
    lea dx, filename_input
    int 21h
    ; convert filename to ASCIZ
    xor bh, bh
    mov bl, BYTE PTR filename_input[1]
    mov BYTE PTR filename_input[2+bx], 0
open_file:
    mov ah, 3Dh
    mov al, 0
    lea dx, filename_input[2]   ; ASCIZ filename
    int 21h
    ; ax = file handle
read_file_format:
    mov bx, ax ; file handle
    mov ah, 3Fh
    mov cx, 1  ; num of bytes to read
    lea dx, read_buf
    int 21h
    ; 'm' or 'u'
    ; unit_recp = ms : 1, us : 0.001
    ; will be used to multiply values and make them in ms unit
    cmp read_buf, 'm'
    je  ms_unit
us_unit:
    fld us_unit_recp 
    jmp unit_end
ms_unit:
    fld ms_unit_recp
unit_end:
    ; st { unit_recp }


calculate_data_sum:
; bx = filehandle
; di = count_read
; cx = 64bit read size {8}
    xor     di, di
    fldz                        ; st { sum=0, unit_recp }
sum_loop:
    ; read 64bit IEEE-754, one
    mov     ah, 3Fh
    mov     cx, 8
    lea     dx, read_buf
    int     21h
    ; return AX = num of bytes actually read
    test    ax, ax
    je      sum_loop_end

    wait                        ; st { sum, unit_recp }
    fld     REAL8 PTR read_buf  ; st { time, sum, unit_recp }
    fadd                        ; st { sum+time,  unit_recp }
    inc     di
    jmp     sum_loop
sum_loop_end:
    ; calculate mean (sum / count)
    mov     size_val, di
    wait
    fild    WORD PTR size_val   ; st { size, sum, unit_recp }
    fdivp   st[1], st           ; st { mean, unit_recp } (div st(1) by st(0), and pop)
    fmulp   st[1], st           ; st { mean(ms) }
    frndint                     ; round st[0] to integer
    fistp   mean_val
    fwait

    ; print format = ss.mmm
sprint:
    mov ax, WORD PTR [mean_val]
    ; X0.000
    mov cx, 10000
    xor dx, dx
    div cx          ; ax = quot, dx = rem
    add print_buf[0], al
    ; _X.000
    mov cx, 1000
    mov ax, dx
    xor dx, dx
    div cx
    add print_buf[1], al
    ; __.X00
    mov cx, 100
    mov ax, dx
    xor dx, dx
    div cx
    add print_buf[3], al
    ; __._X0
    mov cx, 10
    mov ax, dx
    xor dx, dx
    div cx
    add print_buf[4], al
    ; __.__X
    add print_buf[5], dl
sprint_end:

    print_str print_buf

terminate:
    mov ah, 4Ch
    xor al, al
    int 21h
END
