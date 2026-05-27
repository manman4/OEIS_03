\\ A(n,k) = [x^n] (F_k(x)/x)^(1/2) and F_k(x) is the k-th iteration of x*G3(x)^2 with G3(x) = 1 + x*G3(x)^3.
T3(n,k) = {
  my(N = n+1, x = 'x + O('x^(N+1))); my(g = sum(k = 0, N, binomial(3*k, k)/(2*k+1) * 'x^k)); 
  my(F_iter = x);
  for(i=1, k, F_iter = subst(x * g^2, 'x, F_iter));
  polcoef((F_iter/x)^(1/2), n)
};

matrix(7, 7, n, k, T3(n-1, k-1))
for(n=0, 10, for(k=0, n, print1(T3(k, n-k),", ")));
for(k=0, 4, for(n=0, 20, print1(T3(n, k),", ")); print);

\\ G.f. of column k: ((1/x) * Series_Reversion( H_k(x) ))^(1/2), where H_k(x) is the k-th iteration of x*(1 - x)^2.
S4(n,k) = {
  my(N = n+1, x = 'x + O('x^(N+1)), H = x*(1 - x)^2);
  my(H_n = x);
  for(i=1, k, H_n = subst(H_n, 'x, H));
  polcoef((serreverse(H_n)/x)^(1/2), n)
};
for(n=0, 10, for(k=0, n, print1(S4(k, n-k)-T3(k, n-k),", ")));
for(k=0, 4, for(n=0, 20, print1(S4(n, k)-T3(n, k),", ")); print);
