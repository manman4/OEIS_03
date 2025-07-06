M=17;

a(n) = sum(k=0, n, (3*n-3*k+1)^(k-1)*(-3*k)^(n-k)*binomial(n, k));
for(n=0, M, print1(a(n),", "))  


