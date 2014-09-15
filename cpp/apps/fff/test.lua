function check(wanted, actual)
	if (wanted != actual) then
		print("Failure")
	else
		print("Test passed.");
	end
end
check(false, fff_board_add());
check(false, fff_board_add(0));
check(true, fff_board_add(0, 1));
--fff_board_add({"c++", "include_path"}, {file: "blabla"})
fff_board_add({"c++", "include_path"}, {test="blabla"})
