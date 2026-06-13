\\ E.g.f. A(x) satisfies A'(x) = 1 + A^5(x), where A^l(x) denotes the l-th iterate of A, with A(0) = 0.
iter(F, k, N) = {
  my(y = 'x + O('x^(N + 1)));
  if(k == 0, return(y));
  for(i = 1, k, y = subst(F, 'x, y));
  y
}

Aseries(N, l=5) = {
  my(A = 'x + O('x^(N + 1)));
  for(i = 1, N, A = intformal(1 + iter(A, l, N)));
  A
}

a_vector(N, l=5) = {
  my(A = Aseries(N, l));
  vector(N, n, n! * polcoef(A, n))
}
a_vector(20)
