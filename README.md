# nek-ot
nek-ot is a programming language.  

This is fibonacci example program.  
```
fn fibo(n : i32) -> i32{
	val := i32;
	if$ n < 2 
		ret n;
	else
		ret fibo$ n-1 + fibo$ n-2;
}
fn main() -> i32 {
	writefln$ "%d", fibo$35;
	ret res;
}
```
This is arr comprehension
```
fn main() -> i32{
	arr := i32[5]{x * 2 | _ <- i32[5]{0, 1, 2, 3, 4}};
	writefln$ %d\n%d\n%d\n%d\n%d\n, arr[0], arr[1], arr[2], arr[3], arr[4];
	ret 0;
}
```
This is Mandelbrot

'''
fn main(){
	MAX := i64$447LL;
	DN := i64$4LL;
	x := i64;
	y := i64;
	sx := i64;
	sy := i64;
	zx := i64;
	zy := i64;
	cx := i64;
	cy := i64;
	xx := i64;
	yy := i64;

	n := i32;
	sn := i32;
	h := i32$0;
	y = 0LL;
	while(y < 384){
		x = 0LL;
		while(x < 512){
			sn = 0;
			sx = 0LL;
			while(sx < DN){
				cx = (x * 4 + sx) * 56 + 4673536LL;
				sy = 0LL;
				while(sy < DN){
					cy = (y * 4 + sy) * 56 + 124928LL;
					zx = 0LL;
					zy = 0LL;
					n = 0;
					while(n < MAX){
						xx = (zx * zx) >> 24;
						yy = (zy * zy) >> 24;
						if(xx + yy > 67108864)goto jmp;
						zy = (zx * zy) >> 23;
						zx = xx - yy + cx;
						zy = zy - cy;
						n = n + 1;
					}
					@jmp:
					sn = sn + n;
					sy = sy + 1LL;
				}
				sx = sx + 1LL;
			}
			n = sn >> 4;
			h = h + n;
			x = x + 1LL;
		}
		y = y + 1LL;
	}
	writefln("%d", h);
	ret;
}
'''
