M=17;

a(n) = sum(k=0, n, (2*n-2*k+1)^(k-1)*(-2*k)^(n-k)*binomial(n, k));
for(n=0, M, print1(a(n),", "))  

