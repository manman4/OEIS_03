\\ E.g.f. A'(x) = exp(A^l(x)), A(0) = 0.
\\ a(n,k,l) = n! * [x^n] A^k(x)

iter(F, k, N) = {
  my(y = 'x + O('x^(N + 1)));
  if(k == 0, return(y));               \\ A^0(x) = x
  for(i = 1, k, y = subst(F, 'x, y));
  y
}

Aseries(N, l = 3) = {
  my(A = 'x + O('x^(N + 1)));
  for(i = 1, N,
    A = intformal(exp(iter(A, l, N)));
  );
  A
}

a(n, k = 1, l = 3) = {
  my(A = Aseries(n, l));
  n! * polcoef(iter(A, k, n), n)
}

\\ Square array 
matrix(6, 7, n, k, a(n, k-1, 2))