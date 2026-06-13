\\ E.g.f. A(x) satisfies A'(x) = 1 + A^5(x), where A^l(x) denotes the l-th iterate of A, with A(0) = 0.
iter(F, k, N) = {
  my(y = 'x + O('x^(N + 1)));
  if(k == 0, return(y));
  for(i = 1, k, y = subst(F, 'x, y));
  y
};

Aseries(N, l=5) = {
  my(A = 'x + O('x^(N + 1)));
  for(i = 1, N, A = intformal(1 + iter(A, l, N)));
  A
};

\\ A(x) = Series_Reversion( Integral 1/(1 + A^{l-1}(x)) dx ). for l > 0.
Bseries(N, l=5) = {
  my(A = Aseries(N, l));
  serreverse(intformal(1 / (1 + iter(A, l - 1, N))))
};
check_reversion(N, l=5) = {
  my(A = Aseries(N, l), B = Bseries(N, l));
  if(A == B, print("ok"), print([A, B]));
};


a_vector(20)

check_reversion(12, 5)
