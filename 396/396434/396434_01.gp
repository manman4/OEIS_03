lista(nn, k=2, p=4, s=3, r=1) = {
  my(T=matrix(nn+1, nn+1, row, col, my(xr=row-1, xc=col-1); if(xc<xr, 0, (s*xr+r)*binomial(p*xc-(p-s)*xr+r, xc-xr)/(p*xc-(p-s)*xr+r))));
  my(TK=T^k);
  TK[1, ];
};
lista(21)

\\ a(n) = Sum_{k=0..n} (3*k+1) * binomial(4*k+1,k) * binomial(4*n-k+1,n-k)/((4*k+1) * (4*n-k+1)).
a(n) = sum(k=0, n, (3*k+1) * binomial(4*k+1,k) * binomial(4*n-k+1,n-k)/((4*k+1) * (4*n-k+1)));
for(n=0, 21, print1(a(n),", "));