\\ G.f. A(x) satisfies A(x) = A(x^2) / (1-4*x), with A(0) = 1.
A(N) =
{
  my(x = 'x, A = 1 + O(x^(N + 1)), q = 1);
  while(q <= N,
    A = subst(A, x, x^2)/(1 - 4*x) + O(x^(N + 1));
    q *= 2
  );
  A
};

A(25)
Vec(A(25))
