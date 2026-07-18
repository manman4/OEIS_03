\\ a(0) = 1; for n >= 1, a(n) = 4*a(n-1) + a(n/2) if 2|n, and a(n) = 4*a(n-1) otherwise.
\\ vは１から始まる。一方a(0) = 1なので、a(n)はv(n+1)に対応する。
a(N) =
{
  my(a = vector(N+1), n);
  if(N < 0, return(a));
  a[1] = 1;
  for(n = 1, N,
    a[n+1] = 4*a[n] + if(n % 2 == 0, a[n/2+1], 0)
  );
  a
};
a(25)

b(n) = if(n==0, 1, 4*b(n-1) + if(n % 2 == 0, b(n/2), 0));
for(n=0, 15, print1(b(n),", "));
