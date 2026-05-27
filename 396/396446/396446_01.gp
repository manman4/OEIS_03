\\ A(n,k) = Sum_{0 = x_0 <= x_1 <= ... <= x_{k-1} <= x_k = n} Product_{j=0..k-1} (2*x_j + 1) * binomial(3*x_{j+1} - x_j + 1,x_{j+1} - x_j)/(3*x_{j+1} - x_j + 1).
a(n, k, p=4, s=3, r=3) = {
  my(T=matrix(n+1, n+1, row, col, my(xr=row-1, xc=col-1); if(xc<xr, 0, (s*xr+r)*binomial(p*xc-(p-s)*xr+r, xc-xr)/(p*xc-(p-s)*xr+r))));
  my(TK=T^k);
  TK[1, n+1];
};
for(k=0, 15, print1(a(1, k),", "));
for(k=0, 15, print1(a(2, k),", "));
for(k=0, 15, print1(a(3, k),", "));
for(k=0, 15, print1(a(4, k),", "));
for(k=0, 15, print1(a(5, k),", "));

matrix(7, 7, n, k, a(n-1, k-1))
for(n=0, 10, for(k=0, n, print1(a(k, n-k), ", ")));

