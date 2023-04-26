M=21;

a(n) = n! * sum(k=0, n\3, (-1)^k * (n-2*k)^k * binomial(n-2*k-1,k)/(n-2*k)!);
for(n=0, M, print1(a(n), ", "))
