\\ G.f. A(x) satisfies A(x) = A(x^3) / (1-4*x), with A(0) = 1.
A(N) =
{
  my(x = 'x, A = 1 + O(x^(N + 1)), q = 1);
  while(q <= N,
    A = subst(A, x, x^3)/(1 - 4*x) + O(x^(N + 1));
    q *= 3
  );
  A
};

A(25)
Vec(A(25))
