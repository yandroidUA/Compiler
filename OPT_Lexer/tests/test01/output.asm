DATA SEGMENT
	A dw ?
	B dw ?
	C dw 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
DATA ENDS
CODE SEGMENT
ASSUME CS:CODE, DS:DATA
PR PROC
		; C[10] := A
	MOV BX, 10
	MOV BP, A
	MOV C[BX], BP
		; A := C[10]
	MOV BP, 10
	MOV BP, C[BP]
	MOV A, BP
		; LOOP
?L0: nop
		; C[A] := A
	MOV BX, A
	MOV BP, A
	MOV C[BX], BP
		; LOOP
?L1: nop
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
		; A := C[11]
	MOV BP, 11
	MOV BP, C[BP]
	MOV A, BP
	nop
PR ENDP
CODE ENDS
END

