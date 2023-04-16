M=18;

a(n) = n! * sum(k=0, n\5, (-n)^k /(k! * (n-5*k)!));
for(n=0, M, print1(a(n), ", ")); 
