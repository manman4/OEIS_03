a_vector(n, k=1, l=5) = {
  my(k_limit(row)=k+(n-row)*l);
  my(A=vector(n, row, vector(k_limit(row)+1)));
  for(col=0, k_limit(1), A[1][col+1]=1);
  for(row=2, n, A[row][1]=0);
  for(row=2, n,
    for(col=1, k_limit(row),
      A[row][col+1] = A[row][col]
        + sum(j=1, row-1, binomial(row-1, j) * A[j][col+l] * A[row-j][col]);
    );
  );
  vector(n, row, A[row][k+1])
};

a_vector(30, 1, 5)

