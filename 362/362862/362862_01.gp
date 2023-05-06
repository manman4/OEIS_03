M=13;

a(n) = (-1)^n * sum(k=0, n, (-n*k)^k*binomial(n, k));
for(n=0, M, print1(a(n),", "))
