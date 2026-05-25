\\ Expansion of F_3(x)/x, where F_k(x) is the k-th iteration of x*G3(x) with G3(x) = 1 + x*G3(x)^3.
T3(n,k) = {
  my(N = n+1, x = 'x + O('x^(N+1)), g = sum(k = 0, N, binomial(3*k, k)/(2*k+1) * 'x^k));
  my(F_iter = x);
  for(i = 1, k, F_iter = subst(x * g, 'x, F_iter));
  F_iter/x
};

Vec(T3(21,3))

\\ G.f.: (1/x) * Series_Reversion( H_3(x) ), where H_k(x) is the k-th iteration of x*(1 - x*C(x)) with C(x) = 1 + x*C(x)^2.
S3(n,k) = {
  my(N = n+1, x = 'x + O('x^(N+1)), C = sum(k=0, N, binomial(2*k, k)/(k+1) * 'x^k), H = x*(1 - x*C));
  my(H_n = x);
  for(i=1, k, H_n = subst(H_n, 'x, H));
  serreverse(H_n)/x
};

S3(21,3)-T3(21,3)

