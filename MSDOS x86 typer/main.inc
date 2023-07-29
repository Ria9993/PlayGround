printstr MACRO string
   mov ah, 09h
   mov dx, OFFSET string
   int 21h
ENDM

.DATA
MAX_SIZE EQU 255
; size, bytes_read, buffer[255]
filename DB MAX_SIZE
         DB ?
         DB MAX_SIZE DUP (?)
         DB '$'

filedata DB MAX_SIZE DUP (?)
         DB '$'

MSG_PROMPT_FILENAME     DB 'FILENAME:',0Dh,0Ah,'$'
MSG_PROMPT_START_TYPING DB 'Start Typing!',0Dh,0Ah,'$'
MSG_ERROR_OPEN_FILE     DB 'Failed to open file',0Dh,0Ah,'$'
MSG_ERROR_READ_FILE     DB 'Failed to read file',0Dh,0Ah,'$'
MSG_ERROR_TYPING_WRONG  DB '<Wrong Type>',0Dh,0Ah,'$'

MSG_DBG  DB 'RUNNING',0Dh,0Ah,'$'

int3_original_vector    DD ?  ; offset:segment

.CODE
.STARTUP
read_filename:
   printstr MSG_PROMPT_FILENAME
   mov ah, 0Ah
   mov dx, OFFSET filename
   int 21h

open_file:
   ; put NULL character at end of string (require asciz)
   mov bx, OFFSET filename[2]
   xor ah, ah
   mov al, BYTE PTR [bx-1]
   add bx, ax
   mov BYTE PTR [bx], 0
   ; OPEN FILE. return AX = file handle
   mov ah, 3Dh
   mov al, 0   ; read only mode
   mov dx, OFFSET filename[2]
   int 21h
;   jnc open_file_success
;open_file_error:
;   printstr MSG_ERROR_OPEN_FILE
;   jmp   terminate
open_file_success:

read_file:
   ; READ FILE, return AX = number of bytes read
   mov bx, ax
   mov cx, MAX_SIZE
   mov dx, OFFSET filedata
   mov ah, 3Fh
   int 21h
;   jnc read_file_success
;read_file_error:
;   printstr MSG_ERROR_READ_FILE
;   jmp terminate
read_file_success:
   ; put '\r\n' and '$' at end of string
   mov si, ax
   mov filedata[si], 0Dh
   mov filedata[si+1], 0Ah
   mov filedata[si+2], '$'

typing_start:
   printstr MSG_PROMPT_START_TYPING          
   printstr filedata
save_original_int3_vector:
   mov ah, 35h
   mov al, 03h
   int 21h      ; Return ES:BX
   mov WORD PTR int3_original_vector[2], es  ; segment
   mov WORD PTR int3_original_vector, bx     ; offset
change_int3_vector:
   mov ah, 25h
   mov al, 03h
   mov dx, OFFSET int3_handler
   mov bx, ds   ; save ds
   mov cx, cs   ; ds = cs
   mov ds, cx
   int 21h
   mov ds, bx  ; restore ds
typing_loop_init:
   mov cx, si
   jz  terminate
   mov di, OFFSET filedata
typing_loop:
   get_key:
      mov ah, 01h
      int 21h ; al = character read
   check_exit:
      cmp al, 0Dh ; '\r'
      je  typing_end
   check_currect:
      xor ah, ah
      xor bh, bh
      mov bl, [di]
      assert bx, ax, MSG_ERROR_TYPING_WRONG
typing_continue:
   inc di
   loop typing_loop
typing_end:

restore_int3_vector:
   mov ah, 25h
   mov al, 03h
   mov dx, WORD PTR int3_original_vector     ; offset  (dx, ds must be processed in order. Offset becomes invalid)
   mov ds, WORD PTR int3_original_vector[2]  ; segment
   int 21h

; If add code, need to restore ds

terminate:
   mov ah, 4Ch
   xor al, al
   int 21h
