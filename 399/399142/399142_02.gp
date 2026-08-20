\\ Let f(x)=x*exp(x), and let f^t(x) denote its formal t-th iterate under composition. f^t(x) = Sum_{n>=1} x^n/(n-1)! * Sum_{k=0..n-1} binomial(t,k) * T(n,k).

triangle_xexp_by_iteration(N) =
{
  my(f = x*exp(x + O(x^(N+2))), It = vector(N+1));
  It[1] = x + O(x^(N+2));
  for(m = 1, N, It[m+1] = subst(f, x, It[m]));

  for(n = 1, N,
    print("n=", n, ": ", vector(n, k,
      (n-1)! * sum(r = 0, k-1,
        (-1)^(k-1-r)*binomial(k-1,r)*polcoef(It[r+1],n))))
  )
};

triangle_xexp_by_iteration(9);






/* T(n,k) is stored as T[n][k+1]. */
triangle_xexp(N) =
{
  my(T = vector(N, n, vector(n)));
  T[1][1] = 1;
  for(n = 2, N,
    for(k = 1, n-1,
      T[n][k+1] = sum(j = k, n-1,
        j^(n-j)*binomial(n-1,j-1)*T[j][k])
    )
  );
  T
};

/* Reconstruct the right-hand side from T(n,k). */
formal_iterate_from_T(T, N) =
{
  my(x = 'x, t = 't);
  sum(n = 1, N,
    x^n/(n-1)! *
      sum(k = 0, n-1, T[n][k+1]*binomial(t,k))
  ) + O(x^(N+1))
};

/* Compare the reconstruction with f^r(x), r=0,...,N-1. */
check_formula(N) =
{
  my(
    x = 'x,
    t = 't,
    T = triangle_xexp(N),
    F = formal_iterate_from_T(T, N),
    f = x*exp(x + O(x^(N+1))),
    g = x + O(x^(N+1)),
    h
  );
  for(r = 0, N-1,
    h = subst(F, t, r);
    if(h != g,
      error(Str("Failed at t=", r))
    );
    g = subst(f, x, g);
  );
  print("OK through x^", N);
  F
};

for(n=1, 12, check_formula(n));
