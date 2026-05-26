lista(n, k=2, l=3, p=2, r=1) = {
  my(T=matrix(n+1, n+1, row, col, my(xr=row-1, xc=col-1); if(xc<xr, 0, (p*xr+r)*binomial(l*xc-(l-p)*xr+r, xc-xr)/(l*xc-(l-p)*xr+r))));
  my(TK=T^k);
  TK[1, ];
};
lista(21)

\\ a(n) = Sum_{k=0..n} (2*k+1) * binomial(3*k+1,k) * binomial(3*n-k+1,n-k)/((3*k+1) * (3*n-k+1)).
a(n) = sum(k=0, n, (2*k+1) * binomial(3*k+1,k) * binomial(3*n-k+1,n-k)/((3*k+1) * (3*n-k+1)));
for(n=0, 21, print1(a(n),", "));