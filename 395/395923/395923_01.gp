default(parisize, 120000000);

\\ T(n,k) = [x^n] Sum_{l>=1} q^(k*l) * Product_{i=1..k} Product_{j=1..l} 1/(1 - q^(i+j-1))
column_gf(max_n, k) =
{
  if(type(max_n) != "t_INT" || max_n < 0, error("max_n must be a nonnegative integer"));
  if(type(k) != "t_INT" || k < 1, error("k must be a positive integer"));
  my(q = 'q + O('q^(max_n + 1)), total = O('q^(max_n + 1)), rect = 1 + O('q^(max_n + 1)));
  for(l = 1, max_n\k,
    rect *= q^k;
    for(i = 1, k,
      rect /= (1 - q^(l + i - 1));
    );
    total += rect;
  );
  total
};

M=140;
cols=vector(M, k, column_gf(M, k));
cnt=1;
for(n=1, M, for(k=1, n, write("b395923_1.txt", cnt, " ", polcoef(cols[k], n)); cnt++));



