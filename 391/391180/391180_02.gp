\\ Recursive definition (for reference)
T(n, k) = if(k < 0 || k >= n, 0, if(n == 1 && k == 0, 1, 2*(k+1)*(2*k+1)*T(n-1, k+1) + 4*((4*k+1)*n+2*k^2-3*k-1)*T(n-1, k) + 2*(4*n+2*k-3)*(2*n+k-3)*T(n-1, k-1)));

kill(n);
kill(k);

\\ Fast DP: returns row n (k = 0..n-1)
T_row(n) =
{
  if (n <= 0, return([]));
  a = [1];            \\ n = 1 row
  if (n == 1, return(a));
  for (m=2, n,
    nxt = vector(m, k, 0);
    for (k=0, m-1,
      nxt[k+1] =
        if(k+2 <= #a, 2*(k+1)*(2*k+1)*a[k+2], 0)
      + if(k < #a, 4*((4*k+1)*m+2*k^2-3*k-1)*a[k+1], 0)
      + if(k > 0, 2*(4*m+2*k-3)*(2*m+k-3)*a[k], 0);
    );
    a = nxt;
  );
  a;
}

\\ Fast sequence: T(1,1), T(2,1), ... (note T(n,1) is k=1)
T_k1_seq(N) =
{
  seq = vector(N, i, 0);
  a = [1];            \\ n = 1 row
  seq[1] = 0;         \\ T(1,1)=0 because k>=n
  if (N == 1, return(seq));
  for (m=2, N,
    nxt = vector(m, k, 0);
    for (k=0, m-1,
      nxt[k+1] =
        if(k+2 <= #a, 2*(k+1)*(2*k+1)*a[k+2], 0)
      + if(k < #a, 4*((4*k+1)*m+2*k^2-3*k-1)*a[k+1], 0)
      + if(k > 0, 2*(4*m+2*k-3)*(2*m+k-3)*a[k], 0);
    );
    a = nxt;
    seq[m] = a[2];    \\ k=1 -> index 2
  );
  seq;
}

print(T_k1_seq(10));
