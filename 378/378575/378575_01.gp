a_vector(n, k=1, l=5) = {
  my(k_limit(row)=k+(n-row)*(l-1), A=vector(n, row, vector(k_limit(row)+1)));
  for(col=0, k_limit(1), A[1][col+1]=1);
  for(row=2, n, A[row][1]=0);
  for(row=2, n,
    for(col=1, k_limit(row),
      A[row][col+1]=A[row][col]+sum(j=1, row-1, A[j][col+l]*A[row-j][col]);
    );
  );
  vector(n, row, A[row][k+1])
}; 

a_vector(10, 0, 5)
a_vector(10, 1, 5)
a_vector(10, 2, 5)
a_vector(10, 3, 5)
a_vector(10, 4, 5)
a_vector(10, 5, 5)
a_vector(10, 6, 5)

