\\ Let A(n,k) = (2*n)! * [x^(2*n)] C(x)^k. A(0,k) = 1 and A(n,k) = k*(k+5) * A(n-1,k+10) - k*(k+4) * A(n-1,k+8) for n > 0. a(n) = A(n,3).
a(n, k=3) = if(n==0, 1, k*(k+5)*a(n-1, k+10)-k*(k+4)*a(n-1, k+8));
for(n=0, 14, print1(a(n),", "));