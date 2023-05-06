M=16;

a(n) = (-1)^n * sum(k=0, n, (-3*k)^k * binomial(n,k));
for(n=0, M, print1(a(n),", "))
