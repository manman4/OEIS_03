lista(nn, k=2, p=4, s=1, r=1) = {
  my(T=matrix(nn+1, nn+1, row, col, my(xr=row-1, xc=col-1); if(xc<xr, 0, (s*xr+r)*(p*xc-(p-s)*xr+r)^(xc-xr-1)*binomial(xc, xr))));
  my(TK=T^k);
  TK[1, ];
};
lista(17)

\\ a(n) = Sum_{k=0..n} (k+1) * (4*k+1)^(k-1) * (4*n-3*k+1)^(n-k-1) * binomial(n,k).
a(n) = sum(k=0, n, (k+1) * (4*k+1)^(k-1) * (4*n-3*k+1)^(n-k-1) * binomial(n,k));
for(n=0, 21, print1(a(n),", "));