		.include	"hardware.inc"
		.include	"oslib.inc"

		.ZEROPAGE
zp_ptr:		.res 2
zp_tmp1:		.res 1

		.CODE 

start:		ldx	#<str_message
		ldy	#>str_message
		jsr	uart_tx_str

main_loop:	jsr	uart_rx_char
		bcs	@sk1
		jsr	OSASCI
@sk1:		ldx	#0
		ldy	#0
		lda	#OSBYTE_129_INKEY
		jsr	OSBYTE
		bcs	main_loop
		txa
		jsr	uart_tx_char
		jmp	main_loop


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


uart_rx_char:	php
		sei
		; enable keyboard via latch
		lda	#$03				; stop Auto scan
		sta	sheila_SYSVIA_orb			; by writing to system VIA PORT B
		lda	#$7f				; set bits 0 to 6 of port A to input on bit 7
							; output on bits 0 to 6
		sta	sheila_SYSVIA_ddra		; 

		sec
		lda	#$7F
		sta	sheila_SYSVIA_ora_nh
		lda	sheila_SYSVIA_ora_nh
		bpl	@sk

	
		lda	#$0E
		sta	sheila_SYSVIA_ora_nh
		lda	sheila_SYSVIA_ora_nh
		rol	A
		ror	zp_tmp1
		lda	#$1E
		sta	sheila_SYSVIA_ora_nh
		lda	sheila_SYSVIA_ora_nh
		rol	A
		ror	zp_tmp1
		lda	#$2E
		sta	sheila_SYSVIA_ora_nh
		lda	sheila_SYSVIA_ora_nh
		rol	A
		ror	zp_tmp1
		lda	#$3E
		sta	sheila_SYSVIA_ora_nh
		lda	sheila_SYSVIA_ora_nh
		rol	A
		ror	zp_tmp1
		lda	#$4E
		sta	sheila_SYSVIA_ora_nh
		lda	sheila_SYSVIA_ora_nh
		rol	A
		ror	zp_tmp1
		lda	#$5E
		sta	sheila_SYSVIA_ora_nh
		lda	sheila_SYSVIA_ora_nh
		rol	A
		ror	zp_tmp1
		lda	#$6E
		sta	sheila_SYSVIA_ora_nh
		lda	sheila_SYSVIA_ora_nh
		rol	A
		ror	zp_tmp1
		lda	#$7E
		sta	sheila_SYSVIA_ora_nh
		lda	sheila_SYSVIA_ora_nh
		rol	A
		ror	zp_tmp1


		; ack received char
		lda	#$0A
		sta	sheila_SYSVIA_ora_nh

		lda	#$0B
		sta	sheila_SYSVIA_orb
		lda	#0
		sta	sheila_SYSVIA_ddra

		lda	zp_tmp1

		plp
		clc
		rts

@sk:		lda	#$0B
		sta	sheila_SYSVIA_orb
		lda	#0
		sta	sheila_SYSVIA_ddra
		plp
		sec
		rts


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

		; wait for TXE
		lda	#$6f
		sta	sheila_SYSVIA_ora_nh
SendWt:
		lda	sheila_SYSVIA_ora_nh
		bpl	SendWt

		lda	#%01101000
		lsr	zp_tmp1
		ror	A
		lsr	zp_tmp1
		ror	A
		ror	A
		sta	sheila_SYSVIA_ora_nh
		dec	sheila_SYSVIA_ora_nh

		lda	#%11010000
		lsr	zp_tmp1
		ror	A
		lsr	zp_tmp1
		ror	A
		lsr	zp_tmp1
		ror	A
		ror	A
		sta	sheila_SYSVIA_ora_nh
		dec	sheila_SYSVIA_ora_nh

		lda	#%11010000
		lsr	zp_tmp1
		ror	A
		lsr	zp_tmp1
		ror	A
		lsr	zp_tmp1
		ror	A
		ror	A
		sta	sheila_SYSVIA_ora_nh
		dec	sheila_SYSVIA_ora_nh

		lda	#$0A				; ack char
		sta	sheila_SYSVIA_ora_nh

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
str_message:	.byte 	"Type something...",13,10,0


		.END
