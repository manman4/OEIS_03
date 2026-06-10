


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
  A
};

a(n, k, l) =
{
  my(A = Aseries(l, n));
  n! * polcoef(iter(A, k, n), n)
};

for(n=1, 9, print1(a(n, 1, 5), ", "));
