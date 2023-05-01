M=18;

a(n) = sum(k=0, n, (-k+1)^(n-1) * binomial(n,k));
for(n=0, M, print1(a(n), ", "))
