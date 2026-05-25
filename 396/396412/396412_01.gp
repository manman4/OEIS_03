\\ A(n,k) = Sum_{0 = x_0 <= x_1 <= ... <= x_{k-1} <= x_k = n} Product_{j=0..k-1} (x_j + 1) * binomial(3*x_{j+1} - 2*x_j + 1,x_{j+1} - x_j)/(3*x_{j+1} - 2*x_j + 1).
a(n, k, m=3) = {
  my(T=matrix(n+1, n+1, r, c, my(xr=r-1, xc=c-1); if(xc<xr, 0, (xr+1)*binomial(m*xc-(m-1)*xr+1, xc-xr)/(m*xc-(m-1)*xr+1))));
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

