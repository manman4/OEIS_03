\\ A(n,k) = n! * [x^n] log(1 - W_k(-x)) and W_k(x) is the k-th iterate of LambertW(x).
a(n,k) = {
  my(N=n+1, x = 'x + O('x^(N+1)));
  my(Finv=lambertw(x));
  my(Finv_k=x);

  for(i=1, k, Finv_k = subst(Finv, 'x, Finv_k));

  n! * polcoef(log(1 - subst(Finv_k, 'x, -x)), n)
};
\\ Square array A(n,k), n>=1, k>=0
matrix(6, 6, n, k, a(n, k-1))
for(n=1, 10, for(k=1, n, print1(a(k, n-k),", ")));
