\\ Square array A(n,k), n>=0, k>=0, read by antidiagonals downwards, where A(n,k) = n! * [x^n] F_k(x)/x and F_k(x) is the k-th iteration of x*G(x)^2 with G(x) = exp(x*G(x)^4).
Egen(n,k) = {
  my(N = n+1, x = 'x + O('x^(N+1)));
  my(g = 1 + sum(j = 1, N, (4*j+1)^(j-1)/j! * x^j));
  \\ G(x) = exp(x*G(x)^4)の確認
  if(g != exp(x*g^4), print("G(x) is wrong"));
  my(F_iter = x);

  for(i = 1, k, F_iter = subst(x * g^2, 'x, F_iter));

  n! * polcoef(F_iter/x, n)
};
matrix(7, 7, n, k, Egen(n-1, k-1))
for(n=0, 10, for(k=0, n, print1(Egen(k, n-k),", ")));

\\ E.g.f. of column k: (1/x) * Series_Reversion( H_k(x) ), where H_k(x) is the k-th iterate of -2*x^2 / LambertW(-2*x).
Tgen(n,k) = {
  my(N = n+1, x = 'x + O('x^(N+1)));
  \\ U = -LambertW(-(p-s)*x)/(p-s), or U = x when p = s
  my(Finv =  -2*x^2/lambertw(-2*x));
  my(Finv_k = x);

  for(i = 1, k, Finv_k = subst(Finv, 'x, Finv_k));

  n! * polcoef(serreverse(Finv_k)/x, n)
};
matrix(7, 7, n, k, Tgen(n-1, k-1))
for(n=0, 10, for(k=0, n, print1(Tgen(k, n-k)-Egen(k, n-k),", ")));
