M=22;

a(n) = n! * sum(k=0, n\2, (-5/2)^k * binomial(n-k,k)/(n-k)!);
for(n=0, M, print1(a(n), ", ")); 