PROGRAM PR;
   VAR A:INTEGER;
   B:FLOAT;
   C: [0 .. 10];
BEGIN
	C[10] := 0;
	( and this is incorrect comment to show that it can fail the programm *)
	LOOP 
		(* yes, it isn't work *)
		A := C[0];
	ENDLOOP;
END.