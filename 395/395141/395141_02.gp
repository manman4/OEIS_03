\\ a(n) = (2*n)! * [x^(2*n)] C(x)^k, where C(x) satisfies
\\ C(x) = cosh( Integral C(x)^3 dx ).
\\ Speedup: compute C(x) once to max degree, then reuse powers C^k.

compute_C(maxN) =
{
  my(S = x, C = 1, D = 2*maxN);
  for(i=0, maxN,
    S = intformal(C^4 + x*O(x^(D+1)));
    C = 1 + intformal(S*C^3);
  );
  C;
}

\\ Precompute all coefficients needed up to n = 139
maxN = 139;
C = compute_C(maxN);

\\ Precompute powers C^k up to k = maxN
powC = vector(maxN + 1);
powC[1] = 1 + O(x^(2*maxN+1)); \\ k = 0
for(k=1, maxN, powC[k+1] = powC[k]*C);

cnt = 0;
for(n=0, maxN, for(k=0, n, n0=k; k0=n-k; write("b395141_1.txt", cnt, " ", (2*n0)! * polcoef(powC[k0+1], 2*n0)); cnt++));

