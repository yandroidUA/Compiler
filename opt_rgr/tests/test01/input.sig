PROGRAM PR;
   VAR A:INTEGER;
   B:FLOAT;
   C: [0 .. 10];
BEGIN
	C[10]:=A;
	A := C[10];
	LOOP 
		C[A]:=A;
		LOOP
			A:=C[A];
			A:=C[1];
		ENDLOOP;
		A:=C[1];
	ENDLOOP;
	A:=C[11];
END.