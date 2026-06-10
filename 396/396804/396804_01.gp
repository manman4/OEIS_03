\\ E.g.f. A(x) satisfies A(x) = x*exp(A^l(x)), where A^l(x) denotes the l-th iterate of A.
\\ Let a(n,k,l) = n! * [x^n] A^k(x), where A^k(x) is the k-th iterate of A.
\\ a(1,k,l) = 1; a(n,k,l) = (1/(n-1)) * Sum_{i=1..n-1} i * binomial(n,i) * a(n-i,k,l) * Sum_{j=1..k} a(i,j+l-1,l).
a(n, k, l) = if(n==1, 1, 1/(n-1) * sum(i=1, n-1, i * binomial(n,i) * a(n-i, k, l) * sum(j=1, k, a(i, j+l-1, l))) );


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

b(n, k, l) =
{
  my(A = Aseries(l, n));
  n! * polcoef(iter(A, k, n), n)
};

for(n=1, 9, print1(a(n, 1, 4), ", "));
for(n=1, 8, print1(b(n, 0, 4) - a(n, 0, 4), ", "));
for(n=1, 8, print1(b(n, 1, 4) - a(n, 1, 4), ", "));
for(n=1, 8, print1(b(n, 2, 4) - a(n, 2, 4), ", "));
for(n=1, 8, print1(b(n, 3, 4) - a(n, 3, 4), ", "));
for(n=1, 8, print1(b(n, 4, 4) - a(n, 4, 4), ", "));
for(n=1, 8, print1(b(n, 5, 4) - a(n, 5, 4), ", "));
for(n=1, 8, print1(b(n, 6, 4) - a(n, 6, 4), ", "));
for(n=1, 8, print1(b(n, 7, 4) - a(n, 7, 4), ", "));
