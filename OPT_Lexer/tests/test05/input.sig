PROGRAM PR;
   VAR A:INTEGER;
   VAR B:FLOAT;
   B:FLOAT;
   C: [0 .. 10];
BEGIN
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