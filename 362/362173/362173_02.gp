M=22;

a(n) = n! * sum(k=0, n\3, (n/6)^k * binomial(n-2*k,k)/(n-2*k)!);
for(n=0, M, print1(a(n), ", ")); 
