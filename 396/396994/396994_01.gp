\\ A[row][col+1] = a(row,col,l)

a_antidiagonal(row_num, l=3) = {
  my(k_limit(row)=(row_num-row)*l, A=vector(row_num, row, vector(k_limit(row)+1)), v=[]);

  for(col=0, k_limit(1), A[1][col+1]=1);
  for(row=2, row_num, A[row][1]=0);

  for(row=2, row_num,
    for(col=1, k_limit(row),
      A[row][col+1] = A[row][col] + sum(j=1, row-1, A[j][col+l] * A[row-j][col]);
    );
  );

  for(d=1, row_num,
    for(row=1, d,
      v = concat(v, A[row][d-row+1]); \\ k = d-row
    );
  );

  Vec(v)
};

v = a_antidiagonal(140, 5);
cnt = 1;
for(n=1, 9870, write("b396994_1.txt", n, " ", v[cnt]); cnt++);



