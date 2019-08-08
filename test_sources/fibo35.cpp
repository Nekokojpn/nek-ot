fn fibo(i32 n) -> i32 {
	i32 val = 0;
	if (n < 2) {
		val = n;
	}
	else {
		val = fibo(n - 1) + fibo(n - 2);
	}
	ret val;
}
fn main() -> void {
	i32 i = 1;
	while (i < 35) {
		string f = "fibo(%d) -> %d";
		writefln(f, i, fibo(i));
		i = i + 1;
	}
	ret;
}