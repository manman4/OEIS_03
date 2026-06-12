a_vector(n, l=5) = {
  my(k_limit(r) = 1 + (n - r) * l);
  my(A = vector(n, r, vector(k_limit(r) + 1)));
  for(k=0, k_limit(1), A[1][k + 1] = 1);
  for(r=2, n, A[r][1] = 0);
  for(r=2, n,
    for(k=1, k_limit(r),
      my(s=A[r][k]);
      for(j=1, r-1,
        s += binomial(r - 1, j) * A[j][k + l] * A[r - j][k];
      );
      A[r][k+1]=s;
    );
  );
  vector(n, r, A[r][2])
};
a_vector(30)

