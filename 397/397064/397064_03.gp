\\ a(1) = 1; for n >= 2, a(n) = 2*a(n-1) - a(n/3) if 3|n, and a(n) = 2*a(n-1) otherwise.
a(N) =
{
  my(a = vector(N), n);
  if(N < 1, return(a));
  a[1] = 1;
  for(n = 2, N,
    a[n] = 2*a[n - 1] - if(n % 3 == 0, a[n/3], 0)
  );
  a
};

a(35)
