\\ E.g.f. F(x) satisfies F(x) = x*exp(F^l(x)), where F^l(x) denotes the l-th iterate of F.
\\ Let b(n,k,l) = n! * [x^n] F^k(x), where F^k(x) is the k-th iterate of F.
\\ b(1,k,l) = 1; b(n,k,l) = (1/(n-1)) * Sum_{i=1..n-1} i * binomial(n,i) * b(n-i,k,l) * Sum_{j=1..k} b(i,j+l-1,l).
b(n, k, l) = if(n==1, 1, 1/(n-1) * sum(i=1, n-1, i * binomial(n,i) * b(n-i, k, l) * sum(j=1, k, b(i, j+l-1, l))) );


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

for(n=1, 8, print1(a(n, 1, 3), ", "));
for(n=1, 8, print1(b(n, 0, 3) - a(n, 0, 3), ", "));
for(n=1, 8, print1(b(n, 1, 3) - a(n, 1, 3), ", "));
for(n=1, 8, print1(b(n, 2, 3) - a(n, 2, 3), ", "));
for(n=1, 8, print1(b(n, 3, 3) - a(n, 3, 3), ", "));
for(n=1, 8, print1(b(n, 4, 3) - a(n, 4, 3), ", "));
for(n=1, 8, print1(b(n, 5, 3) - a(n, 5, 3), ", "));
for(n=1, 8, print1(b(n, 6, 3) - a(n, 6, 3), ", "));
for(n=1, 8, print1(b(n, 7, 3) - a(n, 7, 3), ", "));
