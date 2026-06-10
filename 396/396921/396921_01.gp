default(parisize, 120000000);

iter(F, k, N) =
{
  my(y = 'x + O('x^(N + 1)));
  if (k == 0, return(y));
  for (i = 1, k, y = subst(F, 'x, y));
  y
};

Aseries(l, N) =
{
  my(A = 'x + O('x^(N + 1)));
  for (t = 1, N, A = 'x * exp(iter(A, l, N)));
  exp(A)
};

v = Aseries(4, 110);
for(n=0, 100, write("b396922.txt", n, " ", n!*polcoef(v, n)));
