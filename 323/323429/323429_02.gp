default(parisize, 120000000);

\\ G.f.: 1 + Sum_{k>=1, l>=1} q^(k*l) * Product_{i=1..k} Product_{j=1..l} 1/(1 - q^(i+j-1)).
v(n) =
{
  if(type(n) != "t_INT" || n < 0, error("n must be a nonnegative integer"));
  my(q = 'q + O('q^(n + 1)), total = 1, rect);
  for(k = 1, n, rect = 1 + O('q^(n + 1)); for(l = 1, n\k, rect *= q^k; for(i = 1, k, rect /= (1 - q^(l + i - 1))); total += rect));
  total
};

M=1000;
v=v(M);
for(n=0, M, write("b323429_1.txt", n, " ", polcoef(v, n)));
