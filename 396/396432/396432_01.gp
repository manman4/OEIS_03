lista(nn, k=2, l=3) = {
  my(T=matrix(nn+1, nn+1, row, col, my(xr=row-1, xc=col-1); if(xc<xr, 0, (xr+1)*binomial(l*xc-(l-1)*xr+1, xc-xr)/(l*xc-(l-1)*xr+1))));
  my(TK=T^k);
  TK[1, ];
};
lista(22)

\\ a(n) = Sum_{k=0..n} (k+1) * binomial(3*k+1,k) * binomial(3*n-2*k+1,n-k)/((3*k+1) * (3*n-2*k+1)).
a(n) = sum(k=0, n, (k+1) * binomial(3*k+1,k) * binomial(3*n-2*k+1,n-k)/((3*k+1) * (3*n-2*k+1)));
for(n=0, 22, print1(a(n),", "));