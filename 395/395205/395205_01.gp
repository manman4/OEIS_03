\\ Let A(n,k) = (2*n)! * [x^(2*n)] (f(x)^k + f(-x)^k)/2. A(0,k) = 1 and A(n,k) = k*(k+2) * A(n-1,k+4) + k*(k+1) * A(n-1,k+2) for n > 0. a(n) = A(n,3).
a(n, k=3) = if(n==0, 1, k*(k+2)*a(n-1, k+4)+k*(k+1)*a(n-1, k+2));
for(n=0, 13, print1(a(n), ", "));