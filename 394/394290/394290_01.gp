default(parisize, 120000000)

\\ a(n): 最大成分gの重複度mが成分であり、かつ m < g である分割の個数
a_m_lt_g(n_max) = {
  my(x = 'x + O('x^(n_max + 1)));
  my(res_ser = 0);
  my(P_inv = 1); \\ 1 / prod_{j=1}^{g-1} (1-x^j) を保持

  for(g = 1, n_max,
    if(g > 1,
      my(q = x^(g+1));
      \\ Sum_{m=1}^{g-1} q^m を等比級数で計算
      my(inner_sum = q * (1 - q^(g-1)) / (1 - q));
      res_ser += P_inv * inner_sum;
    );
    
    \\ P_inv を 1 / prod_{j=1}^{g} (1-x^j) に更新
    P_inv /= (1 - x^g);
    
    \\ 現在追加した項の最小次数は g+1 なので、n_max を超えたら終了
    if(g + 1 > n_max, break);
  );
  return(res_ser);
}

M = 1000;
res = a_m_lt_g(M);
\\ 結果の保存
for(n=1, M, write("b394290_1.txt", n, " ", polcoef(res, n)));