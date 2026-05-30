\\ Expansion of e.g.f. F_2(x)/x and F_k(x) is the k-th iteration of x*G(x) with G(x) = exp(x*G(x)^4).
T3(n,k) = {
  my(N = n+1, x = 'x + O('x^(N+1)), g = sum(j = 0, N, (4*j+1)^(j-1)/j! * 'x^j));
  my(F_iter = x);
  for(i = 1, k, F_iter = subst(x * g, 'x, F_iter));
  F_iter/x
};

Vec(serlaplace(T3(21,2)))

\\ E.g.f.: (1/x) * Series_Reversion( H_2(x) ), where H_k(x) is the k-th iterate of U(x)*exp(-4*U(x)) and U(x) = -LambertW(-3*x)/3.
S3(n,k) = {
  my(N = n+1, x = 'x + O('x^(N+1)), U=-lambertw(-3*x)/3, H = U*exp(-4*U));
  my(H_n = x);
  for(i=1, k, H_n = subst(H_n, 'x, H));
  serreverse(H_n)/x
};

S3(21,2)-T3(21,2)

