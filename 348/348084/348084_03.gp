M=15;

a(n) = sum(k=0, 2*n, (-1)^k*k^(3*n)*binomial(2*n, k))/(2*n)!;
for(n=0, M, print1(a(n), ", "));