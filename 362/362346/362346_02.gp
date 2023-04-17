M=24;

a(n) = n! * sum(k=0, n\5, (-n/120)^k /(k! * (n-5*k)!));
for(n=0, M, print1(a(n), ", ")); 
