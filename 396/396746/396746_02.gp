\\ a(n) = (n-1)! * Sum_{i,j >= 0 and i+j=n-1} (-1)^j * (j+1) * n^i / (i!).
a277458(n) = {
  sum(i=0, n-1,
    my(j=n-1-i);
    (n-1)! * (-1)^j * (j+1) * n^i / (i!)
  )
};
for(n=1, 18, print1(a277458(n), ", "));

\\ a(n) = (n-1)! * Sum_{i,j,k >= 0 and i+j+k=n-1} (-1)^k * (k+1) * n^i * (n-i)^j / (i!*j!).
a396745(n) = {
  sum(i=0, n-1,
    sum(j=0, n-1-i,
      my(k=n-1-i-j);
      (n-1)! * (-1)^k * (k+1) * n^i * (n-i)^j / (i!*j!)
    )
  )
};
for(n=1, 19, print1(a396745(n), ", "));

\\ a(n) = (n-1)! * Sum_{i,j,k,l >= 0 and i+j+k+l=n-1} (-1)^l * (l+1) * n^i * (n-i)^j * (n-i-j)^k / (i!*j!*k!).
a396746(n) = {
  sum(i=0, n-1,
    sum(j=0, n-1-i,
      sum(k=0, n-1-i-j,
        my(l=n-1-i-j-k);
        (n-1)! * (-1)^l * (l+1) * n^i * (n-i)^j * (n-i-j)^k / (i!*j!*k!)
      )
    )
  )
};
for(n=1, 18, print1(a396746(n), ", "));

\\ a(n) = Sum_{k=1..n} n^(n-k) * binomial(n-1,k-1) * A277458(k).
a(n) = sum(k=1, n, n^(n-k) * binomial(n-1, k-1) * a277458(k));
for(n=1, 50, print1(a(n)-a396745(n), ", "));
