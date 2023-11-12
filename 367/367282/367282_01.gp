a(n, s=2, t=2, u=2) = sum(k=0, n, binomial(t*k+u*(n-k)+1, k)*binomial(s*k, n-k)/(t*k+u*(n-k)+1));
for(n=0, 22, print1(a(n), ", "))    