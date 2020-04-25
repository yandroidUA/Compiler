PROGRAM PR;
   VAR A:INTEGER;
   (* here program must fail, because only one VAR declaration is expected *)
   VAR B:FLOAT;
   C: [0 .. 10];
BEGIN
END.