\\ a(n) = Sum_{0 = x_0 <= x_1 <= x_2 <= x_3 = n} Product_{k=0..2} (x_k + 1) * binomial(3*x_{k+1} - 2*x_k + 1,x_{k+1} - x_k)/(3*x_{k+1} - 2*x_k + 1).
lista(nn, k=3, l=3) = {
  my(T=matrix(nn+1, nn+1, row, col, my(xr=row-1, xc=col-1); if(xc<xr, 0, (xr+1)*binomial(l*xc-(l-1)*xr+1, xc-xr)/(l*xc-(l-1)*xr+1))));
  my(TK=T^k);
  TK[1, ];
};
lista(21)

