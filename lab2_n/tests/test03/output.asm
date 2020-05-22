DATA SEGMENT
	A dw ?
	B dw ?
	C dw 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
DATA ENDS
CODE SEGMENT
ASSUME CS:CODE, DS:DATA
PR PROC
		; C[A] := C[C[C[A]]]
	MOV BX, A
	MOV BP, A
	MOV BP, C[BP]
	MOV BP, C[BP]
	MOV BP, C[BP]
	MOV C[BX], BP
		; LOOP
?L0: nop
		; LOOP
?L1: nop
		; LOOP
?L2: nop
		; LOOP
?L3: nop
		; LOOP
?L4: nop
		; LOOP
?L5: nop
		; LOOP
?L6: nop
		; C[C[C[C[C[C[C[C[A]]]]]]]] := C[C[C[C[C[C[10]]]]]]
	MOV BX, A
	MOV BX, C[BX]
	MOV BX, C[BX]
	MOV BX, C[BX]
	MOV BX, C[BX]
	MOV BX, C[BX]
	MOV BX, C[BX]
	MOV BX, C[BX]
	MOV BP, 10
	MOV BP, C[BP]
	MOV BP, C[BP]
	MOV BP, C[BP]
	MOV BP, C[BP]
	MOV BP, C[BP]
	MOV BP, C[BP]
	MOV C[BX], BP
	nop
		; ENDLOOP
	JMP ?L6
	nop
	nop
		; ENDLOOP
	JMP ?L5
	nop
	nop
		; ENDLOOP
	JMP ?L4
	nop
	nop
		; ENDLOOP
	JMP ?L3
	nop
	nop
		; ENDLOOP
	JMP ?L2
	nop
	nop
		; ENDLOOP
	JMP ?L1
	nop
	nop
		; ENDLOOP
	JMP ?L0
	nop
	nop
PR ENDP
CODE ENDS
END

