\\ a(n): 最小成分sの重複度mが成分であり、かつ m > s である分割の個数
a_m_gt_s(n) = {
  my(x = 'x + O('x^(n+1)));
  \\ 全分割の母関数 P(x) を事前に計算
  my(P = 1 / prod(k=1, n, 1 - x^k));
  my(curr_prod = 1);
  my(res = 0);
  
  \\ sに関する和。 (s+1)^2 > n になると項が0になるため sqrt(n) までで十分
  for(s=1, sqrtint(n+1),
    \\ 1からsまでの (1-x^j) の積を更新保持
    curr_prod *= (1 - x^s);
    
    \\ 公式: T_s = x^((s+1)^2) / (1-x^(s+1)) * P(x) * (1-x)(1-x^2)...(1-x^s)
    res += x^((s+1)^2) * curr_prod / (1 - x^(s+1));
  );
  
  res *= P;
  return(res);
}

M = 1000;
res = a_m_gt_s(M);
\\ 結果の保存
for(n=1, M, write("b394291_1.txt", n, " ", polcoef(res, n)));