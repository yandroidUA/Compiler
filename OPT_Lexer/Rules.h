#pragma once
enum Rules{
	SIGNAL_PROGRAM = 1,
	PROGRAM_RULE = 2,
	BLOCK = 3,
	VARIABLE_DECLARATIONS = 4,
	DECLARATIONS_LIST = 5,
	DECLARATION = 6,
	ATTRIBUTES_LIST = 7,
	ATTRIBUTE = 8,
	RANGE = 9,
	STATEMENT_LIST = 10,
	STATEMENT = 11,
	EXPRESSION = 12,
	VARIABLE = 13,
	DIMENSION = 14,
	VARIABLE_IDENTIFIER = 15,
	PROCEDURE_IDENTIFIER = 16,
	IDENTIFIER_RULE = 17,
	STRING = 18,
	UNSIGNED_INTEGER = 19,
	EMPTY = 20,
	RANGE_SEPARATED_SYMBOL = 21,
	ADDING_RESERVED_WORD = 22,
	ADDING_INDENTIFIER = 23,
	ADDING_SEPARATED = 24,
	ADDING_CONSTANT = 25 
};

/*									RULES
1. <signal-program> --> <program>
2. <program> --> PROGRAM <procedure-identifier>;
<block>.
3. <block> --> <variable-declarations> BEGIN <statements-list> END
4. <variable-declarations> --> VAR <declarations-list>|<empty>
5. <declarations-list> --> <declaration><declarations-list>|<empty>
6. <declaration> --><variable-identifier>:<attribute><attributes-list> ;
7. <attributes-list> --> <attribute> <attributes-list> | <empty>
8. <attribute> --> INTEGER | FLOAT | [<range>]
9. <range> --> <unsigned-integer> .. <unsigned-integer>
10. <statements-list> --> <statement> <statements-list> | <empty>
11. <statement> --> <variable> := <expression> ; | LOOP <statements-list> ENDLOOP ;
12. <expression> --> <variable> | <unsigned-integer>
13. <variable> --> <variable-identifier><dimension>
14. <dimension> --> [ <expression> ] | <empty>
15. <variable-identifier> --> <identifier>
16. <procedure-identifier> --> <identifier>
----------------------------------------------------------------------------------------------
17. <identifier> --> <letter><string>
18. <string> --> <letter><string> | <digit><string> | <empty>
19. <unsigned-integer> --> <digit><digits-string>
20. <digits-string> --> <digit><digits-string> | <empty>
21. <digit> --> 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
22. <letter> --> A | B | C | D | ... | Z
*/