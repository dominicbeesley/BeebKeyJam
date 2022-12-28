		.include	"hardware.inc"

		.ZEROPAGE
zp_ptr:		.res 2
zp_tmp1:		.res 1

		.CODE 

start:		ldx	#<str_message
		ldy	#>str_message
		jsr	uart_tx_str

		rts


uart_tx_str:	stx	zp_ptr
		sty	zp_ptr+1
		ldy	#0
@lp:		lda	(zp_ptr),y
		beq	@sk
		jsr	uart_tx_char
		iny
		bne	@lp
@sk:		rts



uart_tx_char:	php
		pha
		sta	zp_tmp1
		txa
		pha
		sei
		; enable keyboard via latch
		lda	#$03				; stop Auto scan
		sta	sheila_SYSVIA_orb			; by writing to system VIA PORT B
		lda	#$7f				; set bits 0 to 6 of port A to input on bit 7
							; output on bits 0 to 6
		sta	sheila_SYSVIA_ddra		; 

		lda	#%01100000
		lsr	zp_tmp1
		ror	A
		lsr	zp_tmp1
		ror	A
		ror	A
		sta	sheila_SYSVIA_ora_nh
		inc	sheila_SYSVIA_ora_nh

		lda	#%11000000
		lsr	zp_tmp1
		ror	A
		lsr	zp_tmp1
		ror	A
		lsr	zp_tmp1
		ror	A
		ror	A
		sta	sheila_SYSVIA_ora_nh
		inc	sheila_SYSVIA_ora_nh

		lda	#%11000000
		lsr	zp_tmp1
		ror	A
		lsr	zp_tmp1
		ror	A
		lsr	zp_tmp1
		ror	A
		ror	A
		sta	sheila_SYSVIA_ora_nh
		inc	sheila_SYSVIA_ora_nh

		lda	#$0B
		sta	sheila_SYSVIA_orb
		lda	#0
		sta	sheila_SYSVIA_ddra

		pla
		tax
		pla
		plp

		rts	

		.RODATA
str_message:	.byte 	"Eh up",13,10,0


		.END
