# NEK-OT PROGRAMMING LANGUAGE

fibonacci
```
fn fibo(n : i32) -> i32{
	if(n < 2) 
		ret n;
	else
		ret fibo(n-1) + fibo(n-2);
}
fn main() -> i32 {
	writefln("%d", fibo(35));
	ret 0;
}
```
