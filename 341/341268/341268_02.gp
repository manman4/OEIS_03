\\ T(0,k) = 1 and T(n,k) = k*(k+1) * T(n-1,k+2) - k^2 * T(n-1,k) for n > 0.
T(n, k) = if(n==0, 1, k*(k+1)*T(n-1, k+2)-k^2*T(n-1, k));

for(n=0, 9, for(k=0, n, print1(T(k, n-k),", ")));