\\ Expansion of (F_2(x)/x)^(1/2), where F_k(x) is the k-th iteration of x*G3(x)^2 with G3(x) = 1 + x*G3(x)^3.
T3(n,k) = {
  my(N = n+1, x = 'x + O('x^(N+1)), g = sum(k = 0, N, binomial(3*k, k)/(2*k+1) * 'x^k));
  my(F_iter = x);
  for(i = 1, k, F_iter = subst(x * g^2, 'x, F_iter));
  (F_iter/x)^(1/2)
};

Vec(T3(21,2))

\\ G.f.: ((1/x) * Series_Reversion( H_2(x) ))^(1/2), where H_k(x) is the k-th iteration of x*(1 - x)^2.
S3(n,k) = {
  my(N = n+1, x = 'x + O('x^(N+1)), C = sum(k=0, N, binomial(2*k, k)/(k+1) * 'x^k), H = x*(1 - x)^2);
  my(H_n = x);
  for(i=1, k, H_n = subst(H_n, 'x, H));
  (serreverse(H_n)/x)^(1/2)
};

S3(21,2)-T3(21,2)

