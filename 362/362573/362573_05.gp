M=24;

a(n) = n! * sum(k=0, n\3, ((n-2*k)/6)^k * binomial(n-2*k-1,k)/(n-2*k)!);
for(n=0, M, print1(a(n), ", "))
