M=103;

a(n) = if(n%2, 0^(n-1), 2^omega(2*n));
for(n=1, M, print1(a(n), ", "));