\\ a(n) = (n-1)! * Sum_{i,j >= 0 and i+j=n-1} (j+1) * n^i / (i!).
a000312(n) = {
  sum(i=0, n-1,
    my(j=n-1-i);
    (n-1)! * (j+1) * n^i / (i!)
  )
};
for(n=1, 18, print1(a000312(n), ", "));

\\ a(n) = (n-1)! * Sum_{i,j,k >= 0 and i+j+k=n-1} (k+1) * n^i * (n-i)^j / (i!*j!).
a396712(n) = {
  sum(i=0, n-1,
    sum(j=0, n-1-i,
      my(k=n-1-i-j);
      (n-1)! * (k+1) * n^i * (n-i)^j / (i!*j!)
    )
  )
};
for(n=1, 25, print1(a396712(n), ", "));

\\ a(n) = Sum_{k=1..n} n^(n-k) * binomial(n-1,k-1) * A000312(k).
a(n) = sum(k=1, n, n^(n-k) * binomial(n-1, k-1) * a000312(k));
for(n=1, 50, print1(a(n)-a396712(n), ", "));
