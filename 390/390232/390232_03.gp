\\ Let A(n,k) = (2*n)! * [x^(2*n)] (f(x)^k + f(-x)^k)/2. A(0,k) = 1 and A(0,k) = 1 and A(n,k) = 4*k*(k+1) * A(n-1,k+2) - k^2 * A(n-1,k) for n > 0. a(n) = A(n,2).
a(n, k=2) = if(n==0, 1, 4*k*(k+1)*a(n-1, k+2)-k^2*a(n-1, k));
for(n=0, 15, print1(a(n), ", "));

