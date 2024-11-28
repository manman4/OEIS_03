a005809(n) = binomial(3*n, n);

\\ 4*a(n) - 27*a(n-1) = 3*A005809(n) for n > 0.
a(n) = if(n==0, 1, (3*a005809(n) + 27*a(n-1))/4);
for(n=0, 21, print1(a(n), ", "));