#include <iostream>
#include <stdio.h>
void calendar(int year, int month){
	auto month0 = month--;
	int t0[] = {1,4,3,6,1,4,6,2,5,0,3,5};
	int t1[] = {31,28,31,30,31,30,31,31,30,31,30,31};

	if(year % 400 == 0)
		t1[1] = 29;
	else if(year % 4 == 0 && year % 100 != 0)
		t1[1] = 29;
	
	std::string mstr[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	std::cout << mstr[month] << year << std::endl;
	std::cout << "Sun Mon Tue Wed Thu Fri Sat" << std::endl;
	int w;
	if(month <= 1) w = year - 1; else w = year;
	int w0 = w / 4;
	int w1 = w / 100;
	int w2 = w / 400;
	int w3 = t0[month];
	w = (w + w0 - w1 + w2 + w3) % 7;

	if(w > 0)
		for(int i = 1; i <= w; i++)
			std::cout << "    ";
	
	int d1 = t1[month];
	for(int j = 1; j <= d1; j++){
		printf("%4d", j);
		w = (w + 1) % 7;
		if(w == 0)std::cout << std::endl;
	}
	if(w > 0)
		std::cout << std::endl;
	month++;

	return;
}
int main(){
	calendar(2020, 2);
	return 0;
}