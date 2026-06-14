\\ E.g.f. A(x) satisfies A'(x) = 1 + A^l(x), where A^l(x) denotes the l-th iterate of A, with A(0) = 0.
\\ Let a(n,k,l) = n! * [x^n] A^k(x), where A^k(x) is the k-th iterate of A.
\\ a(n,0,l) = 0^(n-1) and a(n,k,l) = a(n,k-1,l) + Sum_{j=1..n-1} binomial(n-1,j) * a(j,k+l-1,l) * a(n-j,k-1,l) for k > 0.

a_vector(n, k=1, l=5) = {
  my(k_limit(row)=k+(n-row)*l, A=vector(n, row, vector(k_limit(row)+1)));
  for(col=0, k_limit(1), A[1][col+1]=1);
  for(row=2, n, A[row][1]=0);
  for(row=2, n,
    for(col=1, k_limit(row),
      A[row][col+1]=A[row][col]+sum(j=1, row-1, binomial(row-1, j)*A[j][col+l]*A[row-j][col]);
    );
  );
  vector(n, row, A[row][k+1])
};

a_vector(12, 0, 5)
a_vector(12, 1, 5)
a_vector(12, 2, 5)
a_vector(12, 3, 5)
a_vector(12, 4, 5)
a_vector(12, 5, 5)


\\ E.g.f. B'(x) = 1 + B^l(x), B(0) = 0.
\\ B^l(x) denotes the l-th iterate of B.
\\ a(n,k,l) = n! * [x^n] B^k(x)

iter(F, k, N) = {
  my(y = 'x + O('x^(N + 1)));
  if(k == 0, return(y)); \\ B^0(x) = x
  for(i = 1, k, y = subst(F, 'x, y));
  y
};

Bseries(N, l = 3) = {
  my(B = 'x + O('x^(N + 1)));
  for(i = 1, N, B = intformal(1 + iter(B, l, N)));
  B
};

b(n, k = 1, l = 5) = {
  my(B = Bseries(n, l));
  n! * polcoef(iter(B, k, n), n)
};

b_vector(N, k = 1, l = 5) = {
  my(B = Bseries(N, l));
  my(Bk = iter(B, k, N));
  vector(N, n, n! * polcoef(Bk, n))
};

check_vectors(N, k = 1, l = 5) = {
  my(v1 = a_vector(N, k, l), v2 = b_vector(N, k, l));
  if(v1 == v2,
    print("ok");
  ,
    print("mismatch");
    print(v1);
    print(v2);
  );
};

check_vectors(12, 0, 5)
check_vectors(12, 1, 5)
check_vectors(12, 2, 5)
check_vectors(12, 3, 5)
check_vectors(12, 4, 5)
check_vectors(12, 5, 5)



