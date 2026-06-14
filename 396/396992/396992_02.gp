\\ G.f. A(x) satisfies A(x) = x*(1 + A^l(x)), where A^l(x) denotes the l-th iterate of A.
\\ Let a(n,k,l) = [x^n] A^k(x), where A^k(x) is the k-th iterate of A.
\\ a(n,0,l) = 0^(n-1) and a(n,k,l) = a(n,k-1,l) + Sum_{j=1..n-1} a(j,k+l-1,l) * a(n-j,k-1,l) for k > 0.

a_vector(n, k=1, l=3) = {
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

a_vector(10, 0, 3)
a_vector(10, 1, 3)
a_vector(10, 2, 3)
a_vector(10, 3, 3)
a_vector(10, 4, 3)
a_vector(10, 5, 3)
a_vector(10, 6, 3)



\\ G.f. B(x) satisfies B(x) = x*(1 + B^l(x)),
\\ where B^l(x) denotes the l-th iterate of B.
\\ a(n,k,l) = [x^n] B^k(x)

iter(F, k, N) = {
  my(y = x + x * O(x^N));
  if(k == 0, return(y)); \\ B^0(x) = x
  for(i = 1, k, y = subst(F, x, y));
  y
};

Bseries(N, l = 3) = {
  my(B = x + x * O(x^N));
  for(i = 1, N, B = x * (1 + iter(B, l, N)));
  B
};

b(n, k = 1, l = 3) = {
  my(B = Bseries(n + 1, l));
  polcoef(iter(B, k, n + 1), n)
};

b_vector(N, k = 1, l = 3) = {
  my(B = Bseries(N + 1, l));
  my(Bk = iter(B, k, N + 1));
  vector(N, n, polcoef(Bk, n))
};

check_vectors(N, k = 1, l = 2) = {
  my(v1 = a_vector(N, k, l), v2 = b_vector(N, k, l));
  if(v1 == v2,
    print("ok");
  ,
    print("mismatch");
    print(v1);
    print(v2);
  );
};

check_vectors(12, 0, 3)
check_vectors(12, 1, 3)
check_vectors(12, 2, 3)
check_vectors(12, 3, 3)
check_vectors(12, 4, 3)
check_vectors(12, 5, 3)