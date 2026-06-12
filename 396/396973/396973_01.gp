\\ A[n][k+1] = a(n,k,l)

a_table(row_num, l) =
{
  my(k_limit(n) = row_num - 1 + (row_num - n) * l);
  my(A = vector(row_num, n, vector(k_limit(n) + 1)));

  \\ a(1,k,l) = 1
  for (k = 0, k_limit(1),
    A[1][k + 1] = 1;
  );

  \\ a(n,0,l) = 0 for n >= 2
  for (n = 2, row_num,
    A[n][1] = 0;
  );

  \\ a(n,k,l) = a(n,k-1,l) + Sum_{j=1..n-1} binomial(n-1,j) * a(j,k+l-1,l) * a(n-j,k-1,l)
  for (n = 2, row_num,
    for (k = 1, k_limit(n),
      my(s = A[n][k]);
      for (j = 1, n - 1,
        s += binomial(n - 1, j) * A[j][k + l] * A[n - j][k];
      );
      A[n][k + 1] = s;
    );
  );

  A
};

\\ row r = [a(1,r-1,l), a(2,r-2,l), ..., a(r,1,l)]
a_rows(row_num, l) =
{
  my(A = a_table(row_num, l));
  for (r = 1, row_num,
    print(vector(r, n, A[n][r - n + 2]));
  );
};

\\ antidiagonals including the k=0 column
a_antidiagonal(row_num, l) =
{
  my(A = a_table(row_num, l), v = []);
  for (r = 1, row_num,
    v = concat(v, vector(r, n, A[n][r - n + 1]));
  );
  Vec(v)
};

v = a_antidiagonal(8, 5);
print(v);

