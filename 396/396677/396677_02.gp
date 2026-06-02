\\ a(n) = (n-1)! * Sum_{i,j >= 0 and i+j=n-1} (-1)^j * n^i / (i!).
a133297(n) = {
  sum(i=0, n-1,
    my(j=n-1-i);
    (n-1)! * (-1)^j * n^i / (i!)
  )
};
for(n=1, 18, print1(a133297(n), ", "));

\\ a(n) = (n-1)! * Sum_{i,j,k >= 0 and i+j+k=n-1} (-1)^k * n^i * (n-i)^j / (i!*j!).
a396677(n) = {
  sum(i=0, n-1,
    sum(j=0, n-1-i,
      my(k=n-1-i-j);
      (n-1)! * (-1)^k * n^i * (n-i)^j / (i!*j!)
    )
  )
};
for(n=1, 18, print1(a396677(n), ", "));

\\ a(n) = Sum_{k=1..n} n^(n-k) * binomial(n-1,k-1) * A133297(k).
a(n) = sum(k=1, n, n^(n-k) * binomial(n-1, k-1) * a133297(k));
for(n=1, 50, print1(a(n)-a396677(n), ", "));

