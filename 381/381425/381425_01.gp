M=11;

a(n, k) = sum(j=0, k, binomial(k, j)*binomial(n+(k-1)*j-1, n-j));
for(n=0, M, for(k=0, n, print1(a(k, n-k)", ")));






