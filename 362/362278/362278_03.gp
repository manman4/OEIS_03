M=23;

a(n) = n! * sum(k=0, n\2, (-3/2)^k * binomial(n-k,k)/(n-k)!);
for(n=0, M, print1(a(n), ", ")); 