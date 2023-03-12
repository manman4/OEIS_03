M=18;

a(n) = if(n==0, 0, round(((n+sqrt(n))^n-(n-sqrt(n))^n)/(2*sqrt(n))));
for(n=0, M, print1(a(n), ", "));


