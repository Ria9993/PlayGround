tmp=$10 ; [$10 ~ $1F]
btbl=tmp
n=tmp+2
npow=tmp+3
PG1=$0100
pow_table:
    .BYTE 0,1,4,9,16,25,36,49,64,81,100
pow_mi_table:
    .BYTE 0,0,3,8,15,24,35,48,63,80,99
    
callnum: ;(s[0,1](btbl), s[2](n), s[3](num)) | <A,X,Y,P,S>
;========================================
; summary: Marks the called number found on the board
; 
; arguments:    num    7bit number
;========================================
    .SUBROUTINE
.num=tmp+4
.retaddr=tmp+6

; Copy arguments
; A = num, Y = (n * n)
    pla ; retaddr_low
    sta .retaddr
    pla ; retaddr_high
    sta .retaddr+1
    pla ; btbl_low
    sta btbl
    pla ; btbl_high
    sta btbl+1
    pla ; n
    sta n
    tax
    lda pow_table,x
    tay
    pla ; num

.find_loop:
    dey
    bmi .end
    cmp (btbl),y
    bne .find_loop
.find:
    ora #%10000000
    sta (btbl),y
.end:
    ; stack recover
    lda .retaddr+1
    pha
    lda .retaddr
    pha
    
    rts

won: ;(s[0,1](btbl), s[2](n)) | <$01(ret),A,X,Y,P,S>
;========================================
; summary: return is_player_won ($01 or $00)
;
; return:  ret     $01
;========================================
    .SUBROUTINE
.ret=$01
.line_end=tmp+4
.line_start=tmp+5
.retaddr=tmp+6

; Copy arguments
    tsx
    lda PG1+3,x ; btbl_low
    sta btbl
    lda PG1+4,x ; btbl_high
    sta btbl+1
    lda PG1+5,x ; n
    sta n
    sta .line_end ;h_first_end

    ; get n*n
    tax
    lda pow_table,x
    sta npow

; Horizontal Bingo
    ; Y = 0
    ; for (.line_end = n; .line_end <= n*n; .line_end += SIZE)
    ; for (; Y < .line_end; Y++)
    ldy #0
.h_loop:
    ; IF (board[y].MSB == 0) THEN (.h_skip_line)
    lda (btbl),y
    bpl .h_skip_line
    ; Y += 1
    ; IF (bingo) THEN (return)
    iny
    cpy .line_end
    bne .h_loop
.h_bingo:
    ; return is_player_won = $01
    lda #01
    sta .ret
    rts
.h_skip_line:
    ; IF (.line_end == n*n) THEN (.h_end)
    lda .line_end
    cmp npow
    beq .h_end
    ; Y = .line_end
    ; .line_end += n
    tay
    clc
    adc n
    sta .line_end
    jmp .h_loop
.h_end:

; After H_end
; A = n*n
; .line_end = n*n

; Vertical Bingo
    ; Y = 0
    ; X(line_start) = 0
    ; .line_end = n*n
    ldy #0
    ldx #0
.v_loop:
    ; IF (board[y].MSB == 0) THEN (.v_skip_line)
    lda (btbl),y
    bpl .v_skip_line
    ; Y += SIZE
    ; IF (bingo) THEN (return)
    tya
    clc
    adc n
    tay
    cmp .line_end
    bne .v_loop
.v_bingo:
    ; return is_player_won = $01
    lda #01
    sta .ret
    rts
.v_skip_line:
    ; X += 1
    ; Y = X
    ; .line_end += 1
    inx
    txa
    tay
    inc .line_end
    ; IF (X < n) THEN (.v_end) ELSE (.v_loop)
    cpx n
    bne .v_loop
.v_end:

; After v_end
; X = n
; .line_end = (n*n)+n

; Right-down Diagonal Bingo
    ; for (Y = 0; Y < .line_end((n*n)+n); Y += n+1)
    ldy #0
.rd_loop:
    ; IF (board[y].MSB == 0) THEN (.rd_end)
    lda (btbl),y
    bpl .rd_end
    ; Y += n+1
    tya
    sec
    adc n ; Y += n+c(1)
    tay
    cmp .line_end
    bne .rd_loop
.rd_bingo:
    ; return is_player_won = $01
    lda #01
    sta .ret
    rts
.rd_end:

; After rd_end
; X = n
; .line_end = (n*n)+n

; Left_down Diagonal Bingo
    ; for (Y = (n*n)-n; Y != 0; Y -= n-1)
    lda npow
    sec
    sbc n
    tay

    dec n ; n -= 1. for Y -= n-1
.ld_loop:
    ; IF (board[y].MSB == 0) THEN (ld_end)
    lda (btbl),y
    bpl .ld_end
    ; Y -= n-1
    tya
    sec
    sbc n
    tay
    bne .ld_loop
.ld_bingo:
    ; return is_player_won = $01
    lda #01
    sta .ret
    rts
.ld_end:
    
.end
    ; return is_player_lost = $00
    lda #00
    sta .ret
    rts
