﻿   .PROCESSOR 6502
   .ORG $8000
DIM=5
table=$1100


   .INCLUDE "utils/utils.asm"


   ldx #$FF
   txs


   ;===DEFINE BINGO TABLE HERE===
   .SUBROUTINE
   ldx #DIM*DIM
   ldy #DIM*DIM-1
.loop:
   txa
   sta table,y
   dex
   dey
   bpl .loop


   ; ================ 가로 -- 결과값 01 ================
   ; lda $1100
   ; ora #%10000000
   ; sta $1100
  
   ; lda $1100+1
   ; ora #%10000000
   ; sta $1100+1
  
   ; lda $1100+2
   ; ora #%10000000
   ; sta $1100+2
  
   ; lda $1100+3
   ; ora #%10000000
   ; sta $1100+3
  
   ; lda $1100+4
   ; ora #%10000000
   ; sta $1100+4


  
   ; ================ 가로 -- 결과값 00 ================
   ; lda $1100
   ; ora #%10000000
   ; sta $1100
  
   ; lda $1100+1
   ; ora #%10000000
   ; sta $1100+1
  
   ; lda $1100+2
   ; ora #%10000000
   ; sta $1100+2
  
   ; lda $1100+3
   ; ora #%10000000
   ; sta $1100+3
  
   ; lda $1100+77
   ; ora #%10000000
   ; sta $1100+77




   ; ================ 세로 -- 결과값 01 ================
   ; lda $1100
   ; ora #%10000000
   ; sta $1100
  
   ; lda $1100+5
   ; ora #%10000000
   ; sta $1100+5
  
   ; lda $1100+10
   ; ora #%10000000
   ; sta $1100+10
  
   ; lda $1100+15
   ; ora #%10000000
   ; sta $1100+15
  
   ; lda $1100+20
   ; ora #%10000000
   ; sta $1100+20


   ; ================ 세로 -- 결과값 00 ================
   ; lda $1100
   ; ora #%10000000
   ; sta $1100
  
   ; lda $1100+5
   ; ora #%10000000
   ; sta $1100+5
  
   ; lda $1100+10
   ; ora #%10000000
   ; sta $1100+10
  
   ; lda $1100+15
   ; ora #%10000000
   ; sta $1100+15
  
   ; lda $1100+77
   ; ora #%10000000
   ; sta $1100+77




   ; ================ 하향대각선 -- 결과값 01 ================
   ; lda $1100
   ; ora #%10000000
   ; sta $1100
  
   ; lda $1100+6
   ; ora #%10000000
   ; sta $1100+6
  
   ; lda $1100+12
   ; ora #%10000000
   ; sta $1100+12
  
   ; lda $1100+18
   ; ora #%10000000
   ; sta $1100+18
  
   ; lda $1100+24
   ; ora #%10000000
   ; sta $1100+24


  
   ; ================ 하향대각선 -- 결과값 00 ================
   ; lda $1100
   ; ora #%10000000
   ; sta $1100
  
   ; lda $1100+6
   ; ora #%10000000
   ; sta $1100+6
  
   ; lda $1100+12
   ; ora #%10000000
   ; sta $1100+12
  
   ; lda $1100+18
   ; ora #%10000000
   ; sta $1100+18
  
   ; lda $1100+77
   ; ora #%10000000
   ; sta $1100+77




   ; ================ 상향대각선 -- 결과값 01 ================
   ; lda $1100+4
   ; ora #%10000000
   ; sta $1100+4
  
   ; lda $1100+8
   ; ora #%10000000
   ; sta $1100+8
  
   ; lda $1100+12
   ; ora #%10000000
   ; sta $1100+12
  
   ; lda $1100+16
   ; ora #%10000000
   ; sta $1100+16
  
   ; lda $1100+20
   ; ora #%10000000
   ; sta $1100+20
  
   ; ================ 상향대각선 -- 결과값 00  ================
   ; lda $1100+4
   ; ora #%10000000
   ; sta $1100+4
  
   ; lda $1100+8
   ; ora #%10000000
   ; sta $1100+8
  
   ; lda $1100+12
   ; ora #%10000000
   ; sta $1100+12
  
   ; lda $1100+16
   ; ora #%10000000
   ; sta $1100+16
  
   ; lda $1100+77
   ; ora #%10000000
   ; sta $1100+77
  


   lda #DIM
   pha
   lda #$11
   pha
   lda #$00
   pha


   jsr won


   ; stack cleanup
   tsx
   inx
   inx
   inx
   txs


   lda $0001
   sta $5000


   assert $0001, $01


   termin


   .INCLUDE "nbingo.asm"


   .ORG $FFFC,0
   .WORD $8000
   .WORD $0000