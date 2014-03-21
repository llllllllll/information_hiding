	SECTION .data
str:	db "Hello world!",10
len:	equ $-str

	SECTION .text

        global main
main:
	mov	edx,len
	mov	ecx,str
	mov	ebx,1
	mov	eax,4

	int	0x80
	mov	ebx,0
	mov	eax,1
	int	0x80
