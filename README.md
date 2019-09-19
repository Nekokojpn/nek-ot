# nek-ot
nek-ot is a programming language.  

This is fibonacci example program.  
```
fn fibo(i32 n) -> i32{
	val := i32();
	if(n < 2){
		val = n;
	}
	else {
		val = fibo(n-1) + fibo(n-2);
	}
	ret val;
}
fn main() -> i32 {
	writefln("%d", fibo(35));
	ret res;
}
```
This is arr comprehension
```
fn main() -> i32{
	arr := i32[5]{x * 2 | _ <- i32[5]{0, 1, 2, 3, 4}};
	writefln(%d\n%d\n%d\n%d\n%d\n, arr[0], arr[1], arr[2], arr[3], arr[4]);
	ret 0;
}
```
