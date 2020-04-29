DATA SEGMENT
	A dw ?
	B dw ?
	C dw 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
DATA ENDS
CODE SEGMENT
ASSUME CS:CODE, DS:DATA
PR PROC
		; C[10] := 0
	MOV BX, 10
	MOV BP, 0
	MOV C[BX], BP
		; LOOP
?L0: nop
		; A := C[0]
	MOV BP, 0
	MOV BP, C[BP]
	MOV A, BP
	nop
		; ENDLOOP
	JMP ?L0
	nop
	nop
PR ENDP
CODE ENDS
END

