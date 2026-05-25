\\ a(n) = Sum_{0 = x_0 <= x_1 <= x_2 <= x_3 = n} Product_{k=0..2} (x_k + 1) * binomial(3*x_{k+1} - 2*x_k + 1,x_{k+1} - x_k)/(3*x_{k+1} - 2*x_k + 1).
ista(nn, k=2, m=3) = {
  my(T=matrix(nn+1, nn+1, r, c, my(xr=r-1, xc=c-1); if(xc<xr, 0, (xr+1)*binomial(m*xc-(m-1)*xr+1, xc-xr)/(m*xc-(m-1)*xr+1))));
  my(TK=T^k);
  TK[1, ];
};
lista(22)

