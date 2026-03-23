default(parisize, 1200000000)

a(n) = {
  my(n=M, q='q+O('q^(n+1)));
  my(result = sum(j=1, sqrtint(n),
    \\ 第1項: パーツkが少なくともj回現れるケースの積
    my(P1 = q^j^2 / (1-q^j));
    for(k=1, n\j, 
      if(k == j, next);
      P1 *= (1 + q^(j*k) / (1-q^k))
    );
    
    \\ 第2項: 第1項のqの指数をj+1に置き換えたもの
    my(P2 = 0);
    if(j*(j+1) <= n,
      P2 = q^(j*(j+1)) / (1-q^j);
      for(k=1, n\(j+1),
        if(k == j, next);
        P2 *= (1 + q^((j+1)*k) / (1-q^k))
      )
    );
    
    P1 - P2
  ));
  return(result);
}

M = 10000;
res = a(M);
\\ 結果の保存
for(n=0, M, write("b365614_1.txt", n, " ", polcoef(res, n)));


