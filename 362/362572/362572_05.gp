M=21;

a(n) = n! * sum(k=0, n\2, ((n-k)/2)^k * binomial(n-k-1,k)/(n-k)!);
for(n=0, M, print1(a(n), ", "))
