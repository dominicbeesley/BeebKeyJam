		.include	"hardware.inc"
		.include	"oslib.inc"

		.import uart_tx_str
		.import uart_rx_char
		.import uart_tx_char


		.CODE 

start:		ldx	#<str_message
		ldy	#>str_message
		jsr	uart_tx_str

main_loop:	jsr	uart_rx_char
		bcs	@sk1
		jsr	OSASCI
@sk1:		ldx	#10
		ldy	#0
		lda	#OSBYTE_129_INKEY
		jsr	OSBYTE
		jmp	start




		.RODATA
str_message:	.byte 	"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ",13,10,0


		.END
