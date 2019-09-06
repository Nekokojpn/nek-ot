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
