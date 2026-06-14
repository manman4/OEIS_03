\\ E.g.f. A'(x) = exp(A^l(x)), A(0) = 0.
\\ B(n,k) = n! * [x^n] exp(A^k(x)), n >= 0, k >= 0.

iter(F, k, N) = {
  my(y = 'x + O('x^(N + 1)));
  if(k == 0, return(y)); \\ A^0(x) = x
  for(i = 1, k, y = subst(F, 'x, y));
  y
}

Aseries(N, l = 3) = {
  my(A = 'x + O('x^(N + 1)));
  for(i = 1, N, A = intformal(exp(iter(A, l, N))));
  A
}

b_rows(row_num, l = 3) = {
  my(N = row_num - 1, A = Aseries(row_num - 1, l));
  my(F = vector(row_num));

  \\ F[k+1] = A^k(x), k = 0..row_num-1
  F[1] = 'x + O('x^(N + 1));
  for(k = 1, row_num - 1, F[k + 1] = subst(A, 'x, F[k]));

  \\ antidiagonal d = [B(0,d), B(1,d-1), ..., B(d,0)]
  for(d = 0, row_num - 1,
    print(vector(d + 1, n,
      my(G = exp(F[d - n + 2]));
      (n - 1)! * polcoef(G, n - 1)
    ));
  );
}

b_antidiagonal(row_num, l = 3) = {
  my(N = row_num - 1, A = Aseries(row_num - 1, l));
  my(F = vector(row_num), v = []);

  \\ F[k+1] = A^k(x), k = 0..row_num-1
  F[1] = 'x + O('x^(N + 1));
  for(k = 1, row_num - 1, F[k + 1] = subst(A, 'x, F[k]));

  for(d = 0, row_num - 1,
    v = concat(v, vector(d + 1, n,
      my(G = exp(F[d - n + 2]));
      (n - 1)! * polcoef(G, n - 1)
    ));
  );
  Vec(v)
}

b_rows(8, 2);
print(b_antidiagonal(8, 2));
