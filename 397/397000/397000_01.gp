\\ E.g.f. A'(x) = exp(A^l(x)), A(0) = 0.
\\ A^l(x) denotes the l-th iterate of A.
\\ Square array entry: T(n,k) = n! * [x^n] A^k(x), n >= 1, k >= 0.

iter(F, k, N) = {
  my(y = 'x + O('x^(N + 1)));
  if(k == 0, return(y));
  for(i = 1, k, y = subst(F, 'x, y));
  y
}

Aseries(N, l = 3) = {
  my(A = 'x + O('x^(N + 1)));
  for(i = 1, N, A = intformal(exp(iter(A, l, N))));
  A
}

a_rows(row_num, l = 3) = {
  my(A = Aseries(row_num, l));
  my(F = vector(row_num)); \\ F[k+1] = A^k(x)

  F[1] = 'x + O('x^(row_num + 1));          \\ A^0(x) = x
  for(k = 1, row_num - 1, F[k + 1] = subst(A, 'x, F[k]));

  \\ row d = [T(1,d-1), T(2,d-2), ..., T(d,0)]
  for(d = 1, row_num,
    print(vector(d, n, n! * polcoef(F[d - n + 1], n)));
  );
}

a_antidiagonal(row_num, l = 3) = {
  my(A = Aseries(row_num, l));
  my(F = vector(row_num), v = []);

  F[1] = 'x + O('x^(row_num + 1));          \\ A^0(x) = x
  for(k = 1, row_num - 1, F[k + 1] = subst(A, 'x, F[k]));

  for(d = 1, row_num,
    v = concat(v, vector(d, n, n! * polcoef(F[d - n + 1], n)));
  );
  Vec(v)
}

a_rows(10, 3);
print(a_antidiagonal(10, 3));
