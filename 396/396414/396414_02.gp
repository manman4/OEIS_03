\\ Expansion of F_2(x)/x, where F_k(x) is the k-th iteration of x*G3(x) with G3(x) = 1 + x*G3(x)^3.
T3(n,k) = {
  my(N = n+1); my(g = sum(k = 0, N, binomial(3*k, k)/(2*k+1) * 'x^k)); my(x = 'x + O('x^(N+1))); 
  my(F_iter = x);
  for(i = 1, k, F_iter = subst(x * g, 'x, F_iter));
  F_iter/x
};

Vec(T3(22,2))

S3(n,k) = {my(N = n+1, x = 'x + O('x^(N+1)), C = sum(k=0, N, binomial(2*k, k)/(k+1) * 'x^k), H = x - x^2 * C);
  my(H_n = x);
  for(i=1, k, H_n = subst(H_n, 'x, H));
  polcoef(serreverse(H_n)/x, n)};
for(k=0, 4, for(n=0, 20, print1(S3(n, k)-T3(n, k),", ")); print);

