M=20;

a(n) = n! * sum(k=0, n\2, (-1)^k * (n-k)^k * binomial(n-k-1,k)/(n-k)!);
for(n=0, M, print1(a(n), ", "))
