a(n, s=4, t=2, u=1) = sum(k=0, n, binomial(t*k+u*(n-k)+1, k)*binomial(n+(s-1)*k-1, n-k)/(t*k+u*(n-k)+1));
for(n=0, 21, print1(a(n), ", "))    