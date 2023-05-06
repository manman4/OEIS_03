M=17;

a(n) = (-1)^n * sum(k=0, n, (-2*k)^k * binomial(n,k));
for(n=0, M, print1(a(n),", "))
