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
