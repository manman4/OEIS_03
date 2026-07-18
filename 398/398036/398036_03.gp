\\ a(1) = 1; for n >= 2, a(n) = 3*a(n-1) - a(n/4) if 4|n, and a(n) = 3*a(n-1) otherwise.
a(N) =
{
  my(a = vector(N), n);
  if(N < 1, return(a));
  a[1] = 1;
  for(n = 2, N,
    a[n] = 3*a[n - 1] - if(n % 4 == 0, a[n/4], 0)
  );
  a
};

a(29)
