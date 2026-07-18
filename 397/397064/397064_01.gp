\\ G.f. A(x) satisfies (1-2*x)*A(x) + A(x^3) = x.
A(N) =
{
  my(x = 'x, A = 0 + O(x^(N + 1)), q = 1);
  while(q <= N,
    A = (x - subst(A, x, x^3))/(1 - 2*x) + O(x^(N + 1));
    q *= 3
  );
  A
};

A(35)
Vec(A(35))
