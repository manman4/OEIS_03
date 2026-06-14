\\ E.g.f. F(x) satisfies F'(x) = exp(F^l(x)), where F^l(x) denotes the l-th iterate of F, with F(0) = 0.
\\ Let a(n,k,l) = n! * [x^n] F^k(x), where F^k(x) is the k-th iterate of F, and let b(n,k,l) = n! * [x^n] exp(F^k(x)).

iter(F, k, N) = {
  my(y = 'x + O('x^(N + 1)));
  if(k == 0, return(y));               
  for(i = 1, k, y = subst(F, 'x, y));
  y
}

Aseries(N, l = 3) = {
  my(A = 'x + O('x^(N + 1)));
  for(i = 1, N,
    A = intformal(exp(iter(A, l, N)));
  );
  A
}

a(n, k = 1, l = 3) = {
  my(A = Aseries(n, l));
  n! * polcoef(exp(iter(A, k, n)), n)
}

\\ Square array 
matrix(6, 6, n, k, a(n-1, k-1, 2))