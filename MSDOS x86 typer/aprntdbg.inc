int3_handler PROC
   mov ah, 09h
   mov dx, di
   int 21h
   jmp DWORD PTR [int3_original_vector]
int3_handler ENDP
