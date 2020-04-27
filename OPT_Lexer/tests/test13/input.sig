PROGRAM PR;
	VAR A : INTEGER;
BEGIN
	LOOP (* here program must fail because after expression ';' expected *) A := 10 ENDLOOP;
END.
