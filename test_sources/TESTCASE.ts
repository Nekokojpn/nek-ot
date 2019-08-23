cd current
echo "i32_test1"
assert 1 => %f "i32_test1.nk" -stdout
echo "i32_test2"
assert 100 => %f "i32_test2.nk" -stdout
echo "fibo35_test"
assert 9227465 => %f "fibo35.nk" -stdout
end
