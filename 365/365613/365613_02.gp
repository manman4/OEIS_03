default(parisize, 12000000000)

a(n) = {
  my(n=M, q='q+O('q^(n+1)));
  \\ 1. 基本となる母関数（1/prod(1-q^k)）だけを保持
  my(InvPhi = 1 / prod(k=1, n, 1-q^k));
  my(G = 0);

  for(j=1, n,
    \\ 2. 必要な P_m をその場で計算（prod の範囲が N\m なので非常に速い）
    my(Pm_next = InvPhi * prod(k=1, n\(j+1), 1-q^((j+1)*k)));
    my(Term_next = Pm_next * (q^j - q^(j*(j+1))) / (1 - q^(j*(j+1))));
    
    my(Term_curr = 0);
    if(j > 1,
      my(Pm_curr = InvPhi * prod(k=1, n\j, 1-q^(j*k)));
      Term_curr = Pm_curr * (q^j - q^j^2) / (1 - q^j^2)
    );
    
    G += (Term_next - Term_curr)
  );
  return(G);
}

M = 10000;
res = a(M);
\\ 結果の保存
for(n=0, M, write("b365613_1.txt", n, " ", polcoef(res, n)));


