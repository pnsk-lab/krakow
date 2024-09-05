; $Id$

.area CODE (ABS)
.org 0x4000

.db 'A'
.db 'B'
.dw init
.dw 0
.dw 0
.dw 0

.dw 0
.dw 0
.dw 0

init:
	ld a, #32
	ld (0xf3af), a
	call 0x6f

	ld hl, #hello
	call Print
	call 0x4100

Print:
	ld a, (hl)
	and a
	ret z
	call 0xa2
	inc hl
	jr Print

hello:
	.asciz "Krakow BASIC MSX ROM bootstrap"

.org 0x7fff
.db 0
