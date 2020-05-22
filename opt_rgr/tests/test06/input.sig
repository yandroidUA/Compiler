PROGRAM PR;
   VAR A:INTEGER;
   B:FLOAT;
   C: [0 .. 10];
BEGIN
	C[10] := 0;
	(* this is correct test to show that comment cannot fail program *)
	LOOP 
		(* yes, it's works *)
		A := C[0];
	ENDLOOP;
END.