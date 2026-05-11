default(parisize, 120000000);

\\ G.f.: 1 + Sum_{k>=1, l>=1} q^(k*l*(k+l)/2) * Product_{i=1..k} Product_{j=1..l} 1/(1 - q^(i+j-1)).
strict_gf(max_n) =
{
  if(type(max_n) != "t_INT" || max_n < 0, error("max_n must be a nonnegative integer"));
  my(q = 'q + O('q^(max_n + 1)), total = 1 + O('q^(max_n + 1)), rect);
  for(k = 1, max_n,
    rect = 1 + O('q^(max_n + 1));
    for(l = 1, max_n,
      if(k*l*(k+l)/2 > max_n, break);
      rect *= q^(k*(k + 2*l - 1)/2);
      for(i = 1, k,
        rect /= (1 - q^(l + i - 1));
      );
      total += rect;
    );
  );
  total
};

M = 5000;
gf = strict_gf(M);
for(n = 0, M, write("b323430_1.txt", n, " ", polcoef(gf, n)));
