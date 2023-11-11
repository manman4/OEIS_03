a(n, s=3, t=3, u=2) = sum(k=0, n, binomial(t*k+u*(n-k)+1, k)*binomial(n+(s-1)*k-1, n-k)/(t*k+u*(n-k)+1));
for(n=0, 20, print1(a(n), ", "))    