\\ A(n,k) = n! * [x^n] F_k(x)/x and F_k(x) is the k-th iteration of x*G(x) with G(x) = exp(x*G(x)^4).
a(n, k, p=4, s=1, r=1) = {
  my(T=matrix(n+1, n+1, row, col, my(xr=row-1, xc=col-1); if(xc<xr, 0, (s*xr+r)*(p*xc-(p-s)*xr+r)^(xc-xr-1)*binomial(xc, xr))));
  my(TK=T^k);
  TK[1, n+1];
};
for(k=0, 15, print1(a(1, k),", "));
for(k=0, 15, print1(a(2, k),", "));
for(k=0, 15, print1(a(3, k),", "));
for(k=0, 15, print1(a(4, k),", "));
for(k=0, 15, print1(a(5, k),", "));

matrix(6, 6, n, k, a(n-1, k-1))
for(n=0, 10, for(k=0, n, print1(a(k, n-k), ", ")));

