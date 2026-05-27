lista(nn, k=3, p=4) = {
  my(T=matrix(nn+1, nn+1, row, col, my(xr=row-1, xc=col-1); if(xc<xr, 0, (xr+1)*binomial(p*xc-(p-1)*xr+1, xc-xr)/(p*xc-(p-1)*xr+1))));
  my(TK=T^k);
  TK[1, ];
};
lista(22)

