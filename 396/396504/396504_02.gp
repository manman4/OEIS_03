\\ Square array A(n,k), n>=0, k>=0, read by antidiagonals downwards, where A(n,k) = n! * [x^n] (F_k(x)/x)^(r/s) and F_k(x) is the k-th iteration of x*E(x)^s with E(x) = exp(x*E(x)^p).
Egen(n,k,p=4,s=2,r=2) = {
  my(N = n+1, x = 'x + O('x^(N+1)));
  my(g = 1 + sum(j = 1, N, (p*j+1)^(j-1)/j! * x^j));
  my(F_iter = x);

  for(i = 1, k, F_iter = subst(x * g^s, 'x, F_iter));

  n! * polcoef((F_iter/x)^(r/s), n)
};
matrix(7, 7, n, k, Egen(n-1, k-1))


\\ E.g.f. of column k:
\\ Sum_{n>=0} A(n,k) * x^n / n! = (Series_Reversion(H_k(x)) / x)^(r/s),
\\ where H_k(x) is the k-th iterate of F^(-1)(x) = U(x)*exp(-p*U(x)),
\\ and
\\   U(x) = -LambertW(-(p-s)*x) / (p-s)   if p != s,
\\   U(x) = x                               if p = s.
Tgen(n,k,p=4,s=2,r=2) = {
  my(N = n+1, x = 'x + O('x^(N+1)));
  \\ U = -LambertW(-(p-s)*x)/(p-s), or U = x when p = s
  my(U = if(p == s, x, -lambertw(-(p-s)*x)/(p-s)));
  my(Finv = U * exp(-p*U));
  my(Finv_k = x);

  for(i = 1, k, Finv_k = subst(Finv, 'x, Finv_k));

  n! * polcoef((serreverse(Finv_k)/x)^(r/s), n)
};
matrix(7, 7, n, k, Tgen(n-1, k-1))
