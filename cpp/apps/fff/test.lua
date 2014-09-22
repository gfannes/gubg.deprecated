function check(wanted, actual)
	if (wanted ~= actual) then
		print("Failure");
	else
		print("Test passed.");
	end
end
check(false, fff_board_add());
check(false, fff_board_add(0));
check(false, fff_board_add(0, 1));
check(true, fff_board_add("c++.include_path", {file="test1234"}));
check(true, fff_board_add("start", {str="fff.cpp"}));
