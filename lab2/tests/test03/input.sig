PROGRAM PR;
   VAR A:INTEGER;
   B:FLOAT;
   C: [0 .. 10];
BEGIN
	C[A] := C[C[C[A]]];
	LOOP
		LOOP
			LOOP
				LOOP
					LOOP
						LOOP
							LOOP
								C[C[C[C[C[C[C[C[A]]]]]]]] := C[C[C[C[C[C[10]]]]]];
							ENDLOOP;
						ENDLOOP;
					ENDLOOP;
				ENDLOOP;
			ENDLOOP;
		ENDLOOP;
	ENDLOOP;
END.