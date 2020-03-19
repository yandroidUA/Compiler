PROGRAM PR;
   VAR A:INTEGER;
   B:FLOAT;
   C: [0 .. 10];
BEGIN
	VAR C:FLOAT;
	C:=A;
	LOOP 
		C:=A;
		LOOP
			C:=A;
			A:=C;
			A:=C[A];
			A:=C[1];
		ENDLOOP;
		A:=C[1];
	ENDLOOP;
	A:=C[1];
END.