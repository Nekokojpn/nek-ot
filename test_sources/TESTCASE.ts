cd current
echo "int_mul_definition_test"
assert 10 => %f "def_int.nk"
echo "function_definition_test"
assert 840 => %f "def_func.nk"
echo "hello_world_test"
assert "Hello_nek-ot!" => %f "helloworld.nk" -stdout
echo "if_test"
assert 4 => %f "if.nk"
echo "while_test"
assert 1000000000 => %f "while.nk"
echo "fibonacci_test"
assert 0 => %f "fibo.nk"
end
