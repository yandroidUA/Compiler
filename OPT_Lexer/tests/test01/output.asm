DATA SEGMENT
	A dw ?
	B dd ?
	C dw 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
DATA ENDS
CODE SEGMENT
ASSUME CS:CODE, DS:DATA
PR PROC
		; C := A
	MOV BP, A
	MOV C, BP
		; LOOP
?L0: nop
		; C := A
	MOV BP, A
	MOV C, BP
		; LOOP
?L1: nop
		; C := A
	MOV BP, A
	MOV C, BP
		; A := C
	MOV BP, C
	MOV A, BP
		; A := C[A]
	MOV BP, A
	MOV BP, C[BP]
	MOV A, BP
		; A := C[1]
	MOV BP, 1
	MOV BP, C[BP]
	MOV A, BP
	nop
		; ENDLOOP
	JMP ?L1
	nop
		; A := C[1]
	MOV BP, 1
	MOV BP, C[BP]
	MOV A, BP
	nop
		; ENDLOOP
	JMP ?L0
	nop
		; A := C[1]
	MOV BP, 1
	MOV BP, C[BP]
	MOV A, BP
	nop
PR ENDP
CODE ENDS
END

