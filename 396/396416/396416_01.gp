lista(nn, k=2, m=3) = {
  my(T=matrix(nn+1, nn+1, r, c, my(xr=r-1, xc=c-1); if(xc<xr, 0, (xr+1)*binomial(m*xc-(m-1)*xr+1, xc-xr)/(m*xc-(m-1)*xr+1))));
  my(TK=T^k);
  TK[1, ];
};
lista(22)

\\ a(n) = Sum_{k=0..n} (k+1) * binomial(4*k+1,k) * binomial(4*n-3*k+1,n-k)/((4*k+1) * (4*n-3*k+1)).
a(n) = sum(k=0, n, (k+1) * binomial(4*k+1,k) * binomial(4*n-3*k+1,n-k)/((4*k+1) * (4*n-3*k+1)));
for(n=0, 22, print1(a(n),", "));